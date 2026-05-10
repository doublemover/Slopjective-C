#include "ir/objc3_ir_module_body_orchestration.h"

#include <sstream>
#include <string>

#include "ir/objc3_ir_entry_point_emission.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication.h"
#include "ir/objc3_ir_message_send_validation.h"
#include "ir/objc3_ir_module_emission_surface.h"
#include "ir/objc3_ir_prototype_declarations.h"
#include "ir/objc3_ir_runtime_bootstrap_global_emission.h"
#include "ir/objc3_ir_runtime_dispatch_declarations.h"
#include "ir/objc3_ir_runtime_metadata_scaffold_emission.h"
#include "ir/objc3_ir_static_data_emission.h"
#include "ir/objc3_ir_synthetic_method_emission.h"

namespace {

bool ShouldEmitObjc3IRModuleBodyRuntimeBootstrapLowering(
    const Objc3IRModuleBodyOrchestrationOptions &options) {
  return Objc3IRRuntimeBootstrapLoweringReady(options.frontend_metadata);
}

bool ShouldEmitObjc3IRModuleBodyRuntimeBootstrapRegistrationDescriptorImageRoot(
    const Objc3IRModuleBodyOrchestrationOptions &options) {
  return Objc3IRRuntimeBootstrapRegistrationDescriptorImageRootLoweringReady(
      options.frontend_metadata);
}

void EmitObjc3IRModuleBodyRuntimeMetadataSectionScaffold(
    const Objc3IRModuleBodyOrchestrationOptions &options,
    const Objc3IRModuleBodyOrchestrationCallbacks &callbacks,
    std::ostringstream &body) {
  std::string scaffold_error;
  if (!EmitObjc3IRRuntimeMetadataSectionScaffold(
          Objc3IRRuntimeMetadataScaffoldEmissionOptions{
              options.program.module_name,
              options.frontend_metadata,
              options.runtime_metadata_symbols,
              options.selector_pool_globals,
              options.runtime_string_pool_globals,
              options.typed_keypath_artifacts,
              options.method_definitions,
              ShouldEmitObjc3IRModuleBodyRuntimeBootstrapLowering(options),
              ShouldEmitObjc3IRModuleBodyRuntimeBootstrapRegistrationDescriptorImageRoot(
                  options)},
          body, scaffold_error)) {
    callbacks.mark_unsupported_fail_closed_path(
        scaffold_error.empty() ? "runtime metadata scaffold emission failed"
                               : scaffold_error);
  }
}

void EmitObjc3IRModuleBodyPrototypeDeclarations(
    const Objc3IRModuleBodyOrchestrationOptions &options,
    std::ostringstream &body) {
  EmitObjc3IRPrototypeDeclarations(
      Objc3IRPrototypeDeclarationOptions{
          options.program, options.frontend_metadata,
          options.method_definitions, options.function_signatures,
          options.defined_functions, options.synthesized_property_accessor_count,
          ShouldEmitObjc3IRModuleBodyRuntimeBootstrapLowering(options)},
      body);
}

void EmitObjc3IRModuleBodyFunctionDefinitions(
    const Objc3IRModuleBodyOrchestrationOptions &options,
    const Objc3IRModuleBodyOrchestrationCallbacks &callbacks,
    std::ostringstream &body) {
  for (const FunctionDecl *fn : options.function_definitions) {
    EmitObjc3IRFunctionOrchestration(
        *fn, callbacks.build_function_orchestration_options(), body);
    body << "\n";
  }
  for (const Objc3IRMethodDefinition &method_def : options.method_definitions) {
    EmitObjc3IRMethodOrchestration(
        method_def, callbacks.build_function_orchestration_options(), body);
    body << "\n";
  }
  for (const std::string &definition : options.block_function_definitions) {
    body << definition << "\n";
  }
}

}  // namespace

bool EmitObjc3IRModuleBodyOrchestration(
    const Objc3IRModuleBodyOrchestrationOptions &options,
    const Objc3IRModuleBodyOrchestrationCallbacks &callbacks,
    std::ostringstream &body, std::string &error) {
  if (!ValidateObjc3IRMessageSendArityContract(
          options.program, options.lowering_ir_boundary.runtime_dispatch_arg_slots,
          error)) {
    return false;
  }

  if (!EmitObjc3IRStaticData(
          Objc3IRStaticDataEmissionOptions{
              options.program.globals,
              options.mutable_global_symbols,
              options.metaprogramming_global_artifacts,
              options.global_const_values,
              options.global_nil_proven_symbols,
              callbacks.is_compile_time_global_nil_expr},
          body, error)) {
    return false;
  }

  EmitObjc3IRModuleBodyRuntimeMetadataSectionScaffold(
      options, callbacks, body);
  EmitObjc3IRModuleBodyPrototypeDeclarations(options, body);
  EmitObjc3IRRuntimeBootstrapLoweringFunctions(
      options.frontend_metadata, options.runtime_metadata_symbols, body);
  EmitObjc3IRModuleBodyFunctionDefinitions(options, callbacks, body);
  EmitObjc3IREntryPoint(
      options.program, options.function_arity, options.function_signatures,
      body);
  EmitObjc3IRRuntimeDispatchDeclarations(
      options.lowering_ir_boundary, options.runtime_dispatch_call_state, body);
  return true;
}

void EmitObjc3IRModuleFrontendMetadataBoundaryPublication(
    const Objc3IRModuleBodyOrchestrationOptions &options,
    std::ostringstream &out) {
  EmitObjc3IRFrontendMetadataPublication(
      options.frontend_metadata, options.runtime_metadata_symbols,
      options.selector_pool_globals.size(),
      options.runtime_string_pool_globals.size(),
      options.synthesized_property_accessor_count, out);
}

void EmitObjc3IRModuleEmissionSurfacePublications(
    const Objc3IRModuleBodyOrchestrationOptions &options,
    const Objc3IRSyntheticMethodEmissionStats &synthetic_method_stats,
    std::ostringstream &out) {
  EmitObjc3IRRuntimeDispatchDeclarationSurface(
      options.lowering_ir_boundary, options.runtime_dispatch_call_state, out);
  EmitObjc3IRSynthesizedAccessorEmissionSurface(
      Objc3IRSynthesizedAccessorEmissionStats{
          synthetic_method_stats.getter_definition_count,
          synthetic_method_stats.setter_definition_count,
          synthetic_method_stats.current_property_read_helper_call_count,
          synthetic_method_stats.current_property_write_helper_call_count,
          synthetic_method_stats.current_property_exchange_helper_call_count,
          synthetic_method_stats.weak_current_property_load_helper_call_count,
          synthetic_method_stats.weak_current_property_store_helper_call_count,
          synthetic_method_stats.retain_helper_call_count,
          synthetic_method_stats.release_helper_call_count,
          synthetic_method_stats.autorelease_helper_call_count},
      out);
  EmitObjc3IRMethodDispatchEmissionSurface(
      options.lowering_ir_boundary, options.runtime_dispatch_call_state,
      Objc3IRMethodDispatchEmissionStats{
          options.selector_pool_globals.size(),
          options.frontend_metadata
              .dispatch_dispatch_control_lowering_direct_call_candidate_sites,
          options.frontend_metadata
              .dispatch_dispatch_control_lowering_dynamic_opt_out_sites,
          !options.selector_pool_globals.empty()},
      out);
}
