#pragma once

#include <cstddef>

#include "sema/objc3_parser_sema_handoff_decl_counts_top_level.h"
#include "sema/objc3_parser_sema_handoff_decl_counts_protocol.h"
#include "sema/objc3_parser_sema_handoff_decl_counts_interface.h"
#include "sema/objc3_parser_sema_handoff_decl_counts_implementation.h"
#include "sema/objc3_parser_sema_handoff_decl_counts_function.h"

inline bool IsObjc3ParserContractSnapshotNormalizationCandidate(
    const Objc3ParserContractSnapshot &snapshot,
    const Objc3ParsedProgram &program) {
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
  const bool decl_bucket_overflow =
      IsObjc3ParserContractTopLevelDeclBucketOverflow(snapshot);
  const bool missing_decl_buckets =
      IsObjc3ParserContractMissingTopLevelDeclBucketsForProgram(snapshot, program);
  const bool protocol_method_bucket_inconsistent =
      !AreObjc3ParserMethodDeclBucketsConsistent(
          snapshot.protocol_class_method_decl_count,
          snapshot.protocol_instance_method_decl_count,
          snapshot.protocol_method_decl_count);
  const bool interface_method_bucket_inconsistent =
      !AreObjc3ParserMethodDeclBucketsConsistent(
          snapshot.interface_class_method_decl_count,
          snapshot.interface_instance_method_decl_count,
          snapshot.interface_method_decl_count);
  const bool implementation_method_bucket_inconsistent =
      !AreObjc3ParserMethodDeclBucketsConsistent(
          snapshot.implementation_class_method_decl_count,
          snapshot.implementation_instance_method_decl_count,
          snapshot.implementation_method_decl_count);
  const bool protocol_property_count_matches =
      snapshot.protocol_property_decl_count == protocol_property_count;
  const bool interface_method_count_matches =
      snapshot.interface_method_decl_count == interface_method_count;
  const bool implementation_method_count_matches =
      snapshot.implementation_method_decl_count == implementation_method_count;
  return snapshot.ast_shape_fingerprint == 0u ||
         snapshot.ast_top_level_layout_fingerprint == 0u ||
         decl_bucket_overflow ||
         missing_decl_buckets ||
         protocol_method_bucket_inconsistent ||
         interface_method_bucket_inconsistent ||
         implementation_method_bucket_inconsistent ||
         !protocol_property_count_matches ||
         !interface_method_count_matches ||
         !implementation_method_count_matches ||
         (snapshot.top_level_declaration_count == 0u &&
          BuildObjc3ParserContractTopLevelCountFromDeclBuckets(snapshot) != 0u) ||
         (snapshot.protocol_property_decl_count == 0u && protocol_property_count != 0u) ||
         (snapshot.protocol_property_decl_count > protocol_property_count) ||
         (snapshot.protocol_method_decl_count == 0u && protocol_method_count != 0u) ||
         (snapshot.protocol_method_decl_count > protocol_method_count) ||
         (snapshot.protocol_class_method_decl_count == 0u &&
          protocol_class_method_count != 0u) ||
         (snapshot.protocol_class_method_decl_count > protocol_class_method_count) ||
         (snapshot.protocol_instance_method_decl_count == 0u &&
          protocol_instance_method_count != 0u) ||
         (snapshot.protocol_instance_method_decl_count >
          protocol_instance_method_count) ||
         (snapshot.interface_property_decl_count == 0u && interface_property_count != 0u) ||
         (snapshot.interface_property_decl_count > interface_property_count) ||
         (snapshot.interface_method_decl_count == 0u && interface_method_count != 0u) ||
         (snapshot.interface_method_decl_count > interface_method_count) ||
         (snapshot.interface_class_method_decl_count == 0u &&
          interface_class_method_count != 0u) ||
         (snapshot.interface_class_method_decl_count > interface_class_method_count) ||
         (snapshot.interface_instance_method_decl_count == 0u &&
          interface_instance_method_count != 0u) ||
         (snapshot.interface_instance_method_decl_count >
          interface_instance_method_count) ||
         (snapshot.implementation_property_decl_count == 0u &&
          implementation_property_count != 0u) ||
         (snapshot.implementation_property_decl_count > implementation_property_count) ||
         (snapshot.implementation_method_decl_count == 0u &&
          implementation_method_count != 0u) ||
         (snapshot.implementation_method_decl_count > implementation_method_count) ||
         (snapshot.implementation_class_method_decl_count == 0u &&
          implementation_class_method_count != 0u) ||
         (snapshot.implementation_class_method_decl_count >
          implementation_class_method_count) ||
         (snapshot.implementation_instance_method_decl_count == 0u &&
          implementation_instance_method_count != 0u) ||
         (snapshot.implementation_instance_method_decl_count >
          implementation_instance_method_count) ||
         (snapshot.interface_category_decl_count == 0u && interface_category_count != 0u) ||
         (snapshot.implementation_category_decl_count == 0u && implementation_category_count != 0u) ||
         (snapshot.function_prototype_count == 0u && function_prototype_count != 0u) ||
         (snapshot.function_pure_count == 0u && function_pure_count != 0u);
}
