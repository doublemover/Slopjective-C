#include "ir/objc3_ir_function_signature_model.h"

#include <algorithm>
#include <limits>
#include <unordered_set>
#include <utility>

#include "sema/objc3_typed_throws_effect_contract.h"
#include "support/objc3_string_predicates.h"

bool IsArcExecutableObjectParam(const FuncParam &param) {
  return param.id_spelling || param.instancetype_spelling ||
         param.object_pointer_type_spelling;
}

bool IsArcExecutableObjectReturn(const FunctionDecl &fn) {
  return fn.return_id_spelling || fn.return_instancetype_spelling ||
         fn.return_object_pointer_type_spelling;
}

bool IsArcExecutableObjectReturn(const Objc3MethodDecl &method) {
  return method.return_id_spelling || method.return_instancetype_spelling ||
         method.return_object_pointer_type_spelling;
}

bool IsNSErrorOutParameterSite(const FuncParam &param) {
  if (!param.object_pointer_type_spelling) {
    return false;
  }
  const std::string lowered_type =
      objc3c::support::LowercaseAscii(param.object_pointer_type_name);
  if (lowered_type != "nserror") {
    return false;
  }
  const std::string lowered_name =
      objc3c::support::LowercaseAscii(param.name);
  return param.has_pointer_declarator ||
         lowered_name.find("error") != std::string::npos;
}

bool EffectiveArcParamInsertRetain(const FuncParam &param,
                                   bool arc_mode_enabled) {
  return param.ownership_insert_retain ||
         (arc_mode_enabled && !param.has_ownership_qualifier &&
          IsArcExecutableObjectParam(param));
}

bool EffectiveArcParamInsertRelease(const FuncParam &param,
                                    bool arc_mode_enabled) {
  return param.ownership_insert_release ||
         (arc_mode_enabled && !param.has_ownership_qualifier &&
          IsArcExecutableObjectParam(param));
}

bool EffectiveArcReturnInsertRetain(const FunctionDecl &fn,
                                    bool arc_mode_enabled) {
  return fn.return_ownership_insert_retain ||
         (arc_mode_enabled && !fn.has_return_ownership_qualifier &&
          IsArcExecutableObjectReturn(fn));
}

bool EffectiveArcReturnInsertRetain(const Objc3MethodDecl &method,
                                    bool arc_mode_enabled) {
  return method.return_ownership_insert_retain ||
         (arc_mode_enabled && !method.has_return_ownership_qualifier &&
          IsArcExecutableObjectReturn(method));
}

bool EffectiveArcReturnInsertAutorelease(const FunctionDecl &fn) {
  return fn.return_ownership_insert_autorelease;
}

bool EffectiveArcReturnInsertAutorelease(const Objc3MethodDecl &method) {
  return method.return_ownership_insert_autorelease;
}

bool HasInteropInteropSurface(const FunctionDecl &fn) {
  return fn.objc_foreign_declared || fn.objc_import_module_declared ||
         fn.objc_cxx_name_declared || fn.objc_header_name_declared ||
         fn.objc_swift_name_declared || fn.objc_swift_private_declared ||
         fn.objc_export_header_declared || fn.objc_abi_align_declared ||
         fn.objc_foreign_type_declared || fn.objc_mixed_image_declared ||
         fn.objc_package_entry_declared;
}

bool HasInteropLifetimeBridge(const FuncParam &param) {
  return !param.ownership_lifetime_profile.empty();
}

bool HasInteropLifetimeBridge(const FunctionDecl &fn) {
  if (!fn.return_ownership_lifetime_profile.empty()) {
    return true;
  }
  return std::any_of(fn.params.begin(), fn.params.end(),
                     [](const FuncParam &param) {
                       return HasInteropLifetimeBridge(param);
                     });
}

void MarkValueOptionalSignature(
    LoweredFunctionSignature &signature,
    const Objc3ValueOptionalTypeDescriptor &descriptor) {
  if (!descriptor.present) {
    return;
  }
  if (!signature.has_value_optional_type_signature) {
    signature.value_optional_lowering_supported = true;
  }
  signature.has_value_optional_type_signature = true;
  signature.value_optional_lowering_supported =
      signature.value_optional_lowering_supported &&
      descriptor.lowering_supported;
  if (signature.value_optional_payload_type_spelling.empty()) {
    signature.value_optional_payload_type_spelling =
        descriptor.payload_type_spelling;
  }
}

void PopulateMethodLoweredFunctionSignature(
    LoweredFunctionSignature &signature, const Objc3MethodDecl &method) {
  signature.return_type = method.return_type;
  signature.throws_declared = method.throws_declared;
  signature.typed_throws_declared = method.typed_throws_declared;
  signature.throws_error_out_abi_ready =
      Objc3TypedThrowsAbiLoweringReady(
          method.throws_declared,
          method.typed_throws_declared,
          method.typed_throws_payload.canonical_spelling);
  signature.typed_throws_error_type_spelling =
      method.typed_throws_payload.canonical_spelling;
  MarkValueOptionalSignature(signature, method.return_value_optional);
  signature.objc_nserror_declared = method.objc_nserror_declared;
  signature.objc_status_code_declared = method.objc_status_code_declared;
  signature.objc_status_code_mapping_symbol =
      method.objc_status_code_mapping_symbol;
  signature.return_insert_retain =
      EffectiveArcReturnInsertRetain(method, false);
  signature.return_insert_release = method.return_ownership_insert_release;
  signature.return_insert_autorelease =
      EffectiveArcReturnInsertAutorelease(method);
  signature.return_has_lifetime_bridge =
      !method.return_ownership_lifetime_profile.empty();
  if (!method.objc_status_code_success_literal.empty()) {
    try {
      signature.objc_status_code_success_literal =
          std::stoi(method.objc_status_code_success_literal);
    } catch (...) {
      signature.objc_status_code_success_literal = 0;
    }
  }
  signature.param_types.reserve(method.params.size());
  signature.param_insert_retain.reserve(method.params.size());
  signature.param_insert_release.reserve(method.params.size());
  signature.param_insert_autorelease.reserve(method.params.size());
  signature.param_has_lifetime_bridge.reserve(method.params.size());
  for (std::size_t param_index = 0; param_index < method.params.size();
       ++param_index) {
    const auto &param = method.params[param_index];
    MarkValueOptionalSignature(signature, param.value_optional);
    signature.param_types.push_back(param.type);
    signature.param_insert_retain.push_back(
        EffectiveArcParamInsertRetain(param, false));
    signature.param_insert_release.push_back(
        EffectiveArcParamInsertRelease(param, false));
    signature.param_insert_autorelease.push_back(
        param.ownership_insert_autorelease);
    signature.param_has_lifetime_bridge.push_back(
        HasInteropLifetimeBridge(param));
    if (signature.ns_error_out_param_index ==
            std::numeric_limits<std::size_t>::max() &&
        IsNSErrorOutParameterSite(param)) {
      signature.ns_error_out_param_index = param_index;
    }
  }
}

std::map<std::string, LoweredFunctionSignature>
BuildLoweredFunctionSignatures(const Objc3Program &program) {
  std::map<std::string, LoweredFunctionSignature> signatures;
  for (const auto &fn : program.functions) {
    LoweredFunctionSignature signature;
    signature.return_type = fn.return_type;
    signature.throws_declared = fn.throws_declared;
    signature.typed_throws_declared = fn.typed_throws_declared;
    signature.throws_error_out_abi_ready =
        Objc3TypedThrowsAbiLoweringReady(
            fn.throws_declared,
            fn.typed_throws_declared,
            fn.typed_throws_payload.canonical_spelling);
    signature.typed_throws_error_type_spelling =
        fn.typed_throws_payload.canonical_spelling;
    MarkValueOptionalSignature(signature, fn.return_value_optional);
    signature.objc_nserror_declared = fn.objc_nserror_declared;
    signature.objc_status_code_declared = fn.objc_status_code_declared;
    signature.objc_status_code_mapping_symbol =
        fn.objc_status_code_mapping_symbol;
    signature.return_insert_retain = EffectiveArcReturnInsertRetain(fn, false);
    signature.return_insert_release = fn.return_ownership_insert_release;
    signature.return_insert_autorelease =
        EffectiveArcReturnInsertAutorelease(fn);
    signature.return_has_lifetime_bridge =
        !fn.return_ownership_lifetime_profile.empty();
    signature.interop_foreign_surface = HasInteropInteropSurface(fn);
    signature.interop_c_foreign_callable = fn.objc_foreign_declared;
    signature.interop_metadata_preservation = HasInteropInteropSurface(fn);
    if (!fn.objc_status_code_success_literal.empty()) {
      try {
        signature.objc_status_code_success_literal =
            std::stoi(fn.objc_status_code_success_literal);
      } catch (...) {
        signature.objc_status_code_success_literal = 0;
      }
    }
    signature.param_types.reserve(fn.params.size());
    signature.param_insert_retain.reserve(fn.params.size());
    signature.param_insert_release.reserve(fn.params.size());
    signature.param_insert_autorelease.reserve(fn.params.size());
    signature.param_has_lifetime_bridge.reserve(fn.params.size());
    for (std::size_t param_index = 0; param_index < fn.params.size();
         ++param_index) {
      const auto &param = fn.params[param_index];
      MarkValueOptionalSignature(signature, param.value_optional);
      signature.param_types.push_back(param.type);
      signature.param_insert_retain.push_back(
          EffectiveArcParamInsertRetain(param, false));
      signature.param_insert_release.push_back(
          EffectiveArcParamInsertRelease(param, false));
      signature.param_insert_autorelease.push_back(
          param.ownership_insert_autorelease);
      signature.param_has_lifetime_bridge.push_back(
          HasInteropLifetimeBridge(param));
      if (signature.ns_error_out_param_index ==
              std::numeric_limits<std::size_t>::max() &&
          IsNSErrorOutParameterSite(param)) {
        signature.ns_error_out_param_index = param_index;
      }
    }
    auto existing = signatures.find(fn.name);
    if (existing == signatures.end()) {
      signatures.emplace(fn.name, std::move(signature));
    }
  }
  const auto register_method_signature =
      [&](const Objc3MethodDecl &method) {
        if (method.selector.empty() ||
            signatures.find(method.selector) != signatures.end()) {
          return;
        }
        LoweredFunctionSignature signature;
        PopulateMethodLoweredFunctionSignature(signature, method);
        signatures.emplace(method.selector, std::move(signature));
      };
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method : protocol_decl.methods) {
      register_method_signature(method);
    }
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      register_method_signature(method);
    }
  }
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &method : implementation_decl.methods) {
      register_method_signature(method);
    }
  }
  return signatures;
}

std::size_t CountVectorSignatureFunctions(const Objc3Program &program) {
  std::unordered_set<std::string> vector_function_names;
  for (const auto &fn : program.functions) {
    bool has_vector_signature = fn.return_vector_spelling;
    if (!has_vector_signature) {
      for (const auto &param : fn.params) {
        if (param.vector_spelling) {
          has_vector_signature = true;
          break;
        }
      }
    }
    if (has_vector_signature) {
      vector_function_names.insert(fn.name);
    }
  }
  return vector_function_names.size();
}
