//==================================================================================
// Copyright (c) 2016 , Advanced Micro Devices, Inc.  All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file PowerProfileTranslate.cpp
///
//==================================================================================

#include <cstring>
#include <cmath>
#include <cstdarg>
#include <list>

#include <PowerProfileHelper.h>
#include <AMDTPowerProfileControl.h>
#include <AMDTPwrProfAttributes.h>
#include <AMDTSharedBufferConfig.h>
#include <AMDTHistogram.h>
#include <AMDTDriverTypedefs.h>
#include <AMDTPowerProfileInternal.h>
#include <PowerProfileTranslate.h>

struct ContextData;

AMDTUInt32 g_tracePids = 0;

PowerData   g_aggrPidPowerList;

AMDTUInt32 g_markerCnt = 0;

PwrInstrumentedPowerData* g_pInstrumentedPowerData = nullptr;

static std::list <ProcessName> g_processNames;

// Memory pool for translation layer
MemoryPool g_transPool;

#define MICROSEC_PER_SEC 1000000
#define MILLISEC_PER_SEC 1000

#ifdef LINUX
    // Linux driver return TS is micro seconds
    const AMDTFloat32 g_countPerSeconds = static_cast<AMDTFloat32>(MICROSEC_PER_SEC);
#else
    // Win driver return TS is milli seconds
    const AMDTFloat32 g_countPerSeconds = static_cast<AMDTFloat32>(MILLISEC_PER_SEC);
#endif

#define WCHAR_PATH_LEN 2 * OS_MAX_PATH
extern AMDTUInt8* g_pSharedBuffer;
PowerProfileTranslate::PowerProfileTranslate()
{
    m_sampleOffset = 0;
    m_configList.m_cfgCnt = 0;
    m_consumedRecId = 0;

    // Platform id
    m_platformId = PLATFORM_INVALID;
}
PowerProfileTranslate::~PowerProfileTranslate()
{
    //Close the raw file and cleanup some memory. We don't need any more
    //TODO: need to check  m_rawFileHld->Close();
}

#ifdef MEMORY_TIME_TRACE
AMDTUInt64 processContext = 0;
AMDTUInt64 calculatePower = 0;
AMDTUInt64 modInfoTs = 0;
AMDTUInt64 modtab = 0;
AMDTUInt64 outDataTs = 0;

#include <psapi.h>

void PrintMemoryUsage(const char* header)
{
    DWORD                   procId = GetCurrentProcessId();
    HANDLE                  hProc;
    PROCESS_MEMORY_COUNTERS pmc;

    hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                        FALSE, procId);

    if (NULL == hProc)
    {
        return;
    }

    // Print the header, if provided by the caller...
    if (NULL != header)
    {
        PwrTrace("%s", header);
    }

    if (GetProcessMemoryInfo(hProc, &pmc, sizeof(pmc)))
    {
        PwrTrace("\tPeakWorkingSetSize: %d bytes", pmc.PeakWorkingSetSize);
        PwrTrace("\tWorkingSetSize: %d bytes", pmc.WorkingSetSize);
    }

    CloseHandle(hProc);

    return;
}
#endif
void PowerProfileTranslate::ClosePowerProfileTranslate()
{
    m_isRunning =  false;
    AMDTUInt32 cnt = 0;

    PwrTrace("PID Collected %d", g_tracePids);
    //TRACE//
    {


        for (cnt = 0; cnt < g_markerCnt; cnt++)
        {
            PwrTrace("markerid %d\n\
                          pid %d \n\
                          name %s\n\
                          buffer %s\n\
                          pwr %f\n\
                          name %s",
                     g_pInstrumentedPowerData[cnt].m_markerId,
                     g_pInstrumentedPowerData[cnt].m_data.m_pidInfo.m_pid,
                     g_pInstrumentedPowerData[cnt].m_data.m_name,
                     g_pInstrumentedPowerData[cnt].m_data.m_userBuffer,
                     g_pInstrumentedPowerData[cnt].m_data.m_pidInfo.m_power,
                     g_pInstrumentedPowerData[cnt].m_data.m_pidInfo.m_name);
        }
    }

    ReleaseMemoryPool(&g_transPool);

    m_configList.m_pCfg = NULL;

    // Close the m_rawFileHld
    if (NULL != m_rawFileHld)
    {
        // Cleanup the buffer list from previous run
        m_rawFileHld->ReleaseBufferList();
        delete m_rawFileHld; // Destructor calls close
    }

#if !(defined(_WIN64) || defined(LINUX))
    fnCleanupMaps();
#endif
    m_rawFileHld = NULL;
}

//InitPowerProfiler - Initialize power profiler
AMDTResult PowerProfileTranslate::InitPowerTranslate(const wchar_t* pFileName, AMDTUInt64 flag)
{
    AMDTInt32 ret = AMDT_STATUS_OK;
    SectionSampleInfo sampleInfo;
    bool isOnline = (flag & 0x01) ? true : false;

    // Create the memory pool for translation layer
    ret = CreateMemoryPool(&g_transPool, TRANSLATION_POOL_SIZE);

    if (AMDT_STATUS_OK == ret)
    {
        m_rawFileHld = new RawDataReader();

        if (!m_rawFileHld)
        {
            ret = AMDT_ERROR_FAIL;
        }
    }

    //Initialize raw file
    if (AMDT_STATUS_OK == ret)
    {
        ret = m_rawFileHld->RawDataInitialize(pFileName, isOnline);
    }

    if (AMDT_STATUS_OK == ret)
    {
        ret = AMDTPwrGetTargetSystemInfo(&m_sysInfo);

        if (AMDT_STATUS_OK == ret)
        {
            m_targetCoreCnt = m_sysInfo.m_coreCnt;

            if (m_sysInfo.m_isAmdApu)
            {
                m_targetCuCnt = m_sysInfo.m_computeUnitCnt;
                m_coresPerCu = m_targetCoreCnt / m_targetCuCnt;
            }
        }
    }

    if (AMDT_STATUS_OK == ret)
    {
        ret = PwrSetCoreConfigs();
    }

    //Allocate the processed list for sample records
    if (AMDT_STATUS_OK == ret)
    {
        ret = m_rawFileHld->GetSampleSectionInfo(&sampleInfo);

        m_platformId = m_sysInfo.m_platformId;
    }

    if (AMDT_STATUS_OK == ret)
    {
        m_pSahredBuffer = (SharedBuffer*)GetMemoryPoolBuffer(&g_transPool, m_targetCoreCnt * sizeof(SharedBuffer));

        // Reset PID power
        memset(&g_aggrPidPowerList, 0, sizeof(PowerData));
        memset(m_samplePower, 0, sizeof(m_samplePower));

        // Initialize histogram counters
        InitializeHistogram();

        // Initialize TS parameters
        m_prevTs = 0;
        m_currentTs = 0;
        m_prevTs1 = 0;

        // Instrumented power data
        g_pInstrumentedPowerData = (PwrInstrumentedPowerData*)GetMemoryPoolBuffer(&g_transPool, PWR_MAX_MARKER_CNT * sizeof(PwrInstrumentedPowerData));
        memset(g_pInstrumentedPowerData, 0, PWR_MAX_MARKER_CNT * sizeof(PwrInstrumentedPowerData));
        g_markerCnt = 0;
    }

    m_isRunning =  true;

    // Reset previous Smu sample value list
    memset(m_prevSmuSampleData, 0, MAX_PREV_COUNTERS * sizeof(AMDTPwrAttributeInfo));
    //allocate the buffer list again
    m_rawFileHld->PrepareRawBufferList((AMDTUInt16)m_targetCoreCnt);

    memset(m_sampleIpcLoad, 0, sizeof(AMDTFloat32) * MAX_CORE_CNT);

    m_perfCounter = m_rawFileHld->GetSessionProfileStartPerfCounter();
    m_perfFreq = m_rawFileHld->GetSessionPerfFreq();
    return ret;
}

// GetConfigCoreMask: get the core mask set for profiling
AMDTResult PowerProfileTranslate::GetConfigCoreMask(AMDTUInt64* pMask)
{
    AMDTResult ret = AMDT_STATUS_OK;
    ProfileConfigList cfgTab;
    ProfileConfig cfg[MAX_CONFIG_CNT];
    cfgTab.m_profileConfig = &cfg[0];
    AMDTUInt32 cnt = 0;
    AMDTUInt64 cfgMask = 0;

    m_rawFileHld->GetProfileCfgInfo(&cfgTab);

    for (cnt = 0; cnt < cfgTab.m_configCnt; cnt++)
    {
        ProfileConfig* pCfg = cfgTab.m_profileConfig + cnt;

        if (NULL == pCfg)
        {
            ret = AMDT_ERROR_FAIL;
            break;
        }

        cfgMask |= pCfg->m_samplingSpec.m_mask;
    }

    if (AMDT_STATUS_OK == ret)
    {
        *pMask = cfgMask;
    }

    return ret;
}

AMDTResult PowerProfileTranslate::PwrSetCoreConfigs(void)
{
    AMDTInt32 ret = AMDT_STATUS_OK;
    AMDTUInt32 cfgCnt = 0;
    AMDTUInt32 smuCnt = 0;
    ProfileConfigList cfgTab;
    ProfileConfig cfg[MAX_CONFIG_CNT];
    cfgTab.m_profileConfig = &cfg[0];
    m_rawFileHld->GetProfileCfgInfo(&cfgTab);

    for (cfgCnt = 0; cfgCnt < cfgTab.m_configCnt; cfgCnt++)
    {
        AMDTUInt32 packageId = 0;
        ProfileConfig* pSrc = &cfgTab.m_profileConfig[cfgCnt];
        PwrCoreConfig coreCfg;
        memset(&coreCfg, 0, sizeof(PwrCoreConfig));

        if (pSrc)
        {
            AMDTUInt32 instanceId = 0;
            AMDTUInt64 tempCoreCounterMask = 0;
            coreCfg.m_sampleId = pSrc->m_sampleId;
            PwrCounterDecodeInfo decodeInfo;
            memset(&decodeInfo, 0, sizeof(PwrCounterDecodeInfo));

            // Get instance id from core mask and sample id
            GetFirstSetBitIndex(&instanceId, cfgTab.m_profileConfig[cfgCnt].m_samplingSpec.m_mask);

            // Set the parameters
            m_profileType = (ProfileType)pSrc->m_samplingSpec.m_profileType;
            m_samplingPeriod = (AMDTUInt32)pSrc->m_samplingSpec.m_samplingPeriod;

            if (PROFILE_TYPE_PROCESS_PROFILING == m_profileType)
            {
                m_recIdFactor = m_samplingPeriod;
            }
            else
            {
                m_recIdFactor = 1;
            }

            // Core counters
            tempCoreCounterMask = pSrc->m_apuCounterMask;
            packageId = 0;

            while (tempCoreCounterMask > 0)
            {
                AMDTUInt32 counterId = 0;
                GetFirstSetBitIndex(&counterId, tempCoreCounterMask);
                tempCoreCounterMask &= ~(1ULL << counterId);
                PwrSupportedCounterMap* pCounters = PwrGetSupportedCounterList();

                for (auto Iter : *pCounters)
                {
                    if ((Iter.second.m_pkgId == 0)
                        && (counterId == Iter.second.m_basicInfo.m_attrId)
                        && (AMDT_PWR_VALUE_SINGLE == Iter.second.m_basicInfo.m_aggr)
                        && (instanceId == Iter.second.m_instanceId))
                    {
                        decodeInfo.m_clientId = Iter.first;
                        decodeInfo.m_basicId = counterId;
                        decodeInfo.m_pkgId = Iter.second.m_pkgId;
                        decodeInfo.m_instanceId = Iter.second.m_instanceId;
                        decodeInfo.m_smuIpVersion = 0;
                        decodeInfo.m_category = Iter.second.m_basicInfo.m_category;
                        coreCfg.m_counters.push_back(decodeInfo);
                    }
                }
            }

            // Smu counters only for sample id 1
            if (1 == pSrc->m_sampleId)
            {
                for (smuCnt = 0; smuCnt < pSrc->m_activeList.m_count; smuCnt++)
                {
                    tempCoreCounterMask = pSrc->m_activeList.m_info[smuCnt].m_counterMask;
                    packageId = pSrc->m_activeList.m_info[smuCnt].m_packageId;

                    while (tempCoreCounterMask)
                    {
                        AMDTUInt32 counterId = 0;
                        GetFirstSetBitIndex(&counterId, tempCoreCounterMask);
                        tempCoreCounterMask &= ~(1ULL << counterId);
                        PwrSupportedCounterMap* pCounters = PwrGetSupportedCounterList();

                        for (auto Iter : *pCounters)
                        {
                            if ((packageId == Iter.second.m_pkgId)
                                && (counterId == Iter.second.m_basicInfo.m_attrId)
                                && (AMDT_PWR_VALUE_SINGLE == Iter.second.m_basicInfo.m_aggr))
                            {
                                memset(&decodeInfo, 0, sizeof(PwrCounterDecodeInfo));
                                decodeInfo.m_clientId = Iter.first;
                                decodeInfo.m_basicId = counterId;
                                decodeInfo.m_pkgId = packageId;
                                decodeInfo.m_instanceId = Iter.second.m_instanceId;
                                decodeInfo.m_smuIpVersion = pSrc->m_activeList.m_info[smuCnt].m_smuIpVersion;
                                decodeInfo.m_category = Iter.second.m_basicInfo.m_category;
                                coreCfg.m_counters.push_back(decodeInfo);
                            }
                        }
                    }
                }
            }

            m_coreConfigs.push_back(coreCfg);
        }
    }

    if (AMDT_STATUS_OK == ret)
    {
        PrepareInitialProcessList(g_processNames);
    }

#ifdef MEMORY_TIME_TRACE
    PrintMemoryUsage("Profile beginging...");
#endif
    return ret;
}

// GetProcessedList: Get the latest processed data from the list
AMDTResult PowerProfileTranslate::GetProcessedList(AMDTPwrProcessedDataRecord* pData)
{
    AMDTResult ret = AMDT_STATUS_OK;

    std::unique_lock<std::mutex> lock(m_lock);

    if (!m_processedData.empty())
    {
        while (m_processedData.empty())
        {
            m_condition.wait(lock);
        }

        if (nullptr != pData)
        {
            AMDTPwrProcessedDataRecord* pRecord = &m_processedData.front();
            //memcpy(pData, &m_processedData.front(), sizeof(AMDTPwrProcessedDataRecord));
            pData->m_recId = pRecord->m_recId;
            pData->m_recordType = pRecord->m_recordType;
            pData->m_ts = pRecord->m_ts;
            pData->m_attrCnt = pRecord->m_attrCnt;
            pData->m_counters = pRecord->m_counters;
            m_processedData.pop();
        }
        else
        {
            ret = AMDT_ERROR_INTERNAL;
            PwrTrace("pData = NULL");
        }
    }
    else
    {
        ret = AMDT_ERROR_NODATA;
        //PwrTrace("data not available");
    }

    return ret;
}

// ProcessMarkerRecords: process marker records
void PowerProfileTranslate::ProcessMarkerRecords(AMDTUInt8* pRaw, AMDTUInt32 bufferSize, AMDTUInt32* pOffset)
{
    AMDTUInt32 markerLen = PWR_MARKER_BUFFER_SIZE + sizeof(RawRecordHdr);
    RawRecordHdr hdr;
    MarkerTag marker;
    AMDTUInt64 ts = 0;

    memset(&hdr, 0, sizeof(RawRecordHdr));

    while (1)
    {
        if ((*pOffset + markerLen) <= bufferSize)
        {
            memcpy(&hdr, (RawRecordHdr*)(pRaw + *pOffset), sizeof(RawRecordHdr));
        }

        if (REC_TYPE_MARKER_DATA == hdr.m_recordType)
        {
            // Activate the instrument base profile marker
            *pOffset += sizeof(RawRecordHdr);
            memcpy(&marker, (MarkerTag*)&pRaw[*pOffset], sizeof(MarkerTag));

            *pOffset += sizeof(MarkerTag);

            if ((0 != marker.m_markerId) && (g_markerCnt < PWR_MAX_MARKER_CNT))
            {
                if (PWR_MARKER_ENABLE == marker.m_state)
                {
                    g_pInstrumentedPowerData[g_markerCnt].m_state = PWR_MARKER_ENABLE;
                    g_pInstrumentedPowerData[g_markerCnt].m_markerId = marker.m_markerId;
                    g_pInstrumentedPowerData[g_markerCnt].m_data.m_pidInfo.m_pid = marker.m_pid;
                    SetElapsedTime(marker.m_ts, &ts);
                    g_pInstrumentedPowerData[g_markerCnt].m_data.m_startTs = ts;
                    memcpy(g_pInstrumentedPowerData[g_markerCnt].m_data.m_name, marker.m_name, PWR_MARKER_BUFFER_SIZE);
                    memcpy(g_pInstrumentedPowerData[g_markerCnt].m_data.m_userBuffer, marker.m_userBuffer, PWR_MARKER_BUFFER_SIZE);
                    g_markerCnt++;
                }
                else if (PWR_MARKER_DISABLE == marker.m_state)
                {
                    AMDTUInt32 loop = 0;

                    for (loop = 0 ; loop < g_markerCnt; loop++)
                    {
                        if (g_pInstrumentedPowerData[loop].m_markerId == marker.m_markerId)
                        {
                            g_pInstrumentedPowerData[loop].m_state = PWR_MARKER_DISABLE;
                            SetElapsedTime(marker.m_ts, &ts);
                            g_pInstrumentedPowerData[g_markerCnt].m_data.m_startTs = ts;
                        }
                    }
                }
            }
        }
        else
        {
            break;
        }
    }
}

// ProcessSample: Process each data sample for process/module/ip profiling
AMDTResult PowerProfileTranslate::ProcessSample(ContextData* pCtx, AMDTFloat32 ipc, AMDTUInt32 coreId)
{

#if (defined(_WIN64) || defined(LINUX))
    (void)pCtx;
    (void)ipc;
    (void)coreId;
#else

    AMDTUInt64 key = 0;
    bool newModule = false;
    AMDTUInt64 deltaTick = 0;
    SetElapsedTime(pCtx->m_timeStamp, (AMDTUInt64*)&deltaTick);

    AMDTUInt32 instanceId = 0;
#ifdef MEMORY_TIME_TRACE
    AMDTUInt64 ts1 = 0;
#endif
    pCtx->m_ip = pCtx->m_ip & 0x0000FFFFFFFFFFFF;
#ifdef MEMORY_TIME_TRACE
    ts1 = GetOsTimeStamp();
#endif

    if (S_OK != fnGetModuleInstanceId(pCtx->m_processId, pCtx->m_ip, deltaTick, instanceId))
    {
        PwrTrace("fnGetModuleInstanceId failed for PID %d", pCtx->m_processId);
    }

#ifdef MEMORY_TIME_TRACE
    ts1 = GetOsTimeStamp() - ts1;
    modInfoTs += ts1;
#endif
    key = (pCtx->m_ip & 0x0000FFFFFFFFFFFFULL) | ((AMDTUInt64)instanceId << 48);
    SampleMap:: iterator sampleIter = m_sampleMap[coreId].find(key);

    if (sampleIter == m_sampleMap[coreId].end())
    {
        SampleData sample;
        sample.m_ip = pCtx->m_ip;
        sample.m_processId = pCtx->m_processId;
        sample.m_threadId = pCtx->m_threadId;
        sample.m_modInstance = instanceId;
        sample.m_timeStamp = deltaTick;
        sample.m_ipc = ipc;
        sample.m_sampleCnt = 1;
        m_sampleMap[coreId].insert(SampleMap::value_type(key, sample));
        newModule = true;
    }
    else
    {
        sampleIter->second.m_ipc += ipc;
        sampleIter->second.m_sampleCnt++;
    }

    m_sampleIpcLoad[coreId] += ipc;
#ifdef MEMORY_TIME_TRACE
    AMDTUInt64 ts = GetOsTimeStamp();
#endif

    if (newModule)
    {
        ModuleInfoMap::iterator tabIter = m_moduleTable.find(instanceId);

        if (tabIter == m_moduleTable.end())
        {
            LoadModuleInfo mod;
            memset(&mod, 0, sizeof(LoadModuleInfo));
            fnGetModuleInfoByInstanceId(instanceId, &mod);
            m_moduleTable.insert(ModuleInfoMap::value_type(instanceId, mod));
        }
    }

#ifdef MEMORY_TIME_TRACE
    ts = GetOsTimeStamp() - ts;
    modtab += ts;
#endif

#endif
    return AMDT_STATUS_OK;
}

// InsertSampleToSystemTree: Insert each sample to system tree with corresponding power
AMDTResult PowerProfileTranslate::InsertSampleToSystemTree(SampleData* pCtx, AMDTFloat32 power, AMDTUInt32 sampleCnt)
{
    AMDTResult ret = AMDT_STATUS_OK;
    IpAddressInfo ipSample;

    ProcessTreeMap::iterator processIter = m_systemTreeMap.find(pCtx->m_processId);

    if (processIter == m_systemTreeMap.end())
    {
        ipSample.m_sampleCnt = sampleCnt;
        ipSample.m_power = power;

        //Create ip address map
        ModuleIntanceInfo modInfo;

        modInfo.m_ipMap.insert(IpAddressMap::value_type(pCtx->m_ip, ipSample));
        modInfo.m_power = power;
        modInfo.m_sampleCnt = sampleCnt;
        //Create ModuleInstanceMap
        ProcessInfo processInfo;
        processInfo.m_power = power;
        processInfo.m_sampleCnt = sampleCnt;
        processInfo.m_modInstMap.insert(ModuleInstanceMap::value_type(pCtx->m_modInstance, modInfo));
        //Create m_systemTreeMap
        m_systemTreeMap.insert(ProcessTreeMap::value_type(pCtx->m_processId, processInfo));
    }
    else
    {
        ModuleInstanceMap:: iterator moduleIter = processIter->second.m_modInstMap.find(pCtx->m_modInstance);

        if (moduleIter == processIter->second.m_modInstMap.end())
        {
            ModuleIntanceInfo instInfo;
            ipSample.m_sampleCnt = sampleCnt;
            ipSample.m_power = power;
            instInfo.m_power = power;
            instInfo.m_sampleCnt = sampleCnt;
            instInfo.m_ipMap.insert(IpAddressMap::value_type(pCtx->m_modInstance, ipSample));
            //Create
            processIter->second.m_modInstMap.insert(ModuleInstanceMap::value_type(pCtx->m_modInstance, instInfo));
            processIter->second.m_power += power;
        }
        else
        {
            IpAddressMap::iterator ipIter = moduleIter->second.m_ipMap.find(pCtx->m_ip);

            if (ipIter == moduleIter->second.m_ipMap.end())
            {
                ipSample.m_sampleCnt = sampleCnt;
                ipSample.m_power = power;
                moduleIter->second.m_ipMap.insert(IpAddressMap::value_type(pCtx->m_ip, ipSample));
                moduleIter->second.m_power += power;
                moduleIter->second.m_sampleCnt += sampleCnt;
                processIter->second.m_power += power;
                processIter->second.m_sampleCnt += sampleCnt;
            }
            else
            {
                ipIter->second.m_power += power;
                ipIter->second.m_sampleCnt += sampleCnt;
                moduleIter->second.m_power += power;
                moduleIter->second.m_sampleCnt += sampleCnt;
                processIter->second.m_power += power;
                ipIter->second.m_sampleCnt += sampleCnt;
                processIter->second.m_sampleCnt++;
            }
        }
    }

    return ret;
}

//AttributePowerToSample: Calculate the power for each sample and insert in the system tree
AMDTResult PowerProfileTranslate::AttributePowerToSample(AMDTFloat32* cuPower)
{
    AMDTUInt32 coreIdx = 0;
    AMDTFloat32 timeDiv = 0;
    AMDTFloat32 totalCuLoad[MAX_CU_CNT] = {0, 0};
    AMDTFloat32 power = 0;
    timeDiv = (m_currentTs > m_prevTs1) ? ((m_currentTs - m_prevTs1) / g_countPerSeconds) : m_samplingPeriod;

    m_prevTs1 = m_currentTs;

    for (coreIdx = 0; coreIdx < m_targetCoreCnt; coreIdx++)
    {
        totalCuLoad[coreIdx / m_coresPerCu] += m_sampleIpcLoad[coreIdx];
    }

    for (coreIdx = 0; coreIdx < m_targetCoreCnt; coreIdx++)
    {
        AMDTUInt32 cu = coreIdx / m_coresPerCu;

        for (auto sampleIter : m_sampleMap[coreIdx])
        {
            power = (sampleIter.second.m_ipc / totalCuLoad[cu]) * (cuPower[cu] * timeDiv);
            InsertSampleToSystemTree(&sampleIter.second, power, sampleIter.second.m_sampleCnt);
        }

        m_sampleMap[coreIdx].clear();
        m_sampleIpcLoad[coreIdx] = 0;
    }

    return AMDT_STATUS_OK;
}

PwrCoreConfig* PowerProfileTranslate::PwrGetCoreConfig(AMDTUInt16 sampleId)
{
    AMDTUInt32 cnt = 0;

    for (cnt = 0; cnt < m_coreConfigs.size(); cnt++)
    {
        if (sampleId == m_coreConfigs[cnt].m_sampleId)
        {
            return &m_coreConfigs[cnt];
        }
    }

    return (PwrCoreConfig*)nullptr;
}

// TranslateRawData: Read the shared buffer and decode the raw data
AMDTResult PowerProfileTranslate::TranslateRawData()
{
    AMDTInt32 ret = AMDT_STATUS_OK;
    AMDTUInt64 coreMask = 0;
    AMDTUInt32 coreCnt = 0;
    bool dataAvailable = true;
    AMDTUInt32 cnt = 0;
    AMDTUInt64 tempCoreMask = 0;

    if (false == m_isRunning)
    {
        return 0;
    }

    //Get Core mask configured
    ret = GetConfigCoreMask(&coreMask);

    if (AMDT_STATUS_OK == ret)
    {
        coreCnt = GetMaskCount(coreMask);

        if (0 == coreCnt)
        {
            PwrTrace("no configured core");
            ret = AMDT_ERROR_FAIL;
        }
    }

    if (AMDT_STATUS_OK == ret)
    {
        m_data.m_attrCnt = 0;
        m_data.m_recId = 0;
        m_data.m_ts = 0;
        m_data.m_counters.clear();
        ret = ReadSharedBuffer(m_pSahredBuffer, coreMask);
    }

    if (AMDT_STATUS_OK == ret)
    {
        while (dataAvailable & m_isRunning)
        {
            if (AMDT_STATUS_OK != ret)
            {
                break;
            }

            tempCoreMask =  coreMask;

            for (cnt = 0; cnt < coreCnt; cnt++)
            {
                AMDTUInt16 sampleId = 0;
                AMDTUInt64 recId = 0;
                SharedBuffer* pCoreBuffer = nullptr;
                RawRecordHdr hdr;

                AMDTUInt8* pRaw = nullptr;
                AMDTUInt32 offset = 0;
                AMDTUInt32 coreId = 0;
                AMDTUInt64 tsRaw = 0;

                if (true == m_isRunning)
                {
                    GetFirstSetBitIndex(&coreId, tempCoreMask);
                    tempCoreMask &= ~(1 << coreId);
                    pCoreBuffer = m_pSahredBuffer + cnt;
                    offset = pCoreBuffer->m_processedOffset;
                    pRaw = (AMDTUInt8*)(pCoreBuffer->m_pBuffer);
                    memcpy(&hdr, (RawRecordHdr*)(pRaw + offset), sizeof(RawRecordHdr));
                    offset += sizeof(RawRecordHdr);
                }
                else
                {
                    ret = AMDT_ERROR_FAIL;
                    break;
                }

                if (REC_TYPE_SAMPLE_DATA == hdr.m_recordType)
                {
                    PwrCoreConfig* pCoreCfg = nullptr;
                    // Read basic information
                    sampleId = *(AMDTUInt16*)(pRaw + offset);
                    offset += sizeof(AMDTUInt16);
                    recId = *(AMDTUInt64*)(pRaw + offset);
                    offset += sizeof(AMDTUInt64);
                    tsRaw = *(AMDTUInt64*)(pRaw + offset);
                    offset += sizeof(AMDTUInt64);

                    // Fill basic information only for master buffer
                    if (0 == cnt)
                    {
                        m_data.m_recId = recId / m_recIdFactor;
                        m_data.m_recordType = (AMDTPwrProcessedDataType)hdr.m_recordType;
                        m_prevTs = m_currentTs;
                        SetElapsedTime(tsRaw, &m_currentTs);
                        m_data.m_ts = m_currentTs;
                    }

                    pCoreCfg = PwrGetCoreConfig(sampleId);
                    AMDTUInt32 idx = 0;

                    // Decode the counters as per sample id
                    for (idx = 0; idx < pCoreCfg->m_counters.size(); idx++)
                    {
                        AMDTUInt32 len = 0;
                        PwrCounterDecodeInfo* pDecodeInfo = &pCoreCfg->m_counters[idx];

                        if (0 == pDecodeInfo->m_pkgId)
                        {
                            len = DecodeNodeCounters(pDecodeInfo, &m_data, pRaw + offset);
                        }
                        else if (APU_SMU_ID == pDecodeInfo->m_pkgId)
                        {
                            if ((SMU_IPVERSION_7_0 == pDecodeInfo->m_smuIpVersion)
                                || (SMU_IPVERSION_7_1 == pDecodeInfo->m_smuIpVersion)
                                || (SMU_IPVERSION_7_2 == pDecodeInfo->m_smuIpVersion)
                                || (SMU_IPVERSION_7_5 == pDecodeInfo->m_smuIpVersion))
                            {
                                len = DecodeSmu7Counters(pDecodeInfo, &m_data, pRaw + offset, &idx);
                            }
                            else if ((SMU_IPVERSION_8_0 == pDecodeInfo->m_smuIpVersion)
                                     || (SMU_IPVERSION_8_1 == pDecodeInfo->m_smuIpVersion))
                            {
                                len = DecodeSmu8Counters(pDecodeInfo, &m_data, pRaw + offset, &idx);
                            }
                            else if (SMU_IPVERSION_9_0 == pDecodeInfo->m_smuIpVersion)
                            {
#if 0
                                len = PWR_SMU9_DECODE_COUNTER(pDecodeInfo,
                                                              &m_data,
                                                              pRaw + offset,
                                                              &idx,
                                                              &m_sysInfo,
                                                              ((m_currentTs - m_prevTs) / g_countPerSeconds));
#endif
                            }
                        }
                        else if (pDecodeInfo->m_pkgId > APU_SMU_ID)
                        {
                            if ((SMU_IPVERSION_7_0 == pDecodeInfo->m_smuIpVersion)
                                || (SMU_IPVERSION_7_1 == pDecodeInfo->m_smuIpVersion)
                                || (SMU_IPVERSION_7_2 == pDecodeInfo->m_smuIpVersion)
                                || (SMU_IPVERSION_7_5 == pDecodeInfo->m_smuIpVersion))
                            {
                                len = DecodeSmu7DgpuCounters(pDecodeInfo, &m_data, pRaw + offset);
                            }
                        }

                        offset += len;
                    }

                    m_data.m_attrCnt = (AMDTUInt32)m_data.m_counters.size();

                    if (cnt == (coreCnt - 1))
                    {
                        std::lock_guard<std::mutex> lock(m_lock);
                        m_processedData.push(m_data);

                        m_condition.notify_one();
                    }
                }
                else if (REC_TYPE_CONTEXT_DATA == hdr.m_recordType)
                {
                    AMDTFloat32 ipc = 0;
                    ContextData contextData;
                    AMDTUInt32 markerIdx = 0;
                    AMDTUInt32 cuId = coreId / 2;
                    memcpy(&recId, (AMDTUInt64*)&pRaw[offset], sizeof(AMDTUInt64));
                    offset += sizeof(AMDTUInt64);
                    memcpy(&contextData, (ContextData*)&pRaw[offset], sizeof(ContextData));
                    offset += sizeof(ContextData);
                    g_tracePids++;

                    if (MAX_PID_CNT < m_samplePower[cuId].m_numberOfPids)
                    {
                        ret = AMDT_ERROR_UNEXPECTED;
                        PwrTrace("MAX_PID_CNT reached");
                    }

#if !(defined(_WIN64) || defined(LINUX))
                    // ipc = retired micro ops/ cpu cycle not halted
                    ipc = (AMDTFloat32) contextData.m_pmcData[PMC_EVENT_RETIRED_MICRO_OPS]
                          / (AMDTFloat32) contextData.m_pmcData[PMC_EVENT_CPU_CYCLE_NOT_HALTED];
#ifdef MEMORY_TIME_TRACE
                    AMDTUInt64 ts = GetOsTimeStamp();
#endif
                    ProcessSample(&contextData, ipc, coreId);
#ifdef MEMORY_TIME_TRACE

                    ts = GetOsTimeStamp() - ts;
                    processContext += ts;
#endif
#else
                    AMDTUInt32 pidCnt = 0;
                    bool found = false;
                    //TODO: for Linux we are not using PMC as of now
                    ipc = 1;

                    for (pidCnt = 0; pidCnt < m_samplePower[cuId].m_numberOfPids; pidCnt++)
                    {
                        if (contextData.m_processId == m_samplePower[cuId].m_process[pidCnt].m_pid)
                        {
                            m_samplePower[cuId].m_process[pidCnt].m_sampleCnt++;
                            m_samplePower[cuId].m_process[pidCnt].m_ipc += ipc;
                            m_samplePower[cuId].m_totalIpc += ipc;
                            m_samplePower[cuId].m_sampleCnt++;
                            found = true;
                            break;
                        }
                    }

                    if (false == found)
                    {
                        m_samplePower[cuId].m_numberOfPids++;
                        m_samplePower[cuId].m_process[pidCnt].m_pid = contextData.m_processId;
                        m_samplePower[cuId].m_process[pidCnt].m_sampleCnt++;
                        m_samplePower[cuId].m_process[pidCnt].m_ipc += ipc;
                        m_samplePower[cuId].m_totalIpc += ipc;
                        m_samplePower[cuId].m_sampleCnt++;
                    }

#endif

                    for (markerIdx = 0; markerIdx < g_markerCnt; markerIdx++)
                    {
                        if ((g_pInstrumentedPowerData[markerIdx].m_data.m_pidInfo.m_pid == contextData.m_processId)
                            && ((PWR_MARKER_ENABLE == g_pInstrumentedPowerData[markerIdx].m_state)
                                || (PWR_MARKER_DISABLE_INITIATED == g_pInstrumentedPowerData[markerIdx].m_state)))
                        {
                            g_pInstrumentedPowerData[markerIdx].m_cuPidInst[cuId]++;
                            //PwrTrace("inst %d",g_pInstrumentedPowerData[markerIdx].m_cuPidInst[cuId]);
                        }
                    }


                }
                else
                {
                    PwrTrace("Why here");
                    offset = offset - sizeof(RawRecordHdr);
                }

                if ((g_markerCnt > 0) && (nullptr != pRaw) && (nullptr != pCoreBuffer))
                {
                    ProcessMarkerRecords(pRaw, pCoreBuffer->m_size, &offset);
                }

                // Update the process buffer offset
                pCoreBuffer->m_processedOffset = offset;

                if ((pCoreBuffer->m_processedOffset + hdr.m_recordLen) > pCoreBuffer->m_size)
                {
                    // This core buffer doesn't have enough raw data to decode next record
                    dataAvailable = false;
                }
            }
        }

        if (true == m_isRunning)
        {
            // update the shared buffer consumption info and exit
            UpdateSharedBufferInfo(m_pSahredBuffer, coreMask);
        }
    }

    return ret;
}

// GetVDDAPower: This function is specific to SMU7 and it depends of socket types used
AMDTFloat32 GetVDDAPower()
{
    AMDTFloat32 vdda = 0.0;
    return vdda;
}

// SetElapsedTime: Elapse time from the time when first record was collected
void PowerProfileTranslate::SetElapsedTime(AMDTUInt64 raw, AMDTUInt64* pResult)
{
#ifdef LINUX
    const unsigned int nsec_to_microSec = 1000;
    *pResult = static_cast<AMDTUInt64>(raw - m_perfCounter) / nsec_to_microSec;
#else
    *pResult = static_cast<AMDTUInt64>((AMDTUInt64)((raw - m_perfCounter) * 1000) / (AMDTUInt64)m_perfFreq);
#endif
}

// GetProcessName: Get process name
void PowerProfileTranslate:: GetProcessName(AMDTPwrProcessInfo* pInfo)
{
    bool found = false;
    bool refreshed = false;

    if (nullptr != pInfo)
    {
        memset(pInfo->m_name, 0, AMDT_PWR_EXE_NAME_LENGTH);
        memset(pInfo->m_name, 0, AMDT_PWR_EXE_PATH_LENGTH);

        do
        {
            if (g_processNames.size() > 0)
            {
                for (auto itr : g_processNames)
                {
                    if (itr.m_pid == pInfo->m_pid)
                    {
                        memcpy(pInfo->m_name, itr.m_name, strlen(itr.m_name));
                        memcpy(pInfo->m_path, itr.m_path, strlen(itr.m_path));
                        found = true;
                        break;
                    }
                }
            }

            if (found == false)
            {
                found = GetProcessNameFromPid(pInfo);

                if (false == found)
                {
                    if (false == refreshed)
                    {
                        // Prepare the list again
                        PrepareInitialProcessList(g_processNames);
                        refreshed = true;
                    }
                    else
                    {
                        memcpy(pInfo->m_name, ERROR_READING_PROCESS_NAME, strlen(ERROR_READING_PROCESS_NAME) + 1);
                        memcpy(pInfo->m_path, ERROR_READING_PROCESS_PATH, strlen(ERROR_READING_PROCESS_PATH) + 1);
                        break;
                    }
                }
            }
        }
        while (false == found);
    }
}

// CalculatePidPower: Calculate power indicators for each PID and agreegate the indicator values
void PowerProfileTranslate::CalculatePidPower(AMDTFloat32* cuPower, AMDTUInt32 cuCnt)
{
    AMDTUInt32 cnt = 0;
    AMDTUInt32 cnt1 = 0;
    AMDTUInt32 cuIdx = 0;
    AMDTUInt32 pidIdx = 0;
    PowerData* pSrc = nullptr;
    AMDTFloat32 energyPerCuSample = 0.0;
    AMDTUInt32 markerIdx = 0;
    AMDTUInt64 timeSpan = 0;
    AMDTFloat32 load = 0;

    for (cuIdx = 0; cuIdx < cuCnt; cuIdx++)
    {
        pSrc = &m_samplePower[cuIdx];

        if (MAX_PID_CNT < pSrc->m_numberOfPids)
        {
            memset(pSrc, 0, sizeof(PowerData) * 2);
            return;
        }

        timeSpan = (m_currentTs - m_prevTs);
        energyPerCuSample = (cuPower[cuIdx] > 0) ? (cuPower[cuIdx] * timeSpan / g_countPerSeconds) : static_cast<AMDTFloat32>(0.0);

        // Calculate power indicator for each PID
        for (pidIdx = 0; pidIdx < pSrc->m_numberOfPids; pidIdx++)
        {
            load = pSrc->m_process[pidIdx].m_ipc / pSrc->m_totalIpc;
            pSrc->m_process[pidIdx].m_power = energyPerCuSample * load;
        }

        for (markerIdx = 0; markerIdx < g_markerCnt; markerIdx++)
        {
            if ((PWR_MARKER_ENABLE == g_pInstrumentedPowerData[markerIdx].m_state)
                || (PWR_MARKER_DISABLE_INITIATED == g_pInstrumentedPowerData[markerIdx].m_state))
            {
                PwrInstrumentedPowerData* pData = &g_pInstrumentedPowerData[markerIdx];

                load = pData->m_ipc[cuIdx] / pData->m_totalIpc[cuIdx];

                pData->m_data.m_pidInfo.m_power += (energyPerCuSample * load);
                pData->m_data.m_pidInfo.m_sampleCnt += pData->m_cuPidInst[cuIdx];

                if (strlen(pData->m_data.m_pidInfo.m_name) == 0)
                {
                    GetProcessName(&pData->m_data.m_pidInfo);
                }

                if (cuCnt == (cuIdx + 1))
                {
                    memset(pData->m_cuPidInst, 0, MAX_CU_CNT * sizeof(AMDTInt32));
                    memset(pData->m_ipc, 0, MAX_CU_CNT * sizeof(AMDTFloat32));
                    memset(pData->m_totalIpc, 0, MAX_CU_CNT * sizeof(AMDTFloat32));

                    if (PWR_MARKER_DISABLE_INITIATED == pData->m_state)
                    {
                        pData->m_state = PWR_MARKER_DISABLE;
                    }
                }
            }
        }
    }

    for (cuIdx = 0; cuIdx < cuCnt; cuIdx++)
    {
        pSrc = &m_samplePower[cuIdx];

        // Agreegate the power with previous samples
        for (cnt = 0; cnt < pSrc->m_numberOfPids; cnt++)
        {
            bool found = false;

            for (cnt1 = 0; cnt1 < g_aggrPidPowerList.m_numberOfPids; cnt1++)
            {
                if (g_aggrPidPowerList.m_process[cnt1].m_pid == pSrc->m_process[cnt].m_pid)
                {
                    g_aggrPidPowerList.m_process[cnt1].m_power += pSrc->m_process[cnt].m_power;
                    g_aggrPidPowerList.m_process[cnt1].m_sampleCnt += pSrc->m_process[cnt].m_sampleCnt;
                    g_aggrPidPowerList.m_sampleCnt += pSrc->m_process[cnt].m_sampleCnt;
                    g_aggrPidPowerList.m_totalIpc += pSrc->m_totalIpc;
                    g_aggrPidPowerList.m_process[cnt1].m_ipc += pSrc->m_process[cnt].m_ipc;
                    found = true;
                    break;
                }
            }

            if (false == found)
            {
                // Add new entry to the list
                g_aggrPidPowerList.m_process[cnt1].m_pid = pSrc->m_process[cnt].m_pid;
                g_aggrPidPowerList.m_process[cnt1].m_power = pSrc->m_process[cnt].m_power;
                g_aggrPidPowerList.m_process[cnt1].m_sampleCnt += pSrc->m_process[cnt].m_sampleCnt;
                g_aggrPidPowerList.m_totalIpc += pSrc->m_totalIpc;
                g_aggrPidPowerList.m_process[cnt1].m_ipc = pSrc->m_process[cnt].m_ipc;
                g_aggrPidPowerList.m_numberOfPids ++;
                g_aggrPidPowerList.m_sampleCnt += pSrc->m_process[cnt].m_sampleCnt;

                // Fill process name
                GetProcessName(&g_aggrPidPowerList.m_process[cnt1]);
            }
        }
    }

    // Reset sample pid list
    memset(&m_samplePower, 0, sizeof(PowerData) * MAX_CU_CNT);
}

// SaveCurrentSampleData: Insert previous valid data. So that it can be used in case
// there is junk value in the next sample
bool PowerProfileTranslate::SaveCurrentSampleData(AMDTPwrAttributeInfo* pData)
{
    AMDTUInt32 cnt = 0;
    bool ret = false;

    for (cnt = 0; cnt < MAX_PREV_COUNTERS; cnt++)
    {
        if (((pData->m_instanceId == m_prevSmuSampleData[cnt].m_instanceId)
             && (pData->m_counterId == m_prevSmuSampleData[cnt].m_counterId)))
        {
            memcpy(&m_prevSmuSampleData[cnt], pData, sizeof(AMDTPwrAttributeInfo));
            ret = true;
            break;
        }
    }

    return ret;
}

// GetPrevSampleData: Read previous valid record values
AMDTFloat32 PowerProfileTranslate::GetPrevSampleData(AMDTUInt32 counterId, AMDTUInt32 inst)
{
    AMDTUInt32 cnt = 0;
    AMDTFloat32 res = 0;

    PwrTrace("Spike value received for counter %d ins %d", counterId, inst);

    for (cnt = 0; cnt < MAX_PREV_COUNTERS; cnt++)
    {
        AMDTPwrAttributeInfo* pPrev = &m_prevSmuSampleData[cnt];

        if ((pPrev->m_instanceId == inst)
            && (pPrev->m_counterId == counterId))
        {
            res = pPrev->m_float32;
            break;
        }
    }

    return res;
}


// ReadSharedBuffer: Read shared buffer between driver and backend
// and populate per core buffers for processing.
AMDTResult PowerProfileTranslate::ReadSharedBuffer(SharedBuffer* pBuffer, AMDTUInt64 coreMask)
{
    AMDTResult ret = AMDT_STATUS_OK;
    AMDTUInt32 sharedOffset = PWRPROF_SHARED_METADATA_SIZE;
    AMDTUInt64 tempCoreMask = coreMask;
    AMDTUInt32 coreId = 0;
    AMDTUInt32 idx = 0;

    if (AMDT_STATUS_OK == ret)
    {
        while (tempCoreMask)
        {
            GetFirstSetBitIndex(&coreId, tempCoreMask);
            tempCoreMask &= ~(1 << coreId);

            //Buffers are based on index not by core id
            SharedBuffer* pSBuffer = &pBuffer[idx];

            AMDTUInt32 bufferBase = sharedOffset + idx * (sizeof(PageBuffer) + PWRPROF_PERCORE_BUFFER_SIZE);
            AMDTUInt32 rawBufferbase = sharedOffset + (idx + 1) * sizeof(PageBuffer) + idx * PWRPROF_PERCORE_BUFFER_SIZE;
            PageBuffer* pCoreBuffer = (PageBuffer*)(g_pSharedBuffer + bufferBase);
            pSBuffer->m_size = 0;
            idx++;

            ATOMIC_SET(&pSBuffer->m_currentOffset, pCoreBuffer->m_currentOffset);
            ATOMIC_SET(&pSBuffer->m_consumedOffset, pCoreBuffer->m_consumedOffset);
            ATOMIC_SET(&pSBuffer->m_maxValidOffset, pCoreBuffer->m_maxValidOffset);

            if (pSBuffer->m_currentOffset > pSBuffer->m_consumedOffset)
            {
                AMDTUInt32 bufferSize = pSBuffer->m_currentOffset - pSBuffer->m_consumedOffset;
                pSBuffer->m_pBuffer = (AMDTUInt8*)&g_pSharedBuffer[rawBufferbase + pSBuffer->m_consumedOffset];
                pSBuffer->m_size = bufferSize;
                pSBuffer->m_processedOffset = 0;
            }
            else if ((pSBuffer->m_currentOffset < pSBuffer->m_consumedOffset)
                     && (pSBuffer->m_consumedOffset < pSBuffer->m_maxValidOffset))
            {
                AMDTUInt32 bufferSize = pSBuffer->m_maxValidOffset - pSBuffer->m_consumedOffset;
                pSBuffer->m_pBuffer = (AMDTUInt8*)&g_pSharedBuffer[rawBufferbase + pSBuffer->m_consumedOffset];
                pSBuffer->m_size = bufferSize;
                pSBuffer->m_processedOffset = 0;
            }
            else if ((pSBuffer->m_currentOffset < pSBuffer->m_consumedOffset)
                     && (pSBuffer->m_consumedOffset == pSBuffer->m_maxValidOffset))
            {
                AMDTUInt32 bufferSize = 0;
                // Processed till max length, start from 0 again
                ATOMIC_SET(&pCoreBuffer->m_maxValidOffset, 0);
                ATOMIC_SET(&pCoreBuffer->m_consumedOffset, 0);
                pSBuffer->m_consumedOffset = 0;
                pCoreBuffer->m_consumedOffset = 0;
                ATOMIC_SET(&pSBuffer->m_maxValidOffset, pCoreBuffer->m_maxValidOffset);
                bufferSize = pSBuffer->m_currentOffset - 0;
                pSBuffer->m_pBuffer = (AMDTUInt8*)&g_pSharedBuffer[rawBufferbase + 0];
                pSBuffer->m_size = bufferSize;
                pSBuffer->m_processedOffset = 0;

            }
            else
            {
                //PwrTrace("else: core %d cons %d curr %d max %d",cnt,  pSBuffer->m_consumedOffset, pSBuffer->m_currentOffset, pSBuffer->m_maxValidOffset);
            }

            if (0 == pSBuffer->m_size)
            {
                ret = AMDT_ERROR_NODATA;
                //PwrTrace("Shared buffer length = 0, core %d ", cnt);
                break;
            }
        }
    }

    return ret;
}

// UpdateSharedBufferInfo: update the buffer meta data once processing is done
AMDTResult PowerProfileTranslate::UpdateSharedBufferInfo(SharedBuffer* pBuffer, AMDTUInt64 coreMask)
{
    AMDTResult ret = AMDT_STATUS_OK;
    AMDTUInt32 sharedOffset = PWRPROF_SHARED_METADATA_SIZE;
    AMDTUInt64 tempCoreMask = coreMask;
    AMDTUInt32 coreId = 0;
    AMDTUInt32 idx = 0;

    while (tempCoreMask)
    {
        GetFirstSetBitIndex(&coreId, tempCoreMask);
        tempCoreMask &= ~(1 << coreId);
        AMDTUInt32 offset = 0;
        SharedBuffer* localBuffer = pBuffer + idx;

        AMDTUInt32 bufferBase = sharedOffset + idx * (sizeof(PageBuffer) + PWRPROF_PERCORE_BUFFER_SIZE);
        volatile PageBuffer* pCoreBuffer = (PageBuffer*)(g_pSharedBuffer + bufferBase);

        offset = localBuffer->m_processedOffset + localBuffer->m_consumedOffset;

        if (offset == localBuffer->m_maxValidOffset)
        {
            ATOMIC_SET(&pCoreBuffer->m_maxValidOffset, 0);
            ATOMIC_SET(&pCoreBuffer->m_consumedOffset, 0);
        }
        else if ((offset <= PWRPROF_PERCORE_BUFFER_SIZE))
        {
            ATOMIC_SET(&pCoreBuffer->m_consumedOffset, offset);
        }
        else
        {

            ret = AMDT_ERROR_INTERNAL;
            PwrTrace("Wrong buffer size offset %d, size %d m_processedOffset %d, consumed %d max %d", offset,
                     localBuffer->m_size,
                     localBuffer->m_processedOffset,
                     localBuffer->m_consumedOffset,
                     localBuffer->m_maxValidOffset);
            break;
        }

        idx++;
    }

    return ret;
}

// GetInstrumentedData: Get the intrumented power for the selected marker
AMDTResult PowerProfileTranslate::GetInstrumentedData(AMDTUInt32 markerId, PwrInstrumentedPowerData** ppData)
{
    AMDTResult ret = AMDT_STATUS_OK;
    AMDTUInt32 idx = 0;
    bool found = false;

    if (nullptr != ppData)
    {
        ret = AMDT_ERROR_INVALIDARG;
    }

    for (idx = 0; idx < g_markerCnt; idx++)
    {
        if (g_pInstrumentedPowerData[idx].m_markerId == markerId)
        {
            *ppData = &g_pInstrumentedPowerData[idx];
            found = true;
            break;
        }
    }

    if (false == found)
    {
        ret = AMDT_ERROR_MARKER_NOT_SET;
    }

    return ret;
}

// ExtractNameAndPath: Extract file name and path from the full wchar_t path
void PowerProfileTranslate::ExtractNameAndPath(char* pFullPath, char* pName, char* pPath)
{
    AMDTUInt32 cnt = 0;
    AMDTUInt32 begin = 0;
    char* path = pFullPath;
    bool prefix = false;

    while (path[cnt] != '\0')
    {
        if ('\\' == path[cnt])
        {
            begin = cnt;
            prefix = true;
        }

        cnt++;
    }

    if (prefix)
    {
        begin += 1;
    }

    memset(pName, 0, AMDT_PWR_EXE_NAME_LENGTH);
    memset(pPath, 0, AMDT_PWR_EXE_PATH_LENGTH);
    memcpy(pName, &path[begin], strlen(&path[begin]) + 1);
    memcpy(pPath, &path[0], begin);

    if (strlen(pName) == 0)
    {
        memcpy(pName, "unknown name", AMDT_PWR_EXE_NAME_LENGTH);
    }

    if (strlen(pPath) == 0)
    {
        memcpy(pPath, "unknown path", AMDT_PWR_EXE_PATH_LENGTH);
    }
}

// PwrGetProfileData: Provide process data based on process/module/ip profile type
AMDTResult PowerProfileTranslate::PwrGetProfileData(CXLContextProfileType type, void** pData, AMDTUInt32* pCnt, AMDTFloat32* pPower)
{
    AMDTResult ret = AMDT_STATUS_OK;
    wchar_t str[OS_MAX_PATH];

    memset(str, 0, sizeof(wchar_t) * OS_MAX_PATH);
#if (defined(_WIN64) || defined(LINUX))

    (void)type;
    (void)pData;
    (void)pCnt;
    (void)pPower;

#else

    AMDTFloat32 totalPower = 0;

#ifdef MEMORY_TIME_TRACE
    PrintMemoryUsage("before aggregation ...");
#endif

    if (PROCESS_PROFILE == type)
    {
        AMDTPwrProcessInfo process;
        memset(&process, 0, sizeof(AMDTPwrProcessInfo));
        m_processList.clear();

        for (auto processIter : m_systemTreeMap)
        {
            memset(&process, 0, sizeof(AMDTPwrProcessInfo));
            process.m_pid = processIter.first;

            if (S_OK == fnFindProcessName(process.m_pid, str, AMDT_PWR_EXE_PATH_LENGTH))
            {
                char path[OS_MAX_PATH];
                wcstombs(path, str, OS_MAX_PATH);
                ExtractNameAndPath(path, process.m_name, process.m_path);
            }

            process.m_sampleCnt = processIter.second.m_sampleCnt;
            process.m_power = processIter.second.m_power;
            totalPower += process.m_power;
            m_processList.push_back(process);
        }

        if (m_processList.size() > 0)
        {
            *pPower = totalPower;
            std::sort(m_processList.begin(), m_processList.end(),
            [](AMDTPwrProcessInfo const & a, AMDTPwrProcessInfo const & b) { return a.m_power > b.m_power; });
            *pData = &m_processList[0];
            *pCnt = static_cast<AMDTUInt32>(m_processList.size());
        }
    }


    if (MODULE_PROFILE == type)
    {
        AMDTPwrModuleData module;
        m_moduleList.clear();
        memset(&module, 0, sizeof(AMDTPwrModuleData));

        for (auto processIter : m_systemTreeMap)
        {
            for (auto moduleIter : processIter.second.m_modInstMap)
            {
                module.m_power = moduleIter.second.m_power;
                totalPower += module.m_power;

                ModuleInfoMap::iterator tabIter = m_moduleTable.find(moduleIter.first);

                if (tabIter != m_moduleTable.end())
                {
                    module.m_isKernel = tabIter->second.m_isKernel;
                    module.m_loadAddr = tabIter->second.m_moduleStartAddr;
                    module.m_size = tabIter->second.m_modulesize;
                    module.m_processId = (AMDTUInt32)processIter.first;
                    ExtractNameAndPath(tabIter->second.m_pModulename, module.m_moduleName, module.m_modulePath);
                }
                else
                {
                    //PwrTrace("Error finding module pid %d module inst %d", (AMDTUInt32)processIter.first, moduleIter.first);
                }

                module.m_sampleCnt = moduleIter.second.m_sampleCnt;

                // Get process name and path
                if (S_OK == fnFindProcessName(module.m_processId, str, AMDT_PWR_EXE_PATH_LENGTH))
                {
                    char path[OS_MAX_PATH];
                    wcstombs(path, str, OS_MAX_PATH);
                    ExtractNameAndPath(path, module.m_processName, module.m_processPath);
                }

                m_moduleList.push_back(module);
            }
        }

        if (m_moduleList.size() > 0)
        {
            *pPower = totalPower;
            std::sort(m_moduleList.begin(), m_moduleList.end(),
            [](AMDTPwrModuleData const & a, AMDTPwrModuleData const & b) { return a.m_power > b.m_power; });
            *pData = &m_moduleList[0];
            *pCnt = static_cast<AMDTUInt32>(m_moduleList.size());
        }
    }

    if (*pCnt == 0)
    {
        ret = AMDT_ERROR_NODATA;
    }

#ifdef MEMORY_TIME_TRACE
    PrintMemoryUsage("end of ...");
#endif
#endif

    return ret;
}

//DecodeRegisters -decode register values to meaningful data
AMDTUInt32 PowerProfileTranslate::DecodeSmu7Counters(PwrCounterDecodeInfo* pDecodeInfo, AMDTPwrProcessedDataRecord* pOut, AMDTUInt8* pRaw , AMDTUInt32* pIdx)
{

    AMDTUInt32 offset = 0;

    if (pDecodeInfo && pOut && pRaw)
    {
        AMDTFloat32 res = 0;
        AMDTUInt32 counterId = pDecodeInfo->m_basicId;
        AMDTPwrAttributeInfo counter;
        memset(&counter, 0 , sizeof(AMDTPwrAttributeInfo));

        switch (counterId)
        {
            case COUNTERID_SMU7_APU_TEMP_CU:
            case COUNTERID_SMU7_APU_TEMP_MEAS_CU:
            {
                AMDTUInt32 repeat = m_targetCuCnt;
                AMDTUInt32 cuId = 0;

                while (repeat--)
                {
                    AMDTUInt32 data = *(AMDTUInt32*)(pRaw + offset);
                    res = SMU7_PROCESS_TEPERATURE_DATA(data);

                    if (res > MAX_TEMPERATURE)
                    {
                        PwrTrace("Invalid war data %f", res);
                        res = GetPrevSampleData(counterId, cuId);
                    }

                    counter.m_float32 = res;
                    counter.m_counterId = pDecodeInfo->m_clientId;
                    counter.m_instanceId = cuId++;
                    SaveCurrentSampleData(&counter);
                    pOut->m_counters.insert(PwrDecodedCounterMap::value_type(counter.m_counterId, counter));
                    offset += sizeof(AMDTUInt32);

                    if (repeat)
                    {
                        pDecodeInfo++;
                        *pIdx = *pIdx + 1;
                    }
                }

                break;
            }

            case COUNTERID_SMU7_APU_PWR_CU:
            {
                AMDTUInt32 repeat = m_targetCuCnt;
                AMDTUInt32 cuId = 0;
                AMDTFloat32 cuPower[MAX_CU_CNT];

                while (repeat--)
                {
                    AMDTUInt32 data = *(AMDTUInt32*)(pRaw + offset);
                    res = SMU7_PROCESS_POWER_DATA(data);

                    if (res > MAX_POWER)
                    {
                        res = GetPrevSampleData(counterId, cuId);
                    }

                    cuPower[cuId] = res;
                    counter.m_float32 = res;
                    counter.m_counterId = pDecodeInfo->m_clientId;
                    counter.m_instanceId = cuId++;
                    SaveCurrentSampleData(&counter);
                    AddToCumulativeCounter(counter.m_counterId + 1, (res * (m_currentTs - m_prevTs) / g_countPerSeconds));
                    pOut->m_counters.insert(PwrDecodedCounterMap::value_type(counter.m_counterId, counter));
                    offset += sizeof(AMDTUInt32);

                    if (repeat)
                    {
                        pDecodeInfo++;
                        *pIdx = *pIdx + 1;
                    }
                }

                if (PROFILE_TYPE_PROCESS_PROFILING == m_profileType)
                {
#if !(defined(_WIN64) || defined(LINUX))
#ifdef MEMORY_TIME_TRACE
                    AMDTUInt64 ts = GetOsTimeStamp();
#endif
                    AttributePowerToSample(cuPower);
#ifdef MEMORY_TIME_TRACE
                    ts = GetOsTimeStamp() - ts;
                    calculatePower += ts;
#endif
#else
                    CalculatePidPower(cuPower, cuId);
#endif
                }

                break;
            }

            case COUNTERID_SMU7_APU_PWR_IGPU:
            case COUNTERID_SMU7_APU_PWR_PCIE:
            case COUNTERID_SMU7_APU_PWR_DDR:
            case COUNTERID_SMU7_APU_PWR_DISPLAY:
            case COUNTERID_SMU7_APU_PWR_PACKAGE:
            case COUNTERID_SMU7_APU_TEMP_IGPU:
            case COUNTERID_SMU7_APU_TEMP_MEAS_IGPU:
            case COUNTERID_SMU7_APU_FREQ_IGPU:
            {
                AMDTFloat32 maxVal = 0;
                AMDTUInt32 data = *(AMDTUInt32*)(pRaw + offset);

                if (CATEGORY_POWER == pDecodeInfo->m_category)
                {
                    res = SMU7_PROCESS_POWER_DATA(data);
                    maxVal = MAX_POWER;
                }
                else if (CATEGORY_TEMPERATURE == pDecodeInfo->m_category)
                {
                    res = SMU7_PROCESS_TEPERATURE_DATA(data);
                    maxVal = MAX_TEMPERATURE;
                }
                else if (CATEGORY_FREQUENCY == pDecodeInfo->m_category)
                {
                    res = SMU7_PROCESS_FREQUENCY_DATA(data);
                    maxVal = MAX_GPU_FREQUENCY;
                }
                else if (CATEGORY_VOLTAGE == pDecodeInfo->m_category)
                {
                }

                if (res > maxVal)
                {
                    res = GetPrevSampleData(counterId, 0);
                }

                counter.m_float32 = res;
                counter.m_counterId = pDecodeInfo->m_clientId;
                SaveCurrentSampleData(&counter);

                if (CATEGORY_POWER == pDecodeInfo->m_category)
                {
                    AddToCumulativeCounter(counter.m_counterId + 1, (res * (m_currentTs - m_prevTs) / g_countPerSeconds));
                }
                else if (CATEGORY_FREQUENCY == pDecodeInfo->m_category)
                {
                    AddToHistogram(counter.m_counterId + 1, res);
                }

                pOut->m_counters.insert(PwrDecodedCounterMap::value_type(counter.m_counterId, counter));
                offset += sizeof(AMDTUInt32);
                break;
            }

            default:
                offset = 0;
                break;
        }

    }

    return offset;
}

//DecodeRegisters -decode register values to meaningful data
AMDTUInt32 PowerProfileTranslate::DecodeSmu8Counters(PwrCounterDecodeInfo* pDecodeInfo, AMDTPwrProcessedDataRecord* pOut, AMDTUInt8* pRaw, AMDTUInt32* pIdx)
{
    AMDTUInt32 offset = 0;

    if (pDecodeInfo && pOut && pRaw)
    {
        AMDTFloat32 res = 0;
        AMDTUInt32 counterId = pDecodeInfo->m_basicId;
        AMDTPwrAttributeInfo counter;
        memset(&counter, 0 , sizeof(AMDTPwrAttributeInfo));

        switch (counterId)
        {
            case COUNTERID_SMU8_APU_TEMP_CU:
            case COUNTERID_SMU8_APU_C0STATE_RES:
            case COUNTERID_SMU8_APU_C1STATE_RES:
            case COUNTERID_SMU8_APU_CC6_RES:
            {
                AMDTUInt32 repeat = m_targetCuCnt;
                AMDTUInt32 cuId = 0;

                while (repeat--)
                {
                    AMDTUInt32 data = *(AMDTUInt32*)(pRaw + offset);
                    SMU8_PROCESS_RAWDATA(res, data);
                    counter.m_float32 = res;
                    counter.m_counterId = pDecodeInfo->m_clientId;
                    counter.m_instanceId = cuId++;
                    pOut->m_counters.insert(PwrDecodedCounterMap::value_type(counter.m_counterId, counter));
                    offset += sizeof(AMDTUInt32);

                    if (repeat)
                    {
                        pDecodeInfo++;
                        *pIdx = *pIdx + 1;
                    }
                }

                break;
            }

            case COUNTERID_SMU8_APU_PWR_CU:
            {
                AMDTUInt32 repeat = m_targetCuCnt;
                AMDTUInt32 cuId = 0;
                AMDTFloat32 cuPower[MAX_CU_CNT];

                while (repeat--)
                {
                    AMDTUInt32 data = *(AMDTUInt32*)(pRaw + offset);

                    SMU8_PROCESS_RAWDATA(res, data);

                    if (res > MAX_POWER)
                    {
                        res = GetPrevSampleData(counterId, cuId);
                    }

                    cuPower[cuId] = res;
                    counter.m_float32 = res;
                    counter.m_counterId = pDecodeInfo->m_clientId;
                    counter.m_instanceId = cuId++;
                    SaveCurrentSampleData(&counter);
                    AddToCumulativeCounter(counter.m_counterId + 1, (res * (m_currentTs - m_prevTs) / g_countPerSeconds));
                    pOut->m_counters.insert(PwrDecodedCounterMap::value_type(counter.m_counterId, counter));
                    offset += sizeof(AMDTUInt32);

                    if (repeat)
                    {
                        pDecodeInfo++;
                        *pIdx = *pIdx + 1;
                    }
                }

                if (PROFILE_TYPE_PROCESS_PROFILING == m_profileType)
                {
#if !(defined(_WIN64) || defined(LINUX))
#ifdef MEMORY_TIME_TRACE
                    AMDTUInt64 ts = GetOsTimeStamp();
#endif
                    AttributePowerToSample(cuPower);
#ifdef MEMORY_TIME_TRACE
                    ts = GetOsTimeStamp() - ts;
                    calculatePower += ts;
#endif
#else
                    CalculatePidPower(cuPower, cuId);
#endif
                }

                break;
            }

            case COUNTERID_SMU8_APU_PWR_VDDGFX:
            case COUNTERID_SMU8_APU_PWR_APU:
            case COUNTERID_SMU8_APU_PWR_VDDIO:
            case COUNTERID_SMU8_APU_PWR_VDDNB:
            case COUNTERID_SMU8_APU_PWR_VDDP:
            case COUNTERID_SMU8_APU_PWR_UVD:
            case COUNTERID_SMU8_APU_PWR_VCE:
            case COUNTERID_SMU8_APU_PWR_ACP:
            case COUNTERID_SMU8_APU_PWR_UNB:
            case COUNTERID_SMU8_APU_PWR_SMU:
            case COUNTERID_SMU8_APU_PWR_ROC:
            case COUNTERID_SMU8_APU_TEMP_VDDGFX:
            case COUNTERID_SMU8_APU_FREQ_ACLK:
            case COUNTERID_SMU8_APU_FREQ_IGPU:
            {
                AMDTUInt32 data = *(AMDTUInt32*)(pRaw + offset);
                SMU8_PROCESS_RAWDATA(res, data);
                counter.m_float32 = res;
                counter.m_counterId = pDecodeInfo->m_clientId;

                if (CATEGORY_POWER == pDecodeInfo->m_category)
                {
                    AddToCumulativeCounter(counter.m_counterId + 1, (res * (m_currentTs - m_prevTs) / g_countPerSeconds));
                }
                else if (CATEGORY_FREQUENCY == pDecodeInfo->m_category)
                {
                    AddToHistogram(counter.m_counterId + 1, res);
                }

                pOut->m_counters.insert(PwrDecodedCounterMap::value_type(counter.m_counterId, counter));
                offset += sizeof(AMDTUInt32);
                break;
            }

            default:
                offset = 0;
                break;
        }
    }

    return offset;
}

//DecodeRegisters -decode register values to meaningful data
AMDTUInt32 PowerProfileTranslate::DecodeNodeCounters(PwrCounterDecodeInfo* pDecodeInfo, AMDTPwrProcessedDataRecord* pOut, AMDTUInt8* pRaw)
{
    AMDTUInt32 offset = 0;

    if (pDecodeInfo && pOut && pRaw)
    {
        AMDTUInt32 counterId = pDecodeInfo->m_basicId;
        AMDTPwrAttributeInfo counter;
        memset(&counter, 0 , sizeof(AMDTPwrAttributeInfo));

        switch (counterId)
        {
            case COUNTERID_PID:
            case COUNTERID_TID:
            {
                AMDTUInt64 data = *(AMDTUInt64*)(pRaw + offset);
                counter.m_value64 = data;
                counter.m_instanceId = pDecodeInfo->m_instanceId;
                counter.m_counterId = pDecodeInfo->m_clientId;
                offset += sizeof(AMDTUInt64);
                pOut->m_counters.insert(PwrDecodedCounterMap::value_type(counter.m_counterId, counter));
                break;
            }

            case COUNTERID_PSTATE:
            case COUNTERID_CSTATE_RES:
            {
                AMDTUInt32 data = *(AMDTUInt32*)(pRaw + offset);
                counter.m_value64 = (AMDTUInt64)data;
                counter.m_instanceId = pDecodeInfo->m_instanceId;
                counter.m_counterId = pDecodeInfo->m_clientId;
                offset += sizeof(AMDTUInt32);
                pOut->m_counters.insert(PwrDecodedCounterMap::value_type(counter.m_counterId, counter));
                break;
            }

            case COUNTERID_CEF:
            {
                AMDTUInt64 aPerf = 0;
                AMDTUInt64 mPerf = 0;
                AMDTUInt64 pState = 0;
                AMDTFloat64 pStateFreq = 0;
                AMDTFloat32 cef = 0;

                aPerf = *(AMDTUInt64*)(pRaw + offset);
                offset += sizeof(AMDTUInt64);

                mPerf = *(AMDTUInt64*)(pRaw + offset);
                offset += sizeof(AMDTUInt64);

                pState = *(AMDTUInt64*)(pRaw + offset);
                offset += sizeof(AMDTUInt64);
                pStateFreq = (AMDTFloat64)(100.0 * (AMDTFloat64)((pState & AMDT_CPUFID_MASK) + 0x10) /
                                           (AMDTFloat64)(0x1 << ((pState & AMDT_CPUDID_MASK) >> AMDT_CPUDID_BITSHIFT))); // in MHz

                AMDTPwrApuPstateList* pStates = GetApuPStateInfo();
                AMDTFloat64 maxRatio = (AMDTFloat64)pStates->m_stateInfo[0].m_frequency / pStateFreq ;

                if (((AMDTFloat64)aPerf) > ((AMDTFloat64)(maxRatio * mPerf)))
                {
                    aPerf = mPerf;
                }

                cef = (AMDTFloat32)(((AMDTFloat64)aPerf / (AMDTFloat64)mPerf) * pStateFreq);
                counter.m_float32 = cef;
                counter.m_instanceId = pDecodeInfo->m_instanceId;
                counter.m_counterId = pDecodeInfo->m_clientId;
                pOut->m_counters.insert(PwrDecodedCounterMap::value_type(counter.m_counterId, counter));
                AddToHistogram(counter.m_counterId + 1, cef);
                break;
            }

            case COUNTERID_NODE_TCTL_TEPERATURE:
            {
                AMDTFloat32 res = 0;
                AMDTUInt32 data = *(AMDTUInt32*)(pRaw + offset);
                DecodeTctlTemperature(data, &res);
                counter.m_float32 = res;
                counter.m_counterId = pDecodeInfo->m_clientId;
                counter.m_instanceId = 0;
                pOut->m_counters.insert(PwrDecodedCounterMap::value_type(counter.m_counterId, counter));
                offset += sizeof(AMDTUInt32);
                break;
            }

            case COUNTERID_SVI2_CORE_TELEMETRY:
            case COUNTERID_SVI2_NB_TELEMETRY:
            {
                AMDTUInt32 reg = 0;
                AMDTFloat32 v = 0;
                AMDTFloat32 c = 0;
                AMDTUInt32 idx = 0;
                //Reference Spike current is considered as 120A
                AMDTFloat32 IddSpikeOCP = 120.0;

                //Voltage
                reg = *(AMDTUInt32*)((AMDTUInt8*)pRaw + offset);
                v = (AMDTFloat32)((reg & 0x1ff0000) >> 16);
                v = (AMDTFloat32)3.15 - v * (AMDTFloat32)0.00625;

                counter.m_float32 = v;
                counter.m_counterId = pDecodeInfo->m_clientId;
                counter.m_instanceId = idx++;
                pOut->m_counters.insert(PwrDecodedCounterMap::value_type(counter.m_counterId, counter));

                //Current
                c = (AMDTFloat32)(reg & 0xff);
                c = (IddSpikeOCP * c) / (AMDTFloat32)255.0;
                counter.m_float32 = c;
                counter.m_counterId = pDecodeInfo->m_clientId;
                counter.m_instanceId = idx;
                pOut->m_counters.insert(PwrDecodedCounterMap::value_type(counter.m_counterId, counter));
                offset += sizeof(AMDTUInt32);
                break;
            }

            case COUNTERID_FAMILY17_PKG_ENERGY:
            case COUNTERID_FAMILY17_CORE_ENERGY:
            {
#if 0
                AMDTUInt32 data = *(AMDTUInt32*)(pRaw + offset);
                PWR_FAMILY17_DECODE_MSR_COUNTER(pDecodeInfo, data, &counter.m_float32);
                counter.m_instanceId = pDecodeInfo->m_instanceId;
                counter.m_counterId = pDecodeInfo->m_clientId;
                offset += sizeof(AMDTUInt32);
                pOut->m_counters.insert(PwrDecodedCounterMap::value_type(counter.m_counterId, counter));
#endif
                break;
            }

            default:
                offset = 0;
                break;
        }
    }

    return offset;
}


//DecodeRegisters -decode register values to meaningful data
AMDTUInt32 PowerProfileTranslate::DecodeSmu7DgpuCounters(PwrCounterDecodeInfo* pDecodeInfo, AMDTPwrProcessedDataRecord* pOut, AMDTUInt8* pRaw)
{
    AMDTUInt32 offset = 0;

    if (pDecodeInfo && pOut && pRaw)
    {
        AMDTUInt32 counterId = pDecodeInfo->m_basicId;
        AMDTPwrAttributeInfo counter;
        memset(&counter, 0 , sizeof(AMDTPwrAttributeInfo));

        switch (counterId)
        {
            case COUNTERID_PKG_PWR_DGPU:
            case COUNTERID_TEMP_MEAS_DGPU:
            case COUNTERID_FREQ_DGPU:
            case COUNTERID_VOLT_VDDC_LOAD_DGPU:
            case COUNTERID_CURR_VDDC_DGPU:
            {
                AMDTFloat32 res = 0;
                AMDTUInt32 limit = 0;
                AMDTUInt32 data = *(AMDTUInt32*)(pRaw + offset);
                DECODE_SMU7_RAW_DATA(data, res, pDecodeInfo->m_category);
                limit = (COUNTERID_FREQ_DGPU == counterId) ? MAX_GPU_FREQUENCY : MAX_POWER;
                counter.m_counterId = pDecodeInfo->m_clientId;

                if (res > limit)
                {
                    res = GetPrevSampleData(counterId, 0);
                }

                if (CATEGORY_POWER == pDecodeInfo->m_category)
                {
                    AddToCumulativeCounter(counter.m_counterId + 1, (res * (m_currentTs - m_prevTs) / g_countPerSeconds));
                }
                else if (CATEGORY_FREQUENCY == pDecodeInfo->m_category)
                {
                    AddToHistogram(counter.m_counterId + 1, res);
                }

                counter.m_float32 = res;
                counter.m_instanceId = pDecodeInfo->m_instanceId;
                counter.m_counterId = pDecodeInfo->m_clientId;
                pOut->m_counters.insert(PwrDecodedCounterMap::value_type(counter.m_counterId, counter));
                SaveCurrentSampleData(&counter);
                offset += sizeof(AMDTUInt32);
                break;
            }

            default:
                offset = 0;
                break;
        }
    }

    return offset;
}
