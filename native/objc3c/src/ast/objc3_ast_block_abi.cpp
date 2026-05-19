#include "ast/objc3_ast_block_abi.h"

#include <sstream>
#include <utility>

#include "ast/objc3_ast_ordering.h"

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
  return Objc3AstSortedUniqueStrings(std::move(entries));
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
