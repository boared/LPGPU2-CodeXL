//==============================================================================
// Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief This file contains the CL event handling code.
//==============================================================================

#include <iostream>
#include "../Common/OSUtils.h"
#include "../Common/Defs.h"
#include "../Common/Logger.h"
#include "CLStringUtils.h"
#include "CLEventHandler.h"
#include "CLEventManager.h"
#include "CLAPIInfoManager.h"
#include "CLTraceAgent.h"

using namespace std;
using namespace GPULogger;

#ifdef CL_UNITTEST_MOCK
    // Need to initialize static member with specified template type.
    OSUtilsMock* TSingleton<OSUtils>::obj;
#endif

static void CL_CALLBACK EventCreate(cl_agent* agent, cl_event event, cl_command_type type)
{
    SP_UNREFERENCED_PARAMETER(agent);
    SP_UNREFERENCED_PARAMETER(event);
    SP_UNREFERENCED_PARAMETER(type);
}

static void CL_CALLBACK EventFree(cl_agent* agent, cl_event event)
{
    SP_UNREFERENCED_PARAMETER(agent);
    SP_UNREFERENCED_PARAMETER(event);
}

// static void CL_CALLBACK EventStatusChanged(cl_agent* agent, cl_event event, cl_int status, cl_long ts)
void CL_CALLBACK EventStatusChanged(cl_event event, cl_int status, void* user_data)
{
    SP_UNREFERENCED_PARAMETER(user_data);
    cl_long ts = OSUtils::Instance()->GetTimeNanos();

    if (status == CL_QUEUED)
    {
        ULONGLONG ullCPUQueued = OSUtils::Instance()->GetTimeNanos();
        // Create event here
        CLEventPtr clEvent = CLEventManager::Instance()->AddEvent(event);
        // Update queued time
        clEvent->m_ullQueued = ts;
        // Initialize the other three times to be equal to the queued time -- there are instances
        // where we don't get valid values for the other three times:
        //   1) if the user kills the app before a queued event completes
        //   2) if the run time fails to notify us of the the other three times (this is the case
        //      with user events and clEnqueueTask -- we can get a queued notification, but not
        //      a submitted; we then get a running and complete notification with identical gpu times
        clEvent->m_ullComplete = ts;
        clEvent->m_ullRunning = ts;
        clEvent->m_ullSubmitted = ts;
        clEvent->m_ullCPUQueued = ullCPUQueued;

#ifdef NON_BLOCKING_TIMEOUT

        if (CLAPIInfoManager::Instance()->IsTimeOutMode())
        {
            CLEventManager::Instance()->AddEventRawInfo(event, status, ullCPUQueued);
        }

#endif

        return;
    }
    else
    {
        // Update existing CLEvent
        CLEventPtr clEvent = CLEventManager::Instance()->GetCLEvent(event);

        if (clEvent == NULL)
        {
            cl_command_type cmd_type;
            GetRealDispatchTable()->GetEventInfo(event, CL_EVENT_COMMAND_TYPE, sizeof(cl_command_type), &cmd_type, NULL);

            // it is perfectly valid for a user event to be set to CL_COMPLETE without it previously being set to CL_QUEUED.
            // Apparently the runtime can do the same with non-user events (I'm seeing this with a CL_COMMAND_NDRANGE_KERNEL
            // event that is not generated by the user app (no previous calls to clEnqueueNDRangeKernel or clEnqueueTask).
            if (cmd_type != CL_COMMAND_USER)
            {
                //SpAssert(clEvent != NULL);
                Log(logERROR, "[%s] cl event(%s at 0x%p) not managed by CLEventManager.\n", CLStringUtils::GetExecutionStatusString(status).c_str(), CLStringUtils::GetCommandTypeString(cmd_type).c_str(), event);
            }

            return;
        }

        switch (status)
        {
            case CL_COMPLETE:
                clEvent->m_ullComplete = ts;
                clEvent->m_bIsReady = true;
                clEvent->m_pEvent = NULL;

                // If no owner is set it's a blocking event, it will be removed in CLEventManager::UpdateEvent
                // This is needed so the event owner can be set, and the event in the owner updated.
                if (NULL != clEvent->m_pOwner)
                {
                    CLEventManager::Instance()->RemoveEvent(event);
                }

                break;

            case CL_RUNNING:
                clEvent->m_ullRunning = ts;
                break;

            case CL_SUBMITTED:
                clEvent->m_ullSubmitted = ts;
                break;
        }

#ifdef NON_BLOCKING_TIMEOUT

        if (CLAPIInfoManager::Instance()->IsTimeOutMode())
        {
            // always add unmapped timestamp to event raw info
            CLEventManager::Instance()->AddEventRawInfo(event, status, CLEvent::UnmapHelper(clEvent->m_ullCPUQueued, clEvent->m_ullQueued, ts));
        }

#endif
    }
}

// Create CLEvent Callback dispatch Table
// \param callback[inout] return callback dispatch table
void CreateCLEventCallbackDispatchTable(cl_agent_callbacks& callback)
{
    callback.EventCreate = EventCreate;
    callback.EventFree = EventFree;
    // callback.EventStatusChanged = EventStatusChanged;
}
