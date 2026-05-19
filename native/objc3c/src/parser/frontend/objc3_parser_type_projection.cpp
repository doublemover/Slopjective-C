#include "parser/frontend/objc3_parser_type_projection.h"

#include "parser/frontend/objc3_parser_type_projection_internal.h"

namespace objc3c::parse {

void CopyObjc3MethodReturnTypeFromFunctionDecl(const FunctionDecl &source,
                                               Objc3MethodDecl &target) {
  CopyObjc3MethodReturnTypeCoreProjection(source, target);
  CopyObjc3MethodReturnTypeErrorProjection(source, target);
  CopyObjc3MethodReturnTypeConcurrencyProjection(source, target);
  CopyObjc3MethodReturnTypeUnsafeProjection(source, target);
}

}  // namespace objc3c::parse
