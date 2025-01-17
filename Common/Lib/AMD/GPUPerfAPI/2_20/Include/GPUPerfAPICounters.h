//==============================================================================
// Copyright (c) 2012-2016 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Implements a library that allows access to the available counters in GPUPerfAPI.
//==============================================================================

#ifndef _GPUPERFAPI_COUNTERS_H_
#define _GPUPERFAPI_COUNTERS_H_

#ifndef GPUPERFAPI_COUNTERS_DECL
    #ifdef _WIN32
        #ifdef __cplusplus
            #define GPUPERFAPI_COUNTERS_DECL extern "C" __declspec( dllimport )
        #else
            #define GPUPERFAPI_COUNTERS_DECL __declspec( dllimport )
        #endif
    #else //_LINUX
        #define GPUPERFAPI_COUNTERS_DECL extern
    #endif
#endif

/// Entrypoint to get the available counters
/// \param api the api whose available counters are requested
/// \param vendorId the vendor id of the device whose available counters are requested
/// \param deviceId the device id of the device whose available counters are requested
/// \param revisionId the revision id of the device whose available counters are requested
/// \param[out] ppCounterAccessorOut the accessor that will provide the counters
/// \param[out] ppCounterSchedulerOut the scheduler that will provide the counters
GPUPERFAPI_COUNTERS_DECL GPA_Status GPA_GetAvailableCounters(GPA_API_Type api, gpa_uint32 vendorId, gpa_uint32 deviceId, gpa_uint32 revisionId, GPA_ICounterAccessor** ppCounterAccessorOut, GPA_ICounterScheduler** ppCounterSchedulerOut);

/// Entrypoint to get the available counters by hardware generation
/// \param api the api whose available counters are requested
/// \param generation the hardware generation whose available counters are requested
/// \param[out] ppCounterAccessorOut the accessor that will provide the counters
GPUPERFAPI_COUNTERS_DECL GPA_Status GPA_GetAvailableCountersByGeneration(GPA_API_Type api, GPA_HW_GENERATION generation, GPA_ICounterAccessor** ppCounterAccessorOut);

#endif // _GPUPERFAPI_COUNTERS_H_
