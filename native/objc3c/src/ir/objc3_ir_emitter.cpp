#include "ir/objc3_ir_emitter.h"

#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_emitter_module_services.h"
#include "ir/objc3_ir_emitter_runtime_session.h"
#include "ir/objc3_ir_emitter_service_contexts.h"
#include "ir/objc3_ir_emitter_state_initialization.h"
#include "ir/objc3_ir_module_body_orchestration.h"
#include "ir/objc3_ir_module_metadata_publication.h"

class Objc3IREmitter {
 public:
  Objc3IREmitter(const Objc3Program &program,
                 const Objc3LoweringContract &lowering_contract,
                 const Objc3IRFrontendMetadata &frontend_metadata)
      : program_(program),
        frontend_metadata_(frontend_metadata),
        initialized_state_(BuildObjc3IREmitterStateInitialization(
            program, lowering_contract, frontend_metadata)) {}

  bool Emit(std::string &ir, std::string &error) {
    Objc3IREmitterRuntimeSession session(RuntimeSessionInputs());

    if (!initialized_state_.boundary_error.empty()) {
      error = initialized_state_.boundary_error;
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

 private:
  Objc3IREmitterRuntimeSessionInputs RuntimeSessionInputs() {
    return Objc3IREmitterRuntimeSessionInputs{
        program_,
        frontend_metadata_,
        initialized_state_,
        global_const_values_,
        global_nil_proven_symbols_};
  }

  const Objc3Program &program_;
  Objc3IRFrontendMetadata frontend_metadata_;
  Objc3IREmitterStateInitialization initialized_state_;
  std::unordered_map<std::string, int> global_const_values_;
  std::unordered_set<std::string> global_nil_proven_symbols_;
};

bool EmitObjc3IRText(const Objc3Program &program,
                     const Objc3LoweringContract &lowering_contract,
                     const Objc3IRFrontendMetadata &frontend_metadata,
                     std::string &ir,
                     std::string &error) {
  Objc3IREmitter emitter(program, lowering_contract, frontend_metadata);
  return emitter.Emit(ir, error);
}
