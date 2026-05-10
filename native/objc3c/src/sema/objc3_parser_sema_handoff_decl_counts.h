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
