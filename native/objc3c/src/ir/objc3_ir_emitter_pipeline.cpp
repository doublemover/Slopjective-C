#include "ir/objc3_ir_emitter_pipeline.h"

#include <sstream>

#include "ir/objc3_ir_emitter_module_services.h"
#include "ir/objc3_ir_emitter_runtime_session.h"
#include "ir/objc3_ir_emitter_service_contexts.h"
#include "ir/objc3_ir_emitter_state_initialization.h"
#include "ir/objc3_ir_module_body_orchestration.h"
#include "ir/objc3_ir_module_metadata_publication.h"

bool EmitObjc3IREmitterPipeline(
    const Objc3IREmitterPipelineInputs &inputs,
    std::string &ir,
    std::string &error) {
  Objc3IREmitterRuntimeSession session(Objc3IREmitterRuntimeSessionInputs{
      inputs.program,
      inputs.frontend_metadata,
      inputs.initialized_state,
      inputs.global_const_values,
      inputs.global_nil_proven_symbols});

  if (!inputs.initialized_state.boundary_error.empty()) {
    error = inputs.initialized_state.boundary_error;
    return false;
  }

  Objc3IREmitterServiceContextState service_state =
      session.ServiceContextState();
  Objc3IREmitterServiceContextCallbacks service_callbacks =
      session.ServiceContextCallbacks();
  Objc3IRModuleBodyOrchestrationOptions module_body_options =
      BuildObjc3IREmitterModuleBodyOrchestrationOptions(service_state);
  std::ostringstream body;
  if (!EmitObjc3IRModuleBodyOrchestration(
          module_body_options,
          BuildObjc3IREmitterModuleBodyOrchestrationCallbacks(
              service_state, service_callbacks),
          body, error)) {
    return false;
  }

  if (session.UnsupportedFailClosedPathTriggered()) {
    error = session.UnsupportedFailClosedError();
    return false;
  }

  std::ostringstream out;
  EmitObjc3IRModuleMetadataPublication(
      BuildObjc3IREmitterModuleMetadataPublicationOptions(service_state),
      out);
  EmitObjc3IRModuleFrontendMetadataBoundaryPublication(
      module_body_options, out);
  // Historical extraction contract markers retained for fail-closed tooling:
  // out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";
  // for (std::size_t i = 0; i < lowering_ir_boundary_.runtime_dispatch_arg_slots; ++i) {
  //   out << ", i32";
  // }
  // out << ")\n\n";
  EmitObjc3IRModuleEmissionSurfacePublications(
      module_body_options, session.SyntheticMethodStats(), out);
  out << body.str();
  ir = out.str();
  return true;
}
