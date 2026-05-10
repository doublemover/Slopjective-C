#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "parse/objc3_parser_contract_fingerprints.h"
#include "parse/objc3_parser_contract_types.h"

inline std::size_t BuildObjc3LongTailGrammarConstructCount(const Objc3ParserContractSnapshot &snapshot) {
  return snapshot.protocol_property_decl_count + snapshot.protocol_method_decl_count +
         snapshot.interface_property_decl_count + snapshot.interface_method_decl_count +
         snapshot.implementation_property_decl_count + snapshot.implementation_method_decl_count +
         snapshot.interface_category_decl_count + snapshot.implementation_category_decl_count +
         snapshot.function_prototype_count + snapshot.function_pure_count;
}

inline std::size_t BuildObjc3LongTailGrammarCoveredConstructCount(const Objc3ParserContractSnapshot &snapshot) {
  return snapshot.protocol_class_method_decl_count + snapshot.protocol_instance_method_decl_count +
         snapshot.interface_class_method_decl_count + snapshot.interface_instance_method_decl_count +
         snapshot.implementation_class_method_decl_count + snapshot.implementation_instance_method_decl_count +
         snapshot.function_prototype_count + snapshot.function_pure_count;
}

inline std::uint64_t BuildObjc3LongTailGrammarFingerprint(const Objc3ParserContractSnapshot &snapshot) {
  constexpr std::uint64_t kInitialFingerprint = 1469598103934665603ull;
  std::uint64_t fingerprint = kInitialFingerprint;
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.long_tail_grammar_construct_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.long_tail_grammar_covered_construct_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.protocol_property_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.protocol_method_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.interface_property_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.interface_method_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.implementation_property_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.implementation_method_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.interface_category_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.implementation_category_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.function_prototype_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.function_pure_count));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, snapshot.ast_shape_fingerprint);
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, snapshot.ast_top_level_layout_fingerprint);
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, snapshot.draft_syntax_surface_fingerprint);
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.parser_diagnostic_count));
  return fingerprint;
}

inline std::string BuildObjc3LongTailGrammarHandoffKey(
    const Objc3ParserContractSnapshot &snapshot,
    const bool handoff_deterministic) {
  return "long-tail-grammar:v1:constructs=" +
         std::to_string(snapshot.long_tail_grammar_construct_count) + ";covered=" +
         std::to_string(snapshot.long_tail_grammar_covered_construct_count) + ";fingerprint=" +
         std::to_string(snapshot.long_tail_grammar_fingerprint) + ";ast_shape_fingerprint=" +
         std::to_string(snapshot.ast_shape_fingerprint) + ";ast_top_level_layout_fingerprint=" +
         std::to_string(snapshot.ast_top_level_layout_fingerprint) + ";parser_diagnostics=" +
         std::to_string(snapshot.parser_diagnostic_count) + ";deterministic=" +
         (handoff_deterministic ? "true" : "false");
}
