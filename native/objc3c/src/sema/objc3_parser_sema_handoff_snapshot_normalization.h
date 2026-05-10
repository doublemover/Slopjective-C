#pragma once

#include "sema/objc3_parser_sema_handoff_decl_counts.h"

inline Objc3ParserContractSnapshot
NormalizeObjc3ParserContractSnapshotForSemaHandoff(
    const Objc3ParserContractSnapshot &snapshot, const Objc3ParsedProgram &program,
    const Objc3SemaLanguageProfile language_profile, bool &normalization_applied) {
  normalization_applied = false;
  if (language_profile == Objc3SemaLanguageProfile::Canonical) {
    return snapshot;
  }
  if (!IsObjc3ParserContractSnapshotNormalizationCandidate(snapshot, program)) {
    return snapshot;
  }

  Objc3ParserContractSnapshot normalized_snapshot = snapshot;
  if (IsObjc3ParserContractMissingTopLevelDeclBucketsForProgram(
          normalized_snapshot, program) ||
      IsObjc3ParserContractTopLevelDeclBucketOverflow(normalized_snapshot)) {
    const Objc3Program &ast = Objc3ParsedProgramAst(program);
    normalized_snapshot.global_decl_count = ast.globals.size();
    normalized_snapshot.protocol_decl_count = ast.protocols.size();
    normalized_snapshot.interface_decl_count = ast.interfaces.size();
    normalized_snapshot.implementation_decl_count = ast.implementations.size();
    normalized_snapshot.function_decl_count = ast.functions.size();
    normalization_applied = true;
  }
  const std::size_t top_level_count =
      BuildObjc3ParserContractTopLevelCountFromDeclBuckets(normalized_snapshot);
  const std::size_t protocol_property_count =
      BuildObjc3ParserProtocolPropertyDeclCountFromProgram(program);
  const std::size_t protocol_method_count =
      BuildObjc3ParserProtocolMethodDeclCountFromProgram(program);
  const std::size_t protocol_class_method_count =
      BuildObjc3ParserProtocolClassMethodDeclCountFromProgram(program);
  const std::size_t protocol_instance_method_count =
      BuildObjc3ParserProtocolInstanceMethodDeclCountFromProgram(program);
  const std::size_t interface_property_count =
      BuildObjc3ParserInterfacePropertyDeclCountFromProgram(program);
  const std::size_t interface_method_count =
      BuildObjc3ParserInterfaceMethodDeclCountFromProgram(program);
  const std::size_t interface_class_method_count =
      BuildObjc3ParserInterfaceClassMethodDeclCountFromProgram(program);
  const std::size_t interface_instance_method_count =
      BuildObjc3ParserInterfaceInstanceMethodDeclCountFromProgram(program);
  const std::size_t implementation_property_count =
      BuildObjc3ParserImplementationPropertyDeclCountFromProgram(program);
  const std::size_t implementation_method_count =
      BuildObjc3ParserImplementationMethodDeclCountFromProgram(program);
  const std::size_t implementation_class_method_count =
      BuildObjc3ParserImplementationClassMethodDeclCountFromProgram(program);
  const std::size_t implementation_instance_method_count =
      BuildObjc3ParserImplementationInstanceMethodDeclCountFromProgram(program);
  const std::size_t interface_category_count =
      BuildObjc3ParserInterfaceCategoryDeclCountFromProgram(program);
  const std::size_t implementation_category_count =
      BuildObjc3ParserImplementationCategoryDeclCountFromProgram(program);
  const std::size_t function_prototype_count =
      BuildObjc3ParserFunctionPrototypeCountFromProgram(program);
  const std::size_t function_pure_count =
      BuildObjc3ParserFunctionPureCountFromProgram(program);
  if (normalized_snapshot.top_level_declaration_count == 0u &&
      top_level_count != 0u) {
    normalized_snapshot.top_level_declaration_count = top_level_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.protocol_property_decl_count == 0u &&
      protocol_property_count != 0u) {
    normalized_snapshot.protocol_property_decl_count = protocol_property_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.protocol_property_decl_count > protocol_property_count) {
    normalized_snapshot.protocol_property_decl_count = protocol_property_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.protocol_method_decl_count == 0u &&
      protocol_method_count != 0u) {
    normalized_snapshot.protocol_method_decl_count = protocol_method_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.protocol_method_decl_count > protocol_method_count) {
    normalized_snapshot.protocol_method_decl_count = protocol_method_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.protocol_class_method_decl_count == 0u &&
      protocol_class_method_count != 0u) {
    normalized_snapshot.protocol_class_method_decl_count =
        protocol_class_method_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.protocol_class_method_decl_count >
      protocol_class_method_count) {
    normalized_snapshot.protocol_class_method_decl_count =
        protocol_class_method_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.protocol_instance_method_decl_count == 0u &&
      protocol_instance_method_count != 0u) {
    normalized_snapshot.protocol_instance_method_decl_count =
        protocol_instance_method_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.protocol_instance_method_decl_count >
      protocol_instance_method_count) {
    normalized_snapshot.protocol_instance_method_decl_count =
        protocol_instance_method_count;
    normalization_applied = true;
  }
  if (!AreObjc3ParserMethodDeclBucketsConsistent(
          normalized_snapshot.protocol_class_method_decl_count,
          normalized_snapshot.protocol_instance_method_decl_count,
          normalized_snapshot.protocol_method_decl_count)) {
    normalized_snapshot.protocol_class_method_decl_count =
        protocol_class_method_count;
    normalized_snapshot.protocol_instance_method_decl_count =
        protocol_instance_method_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.interface_property_decl_count == 0u &&
      interface_property_count != 0u) {
    normalized_snapshot.interface_property_decl_count = interface_property_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.interface_property_decl_count > interface_property_count) {
    normalized_snapshot.interface_property_decl_count = interface_property_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.interface_method_decl_count == 0u &&
      interface_method_count != 0u) {
    normalized_snapshot.interface_method_decl_count = interface_method_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.interface_method_decl_count > interface_method_count) {
    normalized_snapshot.interface_method_decl_count = interface_method_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.interface_class_method_decl_count == 0u &&
      interface_class_method_count != 0u) {
    normalized_snapshot.interface_class_method_decl_count =
        interface_class_method_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.interface_class_method_decl_count >
      interface_class_method_count) {
    normalized_snapshot.interface_class_method_decl_count =
        interface_class_method_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.interface_instance_method_decl_count == 0u &&
      interface_instance_method_count != 0u) {
    normalized_snapshot.interface_instance_method_decl_count =
        interface_instance_method_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.interface_instance_method_decl_count >
      interface_instance_method_count) {
    normalized_snapshot.interface_instance_method_decl_count =
        interface_instance_method_count;
    normalization_applied = true;
  }
  if (!AreObjc3ParserMethodDeclBucketsConsistent(
          normalized_snapshot.interface_class_method_decl_count,
          normalized_snapshot.interface_instance_method_decl_count,
          normalized_snapshot.interface_method_decl_count)) {
    normalized_snapshot.interface_class_method_decl_count =
        interface_class_method_count;
    normalized_snapshot.interface_instance_method_decl_count =
        interface_instance_method_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.implementation_property_decl_count == 0u &&
      implementation_property_count != 0u) {
    normalized_snapshot.implementation_property_decl_count = implementation_property_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.implementation_property_decl_count >
      implementation_property_count) {
    normalized_snapshot.implementation_property_decl_count = implementation_property_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.implementation_method_decl_count == 0u &&
      implementation_method_count != 0u) {
    normalized_snapshot.implementation_method_decl_count = implementation_method_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.implementation_method_decl_count >
      implementation_method_count) {
    normalized_snapshot.implementation_method_decl_count = implementation_method_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.implementation_class_method_decl_count == 0u &&
      implementation_class_method_count != 0u) {
    normalized_snapshot.implementation_class_method_decl_count =
        implementation_class_method_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.implementation_class_method_decl_count >
      implementation_class_method_count) {
    normalized_snapshot.implementation_class_method_decl_count =
        implementation_class_method_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.implementation_instance_method_decl_count == 0u &&
      implementation_instance_method_count != 0u) {
    normalized_snapshot.implementation_instance_method_decl_count =
        implementation_instance_method_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.implementation_instance_method_decl_count >
      implementation_instance_method_count) {
    normalized_snapshot.implementation_instance_method_decl_count =
        implementation_instance_method_count;
    normalization_applied = true;
  }
  if (!AreObjc3ParserMethodDeclBucketsConsistent(
          normalized_snapshot.implementation_class_method_decl_count,
          normalized_snapshot.implementation_instance_method_decl_count,
          normalized_snapshot.implementation_method_decl_count)) {
    normalized_snapshot.implementation_class_method_decl_count =
        implementation_class_method_count;
    normalized_snapshot.implementation_instance_method_decl_count =
        implementation_instance_method_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.interface_category_decl_count == 0u &&
      interface_category_count != 0u) {
    normalized_snapshot.interface_category_decl_count = interface_category_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.implementation_category_decl_count == 0u &&
      implementation_category_count != 0u) {
    normalized_snapshot.implementation_category_decl_count = implementation_category_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.function_prototype_count == 0u &&
      function_prototype_count != 0u) {
    normalized_snapshot.function_prototype_count = function_prototype_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.function_pure_count == 0u &&
      function_pure_count != 0u) {
    normalized_snapshot.function_pure_count = function_pure_count;
    normalization_applied = true;
  }
  if (normalized_snapshot.ast_shape_fingerprint == 0u) {
    normalized_snapshot.ast_shape_fingerprint =
        BuildObjc3ParsedProgramAstShapeFingerprint(program);
    normalization_applied = true;
  }
  if (normalized_snapshot.ast_top_level_layout_fingerprint == 0u) {
    normalized_snapshot.ast_top_level_layout_fingerprint =
        BuildObjc3ParsedProgramTopLevelLayoutFingerprint(program);
    normalization_applied = true;
  }
  return normalized_snapshot;
}

inline bool IsObjc3ParserContractSnapshotNormalizationRejectedForSemaHandoff(
    const Objc3ParserContractSnapshot &snapshot,
    const Objc3ParsedProgram &program,
    const Objc3SemaLanguageProfile language_profile) {
  return language_profile == Objc3SemaLanguageProfile::Canonical &&
         IsObjc3ParserContractSnapshotNormalizationCandidate(snapshot, program);
}
