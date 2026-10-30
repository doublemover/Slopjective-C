#pragma once

#include "clang_foundation/interface_contract.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Attr.h"
#include "clang/AST/Decl.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/AST/Type.h"

namespace objc3::clang_foundation {

inline std::string
ReadNativeReturnOwnership(const clang::NamedDecl *declaration) {
  const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration);
  // ARC stores ns_returns_retained on the function type, while MRC keeps
  // the declaration attribute. Both spellings have the same return contract.
  if (function && function->getType()
                      ->castAs<clang::FunctionType>()
                      ->getExtInfo()
                      .getProducesResult())
    return "native-return:retained";
  if (declaration->hasAttr<clang::NSReturnsRetainedAttr>())
    return "native-return:retained";
  if (declaration->hasAttr<clang::NSReturnsNotRetainedAttr>())
    return "native-return:not-retained";
  if (declaration->hasAttr<clang::NSReturnsAutoreleasedAttr>())
    return "native-return:autoreleased";
  return {};
}

// Callable effects come from the same canonical function type that Clang
// parsed. C++ exception specifications are a separate part of that type.
inline InterfaceEffectIdentity
ReadCanonicalFunctionEffects(clang::QualType type,
                             const clang::ASTContext &context) {
  InterfaceEffectIdentity result;
  if (type.isNull())
    return result;
  // A callable variable or typedef can carry its function through a native
  // pointer, block pointer, or reference. Preserve that callable's effects.
  for (;;) {
    if (type->isPointerType() || type->isBlockPointerType() || type->isReferenceType())
      type=type->getPointeeType();
    else if (const auto *member=type->getAs<clang::MemberPointerType>())
      type=member->getPointeeType();
    else break;
  }
  const auto *function = type->getAs<clang::FunctionProtoType>();
  if (!function)
    return result;
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

} // namespace objc3::clang_foundation
