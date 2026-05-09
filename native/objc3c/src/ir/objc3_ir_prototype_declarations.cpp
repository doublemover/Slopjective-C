#include "ir/objc3_ir_prototype_declarations.h"

#include <sstream>
#include <string>
#include <unordered_set>

#include "ast/objc3_ast.h"
#include "ast/objc3_ast_contracts.h"
#include "ir/objc3_ir_type_model.h"
#include "lower/contracts/block_runtime_helper_contracts.h"
#include "lower/contracts/concurrency_actor_contracts.h"
#include "lower/contracts/concurrency_continuation_runtime_contracts.h"
#include "lower/contracts/concurrency_task_runtime_helper_contracts.h"
#include "lower/contracts/error_handling_runtime_bridge_contracts.h"
#include "lower/contracts/ownership_runtime_accessor_helper_contracts.h"
#include "lower/contracts/ownership_runtime_memory_management_contracts.h"

namespace {

bool Objc3IRFunctionRequiresArcHelperDeclarations(
    const FunctionDecl &fn, const Objc3IRFrontendMetadata &frontend_metadata) {
  if (EffectiveArcReturnInsertRetain(fn, frontend_metadata.arc_mode_enabled) ||
      fn.return_ownership_insert_release ||
      EffectiveArcReturnInsertAutorelease(fn)) {
    return true;
  }
  for (const auto &param : fn.params) {
    if (EffectiveArcParamInsertRetain(param, frontend_metadata.arc_mode_enabled) ||
        EffectiveArcParamInsertRelease(param, frontend_metadata.arc_mode_enabled) ||
        param.ownership_insert_autorelease) {
      return true;
    }
  }
  return false;
}

bool Objc3IRMethodRequiresArcHelperDeclarations(
    const Objc3IRMethodDefinition &method_def,
    const Objc3IRFrontendMetadata &frontend_metadata) {
  if (method_def.method == nullptr) {
    return false;
  }
  const Objc3MethodDecl &method = *method_def.method;
  if (EffectiveArcReturnInsertRetain(method, frontend_metadata.arc_mode_enabled) ||
      method.return_ownership_insert_release ||
      EffectiveArcReturnInsertAutorelease(method)) {
    return true;
  }
  for (const auto &param : method.params) {
    if (EffectiveArcParamInsertRetain(param, frontend_metadata.arc_mode_enabled) ||
        EffectiveArcParamInsertRelease(param, frontend_metadata.arc_mode_enabled) ||
        param.ownership_insert_autorelease) {
      return true;
    }
  }
  return false;
}

bool Objc3IRRequiresArcHelperDeclarations(
    const Objc3IRPrototypeDeclarationOptions &options) {
  for (const auto &fn : options.program.functions) {
    if (Objc3IRFunctionRequiresArcHelperDeclarations(fn,
                                                     options.frontend_metadata)) {
      return true;
    }
  }
  for (const auto &method_def : options.method_definitions) {
    if (Objc3IRMethodRequiresArcHelperDeclarations(
            method_def, options.frontend_metadata)) {
      return true;
    }
  }
  return false;
}

bool Objc3IRRequiresRuntimeHelperDeclarations(
    const Objc3IRPrototypeDeclarationOptions &options) {
  const Objc3IRFrontendMetadata &frontend_metadata = options.frontend_metadata;
  return options.synthesized_property_accessor_count > 0u ||
         Objc3IRRequiresArcHelperDeclarations(options) ||
         !frontend_metadata.lowering_error_handling_throws_abi_propagation_replay_key
              .empty() ||
         !frontend_metadata.lowering_async_continuation_replay_key.empty() ||
         !frontend_metadata.lowering_await_lowering_suspension_state_replay_key
              .empty() ||
         frontend_metadata.autoreleasepool_scope_lowering_scope_sites > 0u ||
         frontend_metadata.block_storage_escape_lowering_escape_to_heap_sites >
             0u ||
         frontend_metadata.block_copy_dispose_lowering_copy_helper_required_sites >
             0u ||
         frontend_metadata
                 .block_copy_dispose_lowering_dispose_helper_required_sites >
             0u;
}

bool EmitObjc3IRDeclarationOnce(std::unordered_set<std::string> &declared_symbols,
                                bool &emitted, std::ostringstream &out,
                                const std::string &symbol,
                                const std::string &declaration) {
  if (symbol.empty() || !declared_symbols.insert(symbol).second) {
    return false;
  }
  out << declaration;
  emitted = true;
  return true;
}

void EmitObjc3IRRuntimeHelperDeclarations(
    std::unordered_set<std::string> &declared_symbols, bool &emitted,
    std::ostringstream &out) {
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeReadCurrentPropertyI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeReadCurrentPropertyI32Symbol) +
          "()\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeWriteCurrentPropertyI32Symbol,
      "declare void @" +
          std::string(kObjc3RuntimeWriteCurrentPropertyI32Symbol) + "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeExchangeCurrentPropertyI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeExchangeCurrentPropertyI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeStoreThrownErrorI32Symbol,
      "declare void @" + std::string(kObjc3RuntimeStoreThrownErrorI32Symbol) +
          "(ptr, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeLoadThrownErrorI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeLoadThrownErrorI32Symbol) +
          "(ptr)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeBridgeStatusErrorI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeBridgeStatusErrorI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeBridgeNSErrorErrorI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeBridgeNSErrorErrorI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeCatchMatchesErrorI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeCatchMatchesErrorI32Symbol) +
          "(i32, i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeAllocateAsyncContinuationI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeAllocateAsyncContinuationI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeResumeAsyncContinuationI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeResumeAsyncContinuationI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeSpawnTaskI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeSpawnTaskI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeEnterTaskGroupScopeI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeEnterTaskGroupScopeI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeAddTaskGroupTaskI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeAddTaskGroupTaskI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeWaitTaskGroupNextI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeWaitTaskGroupNextI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeCancelTaskGroupI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeCancelTaskGroupI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeTaskIsCancelledI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeTaskIsCancelledI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeTaskOnCancelI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeTaskOnCancelI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeExecutorHopI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeExecutorHopI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeActorEnterIsolationThunkI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeActorEnterIsolationThunkI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeActorEnterNonisolatedI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeActorEnterNonisolatedI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeActorHopToExecutorI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeActorHopToExecutorI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeActorRecordReplayProofI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeActorRecordReplayProofI32Symbol) + "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeActorRecordRaceGuardI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeActorRecordRaceGuardI32Symbol) + "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeActorBindExecutorI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeActorBindExecutorI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeActorMailboxEnqueueI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeActorMailboxEnqueueI32Symbol) +
          "(i32, i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeActorMailboxDrainNextI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeActorMailboxDrainNextI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol) + "()\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol,
      "declare void @" +
          std::string(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeRetainI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeRetainI32Symbol) + "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeReleaseI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeReleaseI32Symbol) + "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeAutoreleaseI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeAutoreleaseI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimePromoteBlockI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimePromoteBlockI32Symbol) +
          "(ptr, i64, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeInvokeBlockI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeInvokeBlockI32Symbol) +
          "(i32, i32, i32, i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimePushAutoreleasepoolScopeSymbol,
      "declare void @" +
          std::string(kObjc3RuntimePushAutoreleasepoolScopeSymbol) + "()\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimePopAutoreleasepoolScopeSymbol,
      "declare void @" +
          std::string(kObjc3RuntimePopAutoreleasepoolScopeSymbol) + "()\n");
}

void EmitObjc3IRExternalFunctionDeclarations(
    const Objc3IRPrototypeDeclarationOptions &options,
    std::unordered_set<std::string> &declared_symbols, bool &emitted,
    std::ostringstream &out) {
  for (const auto &entry : options.function_signatures) {
    if (options.defined_functions.find(entry.first) !=
        options.defined_functions.end()) {
      continue;
    }
    if (!declared_symbols.insert(entry.first).second) {
      continue;
    }
    const LoweredFunctionSignature &signature = entry.second;
    std::ostringstream params;
    for (std::size_t i = 0; i < signature.param_types.size(); ++i) {
      if (i != 0) {
        params << ", ";
      }
      params << LLVMScalarType(signature.param_types[i]);
    }
    if (signature.throws_declared) {
      if (!signature.param_types.empty()) {
        params << ", ";
      }
      params << "ptr";
    }
    out << "declare " << LLVMScalarType(signature.return_type) << " @"
        << entry.first << "(" << params.str() << ")\n";
    emitted = true;
  }
}

}  // namespace

void EmitObjc3IRPrototypeDeclarations(
    const Objc3IRPrototypeDeclarationOptions &options, std::ostringstream &out) {
  bool emitted = false;
  std::unordered_set<std::string> declared_symbols;
  EmitObjc3IRDeclarationOnce(declared_symbols, emitted, out, "abort",
                             "declare void @abort()\n");
  if (options.emit_runtime_bootstrap_lowering) {
    EmitObjc3IRDeclarationOnce(
        declared_symbols, emitted, out,
        kObjc3RuntimeBootstrapStageRegistrationTableSymbol,
        "declare void @" +
            std::string(kObjc3RuntimeBootstrapStageRegistrationTableSymbol) +
            "(ptr)\n");
    EmitObjc3IRDeclarationOnce(
        declared_symbols, emitted, out,
        options.frontend_metadata
            .runtime_bootstrap_lowering_registration_entrypoint_symbol,
        "declare i32 @" +
            options.frontend_metadata
                .runtime_bootstrap_lowering_registration_entrypoint_symbol +
            "(ptr)\n");
  }
  if (Objc3IRRequiresRuntimeHelperDeclarations(options)) {
    EmitObjc3IRRuntimeHelperDeclarations(declared_symbols, emitted, out);
  }
  EmitObjc3IRExternalFunctionDeclarations(options, declared_symbols, emitted,
                                          out);
  if (emitted) {
    out << "\n";
  }
}
