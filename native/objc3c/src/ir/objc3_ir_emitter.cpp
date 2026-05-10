#include "ir/objc3_ir_emitter.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_emitter_pipeline.h"
#include "ir/objc3_ir_emitter_state_initialization.h"

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
    return EmitObjc3IREmitterPipeline(PipelineInputs(), ir, error);
  }

 private:
  Objc3IREmitterPipelineInputs PipelineInputs() {
    return Objc3IREmitterPipelineInputs{
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
