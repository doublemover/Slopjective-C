#pragma once

#include "diag/objc3_diag_format.h"
#include "sema/objc3_semantic_passes.h"
#include "support/objc3_ascii_case_transform.h"

#include <cstddef>
#include <string>
#include <vector>

inline void RecordErrorHandlingSemanticDiagnostic(
    std::size_t &contract_violation_sites,
    unsigned line,
    unsigned column,
    const char *code,
    const std::string &message,
    std::vector<std::string> &diagnostics) {
  ++contract_violation_sites;
  diagnostics.push_back(MakeDiag(line, column, code, message));
}

inline const FunctionDecl *FindFunctionDeclByName(const Objc3Program &program,
                                                  const std::string &name) {
  for (const auto &fn : program.functions) {
    if (fn.name == name) {
      return &fn;
    }
  }
  return nullptr;
}

inline bool IsNSErrorTypeSpellingForErrorHandling(const FuncParam &param) {
  if (!param.object_pointer_type_spelling) {
    return false;
  }
  return objc3c::support::LowercaseAscii(param.object_pointer_type_name) ==
         "nserror";
}

inline bool IsNSErrorOutParameterSiteForErrorHandling(const FuncParam &param) {
  if (!IsNSErrorTypeSpellingForErrorHandling(param)) {
    return false;
  }
  const std::string lowered_name = objc3c::support::LowercaseAscii(param.name);
  return param.has_pointer_declarator ||
         lowered_name.find("error") != std::string::npos;
}

template <typename CallableDecl>
bool CallableHasNSErrorOutParameterForErrorHandling(
    const CallableDecl &decl) {
  for (const auto &param : decl.params) {
    if (IsNSErrorOutParameterSiteForErrorHandling(param)) {
      return true;
    }
  }
  return false;
}

template <typename CallableDecl>
bool CallableReturnsNSErrorObjectForErrorHandling(const CallableDecl &decl) {
  return decl.return_object_pointer_type_spelling &&
         objc3c::support::LowercaseAscii(decl.return_object_pointer_type_name) ==
             "nserror";
}

template <typename CallableDecl>
bool CallableReturnsSupportedNSErrorBoolForErrorHandling(
    const CallableDecl &decl) {
  return decl.return_type == ValueType::Bool;
}

template <typename CallableDecl>
bool CallableReturnsSupportedStatusCodeForErrorHandling(
    const CallableDecl &decl) {
  return decl.return_type == ValueType::Bool ||
         decl.return_type == ValueType::I32;
}

template <typename CallableDecl>
bool CallableHasSemanticallyValidNSErrorStatusBridge(
    const CallableDecl &decl,
    const Objc3Program &program) {
  const bool has_marker =
      decl.objc_nserror_attribute_sites != 0u ||
      decl.objc_status_code_attribute_sites != 0u;
  const bool has_profile_bridge_sites =
      decl.ns_error_bridging_profile_is_normalized &&
      decl.ns_error_bridging_sites != 0u;
  if (!has_marker) {
    return has_profile_bridge_sites;
  }
  if (decl.throws_declared ||
      (decl.objc_nserror_declared && decl.objc_status_code_declared)) {
    return false;
  }
  if (decl.objc_nserror_declared) {
    return CallableHasNSErrorOutParameterForErrorHandling(decl) &&
           CallableReturnsSupportedNSErrorBoolForErrorHandling(decl);
  }
  if (decl.objc_status_code_declared) {
    if (!CallableHasNSErrorOutParameterForErrorHandling(decl) ||
        !CallableReturnsSupportedStatusCodeForErrorHandling(decl) ||
        objc3c::support::LowercaseAscii(
            decl.objc_status_code_error_type_spelling) != "nserror") {
      return false;
    }
    const FunctionDecl *mapping =
        FindFunctionDeclByName(program, decl.objc_status_code_mapping_symbol);
    return mapping != nullptr && mapping->params.size() == 1u &&
           mapping->params.front().type == decl.return_type &&
           CallableReturnsNSErrorObjectForErrorHandling(*mapping);
  }
  return has_profile_bridge_sites;
}
