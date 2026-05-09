#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>

#include "sema/objc3_sema_pass_manager_contract.h"

inline Objc3ParserContractSnapshot ResolveObjc3ParserContractSnapshotForSemaHandoff(
    const Objc3SemaPassManagerInput &input) {
  if (input.parser_contract_snapshot != nullptr) {
    return *input.parser_contract_snapshot;
  }
  if (input.program == nullptr) {
    return Objc3ParserContractSnapshot{};
  }
  return BuildObjc3ParserContractSnapshot(*input.program, 0u, 0u);
}

inline bool TryBuildObjc3ParserContractTopLevelCountFromDeclBuckets(
    const Objc3ParserContractSnapshot &snapshot, std::size_t &top_level_count) {
  top_level_count = 0u;
  const std::size_t max_count = std::numeric_limits<std::size_t>::max();
  if (snapshot.global_decl_count > max_count - top_level_count) {
    return false;
  }
  top_level_count += snapshot.global_decl_count;
  if (snapshot.protocol_decl_count > max_count - top_level_count) {
    return false;
  }
  top_level_count += snapshot.protocol_decl_count;
  if (snapshot.interface_decl_count > max_count - top_level_count) {
    return false;
  }
  top_level_count += snapshot.interface_decl_count;
  if (snapshot.implementation_decl_count > max_count - top_level_count) {
    return false;
  }
  top_level_count += snapshot.implementation_decl_count;
  if (snapshot.function_decl_count > max_count - top_level_count) {
    return false;
  }
  top_level_count += snapshot.function_decl_count;
  return true;
}

inline std::size_t BuildObjc3ParserContractTopLevelCountFromDeclBuckets(
    const Objc3ParserContractSnapshot &snapshot) {
  std::size_t top_level_count = 0u;
  if (!TryBuildObjc3ParserContractTopLevelCountFromDeclBuckets(
          snapshot, top_level_count)) {
    return 0u;
  }
  return top_level_count;
}

inline bool IsObjc3ParserContractTopLevelDeclBucketOverflow(
    const Objc3ParserContractSnapshot &snapshot) {
  std::size_t top_level_count = 0u;
  return !TryBuildObjc3ParserContractTopLevelCountFromDeclBuckets(
      snapshot, top_level_count);
}

inline std::size_t BuildObjc3ParserContractTopLevelCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  return ast.globals.size() + ast.protocols.size() + ast.interfaces.size() +
         ast.implementations.size() + ast.functions.size();
}

inline bool IsObjc3ParserContractTopLevelDeclBucketNormalizationCandidateSnapshot(
    const Objc3ParserContractSnapshot &snapshot) {
  return snapshot.global_decl_count == 0u && snapshot.protocol_decl_count == 0u &&
         snapshot.interface_decl_count == 0u &&
         snapshot.implementation_decl_count == 0u &&
         snapshot.function_decl_count == 0u;
}

inline bool IsObjc3ParserContractMissingTopLevelDeclBucketsForProgram(
    const Objc3ParserContractSnapshot &snapshot, const Objc3ParsedProgram &program) {
  return IsObjc3ParserContractTopLevelDeclBucketNormalizationCandidateSnapshot(
             snapshot) &&
         BuildObjc3ParserContractTopLevelCountFromProgram(program) != 0u;
}

inline bool AreObjc3ParserMethodDeclBucketsConsistent(
    const std::size_t class_method_count, const std::size_t instance_method_count,
    const std::size_t total_method_count) {
  const std::size_t max_count = std::numeric_limits<std::size_t>::max();
  if (class_method_count > max_count - instance_method_count) {
    return false;
  }
  return class_method_count + instance_method_count == total_method_count;
}

inline std::size_t BuildObjc3ParserProtocolPropertyDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t property_count = 0u;
  for (const auto &protocol_decl : ast.protocols) {
    property_count += protocol_decl.properties.size();
  }
  return property_count;
}

inline std::size_t BuildObjc3ParserProtocolMethodDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t method_count = 0u;
  for (const auto &protocol_decl : ast.protocols) {
    method_count += protocol_decl.methods.size();
  }
  return method_count;
}

inline std::size_t BuildObjc3ParserProtocolClassMethodDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t method_count = 0u;
  for (const auto &protocol_decl : ast.protocols) {
    method_count += static_cast<std::size_t>(std::count_if(
        protocol_decl.methods.begin(),
        protocol_decl.methods.end(),
        [](const Objc3MethodDecl &method_decl) {
          return method_decl.is_class_method;
        }));
  }
  return method_count;
}

inline std::size_t BuildObjc3ParserProtocolInstanceMethodDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t method_count = 0u;
  for (const auto &protocol_decl : ast.protocols) {
    method_count += static_cast<std::size_t>(std::count_if(
        protocol_decl.methods.begin(),
        protocol_decl.methods.end(),
        [](const Objc3MethodDecl &method_decl) {
          return !method_decl.is_class_method;
        }));
  }
  return method_count;
}

inline std::size_t BuildObjc3ParserInterfacePropertyDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t property_count = 0u;
  for (const auto &interface_decl : ast.interfaces) {
    property_count += interface_decl.properties.size();
  }
  return property_count;
}

inline std::size_t BuildObjc3ParserInterfaceMethodDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t method_count = 0u;
  for (const auto &interface_decl : ast.interfaces) {
    method_count += interface_decl.methods.size();
  }
  return method_count;
}

inline std::size_t BuildObjc3ParserInterfaceClassMethodDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t method_count = 0u;
  for (const auto &interface_decl : ast.interfaces) {
    method_count += static_cast<std::size_t>(std::count_if(
        interface_decl.methods.begin(),
        interface_decl.methods.end(),
        [](const Objc3MethodDecl &method_decl) {
          return method_decl.is_class_method;
        }));
  }
  return method_count;
}

inline std::size_t BuildObjc3ParserInterfaceInstanceMethodDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t method_count = 0u;
  for (const auto &interface_decl : ast.interfaces) {
    method_count += static_cast<std::size_t>(std::count_if(
        interface_decl.methods.begin(),
        interface_decl.methods.end(),
        [](const Objc3MethodDecl &method_decl) {
          return !method_decl.is_class_method;
        }));
  }
  return method_count;
}

inline std::size_t BuildObjc3ParserImplementationPropertyDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t property_count = 0u;
  for (const auto &implementation_decl : ast.implementations) {
    property_count += implementation_decl.properties.size();
  }
  return property_count;
}

inline std::size_t BuildObjc3ParserImplementationMethodDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t method_count = 0u;
  for (const auto &implementation_decl : ast.implementations) {
    method_count += implementation_decl.methods.size();
  }
  return method_count;
}

inline std::size_t BuildObjc3ParserImplementationClassMethodDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t method_count = 0u;
  for (const auto &implementation_decl : ast.implementations) {
    method_count += static_cast<std::size_t>(std::count_if(
        implementation_decl.methods.begin(),
        implementation_decl.methods.end(),
        [](const Objc3MethodDecl &method_decl) {
          return method_decl.is_class_method;
        }));
  }
  return method_count;
}

inline std::size_t BuildObjc3ParserImplementationInstanceMethodDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t method_count = 0u;
  for (const auto &implementation_decl : ast.implementations) {
    method_count += static_cast<std::size_t>(std::count_if(
        implementation_decl.methods.begin(),
        implementation_decl.methods.end(),
        [](const Objc3MethodDecl &method_decl) {
          return !method_decl.is_class_method;
        }));
  }
  return method_count;
}

inline std::size_t BuildObjc3ParserInterfaceCategoryDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  return static_cast<std::size_t>(std::count_if(
      ast.interfaces.begin(),
      ast.interfaces.end(),
      [](const Objc3InterfaceDecl &interface_decl) { return interface_decl.has_category; }));
}

inline std::size_t BuildObjc3ParserImplementationCategoryDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  return static_cast<std::size_t>(std::count_if(
      ast.implementations.begin(),
      ast.implementations.end(),
      [](const Objc3ImplementationDecl &implementation_decl) { return implementation_decl.has_category; }));
}

inline std::size_t BuildObjc3ParserFunctionPrototypeCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  return static_cast<std::size_t>(std::count_if(
      ast.functions.begin(),
      ast.functions.end(),
      [](const FunctionDecl &function_decl) { return function_decl.is_prototype; }));
}

inline std::size_t BuildObjc3ParserFunctionPureCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  return static_cast<std::size_t>(std::count_if(
      ast.functions.begin(),
      ast.functions.end(),
      [](const FunctionDecl &function_decl) { return function_decl.is_pure; }));
}

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

inline constexpr std::size_t kObjc3ParserSemaConformanceMatrixBuilderMaxLines = 190u;
inline constexpr std::size_t kObjc3ParserSemaConformanceCorpusBuilderMaxLines = 75u;
inline constexpr std::size_t kObjc3ParserSemaHandoffScaffoldBuilderMaxLines = 80u;

inline Objc3ParserSemaConformanceMatrix BuildObjc3ParserSemaConformanceMatrix(
    const Objc3ParserContractSnapshot &snapshot, const Objc3ParsedProgram &program) {
  Objc3ParserSemaConformanceMatrix matrix;
  const Objc3ParserContractSnapshot expected_snapshot =
      BuildObjc3ParserContractSnapshot(program, snapshot.parser_diagnostic_count, snapshot.token_count);

  matrix.parser_top_level_declaration_count = snapshot.top_level_declaration_count;
  matrix.ast_top_level_declaration_count = expected_snapshot.top_level_declaration_count;
  matrix.parser_global_decl_count = snapshot.global_decl_count;
  matrix.ast_global_decl_count = expected_snapshot.global_decl_count;
  matrix.parser_protocol_decl_count = snapshot.protocol_decl_count;
  matrix.ast_protocol_decl_count = expected_snapshot.protocol_decl_count;
  matrix.parser_interface_decl_count = snapshot.interface_decl_count;
  matrix.ast_interface_decl_count = expected_snapshot.interface_decl_count;
  matrix.parser_implementation_decl_count = snapshot.implementation_decl_count;
  matrix.ast_implementation_decl_count = expected_snapshot.implementation_decl_count;
  matrix.parser_function_decl_count = snapshot.function_decl_count;
  matrix.ast_function_decl_count = expected_snapshot.function_decl_count;
  matrix.parser_protocol_property_decl_count = snapshot.protocol_property_decl_count;
  matrix.ast_protocol_property_decl_count = expected_snapshot.protocol_property_decl_count;
  matrix.parser_protocol_method_decl_count = snapshot.protocol_method_decl_count;
  matrix.ast_protocol_method_decl_count = expected_snapshot.protocol_method_decl_count;
  matrix.parser_protocol_class_method_decl_count = snapshot.protocol_class_method_decl_count;
  matrix.ast_protocol_class_method_decl_count = expected_snapshot.protocol_class_method_decl_count;
  matrix.parser_protocol_instance_method_decl_count = snapshot.protocol_instance_method_decl_count;
  matrix.ast_protocol_instance_method_decl_count = expected_snapshot.protocol_instance_method_decl_count;
  matrix.parser_interface_property_decl_count = snapshot.interface_property_decl_count;
  matrix.ast_interface_property_decl_count = expected_snapshot.interface_property_decl_count;
  matrix.parser_interface_method_decl_count = snapshot.interface_method_decl_count;
  matrix.ast_interface_method_decl_count = expected_snapshot.interface_method_decl_count;
  matrix.parser_interface_class_method_decl_count = snapshot.interface_class_method_decl_count;
  matrix.ast_interface_class_method_decl_count = expected_snapshot.interface_class_method_decl_count;
  matrix.parser_interface_instance_method_decl_count = snapshot.interface_instance_method_decl_count;
  matrix.ast_interface_instance_method_decl_count = expected_snapshot.interface_instance_method_decl_count;
  matrix.parser_implementation_property_decl_count = snapshot.implementation_property_decl_count;
  matrix.ast_implementation_property_decl_count = expected_snapshot.implementation_property_decl_count;
  matrix.parser_implementation_method_decl_count = snapshot.implementation_method_decl_count;
  matrix.ast_implementation_method_decl_count = expected_snapshot.implementation_method_decl_count;
  matrix.parser_implementation_class_method_decl_count = snapshot.implementation_class_method_decl_count;
  matrix.ast_implementation_class_method_decl_count = expected_snapshot.implementation_class_method_decl_count;
  matrix.parser_implementation_instance_method_decl_count = snapshot.implementation_instance_method_decl_count;
  matrix.ast_implementation_instance_method_decl_count = expected_snapshot.implementation_instance_method_decl_count;
  matrix.parser_interface_category_decl_count = snapshot.interface_category_decl_count;
  matrix.ast_interface_category_decl_count = expected_snapshot.interface_category_decl_count;
  matrix.parser_implementation_category_decl_count = snapshot.implementation_category_decl_count;
  matrix.ast_implementation_category_decl_count = expected_snapshot.implementation_category_decl_count;
  matrix.parser_function_prototype_count = snapshot.function_prototype_count;
  matrix.ast_function_prototype_count = expected_snapshot.function_prototype_count;
  matrix.parser_function_pure_count = snapshot.function_pure_count;
  matrix.ast_function_pure_count = expected_snapshot.function_pure_count;
  matrix.parser_ast_shape_fingerprint = snapshot.ast_shape_fingerprint;
  matrix.ast_shape_fingerprint = expected_snapshot.ast_shape_fingerprint;
  matrix.parser_ast_top_level_layout_fingerprint = snapshot.ast_top_level_layout_fingerprint;
  const std::uint64_t ast_top_level_layout_fingerprint =
      expected_snapshot.ast_top_level_layout_fingerprint;
  matrix.ast_top_level_layout_fingerprint = ast_top_level_layout_fingerprint;
  matrix.parser_contract_snapshot_fingerprint =
      BuildObjc3ParserContractSnapshotFingerprint(snapshot);
  matrix.expected_parser_contract_snapshot_fingerprint =
      BuildObjc3ParserContractSnapshotFingerprint(expected_snapshot);

  std::size_t top_level_decl_count_from_buckets = 0u;
  const bool top_level_decl_buckets_consistent =
      TryBuildObjc3ParserContractTopLevelCountFromDeclBuckets(snapshot, top_level_decl_count_from_buckets) &&
      snapshot.top_level_declaration_count == top_level_decl_count_from_buckets;
  matrix.top_level_declaration_count_matches =
      snapshot.top_level_declaration_count == expected_snapshot.top_level_declaration_count &&
      top_level_decl_buckets_consistent;
  matrix.global_decl_count_matches =
      snapshot.global_decl_count == expected_snapshot.global_decl_count;
  matrix.protocol_decl_count_matches =
      snapshot.protocol_decl_count == expected_snapshot.protocol_decl_count;
  matrix.interface_decl_count_matches =
      snapshot.interface_decl_count == expected_snapshot.interface_decl_count;
  matrix.implementation_decl_count_matches =
      snapshot.implementation_decl_count == expected_snapshot.implementation_decl_count;
  matrix.function_decl_count_matches =
      snapshot.function_decl_count == expected_snapshot.function_decl_count;
  matrix.protocol_property_decl_count_matches =
      snapshot.protocol_property_decl_count == expected_snapshot.protocol_property_decl_count;
  matrix.protocol_method_decl_count_matches =
      snapshot.protocol_method_decl_count == expected_snapshot.protocol_method_decl_count;
  matrix.protocol_class_method_decl_count_matches =
      snapshot.protocol_class_method_decl_count ==
      expected_snapshot.protocol_class_method_decl_count;
  matrix.protocol_instance_method_decl_count_matches =
      snapshot.protocol_instance_method_decl_count ==
      expected_snapshot.protocol_instance_method_decl_count;
  matrix.interface_property_decl_count_matches =
      snapshot.interface_property_decl_count == expected_snapshot.interface_property_decl_count;
  matrix.interface_method_decl_count_matches =
      snapshot.interface_method_decl_count == expected_snapshot.interface_method_decl_count;
  matrix.interface_class_method_decl_count_matches =
      snapshot.interface_class_method_decl_count ==
      expected_snapshot.interface_class_method_decl_count;
  matrix.interface_instance_method_decl_count_matches =
      snapshot.interface_instance_method_decl_count ==
      expected_snapshot.interface_instance_method_decl_count;
  matrix.implementation_property_decl_count_matches =
      snapshot.implementation_property_decl_count ==
      expected_snapshot.implementation_property_decl_count;
  matrix.implementation_method_decl_count_matches =
      snapshot.implementation_method_decl_count ==
      expected_snapshot.implementation_method_decl_count;
  matrix.implementation_class_method_decl_count_matches =
      snapshot.implementation_class_method_decl_count ==
      expected_snapshot.implementation_class_method_decl_count;
  matrix.implementation_instance_method_decl_count_matches =
      snapshot.implementation_instance_method_decl_count ==
      expected_snapshot.implementation_instance_method_decl_count;
  matrix.interface_category_decl_count_matches =
      snapshot.interface_category_decl_count ==
      expected_snapshot.interface_category_decl_count;
  matrix.implementation_category_decl_count_matches =
      snapshot.implementation_category_decl_count ==
      expected_snapshot.implementation_category_decl_count;
  matrix.function_prototype_count_matches =
      snapshot.function_prototype_count == expected_snapshot.function_prototype_count;
  matrix.function_pure_count_matches =
      snapshot.function_pure_count == expected_snapshot.function_pure_count;
  matrix.ast_shape_fingerprint_matches =
      snapshot.ast_shape_fingerprint == expected_snapshot.ast_shape_fingerprint;
  matrix.ast_top_level_layout_fingerprint_matches =
      snapshot.ast_top_level_layout_fingerprint == ast_top_level_layout_fingerprint;
  matrix.parser_contract_snapshot_fingerprint_matches =
      matrix.parser_contract_snapshot_fingerprint ==
      matrix.expected_parser_contract_snapshot_fingerprint;
  const bool parser_diagnostic_budget_consistent =
      snapshot.token_count == 0u || snapshot.parser_diagnostic_count <= snapshot.token_count;
  const bool parser_token_top_level_budget_consistent =
      snapshot.token_count == 0u || snapshot.token_count >= snapshot.top_level_declaration_count;
  const bool parser_subset_count_consistent =
      AreObjc3ParserMethodDeclBucketsConsistent(
          snapshot.protocol_class_method_decl_count,
          snapshot.protocol_instance_method_decl_count,
          snapshot.protocol_method_decl_count) &&
      AreObjc3ParserMethodDeclBucketsConsistent(
          snapshot.interface_class_method_decl_count,
          snapshot.interface_instance_method_decl_count,
          snapshot.interface_method_decl_count) &&
      AreObjc3ParserMethodDeclBucketsConsistent(
          snapshot.implementation_class_method_decl_count,
          snapshot.implementation_instance_method_decl_count,
          snapshot.implementation_method_decl_count) &&
      snapshot.interface_category_decl_count <= snapshot.interface_decl_count &&
      snapshot.implementation_category_decl_count <= snapshot.implementation_decl_count &&
      snapshot.function_prototype_count <= snapshot.function_decl_count &&
      snapshot.function_pure_count <= snapshot.function_decl_count;
  matrix.parser_diagnostic_budget_consistent = parser_diagnostic_budget_consistent;
  matrix.parser_token_top_level_budget_consistent = parser_token_top_level_budget_consistent;
  matrix.parser_subset_count_consistent = parser_subset_count_consistent;
  matrix.parser_contract_snapshot_deterministic = snapshot.deterministic_handoff;
  matrix.parser_recovery_replay_ready = snapshot.parser_recovery_replay_ready;
  matrix.deterministic =
      matrix.top_level_declaration_count_matches &&
      matrix.global_decl_count_matches &&
      matrix.protocol_decl_count_matches &&
      matrix.interface_decl_count_matches &&
      matrix.implementation_decl_count_matches &&
      matrix.function_decl_count_matches &&
      matrix.protocol_property_decl_count_matches &&
      matrix.protocol_method_decl_count_matches &&
      matrix.protocol_class_method_decl_count_matches &&
      matrix.protocol_instance_method_decl_count_matches &&
      matrix.interface_property_decl_count_matches &&
      matrix.interface_method_decl_count_matches &&
      matrix.interface_class_method_decl_count_matches &&
      matrix.interface_instance_method_decl_count_matches &&
      matrix.implementation_property_decl_count_matches &&
      matrix.implementation_method_decl_count_matches &&
      matrix.implementation_class_method_decl_count_matches &&
      matrix.implementation_instance_method_decl_count_matches &&
      matrix.interface_category_decl_count_matches &&
      matrix.implementation_category_decl_count_matches &&
      matrix.function_prototype_count_matches &&
      matrix.function_pure_count_matches &&
      matrix.ast_shape_fingerprint_matches &&
      matrix.ast_top_level_layout_fingerprint_matches &&
      matrix.parser_contract_snapshot_fingerprint_matches &&
      matrix.parser_diagnostic_budget_consistent &&
      matrix.parser_token_top_level_budget_consistent &&
      matrix.parser_subset_count_consistent &&
      matrix.parser_contract_snapshot_deterministic &&
      matrix.parser_recovery_replay_ready;
  return matrix;
}

inline bool IsObjc3ParserContractSnapshotConsistentWithProgram(
    const Objc3ParserContractSnapshot &snapshot, const Objc3ParsedProgram &program) {
  return BuildObjc3ParserSemaConformanceMatrix(snapshot, program).deterministic;
}

inline Objc3ParserSemaConformanceCorpus BuildObjc3ParserSemaConformanceCorpus(
    const Objc3ParserSemaConformanceMatrix &matrix) {
  Objc3ParserSemaConformanceCorpus corpus;
  corpus.has_top_level_declaration_count_case = true;
  corpus.has_snapshot_fingerprint_case = true;
  corpus.has_diagnostic_budget_case = true;
  corpus.has_subset_count_case = true;
  corpus.has_recovery_replay_case = true;
  corpus.top_level_declaration_count_case_passed =
      matrix.top_level_declaration_count_matches;
  corpus.snapshot_fingerprint_case_passed =
      matrix.parser_contract_snapshot_fingerprint_matches;
  corpus.diagnostic_budget_case_passed =
      matrix.parser_diagnostic_budget_consistent &&
      matrix.parser_token_top_level_budget_consistent;
  corpus.subset_count_case_passed = matrix.parser_subset_count_consistent;
  corpus.recovery_replay_case_passed =
      matrix.parser_contract_snapshot_deterministic &&
      matrix.parser_recovery_replay_ready;
  corpus.required_case_count =
      static_cast<std::size_t>(corpus.has_top_level_declaration_count_case) +
      static_cast<std::size_t>(corpus.has_snapshot_fingerprint_case) +
      static_cast<std::size_t>(corpus.has_diagnostic_budget_case) +
      static_cast<std::size_t>(corpus.has_subset_count_case) +
      static_cast<std::size_t>(corpus.has_recovery_replay_case);
  corpus.passed_case_count =
      static_cast<std::size_t>(corpus.top_level_declaration_count_case_passed) +
      static_cast<std::size_t>(corpus.snapshot_fingerprint_case_passed) +
      static_cast<std::size_t>(corpus.diagnostic_budget_case_passed) +
      static_cast<std::size_t>(corpus.subset_count_case_passed) +
      static_cast<std::size_t>(corpus.recovery_replay_case_passed);
  corpus.failed_case_count =
      corpus.required_case_count >= corpus.passed_case_count
          ? (corpus.required_case_count - corpus.passed_case_count)
          : corpus.required_case_count;
  corpus.deterministic = matrix.deterministic && corpus.required_case_count == 5u &&
                         corpus.passed_case_count == corpus.required_case_count &&
                         corpus.failed_case_count == 0u;
  return corpus;
}
