#pragma once

#include "clang/AST/ASTContext.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/AST/Type.h"
#include "clang_foundation/interface_contract.h"

namespace objc3::clang_foundation {

// Callable effects come from the same canonical function type that Clang
// parsed. C++ exception specifications are a separate part of that type.
inline InterfaceEffectIdentity ReadCanonicalFunctionEffects(
    clang::QualType type, const clang::ASTContext &context) {
  InterfaceEffectIdentity result;
  if (type.isNull()) return result;
  // A callable variable or typedef can carry its function through a native
  // pointer, block pointer, or reference. Preserve that callable's effects.
  while (type->isPointerType() || type->isBlockPointerType() ||
         type->isReferenceType())
    type = type->getPointeeType();
  const auto *function = type->getAs<clang::FunctionProtoType>();
  if (!function) return result;
  const auto effects = function->getObjC3FunctionEffects();
  result.is_async = effects.Async;
  result.is_throwing = effects.Throws != clang::ObjC3ThrowsKind::None;
  if (effects.Throws == clang::ObjC3ThrowsKind::Typed) {
    result.throws_kind = "typed";
    clang::PrintingPolicy policy(context.getLangOpts());
    policy.PrintAsCanonical = true;
    policy.FullyQualifiedName = true;
    result.error_type_identity =
        effects.ErrorType.getCanonicalType().getAsString(policy);
  } else if (effects.Throws == clang::ObjC3ThrowsKind::Untyped) {
    result.throws_kind = "untyped";
  }
  return result;
}

}  // namespace objc3::clang_foundation
