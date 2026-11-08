#include "standard_library_specializations.h"
#include "standard_library_headers.h"
#include "standard_coroutine_types.h"
#include "clang_foundation/interface_emitter.h"
#include "clang_foundation/ast_effect_identity.h"
#include "clang_foundation/native_contract_identity.h"
#include "clang_foundation/interface_module_resolver.h"
#include "clang_foundation/module_names.h"
#include "clang_foundation/runtime_interface_types.h"

#include <algorithm>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "clang_foundation/interface_codec.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/APValue.h"
#include "clang/AST/Attr.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/ObjC3Async.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/AST/RecordLayout.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/DynamicRecursiveASTVisitor.h"
#include "clang/AST/Type.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/LangStandard.h"
#include "clang/Basic/ObjC3Foundation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/ADT/APFloat.h"
#include "clang/Tooling/Tooling.h"
#include "clang/UnifiedSymbolResolution/USRGeneration.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/ScopeExit.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/SHA256.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/VirtualFileSystem.h"
#include "llvm/Support/raw_ostream.h"

#include "clang_foundation/interface_emission_internal.h"

namespace objc3::clang_foundation {
namespace emission_detail {

class RuntimeDeclarationFinder final
    : public clang::RecursiveASTVisitor<RuntimeDeclarationFinder> {
public:
  explicit RuntimeDeclarationFinder(const clang::ASTContext &context)
      : context_(context) {}
  bool HasCxxCoroutines=false;
  std::set<std::string> StandardCoroutineHeaders;
  bool HasStandardLibraryReferences=false;
  StandardLibraryHeaderManifest StandardHeaders;
  void StandardLibraryReference(const clang::NamedDecl *declaration) {
    if (const auto *record=llvm::dyn_cast_or_null<clang::ClassTemplateSpecializationDecl>(declaration))
      StandardLibraryReference(record->getSpecializedTemplate());
    if (const auto *variable=llvm::dyn_cast_or_null<clang::VarTemplateSpecializationDecl>(declaration))
      StandardLibraryReference(variable->getSpecializedTemplate());
    if(!IsStandardLibraryDeclaration(declaration,context_))return;
    if(!HasStandardLibraryReferences)StandardHeaders=ReadStandardLibraryHeaderManifest(context_);
    HasStandardLibraryReferences=true;
  }
  void StandardCoroutineReference(const clang::NamedDecl *declaration) {
    StandardLibraryReference(declaration);
    if (const auto *owner=StandardCoroutineHeaderOwner(declaration,context_)) {
      const auto &source=context_.getSourceManager();bool invalid=false;
      const auto bytes=source.getBufferData(source.getFileID(source.getSpellingLoc(owner->getLocation())),&invalid);
      if (!invalid) StandardCoroutineHeaders.insert(ComputeInterfaceSourceSha256(bytes));
    }
  }
  bool Found=false, HasCallableEffects=false, HasAddressArguments=false, HasFactoryArguments=false, HasFactoryValueArguments=false;
  FactoryTypeFeatures FactoryTypes;
  void InspectArguments(llvm::ArrayRef<clang::TemplateArgument> arguments) {
    for (const auto &argument:arguments) {
      ReadFactoryArgumentFeatures(argument,context_,FactoryTypes);
      HasFactoryValueArguments |= HasFactoryValueArgument(argument,context_);
      HasAddressArguments |= HasAddressTemplateArgument(argument,context_);
      HasFactoryArguments |= HasFactoryTemplateArgument(argument,context_,&FactoryTypes);
    }
  }
  void InspectType(clang::QualType type) {
    if (!type.isNull()) {
      if (const auto *record=type->getAsCXXRecordDecl()) StandardCoroutineReference(record);
      if (const auto *specialization=type->getAs<clang::TemplateSpecializationType>())
        StandardCoroutineReference(specialization->getTemplateName().getAsTemplateDecl());
    }
    if(!type.isNull())FactoryTypeKey(type,context_,&FactoryTypes);
    if (!type.isNull())
      if (const auto *record=llvm::dyn_cast_or_null<clang::ClassTemplateSpecializationDecl>(type->getAsCXXRecordDecl()))
        InspectArguments(record->getTemplateArgs().asArray());
  }
  bool VisitTypeLoc(clang::TypeLoc location) {InspectType(location.getType());return true;}
  bool TraverseTemplateArgument(const clang::TemplateArgument &argument) {
    if(argument.getKind()==clang::TemplateArgument::Template || argument.getKind()==clang::TemplateArgument::TemplateExpansion)
      Reference(argument.getAsTemplateOrTemplatePattern().getAsTemplateDecl());
    ReadFactoryArgumentFeatures(argument,context_,FactoryTypes);
    HasFactoryValueArguments |= HasFactoryValueArgument(argument,context_);
    HasAddressArguments |= HasAddressTemplateArgument(argument,context_);
    HasFactoryArguments |= HasFactoryTemplateArgument(argument,context_,&FactoryTypes);
    return clang::RecursiveASTVisitor<RuntimeDeclarationFinder>::TraverseTemplateArgument(argument);
  }
  bool TraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc &argument) {
    if(argument.getArgument().getKind()==clang::TemplateArgument::Template || argument.getArgument().getKind()==clang::TemplateArgument::TemplateExpansion)
      Reference(argument.getArgument().getAsTemplateOrTemplatePattern().getAsTemplateDecl());
    ReadFactoryArgumentFeatures(argument.getArgument(),context_,FactoryTypes);
    HasFactoryValueArguments |= HasFactoryValueArgument(argument.getArgument(),context_);
    HasAddressArguments |= HasAddressTemplateArgument(argument.getArgument(),context_);
    HasFactoryArguments |= HasFactoryTemplateArgument(argument.getArgument(),context_,&FactoryTypes);
    return clang::RecursiveASTVisitor<RuntimeDeclarationFinder>::TraverseTemplateArgumentLoc(argument);
  }
  bool VisitFunctionProtoType(clang::FunctionProtoType *type) {
    const auto effects=type->getObjC3FunctionEffects();
    HasCallableEffects |= effects.Async || effects.Throws!=clang::ObjC3ThrowsKind::None;
    return true;
  }
  bool TraverseType(clang::QualType type, bool TraverseQualifier=true) {
    InspectType(type);
    // Alias-template specializations can hide their FunctionProtoType from
    // ordinary traversal. Read the desugared callable contract as well.
    const auto effects=ReadCanonicalFunctionEffects(type,context_);
    HasCallableEffects |= effects.is_async || effects.is_throwing;
    return clang::RecursiveASTVisitor<RuntimeDeclarationFinder>::TraverseType(type,TraverseQualifier);
  }
  void Reference(const clang::NamedDecl *declaration) {
    if(!declaration)return;
    StandardCoroutineReference(declaration);
    if(const auto *function=llvm::dyn_cast<clang::FunctionDecl>(declaration))
      if(const auto *arguments=function->getTemplateSpecializationArgs())InspectArguments(arguments->asArray());
    if(const auto *variable=llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(declaration))
      InspectArguments(variable->getTemplateArgs().asArray());
    Found |= IsQualifiedRuntimeServiceDeclaration(declaration,context_) ||
             IsQualifiedRuntimeTaskDeclaration(declaration,context_);
  }
  bool VisitTagType(clang::TagType *type) { Reference(type->getDecl()); return true; }
  bool VisitTypedefType(clang::TypedefType *type) { Reference(type->getDecl()); return true; }
  bool VisitExpr(clang::Expr *expr) { TraverseType(expr->getType()); return true; }
  bool VisitDeclRefExpr(clang::DeclRefExpr *expr) { Reference(expr->getDecl()); return true; }
  bool VisitOverloadExpr(clang::OverloadExpr *expr) {
    for(auto *declaration:expr->decls())Reference(declaration);
    return true;
  }
  bool VisitMemberExpr(clang::MemberExpr *expr) { Reference(expr->getMemberDecl()); return true; }
  bool VisitCXXConstructExpr(clang::CXXConstructExpr *expr) { Reference(expr->getConstructor()); return true; }
  bool TraverseLambdaExpr(clang::LambdaExpr *expression,DataRecursionQueue * = nullptr) {
    if(const auto *pattern=expression->getCallOperator()->getDescribedFunctionTemplate())
      for(const auto *specialization:pattern->specializations())
        if(const auto *arguments=specialization->getTemplateSpecializationArgs())InspectArguments(arguments->asArray());
    lambda_bodies_[expression->getBody()]=expression->getCallOperator();
    TraverseType(expression->getCallOperator()->getType());
    return clang::RecursiveASTVisitor<RuntimeDeclarationFinder>::TraverseLambdaExpr(expression,nullptr);
  }
  bool TraverseStmt(clang::Stmt *statement,DataRecursionQueue *queue=nullptr) {
    const auto *previous=function_;
    auto restore=llvm::scope_exit([&]{function_=previous;});
    if(auto found=lambda_bodies_.find(statement);found!=lambda_bodies_.end()) {
      function_=found->second;
      HasCxxCoroutines |= PublishedCxxCoroutine(function_)!=nullptr;
      statement=const_cast<clang::Stmt *>(PublishedFunctionBody(function_));
    }
    return clang::RecursiveASTVisitor<RuntimeDeclarationFinder>::TraverseStmt(statement,queue);
  }
  bool TraverseDecl(clang::Decl *declaration) {
    if (const auto *named=llvm::dyn_cast_or_null<clang::NamedDecl>(declaration)) StandardCoroutineReference(named);
    if (const auto *record=llvm::dyn_cast_or_null<clang::ClassTemplateSpecializationDecl>(declaration))
      InspectArguments(record->getTemplateArgs().asArray());
    if (const auto *variable=llvm::dyn_cast_or_null<clang::VarTemplateSpecializationDecl>(declaration))
      InspectArguments(variable->getTemplateArgs().asArray());

    if (auto *function=llvm::dyn_cast_or_null<clang::FunctionDecl>(declaration)) {
      if (const auto *arguments=function->getTemplateSpecializationArgs())
        InspectArguments(arguments->asArray());
      const auto *previous=function_;
      function_=function;
      auto restore=llvm::scope_exit([&] { function_=previous; });
      TraverseType(function->getType());
      for (const auto *parameter:function->parameters())
        if (parameter->hasDefaultArg() && !parameter->hasUninstantiatedDefaultArg())
          TraverseStmt(const_cast<clang::Expr *>(parameter->getDefaultArg()));
      if (PublishesFunctionBody(function)) {
        HasCxxCoroutines |= PublishedCxxCoroutine(function)!=nullptr;
        if (const auto *constructor=llvm::dyn_cast<clang::CXXConstructorDecl>(function))
          for (const auto *initializer:constructor->inits())
            if (initializer->isWritten()) TraverseStmt(initializer->getInit());
        TraverseStmt(const_cast<clang::Stmt *>(PublishedFunctionBody(function)));
      }
      return true;
    }
    if (auto *variable=llvm::dyn_cast_or_null<clang::VarDecl>(declaration);
        variable && variable->hasGlobalStorage() && !variable->isLocalVarDecl()) {
      TraverseType(variable->getType());
      if (PublishesVariableInitializer(variable)) TraverseStmt(variable->getInit());
      return true;
    }
    return clang::RecursiveASTVisitor<RuntimeDeclarationFinder>::TraverseDecl(declaration);
  }
  bool TraverseCoroutineBodyStmt(clang::CoroutineBodyStmt *body,
                                 DataRecursionQueue * = nullptr) {
    return TraverseStmt(body->getBody());
  }
  bool TraverseCoawaitExpr(clang::CoawaitExpr *await,DataRecursionQueue * = nullptr) {
    if (const auto *source=PublishedAwaitSource(await,function_,context_)) {
      Found |= !clang::getObjC3AwaitSourceExpression(await);
      return TraverseStmt(const_cast<clang::Expr *>(source));
    }
    return clang::RecursiveASTVisitor<RuntimeDeclarationFinder>::TraverseCoawaitExpr(await,nullptr);
  }
private:
  const clang::ASTContext &context_;
  const clang::FunctionDecl *function_=nullptr;
  std::map<const clang::Stmt *,const clang::FunctionDecl *> lambda_bodies_;
};

void VisitPublishedRuntimeContracts(const clang::NamedDecl *declaration,
                                     RuntimeDeclarationFinder &finder) {
  finder.TraverseDecl(const_cast<clang::NamedDecl *>(declaration));
  if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(declaration))
    for (const auto *specialization:PublishedClassSpecializations(pattern))
      finder.TraverseDecl(const_cast<clang::ClassTemplateSpecializationDecl *>(specialization));
  if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(declaration))
    for (const auto *partial:PublishedVariablePartials(pattern))
      finder.TraverseDecl(const_cast<clang::VarTemplatePartialSpecializationDecl *>(partial));
  for (auto *member:ClosedParentMemberSpecializations(declaration)) finder.TraverseDecl(member);
  for (auto *pattern:ClosedParentPatterns(declaration)) finder.TraverseDecl(pattern);
  for (auto *definition:DetachedCallableTemplateDefinitions(declaration)) finder.TraverseDecl(definition);
  for (auto *definition:DetachedStaticDataTemplateDefinitions(declaration)) finder.TraverseDecl(definition);
  for (auto *definition:DetachedNestedTemplateDefinitions(declaration)) finder.TraverseDecl(definition);
  for (auto *specialization:ConcreteMemberClassSpecializations(declaration)) finder.TraverseDecl(specialization);
  for (auto *specialization:ConcreteMemberFunctionSpecializations(declaration))
    if (specialization->isInlined()) finder.TraverseDecl(specialization);
  for (auto *specialization:ConcreteMemberVariableSpecializations(declaration))
    if (PublishesVariableInitializer(specialization)) finder.TraverseDecl(specialization);
}

bool HasRuntimeDeclarationContract(const clang::NamedDecl *declaration,
                                   const clang::ASTContext &context) {
  // Empty namespace wrappers do not own member runtime contracts.
  if (llvm::isa<clang::NamespaceDecl>(declaration)) return false;
  RuntimeDeclarationFinder finder(context);
  VisitPublishedRuntimeContracts(declaration,finder);
  return finder.Found;
}

bool HasEffectfulTemplateProjection(const clang::NamedDecl *declaration,
                                     const clang::ASTContext &context) {
  const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
  if (!llvm::isa<clang::TemplateDecl>(declaration) && !HasDirectMemberTemplates(record) &&
      !IsExplicitNamespaceFunctionSpecialization(llvm::dyn_cast<clang::FunctionDecl>(declaration)) &&
      !IsExplicitNamespaceVariableSpecialization(llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(declaration)))
    return false;
  RuntimeDeclarationFinder finder(context);
  VisitPublishedRuntimeContracts(declaration,finder);
  return finder.HasCallableEffects;
}

clang::Expr *KeyPathSubscriptSyntax(clang::PseudoObjectExpr *expression) {
  auto *syntax=expression->getSyntacticForm()->IgnoreParenImpCasts();
  auto *base=syntax;
  if (auto *assignment=llvm::dyn_cast<clang::BinaryOperator>(base);
      assignment && assignment->isAssignmentOp())
    base=assignment->getLHS()->IgnoreParenImpCasts();
  if (auto *increment=llvm::dyn_cast<clang::UnaryOperator>(base);
      increment && increment->isIncrementDecrementOp())
    base=increment->getSubExpr()->IgnoreParenImpCasts();
  clang::ObjC3KeyPathInfo info;
  auto *subscript=llvm::dyn_cast<clang::ObjCSubscriptRefExpr>(base);
  return subscript && clang::getObjC3KeyPathInfo(subscript->getKeyExpr()->getType(),info)
      ? syntax : nullptr;
}


// Address template values bind declaration and module ownership, never a host
// process address. Fields inherit their enclosing record's ABI owner.
bool AddressDeclarationKey(const clang::ValueDecl *declaration,
                             const clang::ASTContext &context,std::string &key) {
  if (!declaration) return false;
  if (const auto *field=llvm::dyn_cast<clang::FieldDecl>(declaration)) {
    if (!field->getParent()->hasExternalFormalLinkage()) return false;
  } else if (!llvm::isa<clang::FunctionDecl,clang::VarDecl>(declaration) ||
             !declaration->hasExternalFormalLinkage()) return false;
  const auto id=ReadInterfaceDeclarationStableId(declaration);
  if (id.empty()) return false;
  auto add=[&](llvm::StringRef value){key+=std::to_string(value.size())+":"+value.str();};
  add(declaration->getDeclKindName());add(id);add(ReadNativeSymbolName(declaration));
  add(CanonicalType(declaration->getType(),context));
  for (const auto *scope=declaration->getDeclContext();scope;scope=scope->getParent())
    if (const auto *record=llvm::dyn_cast<clang::RecordDecl>(scope)) {
      add(ReadInterfaceDeclarationStableId(record));add(ReadNativeSymbolName(record));
    }
  return true;
}

bool StructuralArgumentKey(const clang::APValue &value,const clang::ASTContext &context,
                           std::string &key,unsigned &budget,unsigned depth,
                           std::vector<const clang::NamedDecl *> *references) {
  if (!budget || depth > 64) return false;
  --budget;
  auto add = [&](llvm::StringRef text) {
    key += std::to_string(text.size()) + ":" + text.str();
  };
  auto integer = [&](const llvm::APInt &bits) {
    llvm::SmallString<64> text;
    bits.toString(text, 16, false);
    add(std::to_string(bits.getBitWidth()));
    add(text);
  };
  auto element = [&](const clang::APValue &item) {
    return StructuralArgumentKey(item,context,key,budget,depth+1,references);
  };
  add(std::to_string(value.getKind()));
  switch (value.getKind()) {
  case clang::APValue::Int:
    integer(value.getInt());
    return true;
  case clang::APValue::Float:
    // Decimal printing can collapse signed zeros or distinct NaN payloads.
    integer(value.getFloat().bitcastToAPInt());
    return true;
  case clang::APValue::Array:
    add(std::to_string(value.getArraySize()));
    if (value.getArraySize() > budget) return false;
    for (unsigned index = 0; index < value.getArraySize(); ++index)
      if (!element(index < value.getArrayInitializedElts()
                       ? value.getArrayInitializedElt(index)
                       : value.getArrayFiller())) return false;
    return true;
  case clang::APValue::Struct:
    add(std::to_string(value.getStructNumBases()));
    add(std::to_string(value.getStructNumFields()));
    for (unsigned index = 0; index < value.getStructNumBases(); ++index)
      if (!element(value.getStructBase(index))) return false;
    for (unsigned index = 0; index < value.getStructNumFields(); ++index)
      if (!element(value.getStructField(index))) return false;
    return true;
  case clang::APValue::Union:
    if (const auto *field = value.getUnionField()) {
      add(std::to_string(field->getFieldIndex()));
      return element(value.getUnionValue());
    }
    add("no-active-member");
    return true;
  case clang::APValue::LValue: {
    if (value.isNullPointer()) {add("null");return true;}
    const auto *base=value.getLValueBase().dyn_cast<const clang::ValueDecl *>();
    if (!AddressDeclarationKey(base,context,key)) return false;
    if (references) references->push_back(base);
    add(std::to_string(value.getLValueOffset().getQuantity()));
    add(value.isLValueOnePastTheEnd()?"one-past":"within");
    add(value.hasLValuePath()?"path":"no-path");
    if (!value.hasLValuePath()) return true;
    clang::QualType type=base->getType();
    add(std::to_string(value.getLValuePath().size()));
    for (const auto &entry:value.getLValuePath()) {
      if (!budget) return false;
      --budget;
      if (const auto *array=context.getAsArrayType(type)) {
        add("index");add(std::to_string(entry.getAsArrayIndex()));type=array->getElementType();
      } else if (type->isRecordType()) {
        const auto member=entry.getAsBaseOrMember();
        if (const auto *field=llvm::dyn_cast<clang::FieldDecl>(member.getPointer())) {
          add("field");add(ReadInterfaceDeclarationStableId(field));
          type=field->getType();if(references)references->push_back(field);
        } else if (const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(member.getPointer())) {
          add(member.getInt()?"virtual-base":"base");add(ReadInterfaceDeclarationStableId(record));
          add(ReadNativeSymbolName(record));type=context.getCanonicalTagType(record);
          if(references)references->push_back(record);
        } else return false;
      } else return false;
    }
    return true;
  }
  case clang::APValue::MemberPointer: {
    const auto *member=value.getMemberPointerDecl();
    if (!member) {add("null");return true;}
    if (!AddressDeclarationKey(member,context,key)) return false;
    if(references)references->push_back(member);
    add(value.isMemberPointerToDerivedMember()?"derived":"base");
    add(std::to_string(value.getMemberPointerPath().size()));
    for (const auto *record:value.getMemberPointerPath()) {
      if (!budget) return false;
      --budget;
      add(ReadInterfaceDeclarationStableId(record));add(ReadNativeSymbolName(record));
      if(references)references->push_back(record);
    }
    return true;
  }
  default:
    return false;
  }
}

bool HasAddressTemplateArgument(const clang::TemplateArgument &argument,const clang::ASTContext &context) {
  if (argument.getKind()==clang::TemplateArgument::Declaration &&
      !llvm::isa<clang::TemplateParamObjectDecl>(argument.getAsDecl())) return true;
  const clang::APValue *value=nullptr;
  clang::Expr::EvalResult evaluated;
  if (argument.getKind()==clang::TemplateArgument::Expression) {
    const auto *expression=argument.getAsExpr();
    if (!expression->isValueDependent() && expression->EvaluateAsRValue(evaluated,context) &&
        !evaluated.HasSideEffects) value=&evaluated.Val;
  }
  if (argument.getKind()==clang::TemplateArgument::StructuralValue)value=&argument.getAsStructuralValue();
  if (argument.getKind()==clang::TemplateArgument::Declaration)
    if(const auto *object=llvm::dyn_cast<clang::TemplateParamObjectDecl>(argument.getAsDecl()))value=&object->getValue();
  if (!value)return false;
  unsigned budget=4096;std::string key;std::vector<const clang::NamedDecl *> references;
  StructuralArgumentKey(*value,context,key,budget,0,&references);
  return !references.empty();
}

bool GenericArgumentKey(const clang::TemplateArgument &argument,
                         const clang::ASTContext &context,std::string &key,
                         unsigned depth) {
  if (depth>64 || argument.isDependent()) return false;
  auto add=[&](llvm::StringRef value) {key+=std::to_string(value.size())+":"+value.str();};
  auto type=[&](clang::QualType value,bool allow_factory=false) {
    if (value.isNull() || value->isDependentType())return false;
    const auto origins=FactoryTypeKey(value,context);
    const auto members=UnnamedTemplateTypeKey(value,context);
    const auto canonical=CanonicalType(value,context);
    const bool native_enums=canonical.find(";native-unnamed-enums:v1;")!=std::string::npos;
    if(value->hasUnnamedOrLocalType() || !origins.empty() || !members.empty())
      if(!allow_factory || (!native_enums && origins.empty() && members.empty()) || !HasOnlyPublishedUnnamedTypes(value))return false;
    add(canonical);return true;
  };
  add(std::to_string(argument.getKind()));
  switch(argument.getKind()) {
  case clang::TemplateArgument::Type:return type(argument.getAsType(),true);
  case clang::TemplateArgument::Integral: {
    if (!type(argument.getIntegralType(),true)) return false;
    llvm::SmallString<32> value;argument.getAsIntegral().toString(value,10);add(value);return true;
  }
  case clang::TemplateArgument::NullPtr:return type(argument.getNullPtrType(),true);
  case clang::TemplateArgument::Declaration: {
    const auto *declaration=argument.getAsDecl();
    if (const auto *object=llvm::dyn_cast<clang::TemplateParamObjectDecl>(declaration)) {
      if (!type(object->getType(),true)) return false;
      unsigned budget=4096;return StructuralArgumentKey(object->getValue(),context,key,budget);
    }
    if (!type(argument.getParamTypeForDecl(),true)) return false;
    return AddressDeclarationKey(declaration,context,key);
  }
  case clang::TemplateArgument::StructuralValue: {
    if (!type(argument.getStructuralValueType(),true)) return false;
    unsigned budget = 4096;
    return StructuralArgumentKey(argument.getAsStructuralValue(),context,key,budget);
  }
  case clang::TemplateArgument::Template: {
    const auto *pattern=argument.getAsTemplate().getAsTemplateDecl();
    if (!pattern || !llvm::isa<clang::ClassTemplateDecl,clang::TypeAliasTemplateDecl,
                              clang::BuiltinTemplateDecl>(pattern)) return false;
    add(pattern->getDeclKindName());
    add(ReadInterfaceDeclarationStableId(pattern));
    add(ReadNativeSymbolName(pattern));
    const auto *record=llvm::dyn_cast<clang::ClassTemplateDecl>(pattern);
    const auto *member=record?record->getTemplatedDecl():llvm::dyn_cast<clang::CXXRecordDecl>(pattern->getDeclContext());
    const auto origin=UnnamedTemplateRecordKey(member,context);
    add(origin.empty()?pattern->getQualifiedNameAsString():origin);
    for (const auto *scope=pattern->getDeclContext();scope;scope=scope->getParent())
      if (const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(scope)) {
        add(ReadInterfaceDeclarationStableId(specialization->getSpecializedTemplate()));
        for (const auto &enclosing:specialization->getTemplateArgs().asArray())
          if (!GenericArgumentKey(enclosing,context,key,depth+1)) return false;
      }
    return true;
  }
  case clang::TemplateArgument::Pack:
    add(std::to_string(argument.pack_size()));
    for (const auto &element:argument.pack_elements())
      if (!GenericArgumentKey(element,context,key,depth+1)) return false;
    return true;
  default:return false;
  }
}

std::string VariablePartialArgumentKey(const clang::VarTemplatePartialSpecializationDecl *partial,
                                       const clang::ASTContext &context) {
  std::string head;llvm::raw_string_ostream stream(head);
  clang::PrintingPolicy policy(context.getLangOpts());policy.PrintAsCanonical=true;
  partial->getTemplateParameters()->print(stream,context,policy);
  for (const auto &argument:partial->getTemplateArgs().asArray()) {
    stream << ";argument=";argument.print(policy,stream,true);
  }
  return ComputeInterfaceSourceSha256(head);
}

bool ClassSpecializationArgumentKey(const clang::ClassTemplateSpecializationDecl *specialization,
                                    const clang::ASTContext &context,std::string &key) {
  if (const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(specialization)) {
    std::string head;llvm::raw_string_ostream stream(head);
    clang::PrintingPolicy policy(context.getLangOpts());policy.PrintAsCanonical=true;
    partial->getTemplateParameters()->print(stream,context,policy);
    for (const auto &argument:partial->getTemplateArgs().asArray()) {
      stream << ";argument=";argument.print(policy,stream,true);
    }
    key="partial:"+ComputeInterfaceSourceSha256(head);
    return true;
  }
  for (const auto &argument:specialization->getTemplateArgs().asArray())
    if (!GenericArgumentKey(argument,context,key)) return false;
  return true;
}

bool GenericLambdaBodies(const clang::LambdaExpr *expression,
                          const clang::ASTContext &context,
                          std::vector<GenericLambdaBody> &bodies) {
  if (!expression->isGenericLambda()) return true;
  const auto *pattern=expression->getCallOperator()->getDescribedFunctionTemplate();
  if (!pattern) return false;
  std::map<std::string,const clang::FunctionDecl *> ordered;
  for (const auto *specialization:pattern->specializations()) {
    const auto *definition=specialization->getDefinition();
    const auto *arguments=specialization->getTemplateSpecializationArgs();
    if (!definition || !arguments || definition->getType()->isDependentType() || ordered.size()>=256)
      return false;
    std::string key;
    for (const auto &argument:arguments->asArray())
      if (!GenericArgumentKey(argument,context,key)) return false;
    const auto [found,inserted]=ordered.emplace(key,definition);
    if (!inserted && found->second->getCanonicalDecl()!=definition->getCanonicalDecl()) return false;
  }
  for (const auto &[key,function]:ordered) bodies.push_back({function,key});
  return true;
}

const clang::Expr *WrittenDefaultArgument(const clang::ParmVarDecl *parameter) {
  if (!parameter->hasDefaultArg()) return nullptr;
  return parameter->hasUninstantiatedDefaultArg()
      ? parameter->getUninstantiatedDefaultArg() : parameter->getDefaultArg();
}

// ABI ownership binds constraint structure, not defaults or source parameter
// names. Normalized names are used only while printing these constraints.
std::string FunctionTemplateConstraintKey(const clang::FunctionTemplateDecl *pattern) {
  auto &context = pattern->getASTContext();
  clang::PrintingPolicy policy(context.getLangOpts());
  policy.PrintAsCanonical = policy.FullyQualifiedName = true;
  struct Parameters : clang::DynamicRecursiveASTVisitor {
    clang::ASTContext &context;
    std::map<const clang::DeclContext *, unsigned> scopes;
    std::map<clang::NamedDecl *, clang::DeclarationName> saved;
    explicit Parameters(clang::ASTContext &context) : context(context) {}
    bool VisitParmVarDecl(clang::ParmVarDecl *parameter) override {
      if (saved.count(parameter)) return true;
      auto [scope, inserted] = scopes.emplace(parameter->getDeclContext(), scopes.size());
      saved.emplace(parameter, parameter->getDeclName());
      parameter->setDeclName(&context.Idents.get("p" + std::to_string(scope->second) +
          "_" + std::to_string(parameter->getFunctionScopeIndex())));
      return true;
    }
    bool VisitNamedDecl(clang::NamedDecl *parameter) override {
      unsigned depth=0,index=0;
      const char *kind=nullptr;
      if (const auto *type=llvm::dyn_cast<clang::TemplateTypeParmDecl>(parameter)) {
        depth=type->getDepth();index=type->getIndex();kind="type";
      } else if (const auto *value=llvm::dyn_cast<clang::NonTypeTemplateParmDecl>(parameter)) {
        depth=value->getDepth();index=value->getIndex();kind="value";
      } else if (const auto *nested=llvm::dyn_cast<clang::TemplateTemplateParmDecl>(parameter)) {
        depth=nested->getDepth();index=nested->getIndex();kind="template";
      }
      if (kind && !saved.count(parameter)) {
        saved.emplace(parameter,parameter->getDeclName());
        parameter->setDeclName(&context.Idents.get(std::string("__objc3_parameter_")+kind+"_"+
            std::to_string(depth)+"_"+std::to_string(index)));
      }
      return true;
    }
    bool VisitDeclRefExpr(clang::DeclRefExpr *expression) override {
      if (auto *parameter = llvm::dyn_cast<clang::ParmVarDecl>(expression->getDecl()))
        VisitParmVarDecl(parameter);
      return true;
    }
    ~Parameters() {
      for (auto &[parameter, name] : saved) parameter->setDeclName(name);
    }
  } parameters(context);
  for (auto *parameter : pattern->getTemplatedDecl()->parameters())
    parameters.VisitParmVarDecl(parameter);
  std::vector<const clang::Expr *> constraints;
  auto heads=[&](auto &&self,const clang::TemplateParameterList *list)->void {
    for (const auto *parameter:*list) {
      parameters.TraverseDecl(const_cast<clang::NamedDecl *>(parameter));
      if (const auto *type=llvm::dyn_cast<clang::TemplateTypeParmDecl>(parameter))
        constraints.push_back(type->getTypeConstraint()
            ?type->getTypeConstraint()->getImmediatelyDeclaredConstraint():nullptr);
      else if (const auto *value=llvm::dyn_cast<clang::NonTypeTemplateParmDecl>(parameter))
        constraints.push_back(value->getPlaceholderTypeConstraint());
      else if (const auto *nested=llvm::dyn_cast<clang::TemplateTemplateParmDecl>(parameter)) {
        constraints.push_back(nullptr);self(self,nested->getTemplateParameters());
      }
    }
    constraints.push_back(list->getRequiresClause());
  };
  heads(heads,pattern->getTemplateParameters());
  constraints.push_back(pattern->getTemplatedDecl()->getTrailingRequiresClause().ConstraintExpr);
  for (const auto *expression : constraints)
    parameters.TraverseStmt(const_cast<clang::Expr *>(expression));
  std::string key;
  llvm::raw_string_ostream stream(key);
  for (const auto *expression : constraints) {
    stream << "constraint{";
    if (expression) expression->printPretty(stream, nullptr, policy);
    stream << '}';
  }
  return ComputeInterfaceSourceSha256(key);
}


// Unnamed member records inherit their template owner's ABI and lexical
// declaration order. Physical source coordinates are never public identity.
std::string UnnamedTemplateRecordKey(const clang::CXXRecordDecl *record,
                                     const clang::ASTContext &context) {
  const auto *owner=ClassTemplateOwner(record);
  const clang::CXXRecordDecl *ordinary=nullptr;
  if (!owner) {
    bool saw_unnamed=false;
    for (const auto *current=record;current;
         current=llvm::dyn_cast<clang::CXXRecordDecl>(current->getDeclContext())) {
      if (current->isLambda() || current->isLocalClass()) return {};
      if (!current->getIdentifier() && !current->getTypedefNameForAnonDecl()) saw_unnamed=true;
      else if (saw_unnamed) {ordinary=current;break;}
    }
    if (!ordinary) return {};
  }
  std::vector<std::string> path;
  bool unnamed=false;
  for (unsigned depth=0;record && depth<64;++depth) {
    if (record->isLambda() || record->isLocalClass()) return {};
    const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(record);
    const auto *pattern=specialization?specialization->getSpecializedTemplate():record->getDescribedClassTemplate();
    const bool at_owner=(owner && pattern && pattern->getCanonicalDecl()==owner->getCanonicalDecl()) ||
        (ordinary && record->getCanonicalDecl()==ordinary->getCanonicalDecl());
    // Named template roots have no unnamed member path. Inspect this before
    // their arguments: a factory's local result can refer back to its callable
    // identity through those arguments and must not recurse merely to say no.
    if(at_owner && !unnamed)return {};
    std::string arguments;
    if (specialization && !ClassSpecializationArgumentKey(specialization,context,arguments)) return {};
    if (at_owner) {
      const clang::NamedDecl *publication=owner?static_cast<const clang::NamedDecl *>(owner):ordinary;
      std::string key=ReadInterfaceDeclarationStableId(publication)+":"+ReadNativeSymbolName(publication)+":"+arguments;
      for (auto i=path.rbegin();i!=path.rend();++i)key+=";"+*i;
      return key;
    }
    if (!record->getIdentifier() && !record->getTypedefNameForAnonDecl()) {
      const auto *parent=llvm::dyn_cast<clang::CXXRecordDecl>(record->getDeclContext());
      if (!parent) return {};
      unsigned index=0;bool found=false;
      for (const auto *member:parent->decls()) {
        const auto *sibling=llvm::dyn_cast<clang::CXXRecordDecl>(member);
        if (!sibling || sibling->isImplicit() || sibling->getIdentifier() ||
            sibling->getTypedefNameForAnonDecl()) continue;
        if (sibling->getCanonicalDecl()==record->getCanonicalDecl()) {found=true;break;}
        ++index;
      }
      if (!found) return {};
      unnamed=true;path.push_back("unnamed:"+std::to_string(index)+":"+std::to_string(static_cast<unsigned>(record->getTagKind())));
    } else path.push_back("named:"+record->getNameAsString()+":"+arguments);
    record=llvm::dyn_cast<clang::CXXRecordDecl>(record->getDeclContext());
  }
  return {};
}

std::string UnnamedTemplateTagKey(const clang::TagDecl *tag,const clang::ASTContext &context) {
  if(const auto *record=llvm::dyn_cast_or_null<clang::CXXRecordDecl>(tag))
    return UnnamedTemplateRecordKey(record,context);
  const auto *enumeration=llvm::dyn_cast_or_null<clang::EnumDecl>(tag);
  if(!enumeration)return {};
  const auto *parent=llvm::dyn_cast<clang::CXXRecordDecl>(enumeration->getDeclContext());
  auto origin=UnnamedTemplateRecordKey(parent,context);
  if(origin.empty())return {};
  unsigned ordinal=0;
  for(const auto *member:parent->decls())
    if(const auto *sibling=llvm::dyn_cast<clang::EnumDecl>(member)) {
      if(sibling->isImplicit())continue;
      if(sibling->getCanonicalDecl()==enumeration->getCanonicalDecl())
        return origin+";enum:"+std::to_string(ordinal)+":"+enumeration->getNameAsString();
      ++ordinal;
    }
  return {};
}

std::string UnnamedTemplateTypeKey(clang::QualType type,const clang::ASTContext &context) {
  struct Origins:clang::DynamicRecursiveASTVisitor {
    const clang::ASTContext &context;std::string key;
    explicit Origins(const clang::ASTContext &context):context(context) {}
    bool TraverseType(clang::QualType type,bool qualifier=true) override {
      return type.isNull() || clang::DynamicRecursiveASTVisitor::TraverseType(type.getCanonicalType(),qualifier);
    }
    bool TraverseTemplateArgument(const clang::TemplateArgument &argument) override {
      if(argument.getKind()==clang::TemplateArgument::Integral)return TraverseType(argument.getIntegralType());
      if(argument.getKind()==clang::TemplateArgument::NullPtr)return TraverseType(argument.getNullPtrType());
      if(argument.getKind()==clang::TemplateArgument::StructuralValue)return TraverseType(argument.getStructuralValueType());
      if(argument.getKind()==clang::TemplateArgument::Declaration) {
        if(const auto *object=llvm::dyn_cast<clang::TemplateParamObjectDecl>(argument.getAsDecl()))return TraverseType(object->getType());
        return TraverseType(argument.getParamTypeForDecl());
      }
      if(argument.getKind()==clang::TemplateArgument::Template || argument.getKind()==clang::TemplateArgument::TemplateExpansion) {
        if(const auto *pattern=argument.getAsTemplateOrTemplatePattern().getAsTemplateDecl()) {
          const auto *record=llvm::dyn_cast<clang::ClassTemplateDecl>(pattern);
          const auto *member=record?record->getTemplatedDecl():llvm::dyn_cast<clang::CXXRecordDecl>(pattern->getDeclContext());
          const auto origin=UnnamedTemplateRecordKey(member,context);
          if(!origin.empty())key+=std::to_string(origin.size())+":"+origin;
        }
        return true;
      }
      return clang::DynamicRecursiveASTVisitor::TraverseTemplateArgument(argument);
    }
    bool VisitTagType(clang::TagType *type) override {
      const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(type->getDecl());
      const auto origin=UnnamedTemplateTagKey(type->getDecl(),context);
      if(!origin.empty())key+=std::to_string(origin.size())+":"+origin;
      if(const auto *specialization=llvm::dyn_cast_or_null<clang::ClassTemplateSpecializationDecl>(record))
        for(const auto &argument:specialization->getTemplateArgs().asArray())TraverseTemplateArgument(argument);
      return true;
    }
  } visitor(context);
  if(!type.isNull())visitor.TraverseType(type);
  return visitor.key;
}

std::string NativeAttributeValue(const clang::Attr *,const clang::NamedDecl *);

class DeclarationExpressionIdentity final
    : public clang::RecursiveASTVisitor<DeclarationExpressionIdentity> {
public:
  explicit DeclarationExpressionIdentity(const clang::ASTContext &context,
                                         const clang::FunctionDecl *function = nullptr)
      : context_(context), function_(function) {
    if (function) {
      for (const auto *parameter : function->parameters()) Local(parameter);
      for (const auto *redecl : function->redecls())
        if (redecl->getNumParams()==function->getNumParams())
          for (unsigned i=0;i<function->getNumParams();++i)
            locals_.emplace(redecl->getParamDecl(i)->getCanonicalDecl(),
                locals_.at(function->getParamDecl(i)->getCanonicalDecl()));
      OuterTemplateParameters(function);
      if (const auto *pattern = function->getDescribedFunctionTemplate()) {
        for (const auto *parameter : *pattern->getTemplateParameters()) Local(parameter);
        for (const auto *redecl : pattern->redecls())
          if (redecl->getTemplateParameters()->size()==pattern->getTemplateParameters()->size())
            for (unsigned i=0;i<pattern->getTemplateParameters()->size();++i)
              locals_.emplace(redecl->getTemplateParameters()->getParam(i)->getCanonicalDecl(),
                  locals_.at(pattern->getTemplateParameters()->getParam(i)->getCanonicalDecl()));
      }
    }
  }
  void OuterTemplateParameters(const clang::DeclaratorDecl *declaration) {
    auto bind=[&](auto &&self,const clang::TemplateParameterList *head)->void {
      for (const auto *parameter:*head) {
        Local(parameter);
        if (const auto *nested=llvm::dyn_cast<clang::TemplateTemplateParmDecl>(parameter))
          self(self,nested->getTemplateParameters());
      }
    };
    for (const auto *head:declaration->getTemplateParameterLists()) bind(bind,head);
  }
  void TemplateParameters(const clang::TemplateDecl *pattern) {
    auto bind=[&](auto &&self,const clang::TemplateParameterList *parameters)->void {
      for (const auto *parameter:*parameters) {
        Local(parameter);
        if (const auto *nested=llvm::dyn_cast<clang::TemplateTemplateParmDecl>(parameter))
          self(self,nested->getTemplateParameters());
      }
    };
    bind(bind,pattern->getTemplateParameters());
    auto alias=[&](auto &&self,const clang::TemplateParameterList *from,
                   const clang::TemplateParameterList *to)->void {
      if (from->size()!=to->size()) return;
      for (unsigned i=0;i<from->size();++i) {
        locals_.emplace(from->getParam(i)->getCanonicalDecl(),
                        locals_.at(to->getParam(i)->getCanonicalDecl()));
        if (const auto *nested=llvm::dyn_cast<clang::TemplateTemplateParmDecl>(from->getParam(i)))
          if (const auto *target=llvm::dyn_cast<clang::TemplateTemplateParmDecl>(to->getParam(i)))
            self(self,nested->getTemplateParameters(),target->getTemplateParameters());
      }
    };
    if (const auto *redeclarable=llvm::dyn_cast<clang::RedeclarableTemplateDecl>(pattern))
      for (const auto *redecl:redeclarable->redecls())
        alias(alias,redecl->getTemplateParameters(),pattern->getTemplateParameters());
  }
  void Local(const clang::NamedDecl *declaration) {
    const auto *canonical = declaration->getCanonicalDecl();
    if (!locals_.count(canonical)) locals_.emplace(canonical, next_local_slot_++);
  }
  std::string ExpressionType(clang::QualType type) {
    // Dependent init-captures retain an untyped ParenListExpr until native
    // instantiation. Bind that syntax and its children without inventing a type.
    if (type.isNull()) return "<untyped-dependent-expression>";
    const auto *specialization=llvm::dyn_cast_or_null<clang::ClassTemplateSpecializationDecl>(type->getAsCXXRecordDecl());
    if(ClosedClassArguments(specialization))
      // Template spelling sugar may differ for a copied capture after replay.
      // Bind both forms to the same primary owner and converted arguments;
      // the canonical spelling below still preserves value qualifiers.
      TraverseType(context_.getCanonicalTagType(specialization));
    else TraverseType(type);
    clang::PrintingPolicy policy(context_.getLangOpts());
    policy.PrintAsCanonical = true;
    policy.FullyQualifiedName = true;
    policy.SuppressScope = false;
    policy.SuppressUnwrittenScope = false;
    policy.AnonymousTagNameStyle = static_cast<unsigned>(clang::PrintingPolicy::AnonymousTagMode::Plain);
    return type.getCanonicalType().getAsString(policy);
  }
  bool ClosedClassArguments(const clang::ClassTemplateSpecializationDecl *specialization) const {
    if (!specialization) return false;
    const auto *pattern = specialization->getSpecializedTemplate();
    const auto *owner = TemplatePublicationOwner(pattern);
    if (!owner) owner=pattern;
    // An unnamed concrete intermediate record has no independent ABI tag.
    // Its named outer template remains the owner after header replay/import.
    if (ReadNativeSymbolName(owner).empty())
      if (const auto *outer=ClassTemplateOwner(pattern);outer && !ReadNativeSymbolName(outer).empty())owner=outer;
    // Nested patterns inherit the outer primary's ABI owner. Imported nested
    // declarations have no independent tag and are outside the consumer file.
    if (ReadNativeSymbolName(owner).empty() &&
        !context_.getSourceManager().isWrittenInMainFile(owner->getLocation())) return false;
    for (const auto &argument : specialization->getTemplateArgs().asArray()) {
      std::string key;
      if (!GenericArgumentKey(argument,context_,key)) return false;
    }
    return true;
  }
  bool TraverseDeclRefExpr(clang::DeclRefExpr *expression, DataRecursionQueue * = nullptr) {
    if (ClosedClassArguments(llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(
            expression->getDecl()->getDeclContext())))
      // The exact converted class arguments are bound by Reference. Canonical
      // replay can replace a named constant in the qualifier with its value.
      return WalkUpFromDeclRefExpr(expression);
    return clang::RecursiveASTVisitor<DeclarationExpressionIdentity>::TraverseDeclRefExpr(expression);
  }
  bool TraverseTemplateSpecializationTypeLoc(clang::TemplateSpecializationTypeLoc location,
                                             bool traverse_qualifier) {
    if (const auto *record = location.getType()->getAsCXXRecordDecl();
        ClosedClassArguments(llvm::dyn_cast_or_null<clang::ClassTemplateSpecializationDecl>(record)))
      return TraverseType(context_.getCanonicalTagType(record));
    return clang::RecursiveASTVisitor<DeclarationExpressionIdentity>::TraverseTemplateSpecializationTypeLoc(location,traverse_qualifier);
  }
  bool VisitTagType(clang::TagType *type) {
    const auto *tag=type->getDecl();
    if(NativeUnnamedEnumeration(tag) && IsPublishedType(tag) && !IsExpressionLocalTag(tag))Reference(tag);
    if (const auto *specialization = llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(tag);
        ClosedClassArguments(specialization)) Reference(specialization);
    const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(tag);
    if (auto origin=UnnamedTemplateRecordKey(record,context_);!origin.empty()) {
      Add("unnamed-template-record");Add(origin);
    }
    if (!tag->getIdentifier() && IsExpressionLocalTag(tag) && !(record && record->isLambda())) {
      Local(tag);
      Add("unnamed-local-type:"+std::to_string(locals_.at(tag->getCanonicalDecl())));
    }
    return true;
  }
  bool VisitRecordType(clang::RecordType *type) {
    if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(type->getDecl());
        record && record->isLambda()) {
      Local(record);
      Add("closure-type:" + std::to_string(locals_.at(record->getCanonicalDecl())));
    }
    return true;
  }
  bool VisitLambdaExpr(clang::LambdaExpr *expression) {
    Local(expression->getLambdaClass());
    if (const auto *pattern=expression->getCallOperator()->getDescribedFunctionTemplate())
      for (const auto *parameter:*pattern->getTemplateParameters()) Local(parameter);
    Add("lambda-capture-default:" + std::to_string(expression->getCaptureDefault()));
    for (const auto &capture : expression->captures()) {
      Add("capture:" + std::to_string(capture.getCaptureKind()));
      if (capture.capturesVariable()) Local(capture.getCapturedVar());
    }
    return true;
  }
  bool VisitRequiresExpr(clang::RequiresExpr *expression) {
    for (const auto *parameter : expression->getLocalParameters()) Local(parameter);
    return true;
  }
  bool TraverseLambdaExpr(clang::LambdaExpr *expression,DataRecursionQueue * = nullptr) {
    lambda_bodies_[expression->getBody()]=expression->getCallOperator();
    if (!clang::RecursiveASTVisitor<DeclarationExpressionIdentity>::TraverseLambdaExpr(expression,nullptr))
      return false;
    std::vector<GenericLambdaBody> bodies;
    if (!GenericLambdaBodies(expression,context_,bodies)) {Add("unsupported-generic-lambda");return false;}
    for (const auto &body:bodies) {
      Add("generic-lambda-specialization");Add(body.key);
      for (const auto *parameter:body.function->parameters()) Local(parameter);
      Add(ExpressionType(body.function->getType()));
      auto *statement=const_cast<clang::Stmt *>(PublishedFunctionBody(body.function));
      lambda_bodies_[statement]=body.function;
      if (!TraverseStmt(statement)) return false;
    }
    return true;
  }
  bool TraverseStmt(clang::Stmt *statement,DataRecursionQueue *queue=nullptr) {
    const auto *previous=function_;
    auto restore=llvm::scope_exit([&]{function_=previous;});
    if(auto found=lambda_bodies_.find(statement);found!=lambda_bodies_.end()) {
      function_=found->second;
      if (auto contract=CxxCoroutineContract(function_,context_); !contract.empty()) Add(contract);
      statement=const_cast<clang::Stmt *>(PublishedFunctionBody(function_));
    }
    return clang::RecursiveASTVisitor<DeclarationExpressionIdentity>::TraverseStmt(statement,queue);
  }
  bool VisitTagDecl(clang::TagDecl *declaration) {
    if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
        record && record->isLambda()) return true;
    if (IsExpressionLocalTag(declaration)) {
      Local(declaration);
      Add("local-tag:" + std::to_string(locals_.at(declaration->getCanonicalDecl())));
      Add(declaration->clang::Decl::getDeclKindName());
      Add(declaration->getNameAsString());
    }
    return true;
  }
  bool VisitBlockExpr(clang::BlockExpr *expression) {
    const auto *block = expression->getBlockDecl();
    Add("block-this:" + std::to_string(block->capturesCXXThis()));
    Add("block-noescape:" + std::to_string(block->doesNotEscape()));
    for (const auto *parameter : block->parameters()) {
      Local(parameter); Add(ExpressionType(parameter->getType()));
    }
    for (const auto &capture : block->captures()) {
      Reference(capture.getVariable());
      Add("block-capture:" + std::to_string(static_cast<unsigned>(capture.getObjC3CaptureKind())));
      Add("block-byref:" + std::to_string(capture.isByRef()));
      Add("block-nested:" + std::to_string(capture.isNested()));
      Add("block-resource:" + std::to_string(capture.isObjC3ResourceCapture()));
      Add(ExpressionType(capture.getCaptureType()));
    }
    return true;
  }
  bool VisitStmt(clang::Stmt *statement) {
    Add(statement->getStmtClassName()); return true;
  }
  bool TraverseConstantArrayTypeLoc(clang::ConstantArrayTypeLoc location,
                                    bool = true) {
    // Canonical declaration printing resolves a constant bound to its value.
    // The enclosing expression/declaration type binds the element, qualifiers
    // and exact extent. A written sizeof/constexpr bound must not become a
    // different body merely because replay spells that same extent literally.
    // Dependent and variable array bounds retain their expression traversal.
    return WalkUpFromConstantArrayTypeLoc(location) &&
           TraverseTypeLoc(location.getElementLoc());
  }
  bool TraverseFunctionProtoTypeLoc(clang::FunctionProtoTypeLoc location,
                                     bool = true) {
    // A function-pointer type can carry source-only parameter declarations.
    // ASTImporter reconstructs that TypeLoc without them. Bind only parameters
    // belonging to an actual function; type-only parameters contribute their
    // semantic type. Block and lambda parameter lists are traversed separately.
    if (!WalkUpFromFunctionProtoTypeLoc(location) ||
        !TraverseTypeLoc(location.getReturnLoc())) return false;
    const auto *type=location.getTypePtr();
    if (const auto *error=location.getObjC3ThrowsPayloadInfo())
      if (!TraverseTypeLoc(error->getTypeLoc())) return false;
    for (unsigned index=0;index<type->getNumParams();++index) {
      auto *parameter=location.getParam(index);
      const auto *owner=parameter ?
          llvm::dyn_cast<clang::FunctionDecl>(parameter->getDeclContext()) : nullptr;
      const bool actual=owner &&
          std::find(owner->param_begin(),owner->param_end(),parameter)!=owner->param_end();
      if (actual ? !TraverseDecl(parameter) : !TraverseType(type->getParamType(index)))
        return false;
    }
    for (auto exception:type->exceptions())
      if (!TraverseType(exception)) return false;
    return !type->getNoexceptExpr() || TraverseStmt(type->getNoexceptExpr());
  }
  bool VisitVarDecl(clang::VarDecl *declaration) {
    if (declaration->isLocalVarDeclOrParm()) Local(declaration);
    Add(ExpressionType(declaration->getType()));
    Add("storage:" + std::to_string(declaration->getStorageClass()));
    Add("thread-storage:" + std::to_string(declaration->getTSCSpec()));
    Add("block-byref:" + std::to_string(declaration->hasAttr<clang::BlocksAttr>()));
    if(declaration->isLocalVarDeclOrParm()) {
      std::set<std::string> local_contracts;
      for(const auto *attribute:declaration->attrs()) {
        if(!attribute->isImplicit() && llvm::isa<clang::AlignedAttr>(attribute))
          local_contracts.insert("local-alignment:"+NativeAttributeValue(attribute,declaration));
        else if(!attribute->isImplicit() && llvm::isa<clang::SectionAttr,clang::UsedAttr,clang::RetainAttr,clang::NoSanitizeAttr>(attribute))
          local_contracts.insert("local-emission-policy:"+NativeAttributeValue(attribute,declaration));
        else if(!attribute->isImplicit() && llvm::isa<clang::ConstInitAttr,clang::NoDestroyAttr,clang::AlwaysDestroyAttr>(attribute))
          local_contracts.insert("local-storage-lifetime:"+NativeAttributeValue(attribute,declaration));
      }
      for(const auto &contract:local_contracts)Add(contract);
    }
    Add("init-style:" + std::to_string(declaration->getInitStyle()));
    if (!llvm::isa<clang::ParmVarDecl>(declaration))
      Add("nrvo:" + std::to_string(declaration->isNRVOVariable()));
    return true;
  }
  void Add(llvm::StringRef value) {
    bytes_ += std::to_string(value.size()) + ":" + value.str();
  }
  bool VisitExpr(clang::Expr *expression) {
    Add(expression->getStmtClassName());
    Add(ExpressionType(expression->getType()));
    Add(std::to_string(expression->getValueKind()));
    return true;
  }
  bool VisitCastExpr(clang::CastExpr *expression) {
    Add(expression->getCastKindName());
    return true;
  }
  bool VisitDeclRefExpr(clang::DeclRefExpr *expression) {
    Reference(expression->getDecl()); return true;
  }
  bool UnresolvedReferences(clang::OverloadExpr *expression) {
    std::map<std::string, const clang::NamedDecl *> candidates;
    for (const auto *declaration : expression->decls()) {
      const auto *target = declaration->getUnderlyingDecl();
      candidates.emplace(ReadInterfaceDeclarationStableId(target) + ":" +
                         ReadNativeSymbolName(target), target);
    }
    for (const auto &[key, target] : candidates) {
      Add("unresolved-lookup-candidate");
      Add(key);
      Reference(target);
    }
    return true;
  }
  bool VisitUnresolvedLookupExpr(clang::UnresolvedLookupExpr *expression) {
    return UnresolvedReferences(expression);
  }
  bool VisitUnresolvedMemberExpr(clang::UnresolvedMemberExpr *expression) {
    return UnresolvedReferences(expression);
  }
  bool VisitMemberExpr(clang::MemberExpr *expression) {
    Reference(expression->getMemberDecl()); return true;
  }
  bool VisitCXXConstructExpr(clang::CXXConstructExpr *expression) {
    Reference(expression->getConstructor()); return true;
  }
  bool VisitCXXNewExpr(clang::CXXNewExpr *expression) {
    Reference(expression->getOperatorNew());
    Reference(expression->getOperatorDelete());
    return true;
  }
  bool VisitCXXDeleteExpr(clang::CXXDeleteExpr *expression) {
    Reference(expression->getOperatorDelete()); return true;
  }
  bool VisitObjCMessageExpr(clang::ObjCMessageExpr *expression) {
    Reference(expression->getMethodDecl()); return true;
  }
  bool TraverseConceptReference(clang::ConceptReference *reference) {
    if (!reference) return true;
    Reference(reference->getNamedConcept());
    return clang::RecursiveASTVisitor<DeclarationExpressionIdentity>::TraverseConceptReference(reference);
  }
  void Reference(const clang::NamedDecl *declaration) {
    if (!declaration) { Add("<none>"); return; }
    if (llvm::isa<clang::ConceptDecl>(declaration)) {
      Add("concept-reference");
      Add(ReadInterfaceDeclarationStableId(declaration));
      Add(ReadCxxConceptOwner(declaration));
      return;
    }
    if (const auto *object = llvm::dyn_cast<clang::TemplateParamObjectDecl>(declaration)) {
      Add("structural-template-object");
      Add(ExpressionType(object->getType()));
      unsigned budget = 4096;
      std::string key;
      if (!StructuralArgumentKey(object->getValue(),context_,key,budget))
        Add("unsupported-structural-value");
      Add(key);
      return;
    }
    // Callee types expose specialized noexcept expressions before their
    // lambda or public template syntax is traversed. Bind those parameters
    // lexically, independently of producer source locations.
    if (const auto *parameter = llvm::dyn_cast<clang::ParmVarDecl>(declaration))
      if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(parameter->getDeclContext());
          function && (function->getDescribedFunctionTemplate() || function->getPrimaryTemplate()))
        Local(parameter);
    if (const auto *parameter = llvm::dyn_cast<clang::ParmVarDecl>(declaration))
      if (const auto *method = llvm::dyn_cast<clang::CXXMethodDecl>(parameter->getDeclContext());
          method && method->getParent()->isLambda()) Local(parameter);
    if (const auto *tag = llvm::dyn_cast<clang::TagDecl>(declaration);
        tag && IsExpressionLocalTag(tag)) Local(tag);
    if (const auto found = locals_.find(declaration->getCanonicalDecl()); found != locals_.end()) {
      Add("local:" + std::to_string(found->second)); return;
    }
    if (const auto *owner = llvm::dyn_cast<clang::TagDecl>(declaration->getDeclContext());
        owner && (IsExpressionLocalTag(owner) ||
                  (llvm::isa<clang::CXXRecordDecl>(owner) &&
                   llvm::cast<clang::CXXRecordDecl>(owner)->isLambda()))) {
      Local(owner);
      Add("local-type-member:" + std::to_string(locals_.at(owner->getCanonicalDecl())));
      Add(declaration->getDeclKindName());
      // Native lambda constructor/destructor names contain physical locations.
      // Their operator/type contract already distinguishes closure members.
      const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(owner);
      const bool unnamed_structor=record && !record->getIdentifier() &&
          llvm::isa<clang::CXXConstructorDecl,clang::CXXDestructorDecl>(declaration);
      if ((!record || !record->isLambda()) && !unnamed_structor)
        Add(declaration->getNameAsString());
      if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration)) {
        Add(std::to_string(function->getOverloadedOperator()));
        Add(ExpressionType(function->getType()));
      } else if (const auto *field = llvm::dyn_cast<clang::FieldDecl>(declaration)) {
        unsigned index = 0;
        for (const auto *member : llvm::cast<clang::RecordDecl>(owner)->fields()) {
          if (member == field) break;
          ++index;
        }
        Add(std::to_string(index));
      } else if (const auto *constant = llvm::dyn_cast<clang::EnumConstantDecl>(declaration)) {
        llvm::SmallString<32> value;
        constant->getInitVal().toString(value, 10);
        Add(value);
      }
      return;
    }
    const auto *record_reference=llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
    if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(declaration))record_reference=pattern->getTemplatedDecl();
    if (!record_reference)record_reference=llvm::dyn_cast<clang::CXXRecordDecl>(declaration->getDeclContext());
    if (auto origin=UnnamedTemplateRecordKey(record_reference,context_);!origin.empty()) {
      Add("unnamed-template-record-member");Add(origin);Add(declaration->getDeclKindName());
      if (!llvm::isa<clang::CXXConstructorDecl,clang::CXXDestructorDecl>(declaration))Add(declaration->getNameAsString());
      const auto *function=llvm::dyn_cast<clang::FunctionDecl>(declaration);
      if (const auto *pattern=llvm::dyn_cast<clang::FunctionTemplateDecl>(declaration)) {
        function=pattern->getTemplatedDecl();Add(FunctionTemplateConstraintKey(pattern));
      }
      if (function) {
        Add(ExpressionType(function->getType()));
        if (const auto *arguments=function->getTemplateSpecializationArgs())
          for (const auto &argument:arguments->asArray()) {
            std::string key;
            if (GenericArgumentKey(argument,context_,key))Add(key);
          }
      }
      return;
    }
    for (const clang::NamedDecl *owner=declaration; owner;
         owner=llvm::dyn_cast<clang::NamedDecl>(owner->getDeclContext())) {
      const auto *class_specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(owner);
      if (!class_specialization) continue;
      std::vector<std::string> keys;
      bool complete = true;
      for (const auto &argument : class_specialization->getTemplateArgs().asArray()) {
        std::string key;
        if (!GenericArgumentKey(argument, context_, key)) {complete=false;break;}
        keys.push_back(std::move(key));
      }
      if (complete) {
        Add("class-template-specialization");
        Add(ReadInterfaceDeclarationStableId(class_specialization->getSpecializedTemplate()));
        Add(ReadNativeSymbolName(class_specialization->getSpecializedTemplate()));
        for (const auto &key : keys) Add(key);
      }
    }
    if (const auto *variable = llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(declaration)) {
      if (IsExplicitNamespaceVariableSpecialization(variable)) {
        Add("explicit-variable-specialization");
        Add(ReadInterfaceDeclarationStableId(variable));Add(ReadNativeSymbolName(variable));
      }
      std::vector<std::string> keys;
      bool complete = true;
      for (const auto &argument : variable->getTemplateArgs().asArray()) {
        std::string key;
        if (!GenericArgumentKey(argument, context_, key)) { complete = false; break; }
        keys.push_back(std::move(key));
      }
      if (complete) {
        const auto *pattern = variable->getSpecializedTemplate();
        Add("variable-template-specialization");
        Add(ReadInterfaceDeclarationStableId(pattern));
        Add(ReadNativeSymbolName(pattern));
        for (const auto &key : keys) Add(key);
        return;
      }
    }
    if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration))
      if (const auto *pattern = function->getPrimaryTemplate()) {
        if (IsExplicitNamespaceFunctionSpecialization(function)) {
          Add("explicit-function-specialization");
          Add(ReadInterfaceDeclarationStableId(function));Add(ReadNativeSymbolName(function));
        }
        std::vector<std::string> keys;
        const auto *arguments = function->getTemplateSpecializationArgs();
        bool complete = arguments != nullptr;
        if (arguments)
          for (const auto &argument : arguments->asArray()) {
            std::string key;
            if (!GenericArgumentKey(argument, context_, key)) { complete=false; break; }
            keys.push_back(std::move(key));
          }
        if (complete) {
          Add("function-template-specialization");
          Add(ReadInterfaceDeclarationStableId(pattern));
          Add(ReadNativeSymbolName(pattern));
          for (const auto &key : keys) Add(key);
          return;
        }
      }
    Add(ReadInterfaceDeclarationStableId(declaration));
    Add(ReadNativeSymbolName(declaration));
  }
  bool TraverseCoreturnStmt(clang::CoreturnStmt *statement,
                            DataRecursionQueue * = nullptr) {
    const auto *type=clang::getObjC3CallableFunctionType(function_);
    if ((type && type->getObjC3FunctionEffects().Async) || PublishedCxxCoroutine(function_))
      return WalkUpFromCoreturnStmt(statement) && TraverseStmt(statement->getOperand());
    return clang::RecursiveASTVisitor<DeclarationExpressionIdentity>::
        TraverseCoreturnStmt(statement,nullptr);
  }
  bool TraverseCoyieldExpr(clang::CoyieldExpr *expression,
                           DataRecursionQueue * = nullptr) {
    if (PublishedCxxCoroutine(function_))
      return expression->getWrittenOperand() && WalkUpFromCoyieldExpr(expression) && TraverseStmt(expression->getWrittenOperand());
    return clang::RecursiveASTVisitor<DeclarationExpressionIdentity>::TraverseCoyieldExpr(expression,nullptr);
  }
  bool TraverseCoawaitExpr(clang::CoawaitExpr *expression,
                           DataRecursionQueue * = nullptr) {
    if (PublishedCxxCoroutine(function_))
      return WalkUpFromCoawaitExpr(expression) && TraverseStmt(expression->getOperand());
    if (const auto *source=PublishedAwaitSource(expression,function_,context_))
      return WalkUpFromCoawaitExpr(expression) &&
             TraverseStmt(const_cast<clang::Expr *>(source));
    return clang::RecursiveASTVisitor<DeclarationExpressionIdentity>::
        TraverseCoawaitExpr(expression,nullptr);
  }
  bool TraverseCallExpr(clang::CallExpr *expression,
                         DataRecursionQueue * = nullptr) {
    clang::ObjC3DependentKeyPathLiteralInfo literal;
    if (clang::getObjC3DependentKeyPathLiteral(expression, literal)) {
      Add("keypath-dependent-literal:v2");
      Add(ExpressionType(literal.Root->getType()));
      for (unsigned I = 1; I < expression->getNumArgs(); ++I)
        Add(llvm::cast<clang::StringLiteral>(
            expression->getArg(I)->IgnoreParenImpCasts())->getString().str());
      return true;
    }
    clang::ObjC3KeyPathApplicationInfo application;
    if (clang::getObjC3KeyPathApplication(expression, application)) {
      Add("keypath-application:"+std::to_string(application.Operation));
      Add(ExpressionType(expression->getType()));
      for (auto *argument : application.Syntax->arguments())
        if (!TraverseStmt(const_cast<clang::Expr *>(argument))) return false;
      return true;
    }
    return clang::RecursiveASTVisitor<DeclarationExpressionIdentity>::
        TraverseCallExpr(expression, nullptr);
  }
  bool TraversePseudoObjectExpr(clang::PseudoObjectExpr *expression,
                                DataRecursionQueue * = nullptr) {
    if (const auto *source=clang::getObjC3IsolationSourceSyntax(expression))
      return TraverseStmt(const_cast<clang::Expr *>(source));
    clang::ObjC3KeyPathApplicationInfo application;
    if (clang::getObjC3KeyPathApplication(expression,application)) {
      Add("keypath-application:"+std::to_string(application.Operation));
      Add(ExpressionType(expression->getType()));
      for (auto *argument:application.Syntax->arguments())
        if (!TraverseStmt(const_cast<clang::Expr *>(argument))) return false;
      return true;
    }
    if (auto *syntax=KeyPathSubscriptSyntax(expression)) {
      Add("keypath-subscript");Add(ExpressionType(expression->getType()));
      return TraverseStmt(syntax);
    }
    return clang::RecursiveASTVisitor<DeclarationExpressionIdentity>::
        TraversePseudoObjectExpr(expression,nullptr);
  }
  bool TraverseObjC3CarrierTransferExpr(clang::ObjC3CarrierTransferExpr *expression,
                                        DataRecursionQueue * = nullptr) {
    if (!WalkUpFromObjC3CarrierTransferExpr(expression)) return false;
    Add(expression->isMove() ? "carrier-transfer:move" : "carrier-transfer:copy");
    auto *source = expression->getSource();
    // Implicit return moves add a same-type xvalue cast. Published moveValue
    // syntax selects the identical payload recipe directly from the lvalue.
    // Normalize only that synthetic cast; keep qualifiers, explicit casts,
    // the chosen operation and every native payload initializer in identity.
    if (expression->isMove())
      if (auto *cast = llvm::dyn_cast_or_null<clang::ImplicitCastExpr>(source);
          cast && cast->getCastKind() == clang::CK_NoOp && cast->isXValue() &&
          cast->getSubExpr()->isLValue() &&
          context_.hasSameType(cast->getType(), cast->getSubExpr()->getType()))
        source = cast->getSubExpr();
    return TraverseStmt(source) && TraverseStmt(expression->getValueInit()) &&
           TraverseStmt(expression->getSuccessInit()) &&
           TraverseStmt(expression->getErrorInit());
  }
  bool TraverseOpaqueValueExpr(clang::OpaqueValueExpr *expression,
                               DataRecursionQueue * = nullptr) {
    return TraverseStmt(expression->getSourceExpr());
  }
  bool TraverseCompoundLiteralExpr(clang::CompoundLiteralExpr *expression,
                                   DataRecursionQueue * = nullptr) {
    if (const auto *path=clang::getObjC3KeyPathLiteralPath(expression)) {
      Add("keypath-literal");Add(ExpressionType(expression->getType()));Add(path->getString());
      return true;
    }
    return clang::RecursiveASTVisitor<DeclarationExpressionIdentity>::
        TraverseCompoundLiteralExpr(expression,nullptr);
  }
  // Native printers insert protective parentheses for carrier operators. A
  // reparse retains those ParenExpr nodes; they do not change typed semantics.
  bool TraverseParenExpr(clang::ParenExpr *expression,
                         DataRecursionQueue * = nullptr) {
    return TraverseStmt(expression->getSubExpr());
  }
  std::string Get(clang::Stmt *expression,bool parameter_bindings=false) {
    clang::PrintingPolicy policy(context_.getLangOpts());
    const bool unnamed=ContainsUnnamedLocalType(expression);
    // Canonical types erase owned-tag spelling. Keep complete unnamed local
    // definitions in the text binding; typed references use lexical slots.
    policy.PrintAsCanonical = !unnamed;
    policy.IncludeTagDefinition = unnamed;
    policy.AnonymousTagNameStyle = static_cast<unsigned>(
        clang::PrintingPolicy::AnonymousTagMode::Plain);
    std::string printed;
    llvm::raw_string_ostream stream(printed);
    struct CanonicalParens final : clang::PrinterHelper {
      clang::PrintingPolicy policy;
      const std::map<const clang::Decl *,unsigned> *parameters;
      CanonicalParens(clang::PrintingPolicy policy,const std::map<const clang::Decl *,unsigned> *parameters)
          :policy(policy),parameters(parameters) {}
      bool handledStmt(clang::Stmt *statement, llvm::raw_ostream &out) override {
        if (const auto *paren = llvm::dyn_cast<clang::ParenExpr>(statement)) {
          paren->getSubExpr()->printPretty(out, this, policy);
          return true;
        }
        // Native declaration replay may qualify an unqualified static member
        // reference. Bind both spellings to the same declaration spelling;
        // the typed traversal below still binds its owner and exact arguments.
        if (const auto *reference=llvm::dyn_cast<clang::DeclRefExpr>(statement)) {
          if(parameters && llvm::isa<clang::ParmVarDecl>(reference->getDecl()))
            if(const auto slot=parameters->find(reference->getDecl()->getCanonicalDecl());slot!=parameters->end()) {
              out << "__objc3_parameter_" << slot->second;return true;
            }
          if(const auto *constant=llvm::dyn_cast<clang::EnumConstantDecl>(reference->getDecl());
              constant && IsPublishedType(llvm::cast<clang::EnumDecl>(constant->getDeclContext())) &&
              !IsExpressionLocalTag(llvm::cast<clang::EnumDecl>(constant->getDeclContext()))) {
            constant->printQualifiedName(out,policy);return true;
          }
          // Detached specialization replay qualifies ordinary namespace calls.
          // Keep the spelling independent of that lexical replay context; the
          // typed traversal binds the selected overload and its native symbol.
          if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(reference->getDecl());
              function && function->getIdentifier() && function->getDeclContext()->isFileContext() &&
              !function->getPrimaryTemplate() && !function->getDescribedFunctionTemplate() &&
              !reference->hasExplicitTemplateArgs()) {
            function->printQualifiedName(out,policy);
            return true;
          }
          if (const auto *variable=llvm::dyn_cast<clang::VarDecl>(reference->getDecl());
              variable && variable->isStaticDataMember() && variable->getIdentifier() &&
              !variable->getDescribedVarTemplate() &&
              !llvm::isa<clang::VarTemplateSpecializationDecl>(variable)) {
            variable->printQualifiedName(out,policy);
            return true;
          }
        }
        return false;
      }
    } helper(policy,parameter_bindings?&locals_:nullptr);
    expression->printPretty(stream, &helper, policy);
    Add(printed);
    TraverseStmt(expression);
    llvm::SHA256 hash; hash.update(bytes_);
    return llvm::toHex(hash.final(),true);
  }
private:
  const clang::ASTContext &context_;
  const clang::FunctionDecl *function_;
  std::map<const clang::Stmt *,const clang::FunctionDecl *> lambda_bodies_;
  std::string bytes_;
  std::map<const clang::Decl *, unsigned> locals_;
  unsigned next_local_slot_ = 0;
};

std::string DeclarationExpressionContract(const clang::Expr *expression,
                                           const clang::ASTContext &context) {
  return DeclarationExpressionIdentity(context).Get(const_cast<clang::Expr *>(expression));
}

std::string FunctionTemplateHeadContract(const clang::TemplateDecl *pattern,
                                         const clang::ASTContext &context) {
  const auto *function_pattern = llvm::dyn_cast<clang::FunctionTemplateDecl>(pattern);
  const auto *function = function_pattern ? function_pattern->getTemplatedDecl() : nullptr;
  DeclarationExpressionIdentity identity(context, function);
  identity.TemplateParameters(pattern);
  auto expression = [&](const clang::Expr *value) {
    return value ? identity.Get(const_cast<clang::Expr *>(value)) : std::string("none");
  };
  std::string head;
  auto field = [&](llvm::StringRef value) { head += std::to_string(value.size()) + ":" + value.str(); };
  auto parameters=[&](auto &&self,const clang::TemplateParameterList *list)->void {
    field(std::to_string(list->size()));
    for (const auto *parameter:*list) {
    field(parameter->getDeclKindName());
    if (const auto *type = llvm::dyn_cast<clang::TemplateTypeParmDecl>(parameter)) {
      field(type->isParameterPack() ? "pack" : "single");
      field(type->getTypeConstraint()
          ? expression(type->getTypeConstraint()->getImmediatelyDeclaredConstraint()) : "none");
      field(type->hasDefaultArgument()
          ? CanonicalType(type->getDefaultArgument().getArgument().getAsType(), context) : "none");
    } else if (const auto *value = llvm::dyn_cast<clang::NonTypeTemplateParmDecl>(parameter)) {
      field(value->isParameterPack() ? "pack" : "single");
      field(CanonicalType(value->getType(), context));
      field(expression(value->getPlaceholderTypeConstraint()));
      if (value->hasDefaultArgument()) {
        const auto &argument = value->getDefaultArgument().getArgument();
        if (argument.getKind() == clang::TemplateArgument::Expression)
          field(expression(argument.getAsExpr()));
        else {
          std::string printed;llvm::raw_string_ostream stream(printed);
          clang::PrintingPolicy policy(context.getLangOpts());policy.PrintAsCanonical=true;
          argument.print(policy, stream, true);field(printed);
        }
      } else field("none");
    } else if (const auto *nested=llvm::dyn_cast<clang::TemplateTemplateParmDecl>(parameter)) {
      field(nested->isParameterPack()?"pack":"single");
      field(std::to_string(nested->templateParameterKind()));
      self(self,nested->getTemplateParameters());
      if (nested->hasDefaultArgument()) {
        const auto &argument=nested->getDefaultArgument().getArgument();
        std::string key;
        if (GenericArgumentKey(argument,context,key)) field(key);
        else {
          std::string printed;llvm::raw_string_ostream stream(printed);
          clang::PrintingPolicy policy(context.getLangOpts());policy.PrintAsCanonical=true;
          argument.print(policy,stream,true);field(printed);
        }
      } else field("none");
    }
    }
    field(expression(list->getRequiresClause()));
  };
  parameters(parameters,pattern->getTemplateParameters());
  if (function) {
    field(expression(function->getTrailingRequiresClause().ConstraintExpr));
    for (const auto *parameter : function->parameters())
      field(expression(WrittenDefaultArgument(parameter)));
  }
  return ComputeInterfaceSourceSha256(head);
}

std::string ConceptIdentity(const clang::ConceptDecl *declaration,
                            const clang::ASTContext &context) {
  clang::PrintingPolicy policy(context.getLangOpts());
  policy.AnonymousTagNameStyle = static_cast<unsigned>(
      clang::PrintingPolicy::AnonymousTagMode::Plain);
  std::string head;
  llvm::raw_string_ostream stream(head);
  declaration->getTemplateParameters()->print(stream, context, policy);
  DeclarationExpressionIdentity identity(context);
  for (const auto *parameter : *declaration->getTemplateParameters()) identity.Local(parameter);
  return "concept{owner=" + ReadCxxConceptOwner(declaration) +
      ";parameters=" + ComputeInterfaceSourceSha256(head) + ";constraint=" +
      identity.Get(const_cast<clang::Expr *>(declaration->getConstraintExpr())) +
      "};cxx-concepts:v1;";
}

std::string FunctionDefinitionContract(const clang::FunctionDecl *function,
                                        const clang::ASTContext &context) {
  const auto *definition = function->getDefinition();
  if (!definition) return "declaration";
  DeclarationExpressionIdentity identity(context, definition);
  if (const auto *constructor = llvm::dyn_cast<clang::CXXConstructorDecl>(definition))
    for (const auto *initializer : constructor->inits()) {
      if (!initializer->isWritten()) continue;
      if (initializer->isMemberInitializer()) identity.Reference(initializer->getMember());
      else if (initializer->isBaseInitializer())
        identity.Add(CanonicalType(clang::QualType(initializer->getBaseClass(), 0), context));
      identity.Add(identity.Get(initializer->getInit()));
    }
  if (auto contract=CxxCoroutineContract(definition,context); !contract.empty()) identity.Add(contract);
  return identity.Get(const_cast<clang::Stmt *>(PublishedFunctionBody(definition)));
}

std::string NativeAttributeValue(const clang::Attr *attribute,const clang::NamedDecl *declaration) {
  std::string key=std::to_string(attribute->getKind());
  auto add=[&](llvm::StringRef value){key+=";"+std::to_string(value.size())+":"+value.str();};
  if(const auto *section=llvm::dyn_cast<clang::SectionAttr>(attribute))
    add(section->getName());
  else if(const auto *sanitizers=llvm::dyn_cast<clang::NoSanitizeAttr>(attribute)) {
    std::set<std::string> names;
    for(auto name:sanitizers->sanitizers())names.insert(name.str());
    for(const auto &name:names)add(name);
  } else if(const auto *priority=llvm::dyn_cast<clang::InitPriorityAttr>(attribute))
    add(std::to_string(priority->getPriority()));
  else if(llvm::isa<clang::ConstructorAttr,clang::DestructorAttr>(attribute)) {
    const auto *constructor=llvm::dyn_cast<clang::ConstructorAttr>(attribute);
    const auto *destructor=llvm::dyn_cast<clang::DestructorAttr>(attribute);
    auto *expression=constructor?constructor->getPriority():destructor->getPriority();
    clang::Expr::EvalResult value;
    if(!expression)add(std::to_string(clang::ConstructorAttr::DefaultPriority));
    else if(expression->EvaluateAsInt(value,declaration->getASTContext())) {
      llvm::SmallString<32> number;value.Val.getInt().toString(number,10);add(number);
    } else {
      DeclarationExpressionIdentity identity(declaration->getASTContext(),llvm::dyn_cast<clang::FunctionDecl>(declaration));
      if(const auto *function=llvm::dyn_cast<clang::FunctionDecl>(declaration))
        if(const auto *pattern=function->getDescribedFunctionTemplate())identity.TemplateParameters(pattern);
      add(identity.Get(expression));
    }
  } else if(const auto *alignment=llvm::dyn_cast<clang::AlignedAttr>(attribute)) {
    add(alignment->isAlignas() ? "alignas" : "aligned");
    if(!alignment->isAlignmentDependent())
      add(std::to_string(alignment->getAlignment(declaration->getASTContext())));
    else if(alignment->isAlignmentExpr()) {
      DeclarationExpressionIdentity identity(declaration->getASTContext(),llvm::dyn_cast<clang::FunctionDecl>(declaration));
      auto bind_scope=[&](auto &&self,const clang::DeclContext *scope)->void {
        if(!scope)return;
        self(self,scope->getParent());
        if(const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(scope))
          if(const auto *pattern=record->getDescribedClassTemplate())identity.TemplateParameters(pattern);
        if(const auto *function=llvm::dyn_cast<clang::FunctionDecl>(scope))
          if(const auto *pattern=function->getDescribedFunctionTemplate())identity.TemplateParameters(pattern);
      };
      bind_scope(bind_scope,declaration->getDeclContext());
      if(const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(declaration))
        if(const auto *pattern=record->getDescribedClassTemplate())identity.TemplateParameters(pattern);
      if(const auto *variable=llvm::dyn_cast<clang::VarDecl>(declaration))
        if(const auto *pattern=variable->getDescribedVarTemplate())identity.TemplateParameters(pattern);
      add(identity.Get(alignment->getAlignmentExpr()));
    } else add(CanonicalType(alignment->getAlignmentType()->getType(),declaration->getASTContext()));
  } else if(const auto *result=llvm::dyn_cast<clang::WarnUnusedResultAttr>(attribute)) {
    add(result->IsCXX11NoDiscard() ? "nodiscard" : "warn-unused-result");add(result->getMessage());
  } else if(const auto *visibility=llvm::dyn_cast<clang::VisibilityAttr>(attribute))
    add(std::to_string(visibility->getVisibility()));
  else if(const auto *visibility=llvm::dyn_cast<clang::TypeVisibilityAttr>(attribute))
    add(std::to_string(visibility->getVisibility()));
  auto parameter=[&](clang::ParamIdx index){add(index.isValid()?std::to_string(index.getASTIndex()):"none");};
  if(const auto *size=llvm::dyn_cast<clang::PassObjectSizeAttr>(attribute)) {
    add(std::to_string(size->getType()));add(size->isDynamic()?"dynamic":"static");
  } else if(const auto *sentinel=llvm::dyn_cast<clang::SentinelAttr>(attribute)) {
    add(std::to_string(sentinel->getSentinel()));add(std::to_string(sentinel->getNullPos()));
  } else if(const auto *callback=llvm::dyn_cast<clang::CallbackAttr>(attribute)) {
    for(int position:callback->encoding())add(std::to_string(position));
  } else if(llvm::isa<clang::DiagnoseIfAttr,clang::EnableIfAttr>(attribute)) {
    const auto *diagnostic=llvm::dyn_cast<clang::DiagnoseIfAttr>(attribute);
    const auto *enabled=llvm::dyn_cast<clang::EnableIfAttr>(attribute);
    DeclarationExpressionIdentity identity(declaration->getASTContext(),llvm::dyn_cast<clang::FunctionDecl>(declaration));
    auto bind=[&](auto &&self,const clang::DeclContext *scope)->void {
      if(!scope)return;self(self,scope->getParent());
      if(const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(scope))
        if(const auto *pattern=record->getDescribedClassTemplate())identity.TemplateParameters(pattern);
      if(const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(scope))
        for(auto *parameter:*partial->getTemplateParameters())identity.Local(parameter);
    };
    bind(bind,declaration->getDeclContext());
    if(const auto *method=llvm::dyn_cast<clang::ObjCMethodDecl>(declaration))
      for(const auto *parameter:method->parameters())identity.Local(parameter);
    add(identity.Get(diagnostic?diagnostic->getCond():enabled->getCond(),true));
    add(diagnostic?diagnostic->getMessage():enabled->getMessage());
    if(diagnostic) {add(std::to_string(diagnostic->getDefaultSeverity()));add(diagnostic->getWarningGroup());}
  }
  if (const auto *ownership=llvm::dyn_cast<clang::OwnershipAttr>(attribute)) {
    add(ownership->isReturns()?"returns":ownership->isTakes()?"takes":"holds");
    add(ownership->getModule()->getName());
    std::set<unsigned> indices;
    for(auto index:ownership->args())indices.insert(index.getASTIndex());
    for(auto index:indices)add(std::to_string(index));
  } else if (const auto *tag=llvm::dyn_cast<clang::ArgumentWithTypeTagAttr>(attribute)) {
    add(tag->getArgumentKind()->getName());parameter(tag->getArgumentIdx());
    parameter(tag->getTypeTagIdx());add(tag->getIsPointer()?"pointer":"value");
  } else if (const auto *tag=llvm::dyn_cast<clang::TypeTagForDatatypeAttr>(attribute)) {
    add(tag->getArgumentKind()->getName());
    add(CanonicalType(tag->getMatchingCType(),declaration->getASTContext()));
    add(tag->getLayoutCompatible()?"layout-compatible":"exact-type");
    add(tag->getMustBeNull()?"null":"nonnull-permitted");
  } else if (const auto *format=llvm::dyn_cast<clang::FormatMatchesAttr>(attribute)) {
    add(format->getType()->getName());add(std::to_string(format->getFormatIdx()));
    DeclarationExpressionIdentity identity(declaration->getASTContext(),llvm::dyn_cast<clang::FunctionDecl>(declaration));
    add(identity.Get(format->getExpectedFormat()));
  } else if (const auto *builtin=llvm::dyn_cast<clang::DiagnoseAsBuiltinAttr>(attribute)) {
    add(builtin->getFunction()->getName());
    for(unsigned index:builtin->argIndices())add(std::to_string(index));
  }
  if (const auto *allocator=llvm::dyn_cast<clang::RestrictAttr>(attribute)) {
    if (auto *deallocator=allocator->getDeallocator()) {
      DeclarationExpressionIdentity identity(declaration->getASTContext(),llvm::dyn_cast<clang::FunctionDecl>(declaration));
      add(identity.Get(deallocator));parameter(allocator->getDeallocatorPtrArgIndex());
    }
  } else if (const auto *size=llvm::dyn_cast<clang::AllocSizeAttr>(attribute)) {
    parameter(size->getElemSizeParam());parameter(size->getNumElemsParam());
  } else if (const auto *alignment=llvm::dyn_cast<clang::AllocAlignAttr>(attribute)) parameter(alignment->getParamIndex());
  else if (const auto *nonnull=llvm::dyn_cast<clang::NonNullAttr>(attribute)) {
    std::set<unsigned> indices;for(auto index:nonnull->args())indices.insert(index.getASTIndex());
    for(auto index:indices)add(std::to_string(index));
  } else if (const auto *format=llvm::dyn_cast<clang::FormatAttr>(attribute)) {
    add(format->getType()->getName());add(std::to_string(format->getFormatIdx()));add(std::to_string(format->getFirstArg()));
  } else if (const auto *format=llvm::dyn_cast<clang::FormatArgAttr>(attribute)) parameter(format->getFormatIdx());
  else if (const auto *alignment=llvm::dyn_cast<clang::AssumeAlignedAttr>(attribute)) {
    DeclarationExpressionIdentity identity(declaration->getASTContext(),llvm::dyn_cast<clang::FunctionDecl>(declaration));
    auto bind=[&](auto &&self,const clang::DeclContext *scope)->void {
      if(!scope)return;self(self,scope->getParent());
      if(const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(scope))
        if(const auto *pattern=record->getDescribedClassTemplate())identity.TemplateParameters(pattern);
      if(const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(scope))
        for(auto *parameter:*partial->getTemplateParameters())identity.Local(parameter);
    };
    bind(bind,declaration->getDeclContext());
    add(identity.Get(alignment->getAlignment()));
    add(alignment->getOffset()?identity.Get(alignment->getOffset()):"zero");
  }
  return key;
}

const clang::CXXRecordDecl *FriendOwner(const clang::NamedDecl *declaration) {
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration))
    for (const auto *redecl : function->redecls())
      if (redecl->getFriendObjectKind() != clang::Decl::FOK_None)
        if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(redecl->getLexicalDeclContext()))
          return record;
  return nullptr;
}

bool IsInClassDefaultedComparison(const clang::FunctionDecl *function) {
  if (!function) return false;
  function = function->getMostRecentDecl();
  if (!function->isExplicitlyDefaulted() || !function->getLexicalDeclContext()->isRecord())
    return false;
  const auto effects = ReadCanonicalFunctionEffects(function->getType(), function->getASTContext());
  if (effects.is_async || effects.is_throwing) return false;
  switch (function->getOverloadedOperator()) {
  case clang::OO_EqualEqual: case clang::OO_ExclaimEqual:
  case clang::OO_Less: case clang::OO_LessEqual:
  case clang::OO_Greater: case clang::OO_GreaterEqual:
  case clang::OO_Spaceship: return true;
  default: return false;
  }
}

bool IsConcreteFriend(const clang::FriendDecl *declaration) {
  if (declaration->isPackExpansion() || declaration->getFriendTypeNumTemplateParameterLists())
    return false;
  if (const auto *type = declaration->getFriendType())
    return !type->getType()->isDependentType();
  const auto *function = llvm::dyn_cast_or_null<clang::FunctionDecl>(declaration->getFriendDecl());
  return function && !function->isTemplated() && !function->getTrailingRequiresClause() &&
      (!function->isDefaulted() || IsInClassDefaultedComparison(function));
}

bool NeedsStandardCoroutineInclude(const clang::NamedDecl *declaration, const clang::ASTContext &context) {
  RuntimeDeclarationFinder finder(context);
  VisitPublishedRuntimeContracts(declaration, finder);
  return finder.HasCxxCoroutines || !finder.StandardCoroutineHeaders.empty();
}

std::vector<std::string> NeededStandardLibraryHeaders(const clang::NamedDecl *declaration,
                                                       const clang::ASTContext &context) {
  RuntimeDeclarationFinder finder(context);VisitPublishedRuntimeContracts(declaration,finder);
  return finder.HasStandardLibraryReferences ? finder.StandardHeaders.headers : std::vector<std::string>{};
}

struct ClassTemplatePublication {
  bool deduced=false, functions=false, aliases=false, variables=false;
  bool nested=false, thread_local_storage=false, variadic=false, nested_classes=false, effectful=false;
  void Read(const clang::CXXRecordDecl *record) {
    if (!record || !record->isCompleteDefinition()) return;
    for (const auto *member : record->decls()) {
      if (member->isImplicit()) continue;
      if (const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(member);
          specialization && specialization->getSpecializationKind()==clang::TSK_ImplicitInstantiation) continue;
      const auto *method=llvm::dyn_cast<clang::CXXMethodDecl>(member);
      const auto *variable=llvm::dyn_cast<clang::VarDecl>(member);
      if (const auto *pattern=llvm::dyn_cast<clang::FunctionTemplateDecl>(member)) {
        functions=true;method=llvm::dyn_cast<clang::CXXMethodDecl>(pattern->getTemplatedDecl());
      } else if (llvm::isa<clang::TypeAliasTemplateDecl>(member)) aliases=true;
      else if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(member)) {
        variables=nested=true;variable=pattern->getTemplatedDecl();
      } else if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(member)) {
        nested_classes=nested=true;Read(pattern->getTemplatedDecl()->getDefinition());
        for (const auto *specialization:PublishedClassSpecializations(pattern)) Read(specialization);
      } else if (const auto *inner=llvm::dyn_cast<clang::CXXRecordDecl>(member)) {
        nested=true;Read(inner->getDefinition());
      }
      if (method) {
        const auto effects=ReadCanonicalFunctionEffects(method->getType(),method->getASTContext());
        effectful |= (effects.is_async || effects.is_throwing) &&
            (llvm::isa<clang::FunctionTemplateDecl>(member) || record->isDependentContext() ||
             llvm::isa<clang::ClassTemplateSpecializationDecl>(record));
        deduced |= method->getReturnType()->getContainedAutoType()!=nullptr;
        variadic |= method->isVariadic();
      }
      if (variable) {
        deduced |= variable->getType()->getContainedAutoType()!=nullptr;
        thread_local_storage |= variable->getTLSKind()!=clang::VarDecl::TLS_None;
      }
    }
  }
};

std::string DeclarationTypeIdentity(const clang::NamedDecl *declaration,
                                    const clang::ASTContext &context) {
  if (const auto *record=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(declaration);
      record && IsPublishedStandardSpecialization(record)) {
    std::string arguments;
    if (!ClassSpecializationArgumentKey(record,context,arguments)) return {};
    std::string text;llvm::raw_string_ostream stream(text);
    clang::PrintingPolicy policy(context.getLangOpts());
    policy.PrintAsCanonical=true;policy.PolishForDeclaration=true;policy.SuppressScope=true;
    auto *printed=const_cast<clang::ClassTemplateSpecializationDecl *>(record);
    const auto qualifier=printed->getQualifierLoc();
    auto restore=llvm::scope_exit([&]{printed->setQualifierInfo(qualifier);});printed->setQualifierInfo({});
    printed->print(stream,policy);
    if(IsStandardCoroutineTraitsSpecialization(declaration))
      return "coroutine-traits{arguments="+arguments+";definition="+ComputeInterfaceSourceSha256(text)+
          "};cxx-coroutine-traits-specializations:v1;";
    const auto *owner=StandardLibrarySpecializationOwner(record);
    if(!owner)return {};
    return "standard-specialization{primary="+ReadInterfaceDeclarationStableId(record->getSpecializedTemplate())+
        ";owner="+ReadInterfaceDeclarationStableId(owner)+";arguments="+arguments+
        ";definition="+ComputeInterfaceSourceSha256(text)+"};cxx-standard-library-specializations:v1;";
  }
  if (const auto *concept_decl = llvm::dyn_cast<clang::ConceptDecl>(declaration))
    return ConceptIdentity(concept_decl, context);
  if (const auto *pattern = llvm::dyn_cast<clang::ClassTemplateDecl>(declaration)) {
    std::string body;
    llvm::raw_string_ostream stream(body);
    clang::PrintingPolicy policy(context.getLangOpts());
    policy.PrintAsCanonical = true;
    policy.SuppressScope = true;
    policy.AnonymousTagNameStyle=static_cast<unsigned>(clang::PrintingPolicy::AnonymousTagMode::Plain);
    policy.IncludeTagDefinition=HasDeclarationUnnamedLocalType(declaration);
    pattern->getTemplatedDecl()->print(stream, policy);
    const auto origins=FactoryDeclarationTypeKey(pattern);
    if(!origins.empty())body+=";factory-origins="+ComputeInterfaceSourceSha256(origins);
    ClassTemplatePublication members;
    members.Read(pattern->getTemplatedDecl());
    std::map<std::string,std::string> specializations;
    const auto published_specializations=PublishedClassSpecializations(pattern);
    for (const auto *specialization:published_specializations) {
      std::string key,text;
      if (!ClassSpecializationArgumentKey(specialization,context,key)) return {};
      llvm::raw_string_ostream specialization_stream(text);
      auto *printed=const_cast<clang::ClassTemplateSpecializationDecl *>(specialization);
      const auto qualifier=printed->getQualifierLoc();
      auto restore_qualifier=llvm::scope_exit([&]{printed->setQualifierInfo(qualifier);});
      printed->setQualifierInfo({});
      auto specialization_policy=policy;
      specialization_policy.IncludeTagDefinition=HasDeclarationUnnamedLocalType(specialization);
      printed->print(specialization_stream,specialization_policy);
      const auto origins=FactoryDeclarationTypeKey(specialization);
      if(!origins.empty())text+=";factory-origins="+ComputeInterfaceSourceSha256(origins);
      specializations.emplace(key,ComputeInterfaceSourceSha256(text));
      members.Read(specialization);
    }
    for (const auto &[key,digest]:specializations)
      body+=";specialization="+std::to_string(key.size())+":"+key+":"+digest;
    return "class-template{parameters=" + FunctionTemplateHeadContract(pattern, context) +
        ";definition=" + ComputeInterfaceSourceSha256(body) + "};cxx-class-templates:v1;" +
        ";cxx-template-structure:v1;rtti=" + std::to_string(context.getLangOpts().RTTI) + ";" +
        (specializations.empty() || pattern->getDeclContext()->isRecord() ? "" : ";cxx-class-specializations:v1;") +
        (!pattern->getDeclContext()->isRecord() && std::any_of(published_specializations.begin(),published_specializations.end(),
                     [](const auto *specialization){return llvm::isa<clang::ClassTemplatePartialSpecializationDecl>(specialization);})
             ? ";cxx-class-partial-specializations:v1;" : "") +
        (members.deduced ? ";cxx-template-deduction:v1;" : "") +
        (members.effectful ? ";cxx-effectful-template-members:v1;" : "") +
        (members.functions ? ";cxx-member-templates:v1;" : "") +
        (members.aliases ? ";cxx-member-alias-templates:v1;;cxx-alias-templates:v1;" : "") +
        (members.variables ? ";cxx-variable-templates:v1;" : "") +
        (members.nested ? ";cxx-nested-templates:v1;" : "") +
        (members.variadic ? ";cxx-variadic-templates:v1;" : "") +
        (members.thread_local_storage ? ";cxx-thread-local:v1;" : "");
  }
  if (const auto *pattern = llvm::dyn_cast<clang::VarTemplateDecl>(declaration)) {
    const auto *variable = pattern->getTemplatedDecl();
    DeclarationExpressionIdentity identity(context);
    identity.TemplateParameters(pattern);
    std::map<std::string,std::string> partials;
    bool deduced=variable->getType()->getContainedAutoType()!=nullptr;
    for (const auto *partial:PublishedVariablePartials(pattern)) {
      std::string text;llvm::raw_string_ostream stream(text);
      clang::PrintingPolicy policy(context.getLangOpts());policy.PrintAsCanonical=true;
      partial->print(stream,policy);
      partials.emplace(VariablePartialArgumentKey(partial,context),ComputeInterfaceSourceSha256(text));
      deduced |= partial->getType()->getContainedAutoType()!=nullptr;
    }
    std::string partial_contract;
    for (const auto &[key,digest]:partials) partial_contract+=key+":"+digest+";";
    return "variable-template{parameters=" + FunctionTemplateHeadContract(pattern, context) +
        ";type=" + CanonicalType(variable->getType(),context) +
        ";inline=" + std::to_string(variable->isInline()) +
        ";constexpr=" + std::to_string(variable->isConstexpr()) +
        ";thread-storage=" + std::to_string(variable->getTSCSpec()) +
        ";initializer=" + (variable->hasInit() ? identity.Get(const_cast<clang::Expr *>(variable->getInit())) :
            variable->isThisDeclarationADefinition()==clang::VarDecl::DeclarationOnly ? "declaration" : "default") +
        (partials.empty()?"":";partials="+ComputeInterfaceSourceSha256(partial_contract)) +
        "};cxx-variable-templates:v1;" +
        (partials.empty() || pattern->getDeclContext()->isRecord()?"":";cxx-variable-partial-specializations:v1;") +
        ";cxx-template-structure:v1;" +
        (deduced ? ";cxx-template-deduction:v1;" : "") +
        (HasDeclarationThreadLocal(pattern) ? ";cxx-thread-local:v1;" : "");
  }
  if (const auto *pattern = llvm::dyn_cast<clang::TypeAliasTemplateDecl>(declaration)) {
    std::string head;
    llvm::raw_string_ostream stream(head);
    clang::PrintingPolicy policy(context.getLangOpts());
    pattern->getTemplateParameters()->print(stream, context, policy);
    DeclarationExpressionIdentity identity(context);
    identity.TemplateParameters(pattern);
    const auto *constraint = pattern->getTemplateParameters()->getRequiresClause();
    return "alias-template{parameters=" + ComputeInterfaceSourceSha256(head) +
        ";constraint=" + (constraint ? identity.Get(const_cast<clang::Expr *>(constraint)) : "none") +
        ";type=" + CanonicalType(pattern->getTemplatedDecl()->getUnderlyingType(), context) +
        "};cxx-alias-templates:v1;";
  }
  if (const auto *pattern = llvm::dyn_cast<clang::FunctionTemplateDecl>(declaration)) {
    const auto *function = pattern->getTemplatedDecl();
    std::string defaults;
    DeclarationExpressionIdentity identity(context, function);
    for (const auto *parameter : function->parameters())
      if (const auto *expression = WrittenDefaultArgument(parameter))
        defaults += identity.Get(const_cast<clang::Expr *>(expression));
    const auto effects=ReadCanonicalFunctionEffects(function->getType(),context);
    return "function-template{parameters=" + FunctionTemplateHeadContract(pattern, context) +
        ";signature=" + DeclarationTypeIdentity(function, context) +
        (defaults.empty() ? "" : ";cxx-declaration-expressions:v1;template-defaults=" + defaults) +
        "};cxx-function-templates:v2;" +
        ((effects.is_async || effects.is_throwing) ? ";cxx-effectful-function-templates:v1;" : "") +
        (function->isVariadic() ? ";cxx-variadic-templates:v1;" : "") +
        (function->getReturnType()->getContainedAutoType() ? ";cxx-template-deduction:v1;" : "");
  }
  if (const auto *method = llvm::dyn_cast<clang::ObjCMethodDecl>(declaration))
    return ObjCMethodIdentity(method, context);
  if (const auto *property = llvm::dyn_cast<clang::ObjCPropertyDecl>(declaration))
    return CanonicalType(property->getType(), context) + ";objc-property-attributes=" +
           std::to_string(static_cast<unsigned>(property->getPropertyAttributes())) +
           ";objc-property-contract:v1;getter=" + property->getGetterName().getAsString() +
           ";setter=" + (property->isReadOnly() ? std::string() : property->getSetterName().getAsString()) +
           ";implementation=" + std::to_string(static_cast<unsigned>(property->getPropertyImplementation()));
  if (const auto *using_enum=llvm::dyn_cast<clang::UsingEnumDecl>(declaration))
    return "using-enum{target="+ReadInterfaceDeclarationStableId(using_enum->getEnumDecl())+
           ";access="+CxxAccessName(using_enum->getAccess())+"};cxx-using:v1;";
  if (const auto *using_decl=llvm::dyn_cast<clang::UsingDecl>(declaration)) {
    std::set<std::string> targets;
    bool functions=true;
    for (const auto *shadow:using_decl->shadows()) {
      const auto *target=shadow->getTargetDecl()->getUnderlyingDecl();
      functions &= llvm::isa<clang::FunctionDecl>(target);
      targets.insert(ReadInterfaceDeclarationStableId(target));
    }
    std::string value="using{typename="+std::to_string(using_decl->hasTypename())+
        ";access="+CxxAccessName(using_decl->getAccess())+"};cxx-using:v1;functions="+
        std::to_string(functions)+";";
    for(const auto &target:targets) value+="target="+target+";";
    return value;
  }
  if (const auto *directive=llvm::dyn_cast<clang::UsingDirectiveDecl>(declaration))
    return "using-namespace{target="+
        ReadInterfaceDeclarationStableId(directive->getNominatedNamespaceAsWritten())+
        ";namespace="+ReadInterfaceDeclarationStableId(directive->getNominatedNamespace())+
        "};cxx-using:v1;";
  if (const auto *alias = llvm::dyn_cast<clang::NamespaceAliasDecl>(declaration))
    return "namespace-alias{target=" +
           ReadInterfaceDeclarationStableId(alias->getAliasedNamespace()) +
           ";namespace=" + ReadInterfaceDeclarationStableId(alias->getNamespace()) +
           "};cxx-namespaces:v1;";
  if (const auto *space = llvm::dyn_cast<clang::NamespaceDecl>(declaration))
    return std::string("namespace{") + space->getQualifiedNameAsString() +
           ";inline=" + std::to_string(space->isInline()) + "}";
  if (const auto *record = llvm::dyn_cast<clang::RecordDecl>(declaration)) {
    std::string value=RecordLayoutIdentity(record, context);
    const auto *cxx=llvm::dyn_cast<clang::CXXRecordDecl>(record);
    if (HasDirectMemberTemplates(cxx)) {
      ClassTemplatePublication members;members.Read(cxx);
      value+=";cxx-concrete-member-templates:v1;";
      if (members.effectful) value+=";cxx-effectful-template-members:v1;";
      if (members.nested_classes)
        value+=";cxx-concrete-nested-templates:v1;;cxx-nested-templates:v1;";
      if (members.functions) value+=";cxx-member-templates:v1;";
      if (members.aliases) value+=";cxx-member-alias-templates:v1;;cxx-alias-templates:v1;";
      if (members.variables) value+=";cxx-variable-templates:v1;";
      if (members.deduced) value+=";cxx-template-deduction:v1;";
      if (members.variadic) value+=";cxx-variadic-templates:v1;";
      if (members.thread_local_storage) value+=";cxx-thread-local:v1;";
      std::set<std::string> patterns;
      for (const auto *member:cxx->decls())
        if (const auto *pattern=llvm::dyn_cast<clang::TemplateDecl>(member))
          patterns.insert(ReadInterfaceDeclarationStableId(pattern)+":access="+
              CxxAccessName(pattern->getAccess())+":"+DeclarationTypeIdentity(pattern,context));
      for (const auto &pattern:patterns) value+="member-template="+pattern+";";
    }
    return value;
  }
  if (const auto *enumeration = llvm::dyn_cast<clang::EnumDecl>(declaration))
    return EnumIdentity(enumeration, context);
  if (llvm::isa<clang::ObjCInterfaceDecl, clang::ObjCProtocolDecl,
                clang::ObjCCategoryDecl>(declaration))
    return ObjCContainerIdentity(declaration, context);
  if (const auto *constant = llvm::dyn_cast<clang::EnumConstantDecl>(declaration)) {
    llvm::SmallString<32> value;
    constant->getInitVal().toString(value, 10);
    return CanonicalType(constant->getType(), context) + "=" + value.str().str();
  }
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration)) {
    // Retained results have a dedicated ownership identity. Normalize this
    // top-level type bit while keeping nested callable contracts intact.
    clang::QualType type = function->getType();
    if (const auto *proto = type->getAs<clang::FunctionProtoType>()) {
      auto info = proto->getExtProtoInfo();
      info.ExtInfo = info.ExtInfo.withProducesResult(false);
      // Defaulted implicit exception specifications are derived from the bound
      // base/member declarations. Their evaluation is lazy and must not make
      // an interface identity depend on whether the provider used the member.
      if (function->isExplicitlyDefaulted() &&
          function->getExceptionSpecSourceRange().isInvalid())
        info.ExceptionSpec =
            clang::FunctionProtoType::ExceptionSpecInfo(clang::EST_None);
      type = context.getFunctionType(proto->getReturnType(), proto->getParamTypes(), info);
    } else if (const auto *no_proto = type->getAs<clang::FunctionNoProtoType>()) {
      type = context.getFunctionNoProtoType(no_proto->getReturnType(),
          no_proto->getExtInfo().withProducesResult(false));
    }
    std::string identity = CanonicalType(type, context) +
           (context.getLangOpts().CPlusPlus
                ? ";language-linkage=" + std::to_string(static_cast<unsigned>(
                                             function->getLanguageLinkage()))
                : "");
    unsigned index = 0;
    for (const auto *parameter : function->parameters()) {
      if (parameter->hasDefaultArg() && !parameter->hasUninstantiatedDefaultArg())
        identity += ";cxx-declaration-expressions:v1;default=" +
            std::to_string(index) + ":" +
            (function->getDescribedFunctionTemplate()
                ? DeclarationExpressionIdentity(context, function).Get(
                    const_cast<clang::Expr *>(parameter->getDefaultArg()))
                : DeclarationExpressionContract(parameter->getDefaultArg(), context)) + ";";
      ++index;
    }
    if (PublishesFunctionBody(function) && !context.getLangOpts().CPlusPlus)
      identity += ";c-inline-definitions:v1;linkage=iso-inline;body=" +
          FunctionDefinitionContract(function, context) + ";";
    if (PublishesFunctionBody(function) && context.getLangOpts().CPlusPlus)
      identity += ";cxx-definitions:v1;inline=1;constexpr=" +
          std::to_string(function->isConstexpr()) + ";consteval=" +
          std::to_string(function->isConsteval()) + ";body=" +
          FunctionDefinitionContract(function, context) + ";";
    return identity;
  }
  if (const auto *value = llvm::dyn_cast<clang::ValueDecl>(declaration)) {
    std::string identity = CanonicalType(value->getType(), context);
    if (const auto *ivar = llvm::dyn_cast<clang::ObjCIvarDecl>(value))
      identity += ivar->isBitField() ? ";objc-ivar-bitfield:v1;width=" + std::to_string(ivar->getBitWidthValue())
                                    : ";objc-ivar-ordinary:v1;";
    if (const auto *field = llvm::dyn_cast<clang::FieldDecl>(value);
        field && field->hasInClassInitializer())
      identity += ";cxx-declaration-expressions:v1;init=" +
          DeclarationExpressionContract(field->getInClassInitializer(), context) + ";";
    if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(value);
        variable && context.getLangOpts().CPlusPlus)
      identity +=
          ";language-linkage=" +
          std::to_string(static_cast<unsigned>(variable->getLanguageLinkage()));
    if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(value);
        variable && variable->isStaticDataMember())
      identity += ";cxx-definitions:v1;static-data=1;access=" +
          std::string(CxxAccessName(variable->getAccess())) + ";";
    if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(value);
        PublishesVariableInitializer(variable)) {
      identity += ";cxx-definitions:v1;inline=" + std::to_string(variable->isInline()) +
          ";constexpr=" + std::to_string(variable->isConstexpr()) + ";initializer=" +
          (variable->hasInit() ? DeclarationExpressionContract(variable->getInit(), context) :
           variable->isThisDeclarationADefinition()==clang::VarDecl::DeclarationOnly ? "declaration" : "none") + ";";
    }
    const auto lifetime = value->getType().getObjCLifetime();
    if (lifetime != clang::Qualifiers::OCL_None)
      identity += ";objc-lifetime=" + std::to_string(static_cast<unsigned>(lifetime));
    return identity;
  }
  if (const auto *type = llvm::dyn_cast<clang::TypeDecl>(declaration))
    return CanonicalType(context.getTypeDeclType(type), context);
  return {};
}

std::string ExportRootName(const clang::NamedDecl *declaration) {
  if (const auto *owner=PublishedStandardSpecializationOwner(declaration)) return ExportRootName(owner);
  if (const auto *owner = FriendOwner(declaration)) declaration = owner;
  while (declaration) {
    if (const auto *tag = llvm::dyn_cast<clang::TagDecl>(declaration);
        tag && !tag->getIdentifier())
      if (const auto *alias = tag->getTypedefNameForAnonDecl())
        declaration = alias;
    if (IsTopLevel(declaration))
      return InterfaceQualifiedName(declaration);
    declaration =
        llvm::dyn_cast<clang::NamedDecl>(declaration->getDeclContext());
  }
  return {};
}

// Read native attribute fields rather than source spellings or macro names.
// Length-delimited fields preserve messages, replacement text and platform
// contracts without ambiguity from punctuation or source attribute spelling.
std::string AvailabilityIdentity(const clang::NamedDecl *declaration) {
  std::vector<std::string> attributes;
  for (const auto *attribute : declaration->attrs()) {
    if (attribute->isImplicit())
      continue;
    std::string value;
    auto field = [&](llvm::StringRef text) {
      value += std::to_string(text.size()) + ":" + text.str();
    };
    if (const auto *deprecated =
            llvm::dyn_cast<clang::DeprecatedAttr>(attribute)) {
      field("deprecated");
      field(deprecated->getMessage());
      field(deprecated->getReplacement());
    } else if (const auto *unavailable =
                   llvm::dyn_cast<clang::UnavailableAttr>(attribute)) {
      field("unavailable");
      field(unavailable->getMessage());
    } else if (const auto *availability =
                   llvm::dyn_cast<clang::AvailabilityAttr>(attribute)) {
      field("availability");
      field(availability->getPlatform()->getName());
      field(availability->getIntroduced().normalize().getAsString());
      field(availability->getDeprecated().normalize().getAsString());
      field(availability->getObsoleted().normalize().getAsString());
      field(availability->getUnavailable() ? "true" : "false");
      field(availability->getMessage());
      field(availability->getStrict() ? "true" : "false");
      field(availability->getReplacement());
      field(std::to_string(availability->getPriority()));
      field(availability->getEnvironment()
                ? availability->getEnvironment()->getName()
                : "");
    } else {
      continue;
    }
    // ASTImporter can retain the same availability fact on several inherited
    // redeclarations. Repetition does not change the declaration contract.
    if (std::find(attributes.begin(), attributes.end(), value) == attributes.end())
      attributes.push_back(std::move(value));
  }
  std::string result;
  for (const auto &value : attributes)
    result += std::to_string(value.size()) + ":" + value;
  return result;
}

bool SelectedExport(const clang::NamedDecl *declaration,
                    const std::vector<std::string> &exports) {
  if (exports.empty() ||
      std::find(exports.begin(), exports.end(), "*") != exports.end())
    return true;
  if(const auto *enumeration=NativeUnnamedEnumeration(declaration))
    if(IsTopLevel(enumeration))
      for(const auto *constant:enumeration->enumerators()) {
        const auto member=InterfaceQualifiedName(constant);
        if(std::any_of(exports.begin(),exports.end(),[&](const auto &name) {
          return member==name || member.starts_with(name+"::");
        }))return true;
      }
  const auto root = ExportRootName(declaration);
  return std::any_of(exports.begin(), exports.end(), [&](const auto &name) {
    // Selecting a namespace admits its public descendants. Selecting a leaf
    // retains the namespace declarations needed to look that leaf up.
    return root == name || root.starts_with(name + "::") ||
           (llvm::isa<clang::NamespaceDecl>(declaration) &&
            name.starts_with(root + "::"));
  });
}

bool ValidateExportNames(const std::vector<std::string> &exports,
                         InterfaceEmissionArtifact &result) {
  std::set<std::string> seen;
  if (exports.size() > kMaxInterfaceExportNames) {
    AddDiagnostic(result, "O3IFC155",
                  "export policy exceeds 4096 declaration names");
    return false;
  }
  for (const auto &name : exports)
    if (name.empty() || name.size() > 256 ||
        std::any_of(name.begin(), name.end(),
                    [](unsigned char c) { return c < 33 || c == 127; }) ||
        !seen.insert(name).second || (name == "*" && exports.size() != 1)) {
      AddDiagnostic(
          result, "O3IFC155",
          "exports require distinct declaration names or a single wildcard");
      return false;
    }
  return true;
}

}  // namespace emission_detail

using namespace emission_detail;

std::string ReadInterfaceLanguageProfile(const clang::LangOptions &lang,
                                         const clang::TargetInfo &target) {
  static constexpr const char *profiles[] = {
      "compatibility", "strict", "strict-concurrency", "strict-system"};
  const auto index = static_cast<unsigned>(lang.ObjC3Profile);
  const auto &standard =
      clang::LangStandard::getLangStandardForKind(lang.LangStd);
  std::string profile = std::string("objc3=") + (lang.ObjC3 ? "enabled" : "disabled") +
         ";profile=" +
         (index < std::size(profiles) ? profiles[index] : "unknown") +
         ";strict-concurrency=" +
         (lang.ObjC3StrictConcurrency ? "true" : "false") +
         ";arc=" + (lang.ObjCAutoRefCount ? "true" : "false") + ";language=" +
         (lang.ObjC ? (lang.CPlusPlus ? "objective-c++" : "objective-c")
                    : (lang.CPlusPlus ? "c++" : "c")) +
         ";standard=" + standard.getName() +
         ";blocks=" + (lang.Blocks ? "true" : "false");
  if(lang.CPlusPlus)
    profile += ";native-static-lifetime:v1;destructors=" +
        std::to_string(static_cast<unsigned>(lang.getRegisterStaticDestructors())) +
        ";threadsafe-statics=" + std::to_string(lang.ThreadsafeStatics);
  if (lang.CPlusPlus20)
    profile += std::string(";coroutine-aligned-allocation=") + (lang.CoroAlignedAllocation ? "true" : "false");
  profile += ";primitive-target-abi:v1;layout=" + std::string(target.getDataLayoutString()) +
      ";abi=" + target.getABI().str() +
      ";cxx-abi=" + std::to_string(static_cast<unsigned>(target.getCXXABI().getKind())) +
      ";short-enums=" + std::to_string(lang.ShortEnums) +
      ";char-signed=" + std::to_string(lang.CharIsSigned) +
      ";wchar-size=" + std::to_string(lang.WCharSize) +
      ";wchar-signed=" + std::to_string(lang.WCharIsSigned) +
      ";pack-struct=" + std::to_string(lang.PackStruct) +
      ";max-type-align=" + std::to_string(lang.MaxTypeAlign) +
      ";align-double=" + std::to_string(lang.AlignDouble) +
      ";long-double-size=" + std::to_string(lang.LongDoubleSize) +
      ";long-double-format=" + std::to_string(static_cast<unsigned>(
          llvm::APFloat::SemanticsToEnum(target.getLongDoubleFormat()))) +
      ";clang-abi-compat=" + std::to_string(static_cast<unsigned>(lang.getClangABICompat()));
  profile += ";target-cpu=" + target.getTargetOpts().CPU;
  std::string features;
  for (const auto &feature : target.getTargetOpts().Features)
    AppendNativeContractField(features, feature);
  profile += ";target-features-sha256=" + ComputeInterfaceSourceSha256(features);
  for (const auto [width, alignment] : {
       std::pair{target.getCharWidth(), target.getCharAlign()},
       std::pair{target.getWCharWidth(), target.getWCharAlign()},
       std::pair{target.getShortWidth(), target.getShortAlign()},
       std::pair{target.getIntWidth(), target.getIntAlign()},
       std::pair{target.getLongWidth(), target.getLongAlign()},
       std::pair{target.getLongLongWidth(), target.getLongLongAlign()},
       std::pair{target.getHalfWidth(), target.getHalfAlign()},
       std::pair{target.getFloatWidth(), target.getFloatAlign()},
       std::pair{target.getDoubleWidth(), target.getDoubleAlign()},
       std::pair{target.getLongDoubleWidth(), target.getLongDoubleAlign()}})
    profile += ";primitive=" + std::to_string(width) + ":" + std::to_string(alignment);
  return profile;

}

std::string ReadInterfaceLanguageProfile(const clang::ASTContext &context) {
  return ReadInterfaceLanguageProfile(context.getLangOpts(), context.getTargetInfo());
}

// An unnamed record's methods inherit its stable owner/path even when their
// own signatures contain only scalar types. Native USRs for those methods
// otherwise retain the anonymous parent's physical source coordinates.
static std::string ProjectedCallableKey(const clang::FunctionDecl *function) {
  const auto &context=function->getASTContext();
  const auto *method=llvm::dyn_cast<clang::CXXMethodDecl>(function);
  auto parent=method?UnnamedTemplateRecordKey(method->getParent(),context):std::string();
  if(parent.empty() && method) {
    const auto type=context.getCanonicalTagType(method->getParent());
    // A named specialization can contain an unnamed argument. Its displayed
    // qualified member name retains that argument's physical header path.
    // Bind the exact canonical owner and argument origins instead, including
    // methods whose own parameter/result types do not mention their owner.
    if(!UnnamedTemplateTypeKey(type,context).empty())
      parent="named-record:"+CanonicalType(type,context);
  }
  if (parent.empty() && UnnamedTemplateTypeKey(function->getType(),context).empty()) return {};
  std::string name=function->getQualifiedNameAsString();
  if (!parent.empty()) {
    name=parent+":"+function->clang::Decl::getDeclKindName();
    if (!llvm::isa<clang::CXXConstructorDecl,clang::CXXDestructorDecl,clang::CXXConversionDecl>(function))
      name+=":"+function->getNameAsString();
  }
  return name+"@"+ComputeInterfaceSourceSha256(CanonicalType(function->getType(),context));
}

std::string ReadInterfaceDeclarationStableId(
    const clang::NamedDecl *declaration) {
  if (!declaration) return {};
  // Native primaries can live in inline implementation namespaces. Printed
  // qualification depends on the lookup context (notably libc++ ranges), so
  // use their canonical native identity rather than a display-name key.
  if(llvm::isa<clang::TemplateDecl>(declaration) &&
      IsStandardLibraryDeclaration(declaration,declaration->getASTContext())) {
    llvm::SmallString<128> native;
    if(clang::index::generateUSRForDecl(declaration->getCanonicalDecl(),native))return {};
    return native.str().str();
  }
  if (const auto *alias = llvm::dyn_cast<clang::TypeAliasDecl>(declaration))
    if (const auto *pattern = alias->getDescribedAliasTemplate())
      return ReadInterfaceDeclarationStableId(pattern);
  if (const auto *pattern = llvm::dyn_cast<clang::ClassTemplateDecl>(declaration)) {
    auto origin=UnnamedTemplateRecordKey(pattern->getTemplatedDecl(),pattern->getASTContext());
    if (origin.empty())
      if (const auto parent=UnnamedTemplateRecordKey(llvm::dyn_cast<clang::CXXRecordDecl>(pattern->getDeclContext()),pattern->getASTContext());!parent.empty())
        origin=parent+";named-template:"+pattern->getNameAsString();
    if (!origin.empty()) return "c:@RecordProjectedClassTemplate@"+ComputeInterfaceSourceSha256(origin);
    return "c:@ClassTemplate@" + pattern->getQualifiedNameAsString();
  }
  if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration))
    if (const auto *pattern = record->getDescribedClassTemplate())
      return ReadInterfaceDeclarationStableId(pattern);
  if (const auto *pattern = llvm::dyn_cast<clang::TypeAliasTemplateDecl>(declaration)) {
    if (const auto origin=UnnamedTemplateRecordKey(llvm::dyn_cast<clang::CXXRecordDecl>(pattern->getDeclContext()),pattern->getASTContext());!origin.empty())
      return "c:@RecordProjectedAliasTemplate@"+ComputeInterfaceSourceSha256(origin)+"@"+pattern->getNameAsString();
    return "c:@AliasTemplate@" + pattern->getQualifiedNameAsString();
  }
  if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration))
    if (const auto *pattern=variable->getDescribedVarTemplate()) return ReadInterfaceDeclarationStableId(pattern);
  if (const auto *specialization=llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(declaration);
      IsExplicitNamespaceVariableSpecialization(specialization)) {
    std::string key;
    for (const auto &argument:specialization->getTemplateArgs().asArray())
      if (!GenericArgumentKey(argument,specialization->getASTContext(),key)) return {};
    return "c:@VariableTemplateSpecialization@"+
        ReadInterfaceDeclarationStableId(specialization->getSpecializedTemplate())+"@arguments@"+
        ComputeInterfaceSourceSha256(key);
  }
  if (const auto *specialization=llvm::dyn_cast<clang::VarTemplatePartialSpecializationDecl>(declaration);
      specialization && IsStandardLibrarySpecialization(specialization)) {
    const auto key=VariablePartialArgumentKey(specialization,specialization->getASTContext());
    return "c:@StandardVariablePartial@"+ReadInterfaceDeclarationStableId(specialization->getSpecializedTemplate())+
        "@arguments@"+ComputeInterfaceSourceSha256(key);
  }
  if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(declaration))
    return "c:@VariableTemplate@" + pattern->getQualifiedNameAsString();
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration))
    if (const auto *pattern = function->getDescribedFunctionTemplate())
      return ReadInterfaceDeclarationStableId(pattern);
  if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(declaration);
      IsExplicitNamespaceFunctionSpecialization(function)) {
    const auto *arguments=function->getTemplateSpecializationArgs();
    if (!arguments) return {};
    std::string key;
    for (const auto &argument:arguments->asArray())
      if (!GenericArgumentKey(argument,function->getASTContext(),key)) return {};
    return "c:@FunctionTemplateSpecialization@"+
        ReadInterfaceDeclarationStableId(function->getPrimaryTemplate())+"@arguments@"+
        ComputeInterfaceSourceSha256(key);
  }
  if (const auto *pattern = llvm::dyn_cast<clang::FunctionTemplateDecl>(declaration)) {
    const auto *function=pattern->getTemplatedDecl();
    if (const auto projected=ProjectedCallableKey(function);!projected.empty())
      return "c:@RecordProjectedFunctionTemplate@"+projected+
          "@constraints@"+FunctionTemplateConstraintKey(pattern);
    llvm::SmallString<128> usr;
    if (clang::index::generateUSRForDecl(pattern, usr)) return {};
    return usr.str().str() + "@constraints@" + FunctionTemplateConstraintKey(pattern);
  }
  if(const auto *using_enum=llvm::dyn_cast<clang::UsingEnumDecl>(declaration)) {
    const auto *owner=llvm::dyn_cast<clang::NamedDecl>(using_enum->getDeclContext());
    return "c:@UsingEnum@"+(owner?ReadInterfaceDeclarationStableId(owner):std::string("TU"))+
           "@"+ReadInterfaceDeclarationStableId(using_enum->getEnumDecl());
  }
  if(const auto *directive=llvm::dyn_cast<clang::UsingDirectiveDecl>(declaration)) {
    const auto *owner=llvm::dyn_cast<clang::NamedDecl>(directive->getDeclContext());
    return "c:@UDirective@"+(owner?ReadInterfaceDeclarationStableId(owner):std::string("TU"))+
           "@"+ReadInterfaceDeclarationStableId(directive->getNominatedNamespaceAsWritten());
  }
  if (const auto *tag=llvm::dyn_cast<clang::TagDecl>(declaration))
    if (const auto origin=UnnamedTemplateTagKey(tag,tag->getASTContext());!origin.empty())
      return "c:@RecordProjectedTag@"+std::to_string(tag->getKind())+"@"+ComputeInterfaceSourceSha256(origin);
  if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(declaration))
    if (const auto projected=ProjectedCallableKey(function);!projected.empty())
      return "c:@RecordProjectedFunction@"+projected;
  llvm::SmallString<128> usr;
  if (clang::index::generateUSRForDecl(declaration, usr)) return {};
  if(!usr.str().starts_with("c:@")) {
    if(IsPublishedUnnamedEnumEntity(declaration)) {
      const auto suffix=usr.str().find('@');
      if(suffix!=llvm::StringRef::npos)return "c:"+usr.str().substr(suffix).str();
    }
    // Clang gives a public template instantiation filename-scoped USRs when
    // an argument has an unnamed enum type. Its checked primary owner and
    // converted argument graph already supply stable module-owned identity.
    // Keep the complete native scope/member suffix, including overload types.
    for(const clang::DeclContext *scope=llvm::isa<clang::ClassTemplateSpecializationDecl>(declaration)
          ? llvm::cast<clang::ClassTemplateSpecializationDecl>(declaration):declaration->getDeclContext();scope;scope=scope->getParent())
      if(const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(scope);
          specialization && IsPublishedType(specialization->getSpecializedTemplate())) {
        std::string arguments;
        if(ClassSpecializationArgumentKey(specialization,declaration->getASTContext(),arguments) &&
            arguments.find(";native-unnamed-enums:v1;")!=std::string::npos) {
          const auto suffix=usr.str().find('@');
          if(suffix!=llvm::StringRef::npos)return "c:"+usr.str().substr(suffix).str();
        }
      }
  }
  // Native USRs encode filenames for C typedefs because those names have no
  // linkage. Module provenance provides the provider identity; physical source
  // and canonical artifact filenames must not change the exported type name.
  if (llvm::isa<clang::TypedefNameDecl>(declaration) &&
      declaration->getDeclContext()->getRedeclContext()->isTranslationUnit())
    return "c:@T@" + declaration->getNameAsString();
  if (llvm::isa<clang::TypedefDecl>(declaration))
    if (const auto *space=llvm::dyn_cast<clang::NamespaceDecl>(
            declaration->getDeclContext()->getRedeclContext()))
      return ReadInterfaceDeclarationStableId(space)+"@T@"+declaration->getNameAsString();
  if(const auto *using_decl=llvm::dyn_cast<clang::UsingDecl>(declaration)) {
    std::set<std::string> targets;
    for(const auto *shadow:using_decl->shadows())
      targets.insert(ReadInterfaceDeclarationStableId(shadow->getTargetDecl()->getUnderlyingDecl()));
    std::string key;
    for(const auto &target:targets) key+=std::to_string(target.size())+":"+target;
    return usr.str().str()+"@targets@"+ComputeInterfaceSourceSha256(key);
  }
  return usr.str().str();
}

bool ReadInterfaceDeclarationIdentity(
    clang::NamedDecl *declaration, const clang::ASTContext &context,
    InterfaceDeclarationIdentity &identity) {
  identity = {};
  if (!declaration) return false;
  const auto stable_id = ReadInterfaceDeclarationStableId(declaration);
  if (stable_id.empty()) return false;
  auto *representative = Representative(declaration);
  identity.stable_id = stable_id;
  identity.spi_group = ReadInterfaceSPIGroup(representative);
  identity.native_symbol_name = ReadNativeSymbolName(representative);
  identity.kind = declaration->getDeclKindName();
  identity.export_root = ExportRootName(declaration);
  identity.qualified_name = InterfaceQualifiedName(declaration);
  if (identity.qualified_name.empty())
    identity.qualified_name = declaration->getNameAsString();
  identity.canonical_type_identity =
      DeclarationTypeIdentity(representative, context);
  if(NativeUnnamedEnumeration(representative))
    identity.canonical_type_identity+=";native-unnamed-enums:v1;";
  if (identity.canonical_type_identity.empty()) return false;
  if (HasEffectfulTemplateProjection(representative,context))
    identity.canonical_type_identity+=";cxx-template-callable-projections:v1;";
  if (!llvm::isa<clang::NamespaceDecl>(representative)) {
    RuntimeDeclarationFinder arguments(context);VisitPublishedRuntimeContracts(representative,arguments);
    if (arguments.HasCxxCoroutines)
      identity.canonical_type_identity+=";cxx-coroutine-bodies:v1;";
    if(arguments.HasStandardLibraryReferences) {
      if(!arguments.StandardHeaders.valid)return false;
      std::string contents;for(const auto &entry:arguments.StandardHeaders.contents)
        contents+=std::to_string(entry.size())+":"+entry;
      const auto digest=ComputeInterfaceSourceSha256(contents);
      identity.canonical_type_identity+=std::string(kCxxStandardLibraryMarker)+"headers[";
      bool first=true;for(const auto &header:arguments.StandardHeaders.headers) {
        if(!first)identity.canonical_type_identity+=",";first=false;
        identity.canonical_type_identity+=header+"="+digest;
      }
      identity.canonical_type_identity+="];";
    }
    if (!arguments.StandardCoroutineHeaders.empty()) {
      std::string headers;for (const auto &header:arguments.StandardCoroutineHeaders) headers+=header;
      identity.canonical_type_identity+=";cxx-standard-coroutines:v1;headers="+ComputeInterfaceSourceSha256(headers)+";";
    }
    if (arguments.FactoryTypes.nested)
      identity.canonical_type_identity+=";cxx-nested-factory-types:v1;";
    if (arguments.FactoryTypes.lambda_result)
      identity.canonical_type_identity+=";cxx-lambda-factory-results:v1;";
    if (arguments.FactoryTypes.closure)
      identity.canonical_type_identity+=";cxx-closure-result-projections:v1;";
    if (arguments.HasFactoryValueArguments)
      identity.canonical_type_identity+=";cxx-factory-value-arguments:v1;";
    if (arguments.FactoryTypes.generic)
      identity.canonical_type_identity+=";cxx-generic-factory-projections:v1;";
    if (arguments.HasAddressArguments)
      identity.canonical_type_identity+=";cxx-address-template-arguments:v1;";
    if (arguments.HasFactoryArguments ||
        (llvm::isa<clang::TemplateDecl>(representative) && HasDeclarationFactoryTypeProjection(representative)))
      identity.canonical_type_identity+=";cxx-factory-template-arguments:v1;";
  }
  if (llvm::isa<clang::TypeAliasTemplateDecl>(representative) &&
      !identity.native_symbol_name.empty())
    identity.canonical_type_identity+=";cxx-nominal-alias-templates:v1;owner="+
        identity.native_symbol_name+";";
  if (const auto *specialization=llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(representative);
      specialization && IsStandardLibrarySpecialization(specialization)) {
    const auto *owner=StandardLibrarySpecializationOwner(specialization);if(!owner)return false;
    std::string key;
    if(const auto *partial=llvm::dyn_cast<clang::VarTemplatePartialSpecializationDecl>(specialization))
      key=VariablePartialArgumentKey(partial,context);
    else for(const auto &argument:specialization->getTemplateArgs().asArray())
      if(!GenericArgumentKey(argument,context,key))return false;
    std::string text;llvm::raw_string_ostream stream(text);
    clang::PrintingPolicy policy(context.getLangOpts());policy.PrintAsCanonical=true;policy.PolishForDeclaration=true;
    specialization->print(stream,policy);
    identity.canonical_type_identity+=";standard-variable-specialization{primary="+
        ReadInterfaceDeclarationStableId(specialization->getSpecializedTemplate())+
        ";owner="+ReadInterfaceDeclarationStableId(owner)+";arguments="+key+
        ";definition="+ComputeInterfaceSourceSha256(text)+"};cxx-standard-library-specializations:v1;";
  }
  if (const auto *specialization=llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(representative);
      IsExplicitNamespaceVariableSpecialization(specialization)) {
    std::string key;
    for (const auto &argument:specialization->getTemplateArgs().asArray())
      if (!GenericArgumentKey(argument,context,key)) return false;
    if (!specialization->isInline() || !specialization->hasInit())
      identity.canonical_type_identity+=";cxx-variable-specialization-storage:v1;delivery="+
          std::string(specialization->isInline()?"inline-zero":"external")+";";
    identity.canonical_type_identity+=";cxx-variable-specializations:v1;;cxx-variable-templates:v1;primary="+
        ReadInterfaceDeclarationStableId(specialization->getSpecializedTemplate())+";arguments="+
        ComputeInterfaceSourceSha256(key)+";";
  }
  if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(representative);
      IsExplicitNamespaceFunctionSpecialization(function)) {
    std::string key;
    for (const auto &argument:function->getTemplateSpecializationArgs()->asArray())
      if (!GenericArgumentKey(argument,context,key)) return false;
    identity.canonical_type_identity+=";cxx-function-specializations:v1;;cxx-function-templates:v2;primary="+
        ReadInterfaceDeclarationStableId(function->getPrimaryTemplate())+";arguments="+
        ComputeInterfaceSourceSha256(key)+";";
  }
  const auto closed_patterns=ClosedParentPatterns(representative);
  if (!closed_patterns.empty()) {
    std::set<std::string> definitions;
    for (const auto *pattern:closed_patterns) {
      std::string parent_arguments,text;
      const auto *parent=llvm::cast<clang::ClassTemplateSpecializationDecl>(pattern->getDeclContext());
      if (!ClassSpecializationArgumentKey(parent,context,parent_arguments)) return false;
      llvm::raw_string_ostream stream(text);
      PrintRootDeclaration(const_cast<clang::NamedDecl *>(pattern),const_cast<clang::ASTContext &>(context),stream);
      definitions.insert(ReadInterfaceDeclarationStableId(pattern)+":parent="+parent_arguments+
          ":definition="+ComputeInterfaceSourceSha256(text));
    }
    std::string contract;
    for (const auto &definition:definitions) contract+=std::to_string(definition.size())+":"+definition;
    identity.canonical_type_identity+=";cxx-closed-parent-patterns:v1;definitions="+
        ComputeInterfaceSourceSha256(contract)+";";
  }
  const auto closed_members=ClosedParentMemberSpecializations(representative);
  if (!closed_members.empty()) {
    std::set<std::string> definitions;
    for (const auto *member:closed_members) {
      std::string parent_arguments,text;
      const auto *parent=llvm::cast<clang::ClassTemplateSpecializationDecl>(member->getDeclContext());
      if (!ClassSpecializationArgumentKey(parent,context,parent_arguments)) return false;
      llvm::raw_string_ostream stream(text);
      PrintRootDeclaration(const_cast<clang::NamedDecl *>(member),const_cast<clang::ASTContext &>(context),stream);
      std::string body;
      if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(member); function && function->isInlined())
        body=FunctionDefinitionContract(function,context);
      definitions.insert(ReadInterfaceDeclarationStableId(member)+":parent="+parent_arguments+
          ":declaration="+ComputeInterfaceSourceSha256(text)+":body="+body);
    }
    std::string contract;
    for (const auto &definition:definitions) contract+=std::to_string(definition.size())+":"+definition;
    identity.canonical_type_identity+=";cxx-closed-parent-members:v1;definitions="+
        ComputeInterfaceSourceSha256(contract)+";";
  }
  const auto dependent_partials=DependentMemberPartials(representative);
  if (!dependent_partials.empty()) {
    std::set<std::string> definitions;
    for (const auto *partial:dependent_partials) {
      std::string text;llvm::raw_string_ostream stream(text);
      clang::PrintingPolicy policy(context.getLangOpts());
      policy.PrintAsCanonical=true;policy.PolishForDeclaration=true;
      partial->print(stream,policy);
      definitions.insert(ReadInterfaceDeclarationStableId(partial)+":"+ComputeInterfaceSourceSha256(text));
    }
    std::string contract;
    for (const auto &definition:definitions) contract+=std::to_string(definition.size())+":"+definition;
    identity.canonical_type_identity+=";cxx-dependent-member-partials:v1;definitions="+
        ComputeInterfaceSourceSha256(contract)+";";
  }
  const auto member_variables=ConcreteMemberVariableSpecializations(representative);
  if (!member_variables.empty()) {
    std::set<std::string> specializations;
    for (const auto *variable:member_variables) {
      std::string arguments;
      const auto *partial=llvm::dyn_cast<clang::VarTemplatePartialSpecializationDecl>(variable);
      if (partial) arguments=VariablePartialArgumentKey(partial,context);
      else for (const auto &argument:variable->getTemplateArgs().asArray())
        if (!GenericArgumentKey(argument,context,arguments)) return false;
      DeclarationExpressionIdentity initializer(context);
      if (partial) for (const auto *parameter:*partial->getTemplateParameters()) initializer.Local(parameter);
      specializations.insert(ReadInterfaceDeclarationStableId(variable->getSpecializedTemplate())+
          ":arguments="+ComputeInterfaceSourceSha256(arguments)+":type="+CanonicalType(variable->getType(),context)+
          ":symbol="+(partial?ReadNativeSymbolName(variable->getSpecializedTemplate()):ReadNativeSymbolName(variable))+
          ":inline="+std::to_string(variable->isInline())+":constexpr="+std::to_string(variable->isConstexpr())+
          ":tls="+std::to_string(variable->getTSCSpec())+":delivery="+
          ((partial || PublishesVariableInitializer(variable))?"consumer":"provider")+":init="+
          ((partial || PublishesVariableInitializer(variable)) && variable->hasInit()
              ?initializer.Get(const_cast<clang::Expr *>(variable->getInit())):"none"));
    }
    std::string contract;
    for (const auto &specialization:specializations) contract+=std::to_string(specialization.size())+":"+specialization;
    identity.canonical_type_identity+=";cxx-concrete-member-variable-specializations:v1;definitions="+
        ComputeInterfaceSourceSha256(contract)+";";
  }
  const auto member_functions=ConcreteMemberFunctionSpecializations(representative);
  if (!member_functions.empty()) {
    std::set<std::string> specializations;
    for (const auto *function:member_functions) {
      std::string arguments;
      for (const auto &argument:function->getTemplateSpecializationArgs()->asArray())
        if (!GenericArgumentKey(argument,context,arguments)) return false;
      std::string method_properties=":method-kind="+std::string(function->clang::Decl::getDeclKindName());
      if (const auto *constructor=llvm::dyn_cast<clang::CXXConstructorDecl>(function))
        method_properties+=":explicit="+std::to_string(constructor->getExplicitSpecifier().isExplicit());
      if (const auto *conversion=llvm::dyn_cast<clang::CXXConversionDecl>(function))
        method_properties+=":explicit="+std::to_string(conversion->getExplicitSpecifier().isExplicit());
      specializations.insert(ReadInterfaceDeclarationStableId(function->getPrimaryTemplate())+method_properties+
          ":arguments="+ComputeInterfaceSourceSha256(arguments)+":type="+CanonicalType(function->getType(),context)+
          ":symbol="+ReadNativeSymbolName(function)+":inline="+std::to_string(function->isInlined())+
          ":constexpr="+std::to_string(function->isConstexpr())+":consteval="+std::to_string(function->isConsteval())+
          ":deleted="+std::to_string(function->isDeleted())+":body="+
          (function->isInlined()?FunctionDefinitionContract(function,context):"provider"));
    }
    std::string contract;
    for (const auto &specialization:specializations) contract+=std::to_string(specialization.size())+":"+specialization;
    identity.canonical_type_identity+=";cxx-concrete-member-function-specializations:v1;definitions="+
        ComputeInterfaceSourceSha256(contract)+";";
    if (std::any_of(member_functions.begin(),member_functions.end(),IsConcreteSpecialMemberSpecialization))
      identity.canonical_type_identity+=";cxx-concrete-special-member-specializations:v1;";
  }
  if (!ConcreteMemberClassSpecializations(representative).empty())
    identity.canonical_type_identity+=";cxx-concrete-member-class-specializations:v1;";
  const auto detached_records=DetachedNestedTemplateDefinitions(representative);
  if (!detached_records.empty()) {
    std::set<std::string> definitions;
    for (const auto *definition:detached_records) {
      std::string text;llvm::raw_string_ostream stream(text);
      clang::PrintingPolicy policy(context.getLangOpts());
      policy.PrintAsCanonical=true;policy.FullyQualifiedName=false;policy.PolishForDeclaration=true;
      definition->print(stream,policy);
      definitions.insert(ReadInterfaceDeclarationStableId(definition)+":"+ComputeInterfaceSourceSha256(text));
    }
    std::string contract;
    for (const auto &definition:definitions) contract+=std::to_string(definition.size())+":"+definition;
    identity.canonical_type_identity+=";cxx-detached-nested-templates:v1;definitions="+
        ComputeInterfaceSourceSha256(contract)+";";
  }
  const auto detached_data=DetachedStaticDataTemplateDefinitions(representative);
  if (!detached_data.empty()) {
    std::set<std::string> definitions;
    for (const auto *definition:detached_data) {
      const auto *variable=llvm::dyn_cast<clang::VarDecl>(definition);
      if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(definition)) variable=pattern->getTemplatedDecl();
      if (!variable) return false;
      DeclarationExpressionIdentity initializer(context);
      initializer.OuterTemplateParameters(variable);
      if (const auto *pattern=variable->getDescribedVarTemplate()) initializer.TemplateParameters(pattern);
      definitions.insert(ReadInterfaceDeclarationStableId(definition)+":"+CanonicalType(variable->getType(),context)+
          ":inline="+std::to_string(variable->isInline())+":constexpr="+std::to_string(variable->isConstexpr())+
          ":tls="+std::to_string(variable->getTSCSpec())+":init="+
          (variable->hasInit()?initializer.Get(const_cast<clang::Expr *>(variable->getInit())):"zero"));
    }
    std::string contract;
    for (const auto &definition:definitions) contract+=std::to_string(definition.size())+":"+definition;
    identity.canonical_type_identity+=";cxx-detached-static-data-templates:v1;definitions="+
        ComputeInterfaceSourceSha256(contract)+";";
  }
  const auto detached=DetachedCallableTemplateDefinitions(representative);
  if (!detached.empty()) {
    std::set<std::string> definitions;
    for (const auto *definition:detached) {
      const auto *function=llvm::dyn_cast<clang::FunctionDecl>(definition);
      if (const auto *pattern=llvm::dyn_cast<clang::FunctionTemplateDecl>(definition))
        function=pattern->getTemplatedDecl();
      if (!function) return false;
      definitions.insert(ReadInterfaceDeclarationStableId(definition)+":"+
          CanonicalType(function->getType(),context)+":inline="+std::to_string(function->isInlined())+
          ":constexpr="+std::to_string(function->isConstexpr())+":consteval="+std::to_string(function->isConsteval())+
          ":body="+FunctionDefinitionContract(function,context));
    }
    std::string contract;
    for (const auto &definition:definitions) contract+=std::to_string(definition.size())+":"+definition;
    identity.canonical_type_identity+=";cxx-detached-callable-templates:v1;definitions="+
        ComputeInterfaceSourceSha256(contract)+";";
  }
  if (const auto origins=UnnamedTemplateDeclarationTypeKey(representative);!origins.empty())
    identity.canonical_type_identity+=";cxx-unnamed-record-projections:v2;origins="+ComputeInterfaceSourceSha256(origins)+";";
  if (HasDeclarationFactoryTypeProjection(representative))
    identity.canonical_type_identity+=";cxx-factory-type-projections:v1;";
  if (HasDeclarationLocalFactoryResult(representative))
    identity.canonical_type_identity+=";cxx-local-factory-results:v1;";
  if (HasDeclarationClosureFactory(representative))
    identity.canonical_type_identity+=";cxx-closure-factories:v1;";
  if (HasDeclarationEffectfulLambda(representative))
    identity.canonical_type_identity+=";cxx-effectful-lambdas:v1;";
  if (HasDeclarationTemplateTemplateParameters(representative))
    identity.canonical_type_identity+=";cxx-template-template-parameters:v1;";
  if (HasDeclarationNativeDependentAwait(representative))
    identity.canonical_type_identity+=";native-dependent-await:v1;";
  if (HasDeclarationNSObjectStorage(representative))
    identity.canonical_type_identity+=";nsobject-storage:v1;";
  if (HasDeclarationAutomaticOwnerUnwind(representative))
    identity.canonical_type_identity += ";automatic-owner-unwind:v2;";
  if (HasDeclarationNativeArrayLifetime(representative))
    identity.canonical_type_identity += ";native-array-lifetimes:v1;";
  if (llvm::isa<clang::ClassTemplateDecl,clang::VarTemplateDecl,clang::CXXRecordDecl,
                clang::VarTemplateSpecializationDecl>(representative)) {
    struct IncompleteTemplates:clang::DynamicRecursiveASTVisitor {
      bool found=false;
      bool TraverseStmt(clang::Stmt *) override {return true;}
      bool TraverseDecl(clang::Decl *declaration) override {
        return !declaration || declaration->isImplicit() || clang::DynamicRecursiveASTVisitor::TraverseDecl(declaration);
      }
      bool VisitClassTemplateDecl(clang::ClassTemplateDecl *pattern) override {
        found |= !pattern->getTemplatedDecl()->getDefinition();
        for (const auto *specialization:PublishedClassSpecializations(pattern))found |= !specialization->getDefinition();
        return true;
      }
      bool VisitVarTemplateDecl(clang::VarTemplateDecl *pattern) override {
        const auto *variable=pattern->getTemplatedDecl();
        found |= !variable->getDefinition() && variable->isThisDeclarationADefinition()==clang::VarDecl::DeclarationOnly;
        for (const auto *partial:PublishedVariablePartials(pattern))found |= !partial->getDefinition();
        return true;
      }
      bool VisitCXXRecordDecl(clang::CXXRecordDecl *record) override {
        found |= llvm::isa<clang::ClassTemplateSpecializationDecl>(record) && !record->getDefinition();return true;
      }
      bool VisitVarDecl(clang::VarDecl *variable) override {
        // Provider-owned explicit specializations replay as extern declarations.
        // Their external-storage contract is complete even when the consumer
        // cannot see the provider's definition. Partial patterns and genuinely
        // incomplete primary declarations still require the v2 capability.
        if (const auto *specialization=llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(variable);
            IsExplicitNamespaceVariableSpecialization(specialization) &&
            !PublishesVariableInitializer(variable)) return true;
        found |= llvm::isa<clang::VarTemplateSpecializationDecl>(variable) &&
                 !variable->getDefinition() && variable->isThisDeclarationADefinition()==clang::VarDecl::DeclarationOnly;
        return true;
      }
    } visitor;
    visitor.TraverseDecl(representative);
    if (visitor.found) identity.canonical_type_identity += ";cxx-incomplete-template-declarations:v2;";
  }
  if (HasDeclarationNativeSubobject(representative))
    identity.canonical_type_identity += ";native-subobject-cleanup:v5;";
  if (HasDeclarationNativeAllocation(representative))
    identity.canonical_type_identity += ";native-allocation-cleanup:v2;";
  if (HasDeclarationNativeDisposal(representative))
    identity.canonical_type_identity += ";native-disposal:v2;";
  if (HasDeclarationInitializedOwner(representative))
    identity.canonical_type_identity += ";initialized-owner-prefixes:v1;";
  if (HasDeclarationNativeFinally(representative))
    identity.canonical_type_identity += ";native-finally:v1;";
  if (HasDeclarationManagedRuntimeScope(representative))
    identity.canonical_type_identity += ";managed-runtime-scopes:v1;";
  if (HasDeclarationBorrowedReceiverResult(representative))
    identity.canonical_type_identity += ";borrowed-receiver-results:v1;";
  if (HasDeclarationBorrowedInteriorStorage(representative))
    identity.canonical_type_identity += ";borrowed-interior-storage:v1;";
  if (HasDeclarationBorrowedCallableOwner(representative))
    identity.canonical_type_identity += ";borrowed-callable-owners:v1;";
  if (HasDeclarationAsyncConsumedOwnership(representative))
    identity.canonical_type_identity += ";async-consuming-ownership:v3;";
  if (HasDeclarationDependentKeyPathTypeBody(representative))
    identity.canonical_type_identity += ";keypath-dependent-types:v1;";
  if (HasDeclarationDependentKeyPathLiteral(representative))
    identity.canonical_type_identity += ";keypath-dependent-literals:v1;";
  if (HasDeclarationCheckedKeyPathRead(representative))
    identity.canonical_type_identity += ";keypath-checked-reads:v1;";
  if (HasDeclarationNativeEmissionPolicy(representative))
    identity.canonical_type_identity+=";native-emission-policies:v1;";
  if (HasDeclarationBorrowedUnionStorage(representative))
    identity.canonical_type_identity+=";borrowed-union-byte-storage:v1;";
  if (HasDeclarationBorrowedResultProof(representative))
    identity.canonical_type_identity+=";borrowed-result-proofs:v2;";
  if (HasDeclarationNativeStorageLifetime(representative))
    identity.canonical_type_identity += ";native-storage-lifetime:v1;";
  if (HasDeclarationAtomicManagedProperty(representative))
    identity.canonical_type_identity += ";atomic-managed-properties:v1;";
  if (HasDeclarationNativeLambdaArgumentABI(representative))
    identity.canonical_type_identity += ";native-lambda-argument-abi:v1;";
  if (HasDeclarationNativeMRCRecordPayload(representative))
    identity.canonical_type_identity += ";native-mrc-record-payload:v1;";
  if (HasDeclarationNativeARCRecordPayload(representative))
    identity.canonical_type_identity += ";native-arc-record-payload:v2;";
  if (HasDeclarationNativeCarrierLambda(representative))
    identity.canonical_type_identity += ";native-carrier-lambda:v1;";
  if (HasDeclarationNativeByrefStorage(representative))
    identity.canonical_type_identity += ";native-byref-storage:v1;";
  if (HasDeclarationNativeBlockAlignment(representative))
    identity.canonical_type_identity += ";native-block-alignment:v1;";
  if (HasDeclarationMixedCarrierCall(representative))
    identity.canonical_type_identity += ";mixed-carrier-call:v1;";
  if (HasDeclarationCppCarrierRuntime(representative))
    identity.canonical_type_identity += ";cxx-carrier-runtime:v2;";
  if (HasDeclarationCheckedCppRecordResult(representative))
    identity.canonical_type_identity += ";checked-cxx-record-results:v2;";
  if (HasDeclarationCppCarrierStorage(representative))
    identity.canonical_type_identity += ";cxx-carrier-storage:v1;";
  if (HasDeclarationCheckedCppMessage(representative))
    identity.canonical_type_identity += ";checked-cxx-messages:v1;";
  if (HasDeclarationCppCarrierComposition(representative))
    identity.canonical_type_identity += ";cxx-carrier-composition:v2;";
  if (HasDeclarationCppCarrierCall(representative))
    identity.canonical_type_identity += ";cxx-carrier-call:v2;";
  if (HasDeclarationResourceKeyPathValue(representative))
    identity.canonical_type_identity += ";keypath-resource-values:v2;";
  if (HasDeclarationNullableKeyPathWrite(representative))
    identity.canonical_type_identity += ";keypath-nullable-writes:v1;";
  if (HasDeclarationConcept(representative) && !llvm::isa<clang::ConceptDecl>(representative))
    identity.canonical_type_identity += ";cxx-concepts:v1;";
  if (const auto *method =
          llvm::dyn_cast<clang::CXXMethodDecl>(representative)) {
    identity.canonical_type_identity += method->isStatic()
        ? ";cxx-method=static" : ";cxx-method=instance";
    if (HasClassLifecycle(method->getParent())) {
      identity.canonical_type_identity +=
          ";cxx-class-lifecycle:v1;access=" +
          std::to_string(static_cast<unsigned>(method->getAccess())) +
          ";deleted=" +
          (method->isDeleted() ? std::string("1") : std::string("0"));
      if (const auto *ctor = llvm::dyn_cast<clang::CXXConstructorDecl>(method))
        identity.canonical_type_identity +=
            ctor->isExplicit() ? ";explicit=1" : ";explicit=0";
      if (const auto *conversion =
              llvm::dyn_cast<clang::CXXConversionDecl>(method))
        identity.canonical_type_identity +=
            conversion->isExplicit() ? ";explicit=1" : ";explicit=0";
    }
  }
  if (const auto *method = llvm::dyn_cast<clang::CXXMethodDecl>(representative);
      method && method->isDefaulted() && !IsInClassDefaultedComparison(method))
    identity.canonical_type_identity +=
        ";cxx-class-structure:v1;defaulted=1;constexpr=" +
        (method->isOutOfLine() ? std::to_string(method->isConstexpr()) : std::string("derived")) +
        ";exception=" + std::string(method->getExceptionSpecSourceRange().isInvalid()
                        ? "derived"
                        : "explicit") +
        ";user-provided=" + std::to_string(method->isUserProvided()) +
        ";inline=" + std::to_string(method->isInlined());
  if (const auto *method = llvm::dyn_cast<clang::CXXMethodDecl>(representative);
      method && method->isVirtual()) {
    identity.canonical_type_identity +=
        ";cxx-class-dispatch:v1;virtual=1;pure=" +
        std::to_string(method->isPureVirtual()) +
        ";final=" + std::to_string(method->hasAttr<clang::FinalAttr>());
    for (const auto *overridden : method->overridden_methods())
      identity.canonical_type_identity +=
          ";override=" + ReadInterfaceDeclarationStableId(overridden);
  }
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(representative);
      IsInClassDefaultedComparison(function)) {
    identity.canonical_type_identity += ";cxx-defaulted-comparisons:v1;operator=" +
        std::to_string(function->getOverloadedOperator()) + ";consteval=" +
        std::to_string(function->isConsteval()) + ";deleted=" +
        std::to_string(function->isDeleted()) + ";inline=" +
        std::to_string(function->isInlined()) + ";user-provided=" +
        std::to_string(function->isUserProvided()) + ";";
    if (const auto *owner = FriendOwner(function))
      identity.canonical_type_identity += "friend-owner=" + ReadInterfaceDeclarationStableId(owner) + ";";
  }
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(representative))
    if (const auto *owner = FriendOwner(function); owner && !IsInClassDefaultedComparison(function))
      identity.canonical_type_identity += ";cxx-friends:v1;owner=" +
          ReadInterfaceDeclarationStableId(owner) + ";deleted=" +
          std::to_string(function->isDeleted()) + ";lookup=" +
          std::to_string(function->getIdentifierNamespace()) + ";";
  if (IsOutOfLineDefaultedDefinition(representative))
    identity.canonical_type_identity += ";cxx-out-of-line-defaulting:v1;";
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(representative);
      function && function->isExplicitlyDefaulted() && function->isConsteval())
    identity.canonical_type_identity += ";cxx-immediate-defaulting:v1;consteval=1;";
  if (HasDeclarationGenericLambda(representative))
    identity.canonical_type_identity += ";cxx-generic-local-lambdas:v2;";
  if (HasDeclarationUnionValue(representative))
    identity.canonical_type_identity += ";cxx-union-values:v1;";
  if (HasDeclarationLambda(representative))
    identity.canonical_type_identity += ";cxx-lambda-expressions:v1;";
  if (HasDeclarationLocalExpression(representative, true))
    identity.canonical_type_identity += ";cxx-block-expressions:v3;";
  if (HasDeclarationLocalExpression(representative, false))
    identity.canonical_type_identity += ";cxx-statement-expressions:v1;";
  if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(representative);
      context.getLangOpts().CPlusPlus && PublishesFunctionBody(function)) {
    const auto *type=clang::getObjC3CallableFunctionType(function);
    if (type && type->getObjC3FunctionEffects().Async)
      identity.canonical_type_identity += ";cxx-async-definitions:v1;";
  }
  if (HasRuntimeDeclarationContract(representative,context))
    identity.canonical_type_identity += ";sdk-runtime-declarations:v1;";
  if (HasDeclarationLocalType(representative))
    identity.canonical_type_identity += ";cxx-local-types:v1;";
  if (HasDeclarationUnnamedLocalType(representative))
    identity.canonical_type_identity += ";cxx-unnamed-local-types:v1;";
  if (context.getLangOpts().CPlusPlus && HasDeclarationThreadLocal(representative)) {
    identity.canonical_type_identity += ";cxx-thread-local:v1;";
    if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(representative))
      identity.canonical_type_identity += "specifier=" + std::to_string(variable->getTSCSpec()) +
          ";kind=" + std::to_string(variable->getTLSKind()) + ";";
  }
  bool namespace_attributes = false;
  if (const auto *space = llvm::dyn_cast<clang::NamespaceDecl>(representative)) {
    std::string facts;
    for (const auto &attribute : NamespaceAttributes(space)) {
      namespace_attributes = true;
      facts += std::to_string(attribute.size()) + ":" + attribute;
    }
    if (!facts.empty()) identity.canonical_type_identity += ";namespace-attributes=" + HashInterfaceFacts(facts);
  }
  for (const auto *scope = representative->getDeclContext(); scope; scope = scope->getParent())
    if (const auto *space = llvm::dyn_cast<clang::NamespaceDecl>(scope))
      namespace_attributes |= !NamespaceAttributes(space).empty();
  if (namespace_attributes)
    identity.canonical_type_identity += ";cxx-namespace-attributes:v1;visibility=" +
        std::to_string(representative->getVisibility()) + ";";
  std::vector<const clang::NamespaceDecl *> scopes;
  if (const auto *space = llvm::dyn_cast<clang::NamespaceDecl>(declaration))
    scopes.push_back(space);
  for (const auto *dc = declaration->getDeclContext(); dc; dc = dc->getParent())
    if (const auto *space = llvm::dyn_cast<clang::NamespaceDecl>(dc))
      scopes.push_back(space);
  if (!scopes.empty()) {
    std::reverse(scopes.begin(), scopes.end());
    identity.canonical_type_identity += ";cxx-namespaces:v1;";
    for (const auto *space : scopes)
      identity.canonical_type_identity +=
          "scope=" + ReadInterfaceDeclarationStableId(space) +
          ":inline=" + std::to_string(space->isInline()) + ";";
  }
  if (llvm::isa<clang::TypeDecl>(declaration) &&
      llvm::isa<clang::CXXRecordDecl>(declaration->getDeclContext()))
    identity.canonical_type_identity +=
        std::string(";cxx-nested-types:v1;access=") +
        CxxAccessName(declaration->getAccess());
  const auto availability = AvailabilityIdentity(representative);
  if (!availability.empty())
    identity.canonical_type_identity += ";api-availability=" + HashInterfaceFacts(availability);
  if (auto *value = llvm::dyn_cast<clang::ValueDecl>(representative)) {
    identity.effects = ReadCanonicalFunctionEffects(value->getType(), context);
    const auto lifetime = value->getType().getObjCLifetime();
    if (lifetime != clang::Qualifiers::OCL_None)
      identity.effects.ownership_identity =
          std::to_string(static_cast<unsigned>(lifetime));
  }
  if (auto *type = llvm::dyn_cast<clang::TypeDecl>(representative))
    identity.effects = ReadCanonicalFunctionEffects(
        context.getTypeDeclType(type), context);
  if (auto *method = llvm::dyn_cast<clang::ObjCMethodDecl>(representative))
    identity.effects = ReadCanonicalFunctionEffects(
        method->getObjC3FunctionType(), context);
  if (auto *property = llvm::dyn_cast<clang::ObjCPropertyDecl>(representative))
    identity.effects = ReadCanonicalFunctionEffects(property->getType(), context);
  if (const auto *pattern = llvm::dyn_cast<clang::FunctionTemplateDecl>(representative)) {
    const auto *function = pattern->getTemplatedDecl();
    identity.effects = ReadCanonicalFunctionEffects(function->getType(), context);
    const auto availability = AvailabilityIdentity(function);
    if (!availability.empty())
      identity.canonical_type_identity += ";api-availability=" + HashInterfaceFacts(availability);
  }
  if (const auto *pattern = llvm::dyn_cast<clang::TypeAliasTemplateDecl>(representative))
    identity.effects = ReadCanonicalFunctionEffects(pattern->getTemplatedDecl()->getUnderlyingType(), context);
  if (const auto *pattern = llvm::dyn_cast<clang::VarTemplateDecl>(representative)) {
    const auto type = pattern->getTemplatedDecl()->getType();
    identity.effects = ReadCanonicalFunctionEffects(type, context);
    if (const auto lifetime = type.getObjCLifetime(); lifetime != clang::Qualifiers::OCL_None)
      identity.effects.ownership_identity = std::to_string(static_cast<unsigned>(lifetime));
  }
  const auto return_ownership = ReadNativeReturnOwnership(representative);
  if (!return_ownership.empty()) identity.effects.ownership_identity = return_ownership;
  ReadNativeDeclarationContracts(representative, context, identity.effects);
  const auto native_attributes=DeclarationAttributeKey(representative,true);
  if(!native_attributes.empty())
    identity.canonical_type_identity+=";native-declaration-attributes:v2;contracts="+
        ComputeInterfaceSourceSha256(native_attributes)+";";
  const auto foreign_attributes=DeclarationAttributeKey(representative);
  if(!foreign_attributes.empty())
    identity.canonical_type_identity+=";foreign-import-attributes:v1;contracts="+
        ComputeInterfaceSourceSha256(foreign_attributes)+";";

  return true;
}

}  // namespace objc3::clang_foundation
