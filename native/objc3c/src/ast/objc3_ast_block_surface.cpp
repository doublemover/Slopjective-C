#include "ast/objc3_ast_block_surface.h"

#include <algorithm>
#include <sstream>
#include <utility>

namespace {

std::vector<std::string> BuildSortedUniqueStrings(std::vector<std::string> values) {
  std::sort(values.begin(), values.end());
  values.erase(std::unique(values.begin(), values.end()), values.end());
  return values;
}

}  // namespace

bool Objc3ExprIsBlockLiteral(const Expr &expr) {
  return expr.kind == Expr::Kind::BlockLiteral;
}

bool Objc3BlockRequiresByrefStorage(const Expr &expr) {
  return Objc3ExprIsBlockLiteral(expr) &&
         (expr.block_storage_requires_byref_cells ||
          expr.block_byref_capture_count > 0 ||
          expr.block_storage_byref_slot_count > 0);
}

bool Objc3BlockRequiresRuntimeCopyDispose(const Expr &expr) {
  return Objc3ExprIsBlockLiteral(expr) &&
         (expr.block_copy_helper_required || expr.block_dispose_helper_required ||
          expr.block_runtime_copy_helper_required ||
          expr.block_runtime_dispose_helper_required ||
          expr.block_runtime_owned_object_capture_count > 0);
}

bool Objc3BlockCanUseStackInvokeLowering(const Expr &expr) {
  if (!Objc3ExprIsBlockLiteral(expr) || !expr.block_literal_is_normalized) {
    return false;
  }
  if (expr.block_storage_escape_to_heap || Objc3BlockRequiresRuntimePromotion(expr)) {
    return false;
  }
  return expr.block_abi_has_invoke_trampoline;
}

bool Objc3BlockRequiresRuntimePromotion(const Expr &expr) {
  return Objc3ExprIsBlockLiteral(expr) &&
         (expr.block_storage_escape_to_heap ||
          expr.block_escape_shape_promotes_to_heap_candidate);
}

std::string Objc3BlockLoweringReplayKey(const Expr &expr) {
  std::ostringstream out;
  out << "kind=" << (Objc3ExprIsBlockLiteral(expr) ? "block" : "non-block")
      << ";normalized=" << (expr.block_literal_is_normalized ? "true" : "false")
      << ";params=" << expr.block_parameter_count
      << ";captures=" << expr.block_capture_count
      << ";byref=" << expr.block_byref_capture_count
      << ";owned=" << expr.block_runtime_owned_object_capture_count
      << ";weak=" << expr.block_runtime_weak_object_capture_count
      << ";unowned=" << expr.block_runtime_unowned_object_capture_count
      << ";requires_byref="
      << (Objc3BlockRequiresByrefStorage(expr) ? "true" : "false")
      << ";requires_copy_dispose="
      << (Objc3BlockRequiresRuntimeCopyDispose(expr) ? "true" : "false")
      << ";requires_promotion="
      << (Objc3BlockRequiresRuntimePromotion(expr) ? "true" : "false")
      << ";stack_invoke="
      << (Objc3BlockCanUseStackInvokeLowering(expr) ? "true" : "false")
      << ";descriptor=" << expr.block_abi_descriptor_symbol
      << ";invoke=" << expr.block_invoke_trampoline_symbol
      << ";source_replay=" << expr.block_source_model_replay_key;
  return out.str();
}

std::string BuildBlockLiteralCaptureProfile(
    const std::vector<std::string> &capture_names_lexicographic) {
  if (capture_names_lexicographic.empty()) {
    return "block-captures:none";
  }
  std::ostringstream out;
  out << "block-captures:";
  for (std::size_t i = 0; i < capture_names_lexicographic.size(); ++i) {
    out << capture_names_lexicographic[i];
    if (i + 1u != capture_names_lexicographic.size()) {
      out << ",";
    }
  }
  return out.str();
}

std::string BuildBlockParameterSignatureEntry(
    const Objc3BlockParameterSourceModel &parameter) {
  std::ostringstream out;
  out << "name=" << parameter.name
      << ";type=" << parameter.type_spelling
      << ";explicit_type=" << (parameter.explicit_type ? "true" : "false");
  return out.str();
}

std::vector<std::string> BuildBlockParameterSignatureEntriesLexicographic(
    const std::vector<Objc3BlockParameterSourceModel> &parameters) {
  std::vector<std::string> entries;
  entries.reserve(parameters.size());
  for (const auto &parameter : parameters) {
    entries.push_back(BuildBlockParameterSignatureEntry(parameter));
  }
  return BuildSortedUniqueStrings(std::move(entries));
}

std::vector<ValueType> BuildBlockParameterTypesSourceOrder(
    const std::vector<Objc3BlockParameterSourceModel> &parameters) {
  std::vector<ValueType> types;
  types.reserve(parameters.size());
  for (const auto &parameter : parameters) {
    if (!parameter.explicit_type) {
      types.push_back(ValueType::Unknown);
      continue;
    }
    if (parameter.type_spelling == "i32") {
      types.push_back(ValueType::I32);
      continue;
    }
    if (parameter.type_spelling == "bool") {
      types.push_back(ValueType::Bool);
      continue;
    }
    if (parameter.type_spelling == "void") {
      types.push_back(ValueType::Void);
      continue;
    }
    types.push_back(ValueType::Unknown);
  }
  return types;
}

std::string BuildBlockSignatureProfile(
    const std::vector<Objc3BlockParameterSourceModel> &parameters) {
  const std::size_t explicit_typed_parameter_count =
      static_cast<std::size_t>(std::count_if(
          parameters.begin(), parameters.end(),
          [](const Objc3BlockParameterSourceModel &parameter) {
            return parameter.explicit_type;
          }));
  const std::size_t implicit_parameter_count =
      parameters.size() - explicit_typed_parameter_count;
  std::ostringstream out;
  out << "block-signature:parameters=" << parameters.size()
      << ";explicit-typed=" << explicit_typed_parameter_count
      << ";implicit=" << implicit_parameter_count
      << ";return-surface=body-inferred";
  return out.str();
}

std::string BuildBlockCaptureInventoryEntry(const std::string &capture_name) {
  return "name=" + capture_name +
         ";storage=by-value-readonly;byref=false;mutable=false";
}

std::vector<std::string> BuildBlockCaptureInventoryEntriesLexicographic(
    const std::vector<std::string> &capture_names_lexicographic) {
  std::vector<std::string> entries;
  entries.reserve(capture_names_lexicographic.size());
  for (const auto &capture_name : capture_names_lexicographic) {
    entries.push_back(BuildBlockCaptureInventoryEntry(capture_name));
  }
  return BuildSortedUniqueStrings(std::move(entries));
}

std::string BuildBlockCaptureInventoryProfile(
    std::size_t capture_count,
    std::size_t byvalue_readonly_capture_count) {
  std::ostringstream out;
  out << "block-capture-inventory:captures=" << capture_count
      << ";byvalue-readonly=" << byvalue_readonly_capture_count
      << ";byref=0;mutable=0";
  return out.str();
}

std::string BuildBlockLiteralAbiLayoutProfile(std::size_t parameter_count,
                                              std::size_t capture_count,
                                              std::size_t body_statement_count) {
  std::ostringstream out;
  out << "block-abi-layout:invoke-arg-slots=" << parameter_count
      << ";capture-words=" << capture_count
      << ";body-statements=" << body_statement_count;
  return out.str();
}

std::string BuildBlockLiteralAbiDescriptorSymbol(unsigned line,
                                                 unsigned column,
                                                 std::size_t parameter_count,
                                                 std::size_t capture_count) {
  std::ostringstream out;
  out << "__objc3_block_desc_" << line << "_" << column
      << "_p" << parameter_count
      << "_c" << capture_count;
  return out.str();
}

std::string BuildBlockLiteralInvokeTrampolineSymbol(unsigned line,
                                                    unsigned column,
                                                    std::size_t parameter_count,
                                                    std::size_t capture_count) {
  std::ostringstream out;
  out << "__objc3_block_invoke_" << line << "_" << column
      << "_p" << parameter_count
      << "_c" << capture_count;
  return out.str();
}

std::vector<std::string> BuildBlockInvokeSurfaceEntriesLexicographic(
    const Expr &block) {
  std::vector<std::string> entries;
  entries.push_back("descriptor=" + block.block_abi_descriptor_symbol);
  entries.push_back("invoke=" + block.block_invoke_trampoline_symbol);
  return BuildSortedUniqueStrings(std::move(entries));
}

std::string BuildBlockInvokeSurfaceProfile(const Expr &block) {
  std::ostringstream out;
  out << "block-invoke-surface:invoke-arg-slots="
      << block.block_abi_invoke_argument_slots
      << ";capture-words=" << block.block_abi_capture_word_count
      << ";descriptor-symbol=" << block.block_abi_descriptor_symbol
      << ";invoke-symbol=" << block.block_invoke_trampoline_symbol
      << ";return-surface=body-inferred";
  return out.str();
}

std::string BuildBlockSourceModelReplayKey(const Expr &block) {
  std::ostringstream out;
  out << "signature_entries="
      << block.block_parameter_signature_entries_lexicographic.size()
      << ";explicit_typed_parameters="
      << block.block_explicit_typed_parameter_count
      << ";capture_inventory_entries="
      << block.block_capture_inventory_entries_lexicographic.size()
      << ";byvalue_readonly_captures="
      << block.block_byvalue_readonly_capture_count
      << ";invoke_surface_entries="
      << block.block_invoke_surface_entries_lexicographic.size()
      << ";deterministic="
      << (block.block_source_model_is_normalized ? "true" : "false")
      << ";lane_contract="
      << Expr::kObjc3ExecutableBlockSourceModelLaneContract;
  return out.str();
}

std::string BuildBlockStorageEscapeProfile(std::size_t mutable_capture_count,
                                           std::size_t byref_slot_count,
                                           bool escape_to_heap,
                                           std::size_t body_statement_count) {
  std::ostringstream out;
  out << "block-storage:mutable-captures=" << mutable_capture_count
      << ";byref-slots=" << byref_slot_count
      << ";escape=" << (escape_to_heap ? "heap" : "stack")
      << ";body-statements=" << body_statement_count;
  return out.str();
}

std::string BuildBlockStorageByrefLayoutSymbol(unsigned line,
                                               unsigned column,
                                               std::size_t mutable_capture_count,
                                               std::size_t byref_slot_count,
                                               bool escape_to_heap) {
  std::ostringstream out;
  out << "__objc3_block_byref_layout_" << line << "_" << column
      << "_m" << mutable_capture_count
      << "_b" << byref_slot_count
      << "_" << (escape_to_heap ? "heap" : "stack");
  return out.str();
}

std::string BuildBlockCopyDisposeProfile(std::size_t mutable_capture_count,
                                         std::size_t byref_slot_count,
                                         bool escape_to_heap,
                                         std::size_t body_statement_count) {
  std::ostringstream out;
  out << "block-copy-dispose:copy-helper=" << (mutable_capture_count > 0u ? "enabled" : "elided")
      << ";dispose-helper=" << (byref_slot_count > 0u ? "enabled" : "elided")
      << ";escape=" << (escape_to_heap ? "heap" : "stack")
      << ";body-statements=" << body_statement_count;
  return out.str();
}

std::string BuildBlockCopyHelperSymbol(unsigned line,
                                       unsigned column,
                                       std::size_t mutable_capture_count,
                                       std::size_t byref_slot_count,
                                       bool escape_to_heap) {
  std::ostringstream out;
  out << "__objc3_block_copy_helper_" << line << "_" << column
      << "_m" << mutable_capture_count
      << "_b" << byref_slot_count
      << "_" << (escape_to_heap ? "heap" : "stack");
  return out.str();
}

std::string BuildBlockDisposeHelperSymbol(unsigned line,
                                          unsigned column,
                                          std::size_t mutable_capture_count,
                                          std::size_t byref_slot_count,
                                          bool escape_to_heap) {
  std::ostringstream out;
  out << "__objc3_block_dispose_helper_" << line << "_" << column
      << "_m" << mutable_capture_count
      << "_b" << byref_slot_count
      << "_" << (escape_to_heap ? "heap" : "stack");
  return out.str();
}

std::size_t BuildBlockDeterminismPerfBaselineWeight(std::size_t parameter_count,
                                                    std::size_t capture_count,
                                                    std::size_t body_statement_count,
                                                    bool copy_helper_required,
                                                    bool dispose_helper_required) {
  std::size_t weight = parameter_count * 2u + capture_count * 8u + body_statement_count * 4u;
  if (copy_helper_required) {
    weight += 6u;
  }
  if (dispose_helper_required) {
    weight += 6u;
  }
  return weight;
}

std::string BuildBlockDeterminismPerfBaselineProfile(std::size_t parameter_count,
                                                     std::size_t capture_count,
                                                     std::size_t body_statement_count,
                                                     bool copy_helper_required,
                                                     bool dispose_helper_required,
                                                     bool deterministic_capture_set,
                                                     bool copy_dispose_profile_is_normalized,
                                                     std::size_t baseline_weight) {
  const char *tier = baseline_weight <= 24u ? "light" : (baseline_weight <= 64u ? "medium" : "heavy");
  std::ostringstream out;
  out << "block-det-perf-baseline:params=" << parameter_count
      << ";captures=" << capture_count
      << ";body-statements=" << body_statement_count
      << ";copy-helper=" << (copy_helper_required ? "enabled" : "elided")
      << ";dispose-helper=" << (dispose_helper_required ? "enabled" : "elided")
      << ";deterministic-captures=" << (deterministic_capture_set ? "true" : "false")
      << ";normalized=" << (copy_dispose_profile_is_normalized ? "true" : "false")
      << ";weight=" << baseline_weight
      << ";tier=" << tier;
  return out.str();
}
