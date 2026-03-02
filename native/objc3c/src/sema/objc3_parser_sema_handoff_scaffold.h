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

inline bool IsObjc3ParserContractTopLevelDeclBucketCompatibilityEdgeCaseSnapshot(
    const Objc3ParserContractSnapshot &snapshot) {
  return snapshot.global_decl_count == 0u && snapshot.protocol_decl_count == 0u &&
         snapshot.interface_decl_count == 0u &&
         snapshot.implementation_decl_count == 0u &&
         snapshot.function_decl_count == 0u;
}

inline bool IsObjc3ParserContractMissingTopLevelDeclBucketsForProgram(
    const Objc3ParserContractSnapshot &snapshot, const Objc3ParsedProgram &program) {
  return IsObjc3ParserContractTopLevelDeclBucketCompatibilityEdgeCaseSnapshot(
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

inline bool IsObjc3ParserContractCompatibilityEdgeCaseSnapshot(
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
NormalizeObjc3ParserContractSnapshotForCompatibilityEdgeCases(
    const Objc3ParserContractSnapshot &snapshot, const Objc3ParsedProgram &program,
    const Objc3SemaCompatibilityMode compatibility_mode, bool &normalized) {
  normalized = false;
  if (compatibility_mode != Objc3SemaCompatibilityMode::Legacy) {
    return snapshot;
  }
  if (!IsObjc3ParserContractCompatibilityEdgeCaseSnapshot(snapshot, program)) {
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
    normalized = true;
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
    normalized = true;
  }
  if (normalized_snapshot.protocol_property_decl_count == 0u &&
      protocol_property_count != 0u) {
    normalized_snapshot.protocol_property_decl_count = protocol_property_count;
    normalized = true;
  }
  if (normalized_snapshot.protocol_property_decl_count > protocol_property_count) {
    normalized_snapshot.protocol_property_decl_count = protocol_property_count;
    normalized = true;
  }
  if (normalized_snapshot.protocol_method_decl_count == 0u &&
      protocol_method_count != 0u) {
    normalized_snapshot.protocol_method_decl_count = protocol_method_count;
    normalized = true;
  }
  if (normalized_snapshot.protocol_method_decl_count > protocol_method_count) {
    normalized_snapshot.protocol_method_decl_count = protocol_method_count;
    normalized = true;
  }
  if (normalized_snapshot.protocol_class_method_decl_count == 0u &&
      protocol_class_method_count != 0u) {
    normalized_snapshot.protocol_class_method_decl_count =
        protocol_class_method_count;
    normalized = true;
  }
  if (normalized_snapshot.protocol_class_method_decl_count >
      protocol_class_method_count) {
    normalized_snapshot.protocol_class_method_decl_count =
        protocol_class_method_count;
    normalized = true;
  }
  if (normalized_snapshot.protocol_instance_method_decl_count == 0u &&
      protocol_instance_method_count != 0u) {
    normalized_snapshot.protocol_instance_method_decl_count =
        protocol_instance_method_count;
    normalized = true;
  }
  if (normalized_snapshot.protocol_instance_method_decl_count >
      protocol_instance_method_count) {
    normalized_snapshot.protocol_instance_method_decl_count =
        protocol_instance_method_count;
    normalized = true;
  }
  if (!AreObjc3ParserMethodDeclBucketsConsistent(
          normalized_snapshot.protocol_class_method_decl_count,
          normalized_snapshot.protocol_instance_method_decl_count,
          normalized_snapshot.protocol_method_decl_count)) {
    normalized_snapshot.protocol_class_method_decl_count =
        protocol_class_method_count;
    normalized_snapshot.protocol_instance_method_decl_count =
        protocol_instance_method_count;
    normalized = true;
  }
  if (normalized_snapshot.interface_property_decl_count == 0u &&
      interface_property_count != 0u) {
    normalized_snapshot.interface_property_decl_count = interface_property_count;
    normalized = true;
  }
  if (normalized_snapshot.interface_property_decl_count > interface_property_count) {
    normalized_snapshot.interface_property_decl_count = interface_property_count;
    normalized = true;
  }
  if (normalized_snapshot.interface_method_decl_count == 0u &&
      interface_method_count != 0u) {
    normalized_snapshot.interface_method_decl_count = interface_method_count;
    normalized = true;
  }
  if (normalized_snapshot.interface_method_decl_count > interface_method_count) {
    normalized_snapshot.interface_method_decl_count = interface_method_count;
    normalized = true;
  }
  if (normalized_snapshot.interface_class_method_decl_count == 0u &&
      interface_class_method_count != 0u) {
    normalized_snapshot.interface_class_method_decl_count =
        interface_class_method_count;
    normalized = true;
  }
  if (normalized_snapshot.interface_class_method_decl_count >
      interface_class_method_count) {
    normalized_snapshot.interface_class_method_decl_count =
        interface_class_method_count;
    normalized = true;
  }
  if (normalized_snapshot.interface_instance_method_decl_count == 0u &&
      interface_instance_method_count != 0u) {
    normalized_snapshot.interface_instance_method_decl_count =
        interface_instance_method_count;
    normalized = true;
  }
  if (normalized_snapshot.interface_instance_method_decl_count >
      interface_instance_method_count) {
    normalized_snapshot.interface_instance_method_decl_count =
        interface_instance_method_count;
    normalized = true;
  }
  if (!AreObjc3ParserMethodDeclBucketsConsistent(
          normalized_snapshot.interface_class_method_decl_count,
          normalized_snapshot.interface_instance_method_decl_count,
          normalized_snapshot.interface_method_decl_count)) {
    normalized_snapshot.interface_class_method_decl_count =
        interface_class_method_count;
    normalized_snapshot.interface_instance_method_decl_count =
        interface_instance_method_count;
    normalized = true;
  }
  if (normalized_snapshot.implementation_property_decl_count == 0u &&
      implementation_property_count != 0u) {
    normalized_snapshot.implementation_property_decl_count = implementation_property_count;
    normalized = true;
  }
  if (normalized_snapshot.implementation_property_decl_count >
      implementation_property_count) {
    normalized_snapshot.implementation_property_decl_count = implementation_property_count;
    normalized = true;
  }
  if (normalized_snapshot.implementation_method_decl_count == 0u &&
      implementation_method_count != 0u) {
    normalized_snapshot.implementation_method_decl_count = implementation_method_count;
    normalized = true;
  }
  if (normalized_snapshot.implementation_method_decl_count >
      implementation_method_count) {
    normalized_snapshot.implementation_method_decl_count = implementation_method_count;
    normalized = true;
  }
  if (normalized_snapshot.implementation_class_method_decl_count == 0u &&
      implementation_class_method_count != 0u) {
    normalized_snapshot.implementation_class_method_decl_count =
        implementation_class_method_count;
    normalized = true;
  }
  if (normalized_snapshot.implementation_class_method_decl_count >
      implementation_class_method_count) {
    normalized_snapshot.implementation_class_method_decl_count =
        implementation_class_method_count;
    normalized = true;
  }
  if (normalized_snapshot.implementation_instance_method_decl_count == 0u &&
      implementation_instance_method_count != 0u) {
    normalized_snapshot.implementation_instance_method_decl_count =
        implementation_instance_method_count;
    normalized = true;
  }
  if (normalized_snapshot.implementation_instance_method_decl_count >
      implementation_instance_method_count) {
    normalized_snapshot.implementation_instance_method_decl_count =
        implementation_instance_method_count;
    normalized = true;
  }
  if (!AreObjc3ParserMethodDeclBucketsConsistent(
          normalized_snapshot.implementation_class_method_decl_count,
          normalized_snapshot.implementation_instance_method_decl_count,
          normalized_snapshot.implementation_method_decl_count)) {
    normalized_snapshot.implementation_class_method_decl_count =
        implementation_class_method_count;
    normalized_snapshot.implementation_instance_method_decl_count =
        implementation_instance_method_count;
    normalized = true;
  }
  if (normalized_snapshot.interface_category_decl_count == 0u &&
      interface_category_count != 0u) {
    normalized_snapshot.interface_category_decl_count = interface_category_count;
    normalized = true;
  }
  if (normalized_snapshot.implementation_category_decl_count == 0u &&
      implementation_category_count != 0u) {
    normalized_snapshot.implementation_category_decl_count = implementation_category_count;
    normalized = true;
  }
  if (normalized_snapshot.function_prototype_count == 0u &&
      function_prototype_count != 0u) {
    normalized_snapshot.function_prototype_count = function_prototype_count;
    normalized = true;
  }
  if (normalized_snapshot.function_pure_count == 0u &&
      function_pure_count != 0u) {
    normalized_snapshot.function_pure_count = function_pure_count;
    normalized = true;
  }
  if (normalized_snapshot.ast_shape_fingerprint == 0u) {
    normalized_snapshot.ast_shape_fingerprint =
        BuildObjc3ParsedProgramAstShapeFingerprint(program);
    normalized = true;
  }
  if (normalized_snapshot.ast_top_level_layout_fingerprint == 0u) {
    normalized_snapshot.ast_top_level_layout_fingerprint =
        BuildObjc3ParsedProgramTopLevelLayoutFingerprint(program);
    normalized = true;
  }
  return normalized_snapshot;
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

inline Objc3ParserSemaPerformanceQualityGuardrails BuildObjc3ParserSemaPerformanceQualityGuardrails(
    const Objc3ParserSemaConformanceMatrix &matrix,
    const Objc3ParserSemaConformanceCorpus &corpus) {
  Objc3ParserSemaPerformanceQualityGuardrails guardrails;
  guardrails.conformance_matrix_builder_max_lines = kObjc3ParserSemaConformanceMatrixBuilderMaxLines;
  guardrails.conformance_corpus_builder_max_lines = kObjc3ParserSemaConformanceCorpusBuilderMaxLines;
  guardrails.handoff_scaffold_builder_max_lines = kObjc3ParserSemaHandoffScaffoldBuilderMaxLines;
  guardrails.conformance_matrix_builder_budget_guarded =
      guardrails.conformance_matrix_builder_max_lines >= 150u &&
      guardrails.conformance_matrix_builder_max_lines <= 220u;
  guardrails.conformance_corpus_builder_budget_guarded =
      guardrails.conformance_corpus_builder_max_lines >= 40u &&
      guardrails.conformance_corpus_builder_max_lines <= 100u;
  guardrails.handoff_scaffold_builder_budget_guarded =
      guardrails.handoff_scaffold_builder_max_lines >= 40u &&
      guardrails.handoff_scaffold_builder_max_lines <= 120u;
  guardrails.matrix_diagnostic_budget_consistent =
      matrix.parser_diagnostic_budget_consistent;
  guardrails.matrix_token_top_level_budget_consistent =
      matrix.parser_token_top_level_budget_consistent;
  guardrails.matrix_subset_budget_consistent = matrix.parser_subset_count_consistent;
  guardrails.corpus_case_budget_consistent =
      corpus.required_case_count == 5u &&
      corpus.passed_case_count == corpus.required_case_count &&
      corpus.failed_case_count == 0u;
  guardrails.required_guardrail_count = 7u;
  guardrails.passed_guardrail_count =
      static_cast<std::size_t>(guardrails.conformance_matrix_builder_budget_guarded) +
      static_cast<std::size_t>(guardrails.conformance_corpus_builder_budget_guarded) +
      static_cast<std::size_t>(guardrails.handoff_scaffold_builder_budget_guarded) +
      static_cast<std::size_t>(guardrails.matrix_diagnostic_budget_consistent) +
      static_cast<std::size_t>(guardrails.matrix_token_top_level_budget_consistent) +
      static_cast<std::size_t>(guardrails.matrix_subset_budget_consistent) +
      static_cast<std::size_t>(guardrails.corpus_case_budget_consistent);
  guardrails.failed_guardrail_count =
      guardrails.required_guardrail_count >= guardrails.passed_guardrail_count
          ? (guardrails.required_guardrail_count - guardrails.passed_guardrail_count)
          : guardrails.required_guardrail_count;
  guardrails.deterministic =
      matrix.deterministic &&
      corpus.deterministic &&
      guardrails.required_guardrail_count == 7u &&
      guardrails.passed_guardrail_count == guardrails.required_guardrail_count &&
      guardrails.failed_guardrail_count == 0u;
  return guardrails;
}

struct Objc3ParserSemaHandoffScaffold {
  const Objc3ParsedProgram *program = nullptr;
  Objc3SemanticValidationOptions validation_options;
  Objc3SemaCompatibilityMode compatibility_mode = Objc3SemaCompatibilityMode::Canonical;
  bool migration_assist = false;
  Objc3SemaMigrationHints migration_hints;
  Objc3SemaDiagnosticsBus diagnostics_bus;
  Objc3ParserContractSnapshot parser_contract_snapshot;
  bool parser_contract_compatibility_edge_case_detected = false;
  bool parser_contract_snapshot_compatibility_normalized = false;
  std::uint64_t expected_ast_shape_fingerprint = 0;
  bool parser_contract_ast_shape_fingerprint_matches = false;
  std::uint64_t expected_ast_top_level_layout_fingerprint = 0;
  bool parser_contract_ast_top_level_layout_fingerprint_matches = false;
  std::uint64_t expected_parser_contract_snapshot_fingerprint = 0;
  std::uint64_t parser_contract_snapshot_fingerprint = 0;
  bool parser_contract_snapshot_fingerprint_matches = false;
  Objc3ParserSemaConformanceMatrix parser_sema_conformance_matrix;
  Objc3ParserSemaConformanceCorpus parser_sema_conformance_corpus;
  Objc3ParserSemaPerformanceQualityGuardrails parser_sema_performance_quality_guardrails;
  bool parser_contract_snapshot_matches_program = false;
  bool deterministic = false;
};

inline Objc3ParserSemaHandoffScaffold BuildObjc3ParserSemaHandoffScaffold(const Objc3SemaPassManagerInput &input) {
  Objc3ParserSemaHandoffScaffold scaffold;
  scaffold.program = input.program;
  scaffold.validation_options = input.validation_options;
  scaffold.compatibility_mode = input.compatibility_mode;
  scaffold.migration_assist = input.migration_assist;
  scaffold.migration_hints = input.migration_hints;
  scaffold.diagnostics_bus = input.diagnostics_bus;
  if (input.program == nullptr) {
    return scaffold;
  }

  const Objc3ParserContractSnapshot resolved_snapshot = ResolveObjc3ParserContractSnapshotForSemaHandoff(input);
  scaffold.parser_contract_compatibility_edge_case_detected =
      input.compatibility_mode == Objc3SemaCompatibilityMode::Legacy &&
      IsObjc3ParserContractCompatibilityEdgeCaseSnapshot(resolved_snapshot, *input.program);
  scaffold.parser_contract_snapshot = NormalizeObjc3ParserContractSnapshotForCompatibilityEdgeCases(
      resolved_snapshot,
      *input.program,
      input.compatibility_mode,
      scaffold.parser_contract_snapshot_compatibility_normalized);
  scaffold.expected_ast_shape_fingerprint = BuildObjc3ParsedProgramAstShapeFingerprint(*input.program);
  scaffold.parser_contract_ast_shape_fingerprint_matches =
      scaffold.parser_contract_snapshot.ast_shape_fingerprint == scaffold.expected_ast_shape_fingerprint;
  scaffold.expected_ast_top_level_layout_fingerprint = BuildObjc3ParsedProgramTopLevelLayoutFingerprint(*input.program);
  scaffold.parser_contract_ast_top_level_layout_fingerprint_matches =
      scaffold.parser_contract_snapshot.ast_top_level_layout_fingerprint ==
      scaffold.expected_ast_top_level_layout_fingerprint;
  const Objc3ParserContractSnapshot expected_snapshot = BuildObjc3ParserContractSnapshot(
      *input.program,
      scaffold.parser_contract_snapshot.parser_diagnostic_count,
      scaffold.parser_contract_snapshot.token_count);
  scaffold.parser_contract_snapshot_fingerprint =
      BuildObjc3ParserContractSnapshotFingerprint(scaffold.parser_contract_snapshot);
  scaffold.expected_parser_contract_snapshot_fingerprint =
      BuildObjc3ParserContractSnapshotFingerprint(expected_snapshot);
  scaffold.parser_contract_snapshot_fingerprint_matches =
      scaffold.parser_contract_snapshot_fingerprint == scaffold.expected_parser_contract_snapshot_fingerprint;
  scaffold.parser_sema_conformance_matrix = BuildObjc3ParserSemaConformanceMatrix(
      scaffold.parser_contract_snapshot, *input.program);
  scaffold.parser_sema_conformance_corpus =
      BuildObjc3ParserSemaConformanceCorpus(scaffold.parser_sema_conformance_matrix);
  scaffold.parser_sema_performance_quality_guardrails =
      BuildObjc3ParserSemaPerformanceQualityGuardrails(
          scaffold.parser_sema_conformance_matrix,
          scaffold.parser_sema_conformance_corpus);
  scaffold.parser_contract_snapshot_matches_program =
      scaffold.parser_sema_conformance_matrix.deterministic;
  scaffold.deterministic = scaffold.parser_contract_snapshot_matches_program;
  scaffold.deterministic = scaffold.deterministic &&
                           scaffold.parser_contract_ast_shape_fingerprint_matches &&
                           scaffold.parser_contract_ast_top_level_layout_fingerprint_matches &&
                           scaffold.parser_contract_snapshot_fingerprint_matches &&
                           scaffold.parser_sema_performance_quality_guardrails.deterministic &&
                           scaffold.parser_sema_conformance_corpus.deterministic &&
                           scaffold.parser_sema_conformance_matrix.deterministic;
  return scaffold;
}
