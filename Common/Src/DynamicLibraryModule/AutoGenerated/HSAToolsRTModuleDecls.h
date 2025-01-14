//==============================================================================
// Copyright (c) 2015-2016 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief THIS CODE WAS AUTOGENERATED BY PASSTHROUGHGENERATOR ON 05/18/16
//==============================================================================

#ifndef _HSATOOLSRTMODULEDECLS_H_
#define _HSATOOLSRTMODULEDECLS_H_

#include <amd_hsa_tools_interfaces.h>
#include <hsa_ext_debugger.h>
#include <hsa_ext_profiler.h>

typedef decltype(hsa_ext_tools_set_callback_functions)* hsa_ext_tools_set_callback_functions_t;
typedef decltype(hsa_ext_tools_get_callback_functions)* hsa_ext_tools_get_callback_functions_t;
typedef decltype(hsa_ext_tools_set_callback_arguments)* hsa_ext_tools_set_callback_arguments_t;
typedef decltype(hsa_ext_tools_get_callback_arguments)* hsa_ext_tools_get_callback_arguments_t;

typedef decltype(hsa_ext_tools_set_correlation_handler)* hsa_ext_tools_set_correlation_handler_t;
typedef decltype(hsa_ext_tools_wave_control)* hsa_ext_tools_wave_control_t;
typedef decltype(hsa_ext_tools_flush_cache)* hsa_ext_tools_flush_cache_t;
typedef decltype(hsa_ext_tools_install_trap)* hsa_ext_tools_install_trap_t;
typedef decltype(hsa_ext_tools_set_exception_policy)* hsa_ext_tools_set_exception_policy_t;
typedef decltype(hsa_ext_tools_get_exception_policy)* hsa_ext_tools_get_exception_policy_t;
typedef decltype(hsa_ext_tools_set_kernel_execution_mode)* hsa_ext_tools_set_kernel_execution_mode_t;
typedef decltype(hsa_ext_tools_get_kernel_execution_mode)* hsa_ext_tools_get_kernel_execution_mode_t;
typedef decltype(hsa_ext_tools_register)* hsa_ext_tools_register_t;
typedef decltype(hsa_ext_tools_unregister)* hsa_ext_tools_unregister_t;
typedef decltype(hsa_ext_tools_address_watch)* hsa_ext_tools_address_watch_t;
typedef decltype(hsa_ext_tools_get_dispatch_debug_info)* hsa_ext_tools_get_dispatch_debug_info_t;
typedef decltype(hsa_ext_tools_dmacopy)* hsa_ext_tools_dmacopy_t;
typedef decltype(hsa_ext_tools_create_event)* hsa_ext_tools_create_event_t;
typedef decltype(hsa_ext_tools_wait_event)* hsa_ext_tools_wait_event_t;
typedef decltype(hsa_ext_tools_destroy_event)* hsa_ext_tools_destroy_event_t;

typedef decltype(hsa_ext_tools_create_pmu)* hsa_ext_tools_create_pmu_t;
typedef decltype(hsa_ext_tools_release_pmu)* hsa_ext_tools_release_pmu_t;
typedef decltype(hsa_ext_tools_get_counter_block_by_id)* hsa_ext_tools_get_counter_block_by_id_t;
typedef decltype(hsa_ext_tools_get_all_counter_blocks)* hsa_ext_tools_get_all_counter_blocks_t;
typedef decltype(hsa_ext_tools_get_pmu_state)* hsa_ext_tools_get_pmu_state_t;
typedef decltype(hsa_ext_tools_pmu_begin)* hsa_ext_tools_pmu_begin_t;
typedef decltype(hsa_ext_tools_pmu_end)* hsa_ext_tools_pmu_end_t;
typedef decltype(hsa_ext_tools_pmu_wait_for_completion)* hsa_ext_tools_pmu_wait_for_completion_t;
typedef decltype(hsa_ext_tools_set_pmu_parameter)* hsa_ext_tools_set_pmu_parameter_t;
typedef decltype(hsa_ext_tools_get_pmu_parameter)* hsa_ext_tools_get_pmu_parameter_t;
typedef decltype(hsa_ext_tools_get_pmu_info)* hsa_ext_tools_get_pmu_info_t;
typedef decltype(hsa_ext_tools_create_counter)* hsa_ext_tools_create_counter_t;
typedef decltype(hsa_ext_tools_destroy_counter)* hsa_ext_tools_destroy_counter_t;
typedef decltype(hsa_ext_tools_destroy_all_counters)* hsa_ext_tools_destroy_all_counters_t;
typedef decltype(hsa_ext_tools_get_enabled_counters)* hsa_ext_tools_get_enabled_counters_t;
typedef decltype(hsa_ext_tools_get_all_counters)* hsa_ext_tools_get_all_counters_t;
typedef decltype(hsa_ext_tools_set_counter_block_parameter)* hsa_ext_tools_set_counter_block_parameter_t;
typedef decltype(hsa_ext_tools_get_counter_block_parameter)* hsa_ext_tools_get_counter_block_parameter_t;
typedef decltype(hsa_ext_tools_get_counter_block_info)* hsa_ext_tools_get_counter_block_info_t;
typedef decltype(hsa_ext_tools_get_counter_block)* hsa_ext_tools_get_counter_block_t;
typedef decltype(hsa_ext_tools_set_counter_enabled)* hsa_ext_tools_set_counter_enabled_t;
typedef decltype(hsa_ext_tools_is_counter_enabled)* hsa_ext_tools_is_counter_enabled_t;
typedef decltype(hsa_ext_tools_is_counter_result_ready)* hsa_ext_tools_is_counter_result_ready_t;
typedef decltype(hsa_ext_tools_get_counter_result)* hsa_ext_tools_get_counter_result_t;
typedef decltype(hsa_ext_tools_set_counter_parameter)* hsa_ext_tools_set_counter_parameter_t;
typedef decltype(hsa_ext_tools_get_counter_parameter)* hsa_ext_tools_get_counter_parameter_t;
typedef decltype(hsa_ext_tools_register_aql_trace_callback)* hsa_ext_tools_register_aql_trace_callback_t;
typedef decltype(hsa_ext_tools_queue_create_profiled)* hsa_ext_tools_queue_create_profiled_t;
typedef decltype(hsa_ext_tools_get_kernel_times)* hsa_ext_tools_get_kernel_times_t;



#endif // _HSATOOLSRTMODULEDECLS_H_

