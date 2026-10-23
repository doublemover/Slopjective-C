#pragma once

#include <string>
#include <utility>

#include "clang_foundation/async_abi_identity.h"
#include "clang_foundation/interface_contract.h"
#include "clang_foundation/interface_emitter.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Attr.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ObjC3Borrowed.h"
#include "clang/AST/ObjC3Isolation.h"
#include "clang/AST/ObjC3KeyPath.h"
#include "clang/AST/ObjC3ResourceTypes.h"
#include "llvm/ADT/SmallString.h"

namespace objc3::clang_foundation {

inline constexpr const char *kNativeOwnershipContractSemantic =
    "objc3.native-ownership-contracts.v1";
inline constexpr const char *kNativeIsolationContractSemantic =
    "objc3.isolation-contracts.v1";

// Values, not source expressions or AST addresses, participate in contract
// identity. All fields are length-delimited, including user-defined names.
inline void AppendNativeContractField(std::string &out, llvm::StringRef value) {
  out += std::to_string(value.size()) + ":" + value.str();
}

inline std::string
NativeResourceSentinelIdentity(const clang::Expr *expression,
                               const clang::ASTContext &context) {
  clang::Expr::EvalResult result;
  if (!expression || !expression->EvaluateAsRValue(result, context))
    return "!unmodeled";
  if (result.Val.isInt()) {
    llvm::SmallString<32> value;
    result.Val.getInt().toString(value, 10);
    return "integer:" + value.str().str();
  }
  if (!result.Val.isLValue())
    return "!unmodeled";
  std::string identity = "address:";
  const auto base = result.Val.getLValueBase();
  if (base.isNull()) {
    AppendNativeContractField(identity, "null");
  } else if (const auto *declaration =
                 base.dyn_cast<const clang::ValueDecl *>();
             declaration && declaration->hasExternalFormalLinkage()) {
    const auto id = ReadInterfaceDeclarationStableId(declaration);
    if (id.empty())
      return "!unmodeled";
    AppendNativeContractField(identity, id);
  } else {
    return "!unmodeled";
  }
  AppendNativeContractField(
      identity, std::to_string(result.Val.getLValueOffset().getQuantity()));
  AppendNativeContractField(
      identity, result.Val.isLValueOnePastTheEnd() ? "one-past" : "within");
  return identity;
}

inline void AppendNativeTypeContracts(std::string &out, clang::QualType type,
                                      const clang::ASTContext &context,
                                      const std::string &position,
                                      unsigned depth = 0) {
  if (type.isNull())
    return;
  if (depth > 256) {
    out += "!unmodeled";
    return;
  }
  clang::ObjC3KeyPathInfo keypath;
  if (clang::getObjC3KeyPathInfo(type, keypath)) {
    AppendNativeContractField(
        out, position + (keypath.Set ? ":writable-keypath" : ":keypath"));
    AppendNativeTypeContracts(out, keypath.Root, context, position + ".root",
                              depth + 1);
    AppendNativeTypeContracts(out, keypath.Value, context, position + ".value",
                              depth + 1);
    return;
  }
  const auto resource = clang::getObjC3ResourceTypeContract(context, type);
  if (resource.Cleanup || resource.Family || resource.Invalid) {
    AppendNativeContractField(out, position + ":resource");
    const auto *cleanup = resource.getCleanupFunction();
    AppendNativeContractField(
        out, cleanup ? ReadInterfaceDeclarationStableId(cleanup) : "");
    AppendNativeContractField(out, resource.Family
                                       ? resource.Family->getFamily()->getName()
                                       : llvm::StringRef());
    AppendNativeContractField(
        out, resource.Invalid ? NativeResourceSentinelIdentity(
                                    resource.Invalid->getInvalid(), context)
                              : "");
  }
  if (clang::isObjC3BorrowedType(context, type))
    AppendNativeContractField(out, position + ":borrowed");
  type = type.getDesugaredType(context);
  if (type->isPointerType() || type->isBlockPointerType() ||
      type->isReferenceType()) {
    AppendNativeTypeContracts(out, type->getPointeeType(), context,
                              position + ".pointee", depth + 1);
  } else if (const auto *array = context.getAsArrayType(type)) {
    AppendNativeTypeContracts(out, array->getElementType(), context,
                              position + ".element", depth + 1);
  } else if (const auto *function = type->getAs<clang::FunctionType>()) {
    AppendNativeTypeContracts(out, function->getReturnType(), context,
                              position + ".return", depth + 1);
    if (const auto *prototype =
            llvm::dyn_cast<clang::FunctionProtoType>(function))
      for (unsigned index = 0; index < prototype->getNumParams(); ++index) {
        const auto parameter =
            position + ".parameter[" + std::to_string(index) + "]";
        AppendNativeTypeContracts(out, prototype->getParamType(index), context,
                                  parameter, depth + 1);
        if (prototype->getExtParameterInfo(index).isNoEscape())
          AppendNativeContractField(out, parameter + ":noescape");
      }
  }
}

inline void ReadNativeDeclarationContracts(const clang::NamedDecl *declaration,
                                           const clang::ASTContext &context,
                                           InterfaceEffectIdentity &effects) {
  std::string ownership;
  clang::QualType declaration_type;
  if (const auto *value = llvm::dyn_cast<clang::ValueDecl>(declaration))
    declaration_type = value->getType();
  if (const auto *type = llvm::dyn_cast<clang::TypeDecl>(declaration))
    declaration_type = context.getTypeDeclType(type);
  if (const auto *method = llvm::dyn_cast<clang::ObjCMethodDecl>(declaration))
    declaration_type = method->getObjC3FunctionType();
  if (const auto *property =
          llvm::dyn_cast<clang::ObjCPropertyDecl>(declaration))
    declaration_type = property->getType();
  effects.async_abi_identity = ReadAsyncABIIdentity(declaration_type, context);
  if (const auto *value = llvm::dyn_cast<clang::ValueDecl>(declaration))
    AppendNativeTypeContracts(ownership, value->getType(), context, "type");
  if (const auto *type = llvm::dyn_cast<clang::TypeDecl>(declaration))
    AppendNativeTypeContracts(ownership, context.getTypeDeclType(type), context,
                              "type");
  if (const auto *method = llvm::dyn_cast<clang::ObjCMethodDecl>(declaration))
    AppendNativeTypeContracts(ownership, method->getObjC3FunctionType(),
                              context, "type");
  if (const auto *property =
          llvm::dyn_cast<clang::ObjCPropertyDecl>(declaration))
    AppendNativeTypeContracts(ownership, property->getType(), context, "type");
  if (const auto *borrow = clang::getObjC3BorrowedReturnContract(declaration)) {
    AppendNativeContractField(ownership, "returns-borrowed");
    AppendNativeContractField(ownership,
                              std::to_string(borrow->getOwnerIndex()));
  }
  if (declaration->hasAttr<clang::NoThrowAttr>())
    AppendNativeContractField(ownership, "nothrow");
  auto parameter = [&](const clang::ParmVarDecl *value, unsigned index) {
    if (value->hasAttr<clang::NoEscapeAttr>())
      AppendNativeContractField(
          ownership, "parameter[" + std::to_string(index) + "]:noescape");
  };
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration))
    for (unsigned i = 0; i < function->getNumParams(); ++i)
      parameter(function->getParamDecl(i), i);
  if (const auto *method = llvm::dyn_cast<clang::ObjCMethodDecl>(declaration))
    for (unsigned i = 0; i < method->param_size(); ++i)
      parameter(method->getParamDecl(i), i);
  if (!ownership.empty()) {
    if (!effects.ownership_identity.empty())
      effects.ownership_identity += ";";
    effects.ownership_identity += "native-contracts:v1:" + ownership;
  }
  std::string isolation;
  if (const auto *interface =
          llvm::dyn_cast<clang::ObjCInterfaceDecl>(declaration);
      interface && clang::isObjC3Actor(interface))
    AppendNativeContractField(isolation, "actor-type");
  if (clang::isObjC3ActorIsolated(declaration))
    AppendNativeContractField(isolation, "actor-isolated");
  if (clang::isObjC3Nonisolated(declaration))
    AppendNativeContractField(isolation, "nonisolated");
  if (declaration->hasAttr<clang::ObjCUnsafeSendableAttr>())
    AppendNativeContractField(isolation, "unsafe-sendable");
  if (const auto boundary = clang::getObjC3TaskBoundaryKind(declaration))
    AppendNativeContractField(isolation,
                              "task-boundary:" + std::to_string(boundary));
  effects.actor_isolation_identity = std::move(isolation);
  if (const auto *executor = clang::getObjC3Executor(declaration))
    effects.executor_affinity_identity = executor->getExecutor().str();
}

inline bool
HasNativeOwnershipContracts(const InterfaceEffectIdentity &effects) {
  return effects.ownership_identity.find("native-contracts:v1:") !=
         std::string::npos;
}
inline bool
HasNativeIsolationContracts(const InterfaceEffectIdentity &effects) {
  return !effects.actor_isolation_identity.empty() ||
         !effects.executor_affinity_identity.empty();
}

} // namespace objc3::clang_foundation
