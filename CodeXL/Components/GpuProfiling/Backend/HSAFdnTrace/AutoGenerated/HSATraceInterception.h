//==============================================================================
// Copyright (c) 2015-2016 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief THIS CODE WAS AUTOGENERATED BY PASSTHROUGHGENERATOR ON 06/13/16
//==============================================================================

#ifndef _HSATRACEINTERCEPTION_H_
#define _HSATRACEINTERCEPTION_H_

#include <hsa_api_trace.h>

#ifdef FUTURE_ROCR_VERSION
extern CoreApiTable* g_pRealCoreFunctions;    ///< table of real core functions
extern FinalizerExtTable* g_pRealFinalizerExtFunctions; ///< table of real finalizer extension functions
extern ImageExtTable* g_pRealImageExtFunctions; ///< table of real image extension functions
extern AmdExtTable* g_pRealAmdExtFunctions; ///< table of real AMD extension functions

/// Intercepts HSA APIs for this HSA Profiler agent
void InitHSAAPIInterceptTrace(HsaApiTable* pTable);
#else
extern ApiTable* g_pRealCoreFunctions;    ///< table of real HSA functions
extern ExtTable* g_pRealFinalizerExtFunctions;    ///< table of real HSA extension functions
extern ExtTable* g_pRealImageExtFunctions;    ///< table of real HSA extension functions (duplicate)
#endif

/// Cleans up API interception for this HSA Profiler agent
void DoneHSAAPIInterceptTrace();

hsa_status_t HSA_API_Trace_hsa_status_string(hsa_status_t status, const char** status_string);
hsa_status_t HSA_API_Trace_hsa_init();
hsa_status_t HSA_API_Trace_hsa_shut_down();
hsa_status_t HSA_API_Trace_hsa_system_get_info(hsa_system_info_t attribute, void* value);
hsa_status_t HSA_API_Trace_hsa_system_extension_supported(uint16_t extension, uint16_t version_major, uint16_t version_minor, bool* result);
hsa_status_t HSA_API_Trace_hsa_system_get_extension_table(uint16_t extension, uint16_t version_major, uint16_t version_minor, void* table);
hsa_status_t HSA_API_Trace_hsa_agent_get_info(hsa_agent_t agent, hsa_agent_info_t attribute, void* value);
hsa_status_t HSA_API_Trace_hsa_iterate_agents(hsa_status_t (*callback)(hsa_agent_t agent, void* data), void* data);
hsa_status_t HSA_API_Trace_hsa_agent_get_exception_policies(hsa_agent_t agent, hsa_profile_t profile, uint16_t* mask);
hsa_status_t HSA_API_Trace_hsa_agent_extension_supported(uint16_t extension, hsa_agent_t agent, uint16_t version_major, uint16_t version_minor, bool* result);
hsa_status_t HSA_API_Trace_hsa_signal_create(hsa_signal_value_t initial_value, uint32_t num_consumers, const hsa_agent_t* consumers, hsa_signal_t* signal);
hsa_status_t HSA_API_Trace_hsa_signal_destroy(hsa_signal_t signal);
hsa_signal_value_t HSA_API_Trace_hsa_signal_load_acquire(hsa_signal_t signal);
hsa_signal_value_t HSA_API_Trace_hsa_signal_load_relaxed(hsa_signal_t signal);
void HSA_API_Trace_hsa_signal_store_relaxed(hsa_signal_t signal, hsa_signal_value_t value);
void HSA_API_Trace_hsa_signal_store_release(hsa_signal_t signal, hsa_signal_value_t value);
hsa_signal_value_t HSA_API_Trace_hsa_signal_exchange_acq_rel(hsa_signal_t signal, hsa_signal_value_t value);
hsa_signal_value_t HSA_API_Trace_hsa_signal_exchange_acquire(hsa_signal_t signal, hsa_signal_value_t value);
hsa_signal_value_t HSA_API_Trace_hsa_signal_exchange_relaxed(hsa_signal_t signal, hsa_signal_value_t value);
hsa_signal_value_t HSA_API_Trace_hsa_signal_exchange_release(hsa_signal_t signal, hsa_signal_value_t value);
hsa_signal_value_t HSA_API_Trace_hsa_signal_cas_acq_rel(hsa_signal_t signal, hsa_signal_value_t expected, hsa_signal_value_t value);
hsa_signal_value_t HSA_API_Trace_hsa_signal_cas_acquire(hsa_signal_t signal, hsa_signal_value_t expected, hsa_signal_value_t value);
hsa_signal_value_t HSA_API_Trace_hsa_signal_cas_relaxed(hsa_signal_t signal, hsa_signal_value_t expected, hsa_signal_value_t value);
hsa_signal_value_t HSA_API_Trace_hsa_signal_cas_release(hsa_signal_t signal, hsa_signal_value_t expected, hsa_signal_value_t value);
void HSA_API_Trace_hsa_signal_add_acq_rel(hsa_signal_t signal, hsa_signal_value_t value);
void HSA_API_Trace_hsa_signal_add_acquire(hsa_signal_t signal, hsa_signal_value_t value);
void HSA_API_Trace_hsa_signal_add_relaxed(hsa_signal_t signal, hsa_signal_value_t value);
void HSA_API_Trace_hsa_signal_add_release(hsa_signal_t signal, hsa_signal_value_t value);
void HSA_API_Trace_hsa_signal_subtract_acq_rel(hsa_signal_t signal, hsa_signal_value_t value);
void HSA_API_Trace_hsa_signal_subtract_acquire(hsa_signal_t signal, hsa_signal_value_t value);
void HSA_API_Trace_hsa_signal_subtract_relaxed(hsa_signal_t signal, hsa_signal_value_t value);
void HSA_API_Trace_hsa_signal_subtract_release(hsa_signal_t signal, hsa_signal_value_t value);
void HSA_API_Trace_hsa_signal_and_acq_rel(hsa_signal_t signal, hsa_signal_value_t value);
void HSA_API_Trace_hsa_signal_and_acquire(hsa_signal_t signal, hsa_signal_value_t value);
void HSA_API_Trace_hsa_signal_and_relaxed(hsa_signal_t signal, hsa_signal_value_t value);
void HSA_API_Trace_hsa_signal_and_release(hsa_signal_t signal, hsa_signal_value_t value);
void HSA_API_Trace_hsa_signal_or_acq_rel(hsa_signal_t signal, hsa_signal_value_t value);
void HSA_API_Trace_hsa_signal_or_acquire(hsa_signal_t signal, hsa_signal_value_t value);
void HSA_API_Trace_hsa_signal_or_relaxed(hsa_signal_t signal, hsa_signal_value_t value);
void HSA_API_Trace_hsa_signal_or_release(hsa_signal_t signal, hsa_signal_value_t value);
void HSA_API_Trace_hsa_signal_xor_acq_rel(hsa_signal_t signal, hsa_signal_value_t value);
void HSA_API_Trace_hsa_signal_xor_acquire(hsa_signal_t signal, hsa_signal_value_t value);
void HSA_API_Trace_hsa_signal_xor_relaxed(hsa_signal_t signal, hsa_signal_value_t value);
void HSA_API_Trace_hsa_signal_xor_release(hsa_signal_t signal, hsa_signal_value_t value);
hsa_signal_value_t HSA_API_Trace_hsa_signal_wait_acquire(hsa_signal_t signal, hsa_signal_condition_t condition, hsa_signal_value_t compare_value, uint64_t timeout_hint, hsa_wait_state_t wait_state_hint);
hsa_signal_value_t HSA_API_Trace_hsa_signal_wait_relaxed(hsa_signal_t signal, hsa_signal_condition_t condition, hsa_signal_value_t compare_value, uint64_t timeout_hint, hsa_wait_state_t wait_state_hint);
hsa_status_t HSA_API_Trace_hsa_queue_create(hsa_agent_t agent, uint32_t size, hsa_queue_type_t type, void (*callback)(hsa_status_t status, hsa_queue_t* source, void* data), void* data, uint32_t private_segment_size, uint32_t group_segment_size, hsa_queue_t** queue);
hsa_status_t HSA_API_Trace_hsa_soft_queue_create(hsa_region_t region, uint32_t size, hsa_queue_type_t type, uint32_t features, hsa_signal_t doorbell_signal, hsa_queue_t** queue);
hsa_status_t HSA_API_Trace_hsa_queue_destroy(hsa_queue_t* queue);
hsa_status_t HSA_API_Trace_hsa_queue_inactivate(hsa_queue_t* queue);
uint64_t HSA_API_Trace_hsa_queue_load_read_index_acquire(const hsa_queue_t* queue);
uint64_t HSA_API_Trace_hsa_queue_load_read_index_relaxed(const hsa_queue_t* queue);
uint64_t HSA_API_Trace_hsa_queue_load_write_index_acquire(const hsa_queue_t* queue);
uint64_t HSA_API_Trace_hsa_queue_load_write_index_relaxed(const hsa_queue_t* queue);
void HSA_API_Trace_hsa_queue_store_write_index_relaxed(const hsa_queue_t* queue, uint64_t value);
void HSA_API_Trace_hsa_queue_store_write_index_release(const hsa_queue_t* queue, uint64_t value);
uint64_t HSA_API_Trace_hsa_queue_cas_write_index_acq_rel(const hsa_queue_t* queue, uint64_t expected, uint64_t value);
uint64_t HSA_API_Trace_hsa_queue_cas_write_index_acquire(const hsa_queue_t* queue, uint64_t expected, uint64_t value);
uint64_t HSA_API_Trace_hsa_queue_cas_write_index_relaxed(const hsa_queue_t* queue, uint64_t expected, uint64_t value);
uint64_t HSA_API_Trace_hsa_queue_cas_write_index_release(const hsa_queue_t* queue, uint64_t expected, uint64_t value);
uint64_t HSA_API_Trace_hsa_queue_add_write_index_acq_rel(const hsa_queue_t* queue, uint64_t value);
uint64_t HSA_API_Trace_hsa_queue_add_write_index_acquire(const hsa_queue_t* queue, uint64_t value);
uint64_t HSA_API_Trace_hsa_queue_add_write_index_relaxed(const hsa_queue_t* queue, uint64_t value);
uint64_t HSA_API_Trace_hsa_queue_add_write_index_release(const hsa_queue_t* queue, uint64_t value);
void HSA_API_Trace_hsa_queue_store_read_index_relaxed(const hsa_queue_t* queue, uint64_t value);
void HSA_API_Trace_hsa_queue_store_read_index_release(const hsa_queue_t* queue, uint64_t value);
hsa_status_t HSA_API_Trace_hsa_region_get_info(hsa_region_t region, hsa_region_info_t attribute, void* value);
hsa_status_t HSA_API_Trace_hsa_agent_iterate_regions(hsa_agent_t agent, hsa_status_t (*callback)(hsa_region_t region, void* data), void* data);
hsa_status_t HSA_API_Trace_hsa_memory_allocate(hsa_region_t region, size_t size, void** ptr);
hsa_status_t HSA_API_Trace_hsa_memory_free(void* ptr);
hsa_status_t HSA_API_Trace_hsa_memory_copy(void* dst, const void* src, size_t size);
hsa_status_t HSA_API_Trace_hsa_memory_assign_agent(void* ptr, hsa_agent_t agent, hsa_access_permission_t access);
hsa_status_t HSA_API_Trace_hsa_memory_register(void* ptr, size_t size);
hsa_status_t HSA_API_Trace_hsa_memory_deregister(void* ptr, size_t size);
hsa_status_t HSA_API_Trace_hsa_isa_from_name(const char* name, hsa_isa_t* isa);
hsa_status_t HSA_API_Trace_hsa_isa_get_info(hsa_isa_t isa, hsa_isa_info_t attribute, uint32_t index, void* value);
hsa_status_t HSA_API_Trace_hsa_isa_compatible(hsa_isa_t code_object_isa, hsa_isa_t agent_isa, bool* result);
hsa_status_t HSA_API_Trace_hsa_code_object_serialize(hsa_code_object_t code_object, hsa_status_t (*alloc_callback)(size_t size, hsa_callback_data_t data, void** address), hsa_callback_data_t callback_data, const char* options, void** serialized_code_object, size_t* serialized_code_object_size);
hsa_status_t HSA_API_Trace_hsa_code_object_deserialize(void* serialized_code_object, size_t serialized_code_object_size, const char* options, hsa_code_object_t* code_object);
hsa_status_t HSA_API_Trace_hsa_code_object_destroy(hsa_code_object_t code_object);
hsa_status_t HSA_API_Trace_hsa_code_object_get_info(hsa_code_object_t code_object, hsa_code_object_info_t attribute, void* value);
hsa_status_t HSA_API_Trace_hsa_code_object_get_symbol(hsa_code_object_t code_object, const char* symbol_name, hsa_code_symbol_t* symbol);
hsa_status_t HSA_API_Trace_hsa_code_symbol_get_info(hsa_code_symbol_t code_symbol, hsa_code_symbol_info_t attribute, void* value);
hsa_status_t HSA_API_Trace_hsa_code_object_iterate_symbols(hsa_code_object_t code_object, hsa_status_t (*callback)(hsa_code_object_t code_object, hsa_code_symbol_t symbol, void* data), void* data);
hsa_status_t HSA_API_Trace_hsa_executable_create(hsa_profile_t profile, hsa_executable_state_t executable_state, const char* options, hsa_executable_t* executable);
hsa_status_t HSA_API_Trace_hsa_executable_destroy(hsa_executable_t executable);
hsa_status_t HSA_API_Trace_hsa_executable_load_code_object(hsa_executable_t executable, hsa_agent_t agent, hsa_code_object_t code_object, const char* options);
hsa_status_t HSA_API_Trace_hsa_executable_freeze(hsa_executable_t executable, const char* options);
hsa_status_t HSA_API_Trace_hsa_executable_get_info(hsa_executable_t executable, hsa_executable_info_t attribute, void* value);
hsa_status_t HSA_API_Trace_hsa_executable_global_variable_define(hsa_executable_t executable, const char* variable_name, void* address);
hsa_status_t HSA_API_Trace_hsa_executable_agent_global_variable_define(hsa_executable_t executable, hsa_agent_t agent, const char* variable_name, void* address);
hsa_status_t HSA_API_Trace_hsa_executable_readonly_variable_define(hsa_executable_t executable, hsa_agent_t agent, const char* variable_name, void* address);
hsa_status_t HSA_API_Trace_hsa_executable_validate(hsa_executable_t executable, uint32_t* result);
hsa_status_t HSA_API_Trace_hsa_executable_get_symbol(hsa_executable_t executable, const char* module_name, const char* symbol_name, hsa_agent_t agent, int32_t call_convention, hsa_executable_symbol_t* symbol);
hsa_status_t HSA_API_Trace_hsa_executable_symbol_get_info(hsa_executable_symbol_t executable_symbol, hsa_executable_symbol_info_t attribute, void* value);
hsa_status_t HSA_API_Trace_hsa_executable_iterate_symbols(hsa_executable_t executable, hsa_status_t (*callback)(hsa_executable_t executable, hsa_executable_symbol_t symbol, void* data), void* data);
hsa_status_t HSA_API_Trace_hsa_ext_program_create(hsa_machine_model_t machine_model, hsa_profile_t profile, hsa_default_float_rounding_mode_t default_float_rounding_mode, const char* options, hsa_ext_program_t* program);
hsa_status_t HSA_API_Trace_hsa_ext_program_destroy(hsa_ext_program_t program);
hsa_status_t HSA_API_Trace_hsa_ext_program_add_module(hsa_ext_program_t program, hsa_ext_module_t module);
hsa_status_t HSA_API_Trace_hsa_ext_program_iterate_modules(hsa_ext_program_t program, hsa_status_t (*callback)(hsa_ext_program_t program, hsa_ext_module_t module, void* data), void* data);
hsa_status_t HSA_API_Trace_hsa_ext_program_get_info(hsa_ext_program_t program, hsa_ext_program_info_t attribute, void* value);
hsa_status_t HSA_API_Trace_hsa_ext_program_finalize(hsa_ext_program_t program, hsa_isa_t isa, int32_t call_convention, hsa_ext_control_directives_t control_directives, const char* options, hsa_code_object_type_t code_object_type, hsa_code_object_t* code_object);
hsa_status_t HSA_API_Trace_hsa_ext_image_get_capability(hsa_agent_t agent, hsa_ext_image_geometry_t geometry, const hsa_ext_image_format_t* image_format, uint32_t* capability_mask);
hsa_status_t HSA_API_Trace_hsa_ext_image_data_get_info(hsa_agent_t agent, const hsa_ext_image_descriptor_t* image_descriptor, hsa_access_permission_t access_permission, hsa_ext_image_data_info_t* image_data_info);
hsa_status_t HSA_API_Trace_hsa_ext_image_create(hsa_agent_t agent, const hsa_ext_image_descriptor_t* image_descriptor, const void* image_data, hsa_access_permission_t access_permission, hsa_ext_image_t* image);
hsa_status_t HSA_API_Trace_hsa_ext_image_destroy(hsa_agent_t agent, hsa_ext_image_t image);
hsa_status_t HSA_API_Trace_hsa_ext_image_copy(hsa_agent_t agent, hsa_ext_image_t src_image, const hsa_dim3_t* src_offset, hsa_ext_image_t dst_image, const hsa_dim3_t* dst_offset, const hsa_dim3_t* range);
hsa_status_t HSA_API_Trace_hsa_ext_image_import(hsa_agent_t agent, const void* src_memory, size_t src_row_pitch, size_t src_slice_pitch, hsa_ext_image_t dst_image, const hsa_ext_image_region_t* image_region);
hsa_status_t HSA_API_Trace_hsa_ext_image_export(hsa_agent_t agent, hsa_ext_image_t src_image, void* dst_memory, size_t dst_row_pitch, size_t dst_slice_pitch, const hsa_ext_image_region_t* image_region);
hsa_status_t HSA_API_Trace_hsa_ext_image_clear(hsa_agent_t agent, hsa_ext_image_t image, const void* data, const hsa_ext_image_region_t* image_region);
hsa_status_t HSA_API_Trace_hsa_ext_sampler_create(hsa_agent_t agent, const hsa_ext_sampler_descriptor_t* sampler_descriptor, hsa_ext_sampler_t* sampler);
hsa_status_t HSA_API_Trace_hsa_ext_sampler_destroy(hsa_agent_t agent, hsa_ext_sampler_t sampler);
hsa_status_t HSA_API_Trace_hsa_amd_coherency_get_type(hsa_agent_t agent, hsa_amd_coherency_type_t* type);
hsa_status_t HSA_API_Trace_hsa_amd_coherency_set_type(hsa_agent_t agent, hsa_amd_coherency_type_t type);
hsa_status_t HSA_API_Trace_hsa_amd_profiling_set_profiler_enabled(hsa_queue_t* queue, int enable);
#ifdef FUTURE_ROCR_VERSION
hsa_status_t HSA_API_Trace_hsa_amd_profiling_async_copy_enable(bool enable);
#endif
hsa_status_t HSA_API_Trace_hsa_amd_profiling_get_dispatch_time(hsa_agent_t agent, hsa_signal_t signal, hsa_amd_profiling_dispatch_time_t* time);
#ifdef FUTURE_ROCR_VERSION
hsa_status_t HSA_API_Trace_hsa_amd_profiling_get_async_copy_time(hsa_signal_t signal, hsa_amd_profiling_async_copy_time_t* time);
#endif
hsa_status_t HSA_API_Trace_hsa_amd_profiling_convert_tick_to_system_domain(hsa_agent_t agent, uint64_t agent_tick, uint64_t* system_tick);
hsa_status_t HSA_API_Trace_hsa_amd_signal_async_handler(hsa_signal_t signal, hsa_signal_condition_t cond, hsa_signal_value_t value, hsa_amd_signal_handler handler, void* arg);
hsa_status_t HSA_API_Trace_hsa_amd_async_function(void (*callback)(void* arg), void* arg);
uint32_t HSA_API_Trace_hsa_amd_signal_wait_any(uint32_t signal_count, hsa_signal_t* signals, hsa_signal_condition_t* conds, hsa_signal_value_t* values, uint64_t timeout_hint, hsa_wait_state_t wait_hint, hsa_signal_value_t* satisfying_value);
hsa_status_t HSA_API_Trace_hsa_amd_queue_cu_set_mask(const hsa_queue_t* queue, uint32_t num_cu_mask_count, const uint32_t* cu_mask);
hsa_status_t HSA_API_Trace_hsa_amd_memory_pool_get_info(hsa_amd_memory_pool_t memory_pool, hsa_amd_memory_pool_info_t attribute, void* value);
hsa_status_t HSA_API_Trace_hsa_amd_agent_iterate_memory_pools(hsa_agent_t agent, hsa_status_t (*callback)(hsa_amd_memory_pool_t memory_pool, void* data), void* data);
hsa_status_t HSA_API_Trace_hsa_amd_memory_pool_allocate(hsa_amd_memory_pool_t memory_pool, size_t size, uint32_t flags, void** ptr);
hsa_status_t HSA_API_Trace_hsa_amd_memory_pool_free(void* ptr);
hsa_status_t HSA_API_Trace_hsa_amd_memory_async_copy(void* dst, hsa_agent_t dst_agent, const void* src, hsa_agent_t src_agent, size_t size, uint32_t num_dep_signals, const hsa_signal_t* dep_signals, hsa_signal_t completion_signal);
hsa_status_t HSA_API_Trace_hsa_amd_agent_memory_pool_get_info(hsa_agent_t agent, hsa_amd_memory_pool_t memory_pool, hsa_amd_agent_memory_pool_info_t attribute, void* value);
hsa_status_t HSA_API_Trace_hsa_amd_agents_allow_access(uint32_t num_agents, const hsa_agent_t* agents, const uint32_t* flags, const void* ptr);
hsa_status_t HSA_API_Trace_hsa_amd_memory_pool_can_migrate(hsa_amd_memory_pool_t src_memory_pool, hsa_amd_memory_pool_t dst_memory_pool, bool* result);
hsa_status_t HSA_API_Trace_hsa_amd_memory_migrate(const void* ptr, hsa_amd_memory_pool_t memory_pool, uint32_t flags);
hsa_status_t HSA_API_Trace_hsa_amd_memory_lock(void* host_ptr, size_t size, hsa_agent_t* agents, int num_agent, void** agent_ptr);
hsa_status_t HSA_API_Trace_hsa_amd_memory_unlock(void* host_ptr);
hsa_status_t HSA_API_Trace_hsa_amd_memory_fill(void* ptr, uint32_t value, size_t count);
hsa_status_t HSA_API_Trace_hsa_amd_interop_map_buffer(uint32_t num_agents, hsa_agent_t* agents, int interop_handle, uint32_t flags, size_t* size, void** ptr, size_t* metadata_size, const void** metadata);
hsa_status_t HSA_API_Trace_hsa_amd_interop_unmap_buffer(void* ptr);


#endif // _HSATRACEINTERCEPTION_H_

