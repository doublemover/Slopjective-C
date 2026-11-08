#pragma once

#include <string>
#include <utility>

#include "clang_foundation/async_abi_identity.h"
#include "clang_foundation/interface_contract.h"
#include "clang_foundation/interface_emitter.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Attr.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ObjC3Async.h"
#include "clang/AST/ObjC3Borrowed.h"
#include "clang/AST/ObjC3Isolation.h"
#include "clang/AST/ObjC3KeyPath.h"
#include "clang/AST/ObjC3ResourceTypes.h"
#include "clang/AST/ObjC3TypeContracts.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallPtrSet.h"

namespace objc3::clang_foundation {

inline constexpr const char *kNativeOwnershipContractSemantic =
    "objc3.native-ownership-contracts.v1";
inline constexpr const char *kNativeIsolationContractSemantic =
    "objc3.isolation-contracts.v1";

inline bool HasNativeConsumedCallable(const clang::Decl *declaration) {
  const auto *type = clang::getObjC3CallableFunctionType(declaration);
  if (type)
    for (unsigned i = 0; i != type->getNumParams(); ++i)
      if (type->isParamConsumed(i) || type->getExtParameterInfo(i).isCFConsumed())
        return true;
  if (declaration && declaration->hasAttr<clang::NSConsumesSelfAttr>()) return true;
  if (const auto *method = llvm::dyn_cast_or_null<clang::ObjCMethodDecl>(declaration))
    if (method->isInstanceMethod() && method->getMethodFamily() == clang::OMF_init) return true;
  auto contains = [](auto parameters) {
    for (const clang::ParmVarDecl *parameter : parameters)
      if (parameter->hasAttr<clang::NSConsumedAttr>() ||
          parameter->hasAttr<clang::CFConsumedAttr>()) return true;
    return false;
  };
  if (const auto *function = llvm::dyn_cast_or_null<clang::FunctionDecl>(declaration))
    return contains(function->parameters());
  if (const auto *method = llvm::dyn_cast_or_null<clang::ObjCMethodDecl>(declaration))
    return contains(method->parameters());
  if (const auto *block = llvm::dyn_cast_or_null<clang::BlockDecl>(declaration))
    return contains(block->parameters());
  return false;
}

inline bool HasNativeAsyncConsumedCallable(const clang::Decl *declaration) {
  const auto *type = clang::getObjC3CallableFunctionType(declaration);
  const bool foreign_result = type && type->getObjC3FunctionEffects().Async &&
      clang::hasObjC3CFResultOwnership(declaration);
  return clang::isObjC3OwningAsyncEntry(declaration) &&
      (HasNativeConsumedCallable(declaration) || clang::isObjC3OwnedAsyncResult(declaration) || foreign_result);
}

// Values, not source expressions or AST addresses, participate in contract
// identity. All fields are length-delimited, including user-defined names.
// Only by-value record/array storage participates. An arbitrary pointee is
// not a copied owner. Bounded exhaustion conservatively requires admission.
inline bool ContainsNativeKeyPathResourceValue(clang::QualType type,
                                               const clang::ASTContext &context) {
  llvm::SmallPtrSet<const clang::Type *, 32> seen;
  unsigned budget=256;
  auto visit=[&](auto &&self,clang::QualType value,unsigned depth)->bool {
    if(value.isNull() || value->isDependentType())return false;
    if(depth>64 || !budget)return true;
    --budget;
    const auto resource=clang::getObjC3ResourceTypeContract(context,value);
    if(resource.Cleanup || resource.Family)return true;
    if(!seen.insert(value.getCanonicalType().getTypePtr()).second)return false;
    if(const auto *array=context.getAsArrayType(value))return self(self,array->getElementType(),depth+1);
    if(const auto *record=value->getAs<clang::RecordType>())
      if(const auto *definition=record->getDecl()->getDefinition()) {
        if(const auto *cxx=llvm::dyn_cast<clang::CXXRecordDecl>(definition);
            cxx && cxx->isUnion() && cxx->hasNonTrivialDestructor())return true;
        for(const auto *field:definition->fields())if(self(self,field->getType(),depth+1))return true;
        if(const auto *cxx=llvm::dyn_cast<clang::CXXRecordDecl>(definition))
          for(const auto &base:cxx->bases())if(self(self,base.getType(),depth+1))return true;
      }
    return false;
  };
  return visit(visit,type,0);
}

// The native call contract applies at direct carrier value boundaries. Pointer
// and reference views of carriers do not themselves pass ownership by value.
inline bool IsNativeCppCarrierValue(clang::QualType type) {
  return !type.isNull() && clang::hasObjC3CarrierCppPayload(type);
}

inline bool HasNativeCheckedCppRecordResultType(clang::QualType type,unsigned depth=0) {
  if(type.isNull() || depth>64)return false;
  type=type.getCanonicalType();
  if(type->isPointerType() || type->isBlockPointerType() || type->isReferenceType())
    return HasNativeCheckedCppRecordResultType(type->getPointeeType(),depth+1);
  if(const auto *member=type->getAs<clang::MemberPointerType>())
    return HasNativeCheckedCppRecordResultType(member->getPointeeType(),depth+1);
  if(const auto *function=type->getAs<clang::FunctionProtoType>()) {
    const auto effects=function->getObjC3FunctionEffects();
    return !effects.Async && effects.Throws!=clang::ObjC3ThrowsKind::None &&
        (function->getReturnType()->getAsCXXRecordDecl() || function->getReturnType()->isDependentType());
  }
  return false;
}
inline bool HasNativeCppCarrierCallType(clang::QualType type,unsigned depth=0) {
  if(type.isNull() || depth>64)return false;
  type=type.getCanonicalType();
  if(type->isPointerType() || type->isBlockPointerType() || type->isReferenceType())
    return HasNativeCppCarrierCallType(type->getPointeeType(),depth+1);
  if(const auto *member=type->getAs<clang::MemberPointerType>())
    return HasNativeCppCarrierCallType(member->getPointeeType(),depth+1);
  if(const auto *function=type->getAs<clang::FunctionType>()) {
    if(IsNativeCppCarrierValue(function->getReturnType()))return true;
    if(const auto *prototype=llvm::dyn_cast<clang::FunctionProtoType>(function))
      for(auto parameter:prototype->param_types())if(IsNativeCppCarrierValue(parameter))return true;
  }
  return false;
}

inline bool IsNativeMixedCarrierValue(clang::QualType type) {
  return !type.isNull() && clang::hasObjC3CarrierCppPayload(type) &&
      clang::hasObjC3CarrierManagedPayload(type);
}
inline bool HasNativeMixedCarrierCallType(clang::QualType type,unsigned depth=0) {
  if(type.isNull())return false;
  if(depth>64)return true;
  type=type.getCanonicalType();
  if(type->isPointerType() || type->isBlockPointerType() || type->isReferenceType())
    return HasNativeMixedCarrierCallType(type->getPointeeType(),depth+1);
  if(const auto *member=type->getAs<clang::MemberPointerType>())
    return HasNativeMixedCarrierCallType(member->getPointeeType(),depth+1);
  if(const auto *function=type->getAs<clang::FunctionType>()) {
    if(IsNativeMixedCarrierValue(function->getReturnType()) ||
        HasNativeMixedCarrierCallType(function->getReturnType(),depth+1))return true;
    if(const auto *prototype=llvm::dyn_cast<clang::FunctionProtoType>(function))
      for(auto parameter:prototype->param_types())
        if(IsNativeMixedCarrierValue(parameter) || HasNativeMixedCarrierCallType(parameter,depth+1))return true;
  }
  return false;
}
inline bool HasNativeCppCarrierRuntimeType(clang::QualType type,unsigned depth=0) {
  if(type.isNull())return false;
  if(depth>64)return true;
  type=type.getCanonicalType();
  if(const auto *block=type->getAs<clang::BlockPointerType>()) {
    auto function=block->getPointeeType();
    if(HasNativeCppCarrierCallType(function))return true;
    if(const auto *prototype=function->getAs<clang::FunctionProtoType>()) {
      if(prototype->getReturnType()->isDependentType())return true;
      for(auto parameter:prototype->param_types())if(parameter->isDependentType())return true;
    }
    return HasNativeCppCarrierRuntimeType(function,depth+1);
  }
  if(type->isPointerType() || type->isReferenceType())return HasNativeCppCarrierRuntimeType(type->getPointeeType(),depth+1);
  if(const auto *member=type->getAs<clang::MemberPointerType>())return HasNativeCppCarrierRuntimeType(member->getPointeeType(),depth+1);
  if(const auto *function=type->getAs<clang::FunctionProtoType>()) {
    if(HasNativeCppCarrierRuntimeType(function->getReturnType(),depth+1))return true;
    for(auto parameter:function->param_types())if(HasNativeCppCarrierRuntimeType(parameter,depth+1))return true;
  }
  return false;
}
inline bool RequiresNativeARCRecordPayload(clang::QualType type,
                                             const clang::ASTContext &context) {
  if(!context.getLangOpts().ObjC3 || !context.getLangOpts().ObjCAutoRefCount ||
      !context.getLangOpts().CPlusPlus)return false;
  llvm::SmallPtrSet<const clang::Type *,32> seen,payload_seen;unsigned budget=256;
  auto visit=[&](auto &&self,clang::QualType value,bool payload,unsigned depth)->bool {
    if(value.isNull())return false;
    if(depth>64 || !budget)return true;
    --budget;
    if(value->isDependentType())return payload;
    value=value.getCanonicalType();
    if(payload && (value->isObjCObjectPointerType() || value->isBlockPointerType() || value.hasNonTrivialObjCLifetime()))return true;
    if(!(payload?payload_seen:seen).insert(value.getTypePtr()).second)return false;
    if(const auto *optional=value->getAs<clang::ObjC3OptionalType>())return self(self,optional->getElementType(),true,depth+1);
    if(const auto *result=value->getAs<clang::ObjC3ResultType>())return self(self,result->getSuccessType(),true,depth+1) || self(self,result->getErrorType(),true,depth+1);
    if(const auto *array=context.getAsArrayType(value))return self(self,array->getElementType(),payload,depth+1);
    if(value->isPointerType() || value->isReferenceType())return self(self,value->getPointeeType(),false,depth+1);
    if(const auto *function=value->getAs<clang::FunctionProtoType>()) {
      if(self(self,function->getReturnType(),false,depth+1))return true;
      for(auto parameter:function->param_types())if(self(self,parameter,false,depth+1))return true;
    }
    if(const auto *record=value->getAsCXXRecordDecl())if(const auto *definition=record->getDefinition()) {
      for(const auto &parent:definition->bases())if(self(self,parent.getType(),payload,depth+1))return true;
      for(const auto *field:definition->fields())if(self(self,field->getType(),payload,depth+1))return true;
    }
    return false;
  };
  return visit(visit,type,false,0);
}

inline bool RequiresNativeCppCarrierComposition(clang::QualType type,
                                                 const clang::ASTContext &context) {
  llvm::SmallPtrSet<const clang::Type *,32> seen;unsigned budget=256;
  auto visit=[&](auto &&self,clang::QualType value,unsigned depth)->bool {
    if(value.isNull() || value->isDependentType())return false;
    if(depth>64 || !budget)return true;
    --budget;
    value=value.getCanonicalType();
    if(!seen.insert(value.getTypePtr()).second)return false;
    if(const auto *array=context.getAsArrayType(value))
      return clang::containsObjC3CarrierCppStorage(array->getElementType()) ||
          self(self,array->getElementType(),depth+1);
    auto payload=[&](clang::QualType child) {
      if(child->isRecordType() && clang::containsObjC3CarrierCppStorage(child))return true;
      return self(self,child,depth+1);
    };
    if(const auto *optional=value->getAs<clang::ObjC3OptionalType>())return payload(optional->getElementType());
    if(const auto *result=value->getAs<clang::ObjC3ResultType>())
      return payload(result->getSuccessType()) || payload(result->getErrorType());
    if(const auto *record=value->getAsCXXRecordDecl())
      if(const auto *definition=record->getDefinition()) {
        for(const auto &parent:definition->bases())if(self(self,parent.getType(),depth+1))return true;
        for(const auto *field:definition->fields())if(self(self,field->getType(),depth+1))return true;
      }
    return false;
  };
  return visit(visit,type,0);
}

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
                                      unsigned depth = 0,
                                      bool skip_retained_result = false) {
  if (type.isNull())
    return;
  if (depth > 256) {
    out += "!unmodeled";
    return;
  }
  if (clang::isObjC3NSObjectStorageType(type)) {
    AppendNativeContractField(out, position + ":nsobject-storage");
    AppendNativeContractField(out, ";nsobject-storage:v1;");
  }
  if (const auto nullability = type->getNullability()) {
    AppendNativeContractField(out, position + ":nullability");
    AppendNativeContractField(out, std::to_string(static_cast<unsigned>(*nullability)));
  }
  if(RequiresNativeARCRecordPayload(type,context))
    AppendNativeContractField(out,";native-arc-record-payload:v1;");
  if(RequiresNativeCppCarrierComposition(type,context))
    AppendNativeContractField(out,";cxx-carrier-composition:v2;");
  clang::ObjC3KeyPathInfo keypath;
  if (clang::getObjC3KeyPathInfo(type, keypath)) {
    AppendNativeContractField(
        out, position + (keypath.Writable ? ":writable-keypath" : ":keypath"));
    if (type->isDependentType())
      AppendNativeContractField(out, ";keypath-dependent-types:v1;");
    if (ContainsNativeKeyPathResourceValue(keypath.Value, context))
      AppendNativeContractField(out, ";keypath-resource-values:v2;");
    AppendNativeTypeContracts(out, keypath.Root, context, position + ".root",
                              depth + 1);
    AppendNativeTypeContracts(out, keypath.Value, context, position + ".value",
                              depth + 1);
    return;
  }
  if (const auto *family = clang::getObjC3RetainableFamily(context, type)) {
    AppendNativeContractField(out, position + ":retainable-family");
    AppendNativeContractField(out, family->getFamily()->getName());
    AppendNativeContractField(out, type->isObjCRetainableType() ? "objc-integrated" : "manual");
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
  if (const auto *optional =
          llvm::dyn_cast<clang::ObjC3OptionalType>(type.getTypePtr())) {
    AppendNativeTypeContracts(out, optional->getElementType(), context,
                              position + ".some", depth + 1);
  } else if (const auto *result =
                 llvm::dyn_cast<clang::ObjC3ResultType>(type.getTypePtr())) {
    AppendNativeTypeContracts(out, result->getSuccessType(), context,
                              position + ".ok", depth + 1);
    AppendNativeTypeContracts(out, result->getErrorType(), context,
                              position + ".error", depth + 1);
  } else if (type->isPointerType() || type->isBlockPointerType() ||
      type->isReferenceType()) {
    if(type->isBlockPointerType() && HasNativeCppCarrierRuntimeType(type))
      AppendNativeContractField(out,";cxx-carrier-runtime:v2;");
    AppendNativeTypeContracts(out, type->getPointeeType(), context,
                              position + ".pointee", depth + 1);
  } else if (const auto *member = type->getAs<clang::MemberPointerType>()) {
    AppendNativeTypeContracts(out, member->getPointeeType(), context,
                              position + ".pointee", depth + 1);
  } else if (const auto *array = context.getAsArrayType(type)) {
    AppendNativeTypeContracts(out, array->getElementType(), context,
                              position + ".element", depth + 1);
  } else if (const auto *function = type->getAs<clang::FunctionType>()) {
    if(HasNativeMixedCarrierCallType(type))
      AppendNativeContractField(out,";mixed-carrier-call:v1;");
    if(HasNativeCppCarrierRuntimeType(type))
      AppendNativeContractField(out,";cxx-carrier-runtime:v2;");
    if(HasNativeCheckedCppRecordResultType(type))
      AppendNativeContractField(out,";checked-cxx-record-results:v2;");
    if(HasNativeCppCarrierCallType(type))
      AppendNativeContractField(out,";cxx-carrier-call:v2;");
    if (function->hasObjC3BorrowedResult()) {
      AppendNativeContractField(out, position + ":borrowed-result");
      AppendNativeContractField(out, ";borrowed-callable-owners:v1;");
    }
    if (const auto *prototype = llvm::dyn_cast<clang::FunctionProtoType>(function))
      if (unsigned owner = prototype->getObjC3BorrowedOwner()) {
        AppendNativeContractField(out, position + ":returns-borrowed-owner");
        AppendNativeContractField(out, std::to_string(owner - 1));
        AppendNativeContractField(out, ";borrowed-callable-owners:v1;");
      }
    if (!skip_retained_result && function->getExtInfo().getProducesResult())
      AppendNativeContractField(out, position + ":ns-returns-retained");
    if (const auto *prototype = llvm::dyn_cast<clang::FunctionProtoType>(function);
        prototype && prototype->getObjC3FunctionEffects().Async &&
        prototype->getExtInfo().getProducesResult())
      AppendNativeContractField(out, ";async-consuming-ownership:v3;");
    AppendNativeTypeContracts(out, function->getReturnType(), context,
                              position + ".return", depth + 1);
    if (const auto *prototype =
            llvm::dyn_cast<clang::FunctionProtoType>(function))
      for (unsigned index = 0; index < prototype->getNumParams(); ++index) {
        const auto parameter =
            position + ".parameter[" + std::to_string(index) + "]";
        if (prototype->getObjC3FunctionEffects().Async &&
            (prototype->isParamConsumed(index) ||
             prototype->getExtParameterInfo(index).isCFConsumed()))
          AppendNativeContractField(out, ";async-consuming-ownership:v3;");
        AppendNativeTypeContracts(out, prototype->getParamType(index), context,
                                  parameter, depth + 1);
        if (prototype->getExtParameterInfo(index).isNoEscape())
          AppendNativeContractField(out, parameter + ":noescape");
        if (prototype->getExtParameterInfo(index).isCFConsumed())
          AppendNativeContractField(out, parameter + ":cf-consumed");
        if (prototype->isParamConsumed(index))
          AppendNativeContractField(out, parameter + ":ns-consumed");
      }
  }
}

inline void ReadNativeDeclarationContracts(const clang::NamedDecl *declaration,
                                           const clang::ASTContext &context,
                                           InterfaceEffectIdentity &effects) {
  // A template publishes its underlying callable contract. Its native entry
  // discriminator and nested callable positions are known before substitution.
  if (const auto *pattern=llvm::dyn_cast<clang::FunctionTemplateDecl>(declaration))
    declaration=pattern->getTemplatedDecl();
  if (const auto *pattern=llvm::dyn_cast<clang::TypeAliasTemplateDecl>(declaration))
    declaration=pattern->getTemplatedDecl();
  if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(declaration))
    declaration=pattern->getTemplatedDecl();
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
    AppendNativeTypeContracts(ownership, value->getType(), context, "type", 0,
                              llvm::isa<clang::FunctionDecl>(value));
  if (const auto *type = llvm::dyn_cast<clang::TypeDecl>(declaration))
    AppendNativeTypeContracts(ownership, context.getTypeDeclType(type), context,
                              "type");
  if (const auto *method = llvm::dyn_cast<clang::ObjCMethodDecl>(declaration))
    AppendNativeTypeContracts(ownership, method->getObjC3FunctionType(),
                              context, "type", 0, true);
  if (const auto *property =
          llvm::dyn_cast<clang::ObjCPropertyDecl>(declaration))
    AppendNativeTypeContracts(ownership, property->getType(), context, "type");
  if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration);
      variable && variable->getTLSKind() != clang::VarDecl::TLS_None) {
    AppendNativeContractField(ownership, "variable-storage:v1");
    AppendNativeContractField(ownership, std::to_string(static_cast<unsigned>(variable->getStorageDuration())));
    AppendNativeContractField(ownership, std::to_string(static_cast<unsigned>(variable->getTLSKind())));
  }
  if (auto owner = clang::getObjC3BorrowedReturnOwner(declaration)) {
    AppendNativeContractField(ownership, "returns-borrowed");
    AppendNativeContractField(ownership,
                              std::to_string(*owner));
  }
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration))
    if (const auto *operation = clang::getObjC3FamilyOperation(function)) {
      AppendNativeContractField(ownership,
          llvm::isa<clang::ObjCFamilyRetainAttr>(operation) ? "family-retain" :
          llvm::isa<clang::ObjCFamilyReleaseAttr>(operation) ? "family-release" : "family-autorelease");
      AppendNativeContractField(ownership,
          clang::getObjC3FamilyOperationName(operation)->getName());
    }
  if (declaration->hasAttr<clang::NoThrowAttr>())
    AppendNativeContractField(ownership, "nothrow");
  if (declaration->hasAttr<clang::ObjCReturnsInnerPointerAttr>())
    AppendNativeContractField(ownership, ";borrowed-receiver-results:v1;");
  if (declaration->hasAttr<clang::NSConsumesSelfAttr>())
    AppendNativeContractField(ownership, "ns-consumes-self");
  auto parameter = [&](const clang::ParmVarDecl *value, unsigned index) {
    AppendNativeTypeContracts(ownership, value->getType(), context,
                              "parameter[" + std::to_string(index) + "]");
    if (value->hasAttr<clang::NoEscapeAttr>())
      AppendNativeContractField(
          ownership, "parameter[" + std::to_string(index) + "]:noescape");
    if (value->hasAttr<clang::CFConsumedAttr>())
      AppendNativeContractField(
          ownership, "parameter[" + std::to_string(index) + "]:cf-consumed");
    if (value->hasAttr<clang::NSConsumedAttr>())
      AppendNativeContractField(
          ownership, "parameter[" + std::to_string(index) + "]:ns-consumed");
  };
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration))
    for (unsigned i = 0; i < function->getNumParams(); ++i)
      parameter(function->getParamDecl(i), i);
  if (const auto *method = llvm::dyn_cast<clang::ObjCMethodDecl>(declaration))
    for (unsigned i = 0; i < method->param_size(); ++i)
      parameter(method->getParamDecl(i), i);
  if (HasNativeAsyncConsumedCallable(declaration))
    AppendNativeContractField(ownership, ";async-consuming-ownership:v3;");
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
