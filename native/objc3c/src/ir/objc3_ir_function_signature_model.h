#pragma once

#include <cstddef>
#include <limits>
#include <map>
#include <string>
#include <vector>

#include "ast/objc3_ast.h"

struct LoweredFunctionSignature {
  ValueType return_type = ValueType::I32;
  std::vector<ValueType> param_types;
  std::vector<bool> param_insert_retain;
  std::vector<bool> param_insert_release;
  std::vector<bool> param_insert_autorelease;
  std::vector<bool> param_has_lifetime_bridge;
  bool throws_declared = false;
  bool objc_nserror_declared = false;
  bool objc_status_code_declared = false;
  bool return_insert_retain = false;
  bool return_insert_release = false;
  bool return_insert_autorelease = false;
  bool return_has_lifetime_bridge = false;
  bool interop_foreign_surface = false;
  bool interop_c_foreign_callable = false;
  bool interop_metadata_preservation = false;
  std::size_t ns_error_out_param_index =
      std::numeric_limits<std::size_t>::max();
  int objc_status_code_success_literal = 0;
  std::string objc_status_code_mapping_symbol;
};

bool IsArcExecutableObjectParam(const FuncParam &param);
bool IsArcExecutableObjectReturn(const FunctionDecl &fn);
bool IsArcExecutableObjectReturn(const Objc3MethodDecl &method);

bool IsNSErrorOutParameterSite(const FuncParam &param);
bool EffectiveArcParamInsertRetain(const FuncParam &param,
                                   bool arc_mode_enabled);
bool EffectiveArcParamInsertRelease(const FuncParam &param,
                                    bool arc_mode_enabled);
bool EffectiveArcReturnInsertRetain(const FunctionDecl &fn,
                                    bool arc_mode_enabled);
bool EffectiveArcReturnInsertRetain(const Objc3MethodDecl &method,
                                    bool arc_mode_enabled);
bool EffectiveArcReturnInsertAutorelease(const FunctionDecl &fn);
bool EffectiveArcReturnInsertAutorelease(const Objc3MethodDecl &method);
bool HasInteropInteropSurface(const FunctionDecl &fn);
bool HasInteropLifetimeBridge(const FuncParam &param);
bool HasInteropLifetimeBridge(const FunctionDecl &fn);

std::map<std::string, LoweredFunctionSignature>
BuildLoweredFunctionSignatures(const Objc3Program &program);
std::size_t CountVectorSignatureFunctions(const Objc3Program &program);
