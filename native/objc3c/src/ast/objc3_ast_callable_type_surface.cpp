#include "ast/objc3_ast_callable_type_surface.h"

#include <sstream>

#include "ast/objc3_ast_value_type_queries.h"

bool Objc3FuncParamHasConcreteTypeSurface(const FuncParam &param) {
  if (param.type != ValueType::Unknown) {
    return true;
  }
  return param.vector_spelling || param.id_spelling || param.class_spelling ||
         param.sel_spelling || param.instancetype_spelling ||
         param.object_pointer_type_spelling || param.has_pointer_declarator ||
         param.has_ownership_qualifier;
}

std::string Objc3FuncParamTypeReplayKey(const FuncParam &param) {
  std::ostringstream out;
  out << "name=" << param.name << ";type=" << Objc3ValueTypeSpelling(param.type)
      << ";vector=" << (param.vector_spelling ? "true" : "false")
      << ";vector_base=" << param.vector_base_spelling
      << ";vector_lanes=" << param.vector_lane_count
      << ";id=" << (param.id_spelling ? "true" : "false")
      << ";class=" << (param.class_spelling ? "true" : "false")
      << ";sel=" << (param.sel_spelling ? "true" : "false")
      << ";instancetype=" << (param.instancetype_spelling ? "true" : "false")
      << ";object_ptr="
      << (param.object_pointer_type_spelling ? "true" : "false")
      << ";object_ptr_name=" << param.object_pointer_type_name
      << ";pointer_depth=" << param.pointer_declarator_depth
      << ";ownership=" << param.ownership_qualifier_spelling
      << ";borrowed=" << (param.borrowed_pointer_qualified ? "true" : "false");
  return out.str();
}

std::string Objc3CallableSignatureReplayKey(
    const std::string &name, const std::vector<FuncParam> &params,
    ValueType return_type, bool async_declared, bool throws_declared) {
  std::ostringstream out;
  out << "callable=" << name << ";return="
      << Objc3ValueTypeSpelling(return_type)
      << ";async=" << (async_declared ? "true" : "false")
      << ";throws=" << (throws_declared ? "true" : "false")
      << ";param_count=" << params.size();
  for (const FuncParam &param : params) {
    out << ";param={" << Objc3FuncParamTypeReplayKey(param) << "}";
  }
  return out.str();
}
