//=============================================================================
/// Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file Wrapped_ID3D12CommandAllocator.cpp
/// \brief A class used to wrap D3D12's ID3D12CommandAllocator interface.
//=============================================================================

#include "Wrapped_ID3D12CommandAllocator.h"
#include "../DX12CreateInfoStructs.h"
#include "../DX12ObjectDatabaseProcessor.h"
#include "../../Interception/DX12Interceptor.h"
#include "../../DX12LayerManager.h"
#include "../../../Common/IUnknownWrapperGUID.h"

class Wrapped_ID3D12Device;

//-----------------------------------------------------------------------------
/// Utility function used to wrap the D3D12 interface.
/// \param inParentDevice The parent device for the interface.
/// \param inRealCommandAllocator The real runtime instance of the D3D12 interface.
/// \param inCreateInfo A structure containing the interface creation info.
/// \returns True if the interface was wrapped successfully.
//-----------------------------------------------------------------------------
bool WrapD3D12CommandAllocator(Wrapped_ID3D12Device* inParentDevice, ID3D12CommandAllocator** inRealCommandAllocator, Wrapped_ID3D12CommandAllocatorCreateInfo* inCreateInfo)
{
    return GenericWrapObject<ID3D12CommandAllocator, Wrapped_ID3D12CommandAllocator>(inParentDevice, inRealCommandAllocator, kObjectType_ID3D12CommandAllocator, inCreateInfo);
}

//-----------------------------------------------------------------------------
/// Wrapped_ID3D12CommandAllocator::QueryInterface
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE Wrapped_ID3D12CommandAllocator::QueryInterface(REFIID riid, void** ppvObject)
{
#if SERIALIZE_DX12_ENTRY_POINTS
    ScopeLock lock(&s_mutex);
#endif

    HRESULT result = {};

    if (riid == IID_IWrappedObject)
    {
        *ppvObject = mRealCommandAllocator;
        result = S_OK;
    }
    else
    {
        DX12Interceptor* interceptor = GetDX12LayerManager()->GetInterceptor();

        if (interceptor && interceptor->ShouldCollectTrace())
        {
            ParameterEntry parameters[] =
            {
                { PARAMETER_REFIID, &riid },
                { PARAMETER_POINTER, ppvObject },
            };

            int numParameters = (sizeof(parameters) / sizeof(parameters[0]));
            DX12APIEntry* pNewEntry = interceptor->PreCall(this, FuncId_IUnknown_QueryInterface, numParameters, parameters);
            result = mRealCommandAllocator->QueryInterface(riid, ppvObject);
            interceptor->PostCall(pNewEntry, result);
        }
        else
        {
            result = mRealCommandAllocator->QueryInterface(riid, ppvObject);
        }

        if (result == S_OK)
        {
            if (riid == __uuidof(ID3D12CommandAllocator))
            {
                WrapD3D12CommandAllocator(nullptr, (ID3D12CommandAllocator**)ppvObject);
            }
        }
    }

    return result;
}

//-----------------------------------------------------------------------------
/// Wrapped_ID3D12CommandAllocator::AddRef
//-----------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE Wrapped_ID3D12CommandAllocator::AddRef()
{
#if SERIALIZE_DX12_ENTRY_POINTS
    ScopeLock lock(&s_mutex);
#endif

    ULONG result = {};

    DX12Interceptor* interceptor = GetDX12LayerManager()->GetInterceptor();

    if (interceptor && interceptor->ShouldCollectTrace())
    {
        DX12APIEntry* pNewEntry = interceptor->PreCall(this, FuncId_IUnknown_AddRef, 0, nullptr);
        result = mRealCommandAllocator->AddRef();
        interceptor->PostCall(pNewEntry, result);
    }
    else
    {
        result = mRealCommandAllocator->AddRef();
    }

    return result;
}

//-----------------------------------------------------------------------------
/// Wrapped_ID3D12CommandAllocator::Release
//-----------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE Wrapped_ID3D12CommandAllocator::Release()
{
#if SERIALIZE_DX12_ENTRY_POINTS
    ScopeLock lock(&s_mutex);
#endif

    ULONG result = {};

    DX12Interceptor* interceptor = GetDX12LayerManager()->GetInterceptor();

    if (interceptor && interceptor->ShouldCollectTrace())
    {
        DX12APIEntry* pNewEntry = interceptor->PreCall(this, FuncId_IUnknown_Release, 0, nullptr);
        result = mRealCommandAllocator->Release();
        interceptor->PostCall(pNewEntry, result);
    }
    else
    {
        result = mRealCommandAllocator->Release();
    }

    if (result == 0)
    {
        DX12WrappedObjectDatabase* objectDatabase = (DX12WrappedObjectDatabase*)DX12ObjectDatabaseProcessor::Instance()->GetObjectDatabase();
        IDX12InstanceBase* objectMetadata = objectDatabase->GetMetadataObject(this);

        if (objectMetadata != nullptr)
        {
            objectMetadata->FlagAsDestroyed();
        }
    }

    return result;
}

//-----------------------------------------------------------------------------
/// Wrapped_ID3D12CommandAllocator::GetPrivateData
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE Wrapped_ID3D12CommandAllocator::GetPrivateData(REFGUID guid, UINT* pDataSize, void* pData)
{
#if SERIALIZE_DX12_ENTRY_POINTS
    ScopeLock lock(&s_mutex);
#endif

    HRESULT result = {};

    DX12Interceptor* interceptor = GetDX12LayerManager()->GetInterceptor();

    if (interceptor && interceptor->ShouldCollectTrace())
    {
        ParameterEntry parameters[] =
        {
            { PARAMETER_GUID, &guid },
            { PARAMETER_POINTER, pDataSize },
            { PARAMETER_POINTER, pData },
        };

        int numParameters = (sizeof(parameters) / sizeof(parameters[0]));
        DX12APIEntry* pNewEntry = interceptor->PreCall(this, FuncId_ID3D12Object_GetPrivateData, numParameters, parameters);
        result = mRealCommandAllocator->GetPrivateData(guid, pDataSize, pData);
        interceptor->PostCall(pNewEntry, result);
    }
    else
    {
        result = mRealCommandAllocator->GetPrivateData(guid, pDataSize, pData);
    }

    return result;
}

//-----------------------------------------------------------------------------
/// Wrapped_ID3D12CommandAllocator::SetPrivateData
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE Wrapped_ID3D12CommandAllocator::SetPrivateData(REFGUID guid, UINT DataSize, const void* pData)
{
#if SERIALIZE_DX12_ENTRY_POINTS
    ScopeLock lock(&s_mutex);
#endif

    HRESULT result = {};

    DX12Interceptor* interceptor = GetDX12LayerManager()->GetInterceptor();

    if (interceptor && interceptor->ShouldCollectTrace())
    {
        ParameterEntry parameters[] =
        {
            { PARAMETER_GUID, &guid },
            { PARAMETER_UNSIGNED_INT, &DataSize },
            { PARAMETER_POINTER, pData },
        };

        int numParameters = (sizeof(parameters) / sizeof(parameters[0]));
        DX12APIEntry* pNewEntry = interceptor->PreCall(this, FuncId_ID3D12Object_SetPrivateData, numParameters, parameters);
        result = mRealCommandAllocator->SetPrivateData(guid, DataSize, pData);
        interceptor->PostCall(pNewEntry, result);
    }
    else
    {
        result = mRealCommandAllocator->SetPrivateData(guid, DataSize, pData);
    }

    return result;
}

//-----------------------------------------------------------------------------
/// Wrapped_ID3D12CommandAllocator::SetPrivateDataInterface
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE Wrapped_ID3D12CommandAllocator::SetPrivateDataInterface(REFGUID guid, const IUnknown* pData)
{
#if SERIALIZE_DX12_ENTRY_POINTS
    ScopeLock lock(&s_mutex);
#endif

    HRESULT result = {};

    DX12Interceptor* interceptor = GetDX12LayerManager()->GetInterceptor();

    if (interceptor && interceptor->ShouldCollectTrace())
    {
        ParameterEntry parameters[] =
        {
            { PARAMETER_GUID, &guid },
            { PARAMETER_POINTER_SPECIAL, pData },
        };

        int numParameters = (sizeof(parameters) / sizeof(parameters[0]));
        DX12APIEntry* pNewEntry = interceptor->PreCall(this, FuncId_ID3D12Object_SetPrivateDataInterface, numParameters, parameters);
        result = mRealCommandAllocator->SetPrivateDataInterface(guid, pData);
        interceptor->PostCall(pNewEntry, result);
    }
    else
    {
        result = mRealCommandAllocator->SetPrivateDataInterface(guid, pData);
    }

    return result;
}

//-----------------------------------------------------------------------------
/// Wrapped_ID3D12CommandAllocator::SetName
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE Wrapped_ID3D12CommandAllocator::SetName(LPCWSTR Name)
{
#if SERIALIZE_DX12_ENTRY_POINTS
    ScopeLock lock(&s_mutex);
#endif

    HRESULT result = {};

    DX12Interceptor* interceptor = GetDX12LayerManager()->GetInterceptor();

    if (interceptor && interceptor->ShouldCollectTrace())
    {
        ParameterEntry parameters[] =
        {
            { PARAMETER_WIDE_STRING, Name },
        };

        int numParameters = (sizeof(parameters) / sizeof(parameters[0]));
        DX12APIEntry* pNewEntry = interceptor->PreCall(this, FuncId_ID3D12Object_SetName, numParameters, parameters);
        result = mRealCommandAllocator->SetName(Name);
        interceptor->PostCall(pNewEntry, result);
    }
    else
    {
        result = mRealCommandAllocator->SetName(Name);
    }

    return result;
}

//-----------------------------------------------------------------------------
/// Wrapped_ID3D12CommandAllocator::GetDevice
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE Wrapped_ID3D12CommandAllocator::GetDevice(REFIID riid, void** ppvDevice)
{
#if SERIALIZE_DX12_ENTRY_POINTS
    ScopeLock lock(&s_mutex);
#endif

    HRESULT result = {};

    DX12Interceptor* interceptor = GetDX12LayerManager()->GetInterceptor();

    if (interceptor && interceptor->ShouldCollectTrace())
    {
        ParameterEntry parameters[] =
        {
            { PARAMETER_REFIID, &riid },
            { PARAMETER_POINTER, ppvDevice },
        };

        int numParameters = (sizeof(parameters) / sizeof(parameters[0]));
        DX12APIEntry* pNewEntry = interceptor->PreCall(this, FuncId_ID3D12DeviceChild_GetDevice, numParameters, parameters);
        result = mRealCommandAllocator->GetDevice(riid, ppvDevice);
        interceptor->PostCall(pNewEntry, result);
    }
    else
    {
        result = mRealCommandAllocator->GetDevice(riid, ppvDevice);
    }

    if (ppvDevice != nullptr && *ppvDevice != nullptr)
    {
        DX12WrappedObjectDatabase* objectDatabase = (DX12WrappedObjectDatabase*)DX12ObjectDatabaseProcessor::Instance()->GetObjectDatabase();
        objectDatabase->WrappedObject((IUnknown**)ppvDevice);
    }

    return result;
}

//-----------------------------------------------------------------------------
/// Wrapped_ID3D12CommandAllocator::Reset
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE Wrapped_ID3D12CommandAllocator::Reset()
{
#if SERIALIZE_DX12_ENTRY_POINTS
    ScopeLock lock(&s_mutex);
#endif

    HRESULT result = {};

    DX12Interceptor* interceptor = GetDX12LayerManager()->GetInterceptor();

    if (interceptor && interceptor->ShouldCollectTrace())
    {
        DX12APIEntry* pNewEntry = interceptor->PreCall(this, FuncId_ID3D12CommandAllocator_Reset, 0, nullptr);
        result = mRealCommandAllocator->Reset();
        interceptor->PostCall(pNewEntry, result);
    }
    else
    {
        result = mRealCommandAllocator->Reset();
    }

    return result;
}