#include "ast/objc3_ast_scope_members.h"

std::string BuildObjcMethodScopePathSymbol(const Objc3MethodDecl &method) {
  return (method.is_class_method ? "class_method:" : "instance_method:") +
         method.selector;
}

std::string BuildObjcPropertyScopePathSymbol(
    const Objc3PropertyDecl &property) {
  return "property:" + property.name;
}

std::string BuildObjcPropertySynthesisSymbol(
    const Objc3PropertyDecl &property) {
  return "property_synthesis:" + property.name;
}

std::string BuildObjcIvarBindingSymbol(const Objc3PropertyDecl &property) {
  return "ivar_binding:_" + property.name;
}

std::string BuildObjcTypecheckParamFamilySymbol(const FuncParam &param) {
  if (param.id_spelling) {
    return "id";
  }
  if (param.class_spelling) {
    return "Class";
  }
  if (param.sel_spelling) {
    return "SEL";
  }
  if (param.object_pointer_type_spelling) {
    return "object-pointer:" + param.object_pointer_type_name;
  }
  return "";
}

std::string BuildObjcTypecheckReturnFamilySymbol(const FunctionDecl &fn) {
  if (fn.return_id_spelling) {
    return "id";
  }
  if (fn.return_class_spelling) {
    return "Class";
  }
  if (fn.return_sel_spelling) {
    return "SEL";
  }
  if (fn.return_object_pointer_type_spelling) {
    return "object-pointer:" + fn.return_object_pointer_type_name;
  }
  return "";
}
