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
#include "clang/AST/DeclFriend.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/ObjC3Async.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/AST/RecordLayout.h"
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

class LambdaExpressionFinder final : public clang::DynamicRecursiveASTVisitor {
public:
  bool VisitLambdaExpr(clang::LambdaExpr *expression) override { found = true;generic |= expression->isGenericLambda();return true; }
  bool found = false;
  bool generic = false;
};
bool ContainsLambda(const clang::Stmt *statement) {
  if (!statement) return false;
  LambdaExpressionFinder finder; finder.TraverseStmt(const_cast<clang::Stmt *>(statement));
  return finder.found;
}
template <class Predicate>
bool HasPublishedExpression(const clang::NamedDecl *declaration, const Predicate &contains) {
  for (const auto *member:ClosedParentMemberSpecializations(declaration))
    if (HasPublishedExpression(member,contains)) return true;
  for (const auto *pattern:ClosedParentPatterns(declaration))
    if (HasPublishedExpression(pattern,contains)) return true;
  if (const auto *pattern = llvm::dyn_cast<clang::ClassTemplateDecl>(declaration)) {
    const auto *record=pattern->getTemplatedDecl();
    if (const auto *definition=record->getDefinition()) record=definition;
    for (const auto *member : record->decls())
      if (const auto *named = llvm::dyn_cast<clang::NamedDecl>(member))
        if (HasPublishedExpression(named, contains)) return true;
    for (const auto *specialization:PublishedClassSpecializations(pattern))
      if (HasPublishedExpression(specialization,contains)) return true;
    return false;
  }
  if (const auto *pattern = llvm::dyn_cast<clang::FunctionTemplateDecl>(declaration))
    return HasPublishedExpression(pattern->getTemplatedDecl(), contains);
  if (const auto *pattern = llvm::dyn_cast<clang::VarTemplateDecl>(declaration)) {
    for (const auto *partial:PublishedVariablePartials(pattern))
      if (HasPublishedExpression(partial,contains)) return true;
    return HasPublishedExpression(pattern->getTemplatedDecl(), contains);
  }
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration)) {
    for (const auto *parameter : function->getMostRecentDecl()->parameters())
      if (const auto *expression = WrittenDefaultArgument(parameter);
          expression && contains(expression)) return true;
    if (PublishesFunctionBody(function))
      if (const auto *definition = function->getDefinition()) {
        if (contains(definition->getBody())) return true;
        if (const auto *constructor = llvm::dyn_cast<clang::CXXConstructorDecl>(definition))
          for (const auto *initializer : constructor->inits())
            if (initializer->isWritten() && contains(initializer->getInit())) return true;
      }
  }
  if (const auto *field = llvm::dyn_cast<clang::FieldDecl>(declaration);
      field && field->hasInClassInitializer()) return contains(field->getInClassInitializer());
  if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration);
      PublishesVariableInitializer(variable)) {
    if (const auto *definition=variable->getDefinition()) variable=definition;
    if (variable->hasInit()) return contains(variable->getInit());
  }
  for (const auto *specialization:ConcreteMemberFunctionSpecializations(declaration))
    if (specialization->isInlined() && HasPublishedExpression(specialization,contains)) return true;
  for (const auto *specialization:ConcreteMemberVariableSpecializations(declaration))
    if (HasPublishedExpression(specialization,contains)) return true;
  if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
      record && record->getDefinition())
    for (const auto *member : record->getDefinition()->decls()) {
      if (member->isImplicit()) continue;
      const clang::NamedDecl *named = llvm::dyn_cast<clang::NamedDecl>(member);
      if (const auto *friend_decl = llvm::dyn_cast<clang::FriendDecl>(member))
        named = friend_decl->getFriendDecl();
      if (named && llvm::isa<clang::FieldDecl, clang::FunctionDecl, clang::VarDecl, clang::CXXRecordDecl, clang::TemplateDecl>(named) &&
          HasPublishedExpression(named, contains)) return true;
    }
  return false;
}
bool HasDeclarationNullableKeyPathWrite(const clang::NamedDecl *declaration) {
  return HasPublishedExpression(declaration,[](const clang::Stmt *statement) {
    struct Literals : clang::DynamicRecursiveASTVisitor {
      bool found=false;
      bool VisitCompoundLiteralExpr(clang::CompoundLiteralExpr *literal) override {
        found |= clang::hasObjC3NullableKeyPathWrite(literal);return true;
      }
    } visitor;
    visitor.TraverseStmt(const_cast<clang::Stmt *>(statement));return visitor.found;
  });
}

bool HasDeclarationAsyncConsumedOwnership(const clang::NamedDecl *declaration) {
  return HasPublishedExpression(declaration, [](const clang::Stmt *statement) {
    struct Ownership : clang::DynamicRecursiveASTVisitor {
      bool found = false;
      bool VisitType(clang::Type *type) override {
        if (const auto *function = llvm::dyn_cast<clang::FunctionProtoType>(type);
            function && function->getObjC3FunctionEffects().Async)
          for (unsigned i = 0; i != function->getNumParams(); ++i)
            found |= function->isParamConsumed(i) ||
                     function->getExtParameterInfo(i).isCFConsumed();
        return true;
      }
      bool VisitFunctionDecl(clang::FunctionDecl *function) override {
        found |= HasNativeAsyncConsumedCallable(function); return true;
      }
      bool VisitCallExpr(clang::CallExpr *call) override {
        found |= HasNativeAsyncConsumedCallable(call->getDirectCallee()); return true;
      }
      bool VisitObjCMessageExpr(clang::ObjCMessageExpr *message) override {
        found |= HasNativeAsyncConsumedCallable(message->getMethodDecl()); return true;
      }
      bool VisitBlockDecl(clang::BlockDecl *block) override {
        found |= HasNativeAsyncConsumedCallable(block); return true;
      }
      void NoteSource(const clang::Expr *source) {
        if (!source) return;
        source = source->IgnoreImplicit()->IgnoreParenImpCasts();
        if (const auto *call = llvm::dyn_cast<clang::CallExpr>(source))
          found |= HasNativeConsumedCallable(call->getDirectCallee());
        if (const auto *message = llvm::dyn_cast<clang::ObjCMessageExpr>(source))
          found |= HasNativeConsumedCallable(message->getMethodDecl());
      }
      bool VisitCoawaitExpr(clang::CoawaitExpr *await) override {
        NoteSource(clang::getObjC3AwaitSourceExpression(await));
        return true;
      }
      bool VisitPseudoObjectExpr(clang::PseudoObjectExpr *expression) override {
        NoteSource(clang::getObjC3IsolationSourceSyntax(expression));
        return true;
      }
    } visitor;
    visitor.TraverseStmt(const_cast<clang::Stmt *>(statement));
    return visitor.found;
  });
}

bool HasDeclarationDependentKeyPathTypeBody(const clang::NamedDecl *declaration) {
  return HasPublishedExpression(declaration, [](const clang::Stmt *statement) {
    struct Types : clang::DynamicRecursiveASTVisitor {
      bool found = false;
      bool VisitType(clang::Type *type) override {
        found |= llvm::isa<clang::ObjC3DependentKeyPathType>(type);
        return true;
      }
    } visitor;
    visitor.TraverseStmt(const_cast<clang::Stmt *>(statement));
    return visitor.found;
  });
}

bool HasDeclarationDependentKeyPathLiteral(const clang::NamedDecl *declaration) {
  return HasPublishedExpression(declaration, [](const clang::Stmt *statement) {
    struct Literals : clang::DynamicRecursiveASTVisitor {
      bool found = false;
      bool VisitCallExpr(clang::CallExpr *call) override {
        clang::ObjC3DependentKeyPathLiteralInfo literal;
        found |= clang::getObjC3DependentKeyPathLiteral(call, literal);
        return true;
      }
    } visitor;
    visitor.TraverseStmt(const_cast<clang::Stmt *>(statement));
    return visitor.found;
  });
}

bool HasDeclarationCheckedKeyPathRead(const clang::NamedDecl *declaration) {
  return HasPublishedExpression(declaration,[](const clang::Stmt *statement) {
    struct Literals : clang::DynamicRecursiveASTVisitor {
      bool found=false;
      bool VisitCompoundLiteralExpr(clang::CompoundLiteralExpr *literal) override {
        found |= clang::hasObjC3CheckedKeyPathRead(literal);return true;
      }
    } visitor;
    visitor.TraverseStmt(const_cast<clang::Stmt *>(statement));return visitor.found;
  });
}

bool HasDeclarationLambda(const clang::NamedDecl *declaration) {
  return HasPublishedExpression(declaration, ContainsLambda);
}
// A direct deduced result belongs to its complete published factory body.
// References/pointers and types originating in other factories remain separate.
bool IsPublishedDeducedFactory(const clang::FunctionDecl *function) {
  if(!function || !function->getDefinition() || !function->getDeclaredReturnType()->getContainedAutoType())return false;
  if(PublishesFunctionBody(function))return true;
  const auto *primary=function->getPrimaryTemplate();
  return primary && function->getTemplateSpecializationKind()!=clang::TSK_ExplicitSpecialization &&
         PublishesFunctionBody(primary->getTemplatedDecl());
}

std::vector<const clang::TagDecl *> PublishedFactoryResults(const clang::FunctionDecl *function) {
  if(!IsPublishedDeducedFactory(function))return {};
  struct Results:clang::DynamicRecursiveASTVisitor {
    const clang::FunctionDecl *function;
    std::vector<const clang::TagDecl *> tags;
    explicit Results(const clang::FunctionDecl *function):function(function) {}
    bool TraverseType(clang::QualType type,bool traverse_qualifier=true) override {
      return type.isNull() || clang::DynamicRecursiveASTVisitor::TraverseType(type.getCanonicalType(),traverse_qualifier);
    }
    bool TraverseTemplateArgument(const clang::TemplateArgument &argument) override {
      if(argument.getKind()==clang::TemplateArgument::Integral)return TraverseType(argument.getIntegralType());
      if(argument.getKind()==clang::TemplateArgument::NullPtr)return TraverseType(argument.getNullPtrType());
      if(argument.getKind()==clang::TemplateArgument::StructuralValue)return TraverseType(argument.getStructuralValueType());
      if(argument.getKind()==clang::TemplateArgument::Declaration) {
        if(const auto *object=llvm::dyn_cast<clang::TemplateParamObjectDecl>(argument.getAsDecl()))return TraverseType(object->getType());
        return TraverseType(argument.getParamTypeForDecl());
      }
      return clang::DynamicRecursiveASTVisitor::TraverseTemplateArgument(argument);
    }
    bool VisitTagType(clang::TagType *type) override {
      const auto *tag=type->getDecl();
      if(tag->getDefinition() && IsExpressionLocalTag(tag))
        for(const auto *scope=tag->getDeclContext();scope;scope=scope->getParent())
          if(const auto *owner=llvm::dyn_cast<clang::FunctionDecl>(scope)) {
            if(owner->getCanonicalDecl()==function->getCanonicalDecl() &&
                std::none_of(tags.begin(),tags.end(),[&](const auto *prior){return prior->getCanonicalDecl()==tag->getCanonicalDecl();}))tags.push_back(tag);
            break;
          }
      if(const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(tag))
        for(const auto &argument:specialization->getTemplateArgs().asArray())TraverseTemplateArgument(argument);
      return true;
    }
  } visitor(function);
  visitor.TraverseType(function->getReturnType());return visitor.tags;
}

const clang::TagDecl *PublishedFactoryResult(const clang::FunctionDecl *function) {
  const auto results=PublishedFactoryResults(function);
  return results.empty()?nullptr:results.front();
}

const clang::CXXRecordDecl *PublishedClosureResult(const clang::FunctionDecl *function) {
  const auto *record=llvm::dyn_cast_or_null<clang::CXXRecordDecl>(PublishedFactoryResult(function));
  return record && record->isLambda() ? record : nullptr;
}

bool HasDeclarationLocalFactoryResult(const clang::NamedDecl *declaration) {
  return HasPublishedExpression(declaration,[](const clang::Stmt *statement) {
    struct Finder:clang::DynamicRecursiveASTVisitor {
      bool found=false;
      bool VisitTagDecl(clang::TagDecl *tag) override {
        if(const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(tag);record && record->isLambda())return true;
        for (const auto *scope=tag->getDeclContext();scope;scope=scope->getParent())
          if (const auto *owner=llvm::dyn_cast<clang::FunctionDecl>(scope)) {
            const auto *result=PublishedFactoryResult(owner);
            found |= (result && result->getCanonicalDecl()==tag->getCanonicalDecl()) ||
                (!result && owner->isTemplated() && IsPublishedDeducedFactory(owner));
            break;
          }
        return true;
      }
    } visitor;
    visitor.TraverseStmt(const_cast<clang::Stmt *>(statement));return visitor.found;
  });
}

// Public projections must retain the complete nominal factory origin, not only
// an anonymous spelling or a layout shared by unrelated local declarations.
bool GenericFactoryBindings(const clang::FunctionDecl *function,std::string &key) {
  const auto &context=function->getASTContext();
  auto add=[&](llvm::StringRef value){key+=std::to_string(value.size())+":"+value.str();};
  if(const auto *arguments=function->getTemplateSpecializationArgs()) {
    add("function");
    for(const auto &argument:arguments->asArray())
      if(!GenericArgumentKey(argument,context,key))return false;
  }
  for(const auto *scope=function->getDeclContext();scope;scope=scope->getParent())
    if(const auto *parent=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(scope)) {
      if(parent->isDependentContext())return false;
      add(ReadInterfaceDeclarationStableId(parent->getSpecializedTemplate()));
      for(const auto &argument:parent->getTemplateArgs().asArray())
        if(!GenericArgumentKey(argument,context,key))return false;
    }
  for(const auto *scope=function->getDeclContext();scope;scope=scope->getParent())
    if(const auto *enclosing=llvm::dyn_cast<clang::FunctionDecl>(scope))
      if(const auto *arguments=enclosing->getTemplateSpecializationArgs()) {
        add("enclosing-function");add(ReadInterfaceDeclarationStableId(enclosing));
        for(const auto &argument:arguments->asArray())
          if(!GenericArgumentKey(argument,context,key))return false;
      }
  return true;
}

bool IsWithinFactoryResult(const clang::TagDecl *tag,const clang::TagDecl *result) {
  if(!result)return false;
  for(const clang::DeclContext *scope=tag;scope;scope=scope->getParent()) {
    const auto *enclosing=llvm::dyn_cast<clang::TagDecl>(scope);
    if(!enclosing)return false;
    if(enclosing->getCanonicalDecl()==result->getCanonicalDecl())return true;
  }
  return false;
}

const clang::TagDecl *FactoryResultTagRoot(const clang::FunctionDecl *function,const clang::TagDecl *tag) {
  for(const auto *result:PublishedFactoryResults(function))
    if(IsWithinFactoryResult(tag,result))return result;
  return nullptr;
}

std::string FactoryTagPath(const clang::TagDecl *tag,const clang::TagDecl *root,const clang::ASTContext &context) {
  std::string path;
  for(const clang::DeclContext *scope=tag;scope;scope=scope->getParent()) {
    const auto *part=llvm::cast<clang::TagDecl>(scope);
    const auto name=std::to_string(static_cast<unsigned>(part->getTagKind()))+":"+
        part->getNameAsString()+":"+std::to_string(context.getManglingNumber(part));
    path+=std::to_string(name.size())+":"+name;
    if(part->getCanonicalDecl()==root->getCanonicalDecl())break;
  }
  return path;
}

const clang::FunctionDecl *FactoryResultOrigin(const clang::TagDecl *tag) {
  for(const auto *scope=tag->getDeclContext();scope;scope=scope->getParent())
    if(const auto *function=llvm::dyn_cast<clang::FunctionDecl>(scope)) {
      const clang::FunctionDecl *closure_owner=nullptr;
      if(const auto *method=llvm::dyn_cast<clang::CXXMethodDecl>(function)) {
        if(method->getParent()->isLambda() || IsExpressionLocalTag(method->getParent())) {
          closure_owner=FactoryResultOrigin(method->getParent());
          if(!closure_owner)return nullptr;
        }
      } else if(!IsTopLevel(function))return nullptr;
      const auto *result=FactoryResultTagRoot(function,tag);
      if(!result ||
          function->getType()->isDependentType() || (!closure_owner && !function->isExternallyVisible()))return nullptr;
      bool bound=closure_owner || !ReadNativeSymbolName(function).empty();
      if(const auto *primary=function->getPrimaryTemplate())
        bound |= !ReadNativeSymbolName(primary).empty() || !ReadNativeSymbolName(primary->getTemplatedDecl()).empty();
      if(const auto *owner=TemplatePublicationOwner(function))bound |= !ReadNativeSymbolName(owner).empty();
      std::string bindings;
      return bound && GenericFactoryBindings(function,bindings) ? function : nullptr;
    }
  return nullptr;
}

std::string FactoryOriginKey(const clang::FunctionDecl *function) {
  if(const auto *method=llvm::dyn_cast<clang::CXXMethodDecl>(function);
      method && (method->getParent()->isLambda() || IsExpressionLocalTag(method->getParent()))) {
    const auto *owner=FactoryResultOrigin(method->getParent());
    if(!owner)return {};
    std::string bindings;GenericFactoryBindings(owner,bindings);
    const auto parent=FactoryOriginKey(owner);
    std::string key=std::to_string(parent.size())+":"+parent+":local-call:"+std::to_string(bindings.size())+":"+bindings;
    if(method->getParent()->isLambda())return key+":closure";
    const auto path=FactoryTagPath(method->getParent(),FactoryResultTagRoot(owner,method->getParent()),function->getASTContext());
    key+=":record="+std::to_string(path.size())+":"+path;
    const auto name=method->getNameAsString();key+=":member="+std::to_string(name.size())+":"+name;
    key+=":cv="+std::to_string(method->getMethodQualifiers().getAsOpaqueValue())+":ref="+std::to_string(method->getRefQualifier());
    for(const auto *parameter:function->parameters()) {
      const auto type=CanonicalType(parameter->getType(),function->getASTContext());
      key+=std::to_string(type.size())+":"+type;
    }
    return key;
  }
  return ReadInterfaceDeclarationStableId(function);
}

std::string FactoryTypeKey(clang::QualType type,const clang::ASTContext &context,FactoryTypeFeatures *features) {
  struct Origins:clang::DynamicRecursiveASTVisitor,FactoryTypeFeatures {
    const clang::ASTContext &context;
    std::string key;
    explicit Origins(const clang::ASTContext &context):context(context) {}
    bool TraverseType(clang::QualType type,bool traverse_qualifier=true) override {
      return type.isNull() || clang::DynamicRecursiveASTVisitor::TraverseType(type.getCanonicalType(),traverse_qualifier);
    }
    bool TraverseTemplateArgument(const clang::TemplateArgument &argument) override {
      if(argument.getKind()==clang::TemplateArgument::StructuralValue)
        return TraverseType(argument.getStructuralValueType());
      if(argument.getKind()==clang::TemplateArgument::Declaration) {
        if(const auto *object=llvm::dyn_cast<clang::TemplateParamObjectDecl>(argument.getAsDecl()))
          return TraverseType(object->getType());
        return TraverseType(argument.getParamTypeForDecl());
      }
      return clang::DynamicRecursiveASTVisitor::TraverseTemplateArgument(argument);
    }
    bool VisitTagType(clang::TagType *type) override {
      const auto *tag=type->getDecl();
      if(const auto *factory=FactoryResultOrigin(tag)) {
        if(const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(tag))closure |= record->isLambda();
        if(const auto *method=llvm::dyn_cast<clang::CXXMethodDecl>(factory))lambda_result |= method->getParent()->isLambda();
        const auto owner=FactoryOriginKey(factory);
        auto identity=owner+":kind="+std::to_string(static_cast<unsigned>(tag->getTagKind()))+
            ":name="+tag->getNameAsString()+":discriminator="+std::to_string(context.getManglingNumber(tag));
        const auto *result=FactoryResultTagRoot(factory,tag);
        if(tag->getCanonicalDecl()!=result->getCanonicalDecl()) {
          nested=true;identity+=":lexical="+FactoryTagPath(tag,result,context);
        }
        const auto *direct=factory->getReturnType()->getAsTagDecl();
        nested |= !direct || direct->getCanonicalDecl()!=result->getCanonicalDecl();
        if(const auto *method=llvm::dyn_cast<clang::CXXMethodDecl>(factory))
          nested |= IsExpressionLocalTag(method->getParent()) && !method->getParent()->isLambda();
        std::string bindings;
        if(!GenericFactoryBindings(factory,bindings))return false;
        if(!bindings.empty()){generic=true;identity+=":bindings="+bindings;}
        key+=std::to_string(identity.size())+":"+identity;
      }
      if(const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(tag))
        for(const auto &argument:specialization->getTemplateArgs().asArray())TraverseTemplateArgument(argument);
      return true;
    }
  } visitor(context);
  if(!type.isNull())visitor.TraverseType(type.getCanonicalType());
  if(features) {
    features->generic |= visitor.generic;
    features->closure |= visitor.closure;
    features->lambda_result |= visitor.lambda_result;
    features->nested |= visitor.nested;
  }
  return visitor.key;
}

bool HasOnlyProjectedLocalTypes(clang::QualType type) {
  struct Qualified:clang::DynamicRecursiveASTVisitor {
    bool valid=true;
    bool TraverseType(clang::QualType type,bool traverse_qualifier=true) override {
      return type.isNull() || clang::DynamicRecursiveASTVisitor::TraverseType(type.getCanonicalType(),traverse_qualifier);
    }
    bool TraverseTemplateArgument(const clang::TemplateArgument &argument) override {
      if(argument.getKind()==clang::TemplateArgument::StructuralValue)
        return TraverseType(argument.getStructuralValueType());
      if(argument.getKind()==clang::TemplateArgument::Declaration) {
        if(const auto *object=llvm::dyn_cast<clang::TemplateParamObjectDecl>(argument.getAsDecl()))
          return TraverseType(object->getType());
        return TraverseType(argument.getParamTypeForDecl());
      }
      return clang::DynamicRecursiveASTVisitor::TraverseTemplateArgument(argument);
    }
    bool VisitTagType(clang::TagType *type) override {
      const auto *tag=type->getDecl();
      if(IsExpressionLocalTag(tag) || (!tag->getIdentifier() && !tag->getTypedefNameForAnonDecl()))
        valid &= FactoryResultOrigin(tag)!=nullptr ||
            !UnnamedTemplateTagKey(tag,tag->getASTContext()).empty();
      if(const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(tag))
        for(const auto &argument:specialization->getTemplateArgs().asArray())TraverseTemplateArgument(argument);
      return true;
    }
  } visitor;
  visitor.TraverseType(type);return visitor.valid;
}

bool HasFactoryTemplateArgument(const clang::TemplateArgument &argument,const clang::ASTContext &context,FactoryTypeFeatures *features) {
  if(argument.getKind()==clang::TemplateArgument::Type)
    return !FactoryTypeKey(argument.getAsType(),context,features).empty();
  bool found=false;
  if(argument.getKind()==clang::TemplateArgument::Pack)
    for(const auto &element:argument.pack_elements())found |= HasFactoryTemplateArgument(element,context,features);
  return found;
}

void ReadFactoryArgumentFeatures(const clang::TemplateArgument &argument,const clang::ASTContext &context,FactoryTypeFeatures &features) {
  clang::QualType type;
  if(argument.getKind()==clang::TemplateArgument::Type)type=argument.getAsType();
  else if(argument.getKind()==clang::TemplateArgument::StructuralValue)type=argument.getStructuralValueType();
  else if(argument.getKind()==clang::TemplateArgument::Declaration) {
    if(const auto *object=llvm::dyn_cast<clang::TemplateParamObjectDecl>(argument.getAsDecl()))type=object->getType();
  } else if(argument.getKind()==clang::TemplateArgument::Pack) {
    for(const auto &element:argument.pack_elements())ReadFactoryArgumentFeatures(element,context,features);
  }
  if(!type.isNull())FactoryTypeKey(type,context,&features);
}

bool HasFactoryValueArgument(const clang::TemplateArgument &argument,const clang::ASTContext &context) {
  if(argument.getKind()==clang::TemplateArgument::StructuralValue)
    return !FactoryTypeKey(argument.getStructuralValueType(),context).empty();
  if(argument.getKind()==clang::TemplateArgument::Declaration)
    if(const auto *object=llvm::dyn_cast<clang::TemplateParamObjectDecl>(argument.getAsDecl()))
      return !FactoryTypeKey(object->getType(),context).empty();
  if(argument.getKind()==clang::TemplateArgument::Pack)
    for(const auto &element:argument.pack_elements())
      if(HasFactoryValueArgument(element,context))return true;
  if(argument.getKind()==clang::TemplateArgument::Type)
    if(const auto *specialization=llvm::dyn_cast_or_null<clang::ClassTemplateSpecializationDecl>(argument.getAsType()->getAsCXXRecordDecl()))
      for(const auto &element:specialization->getTemplateArgs().asArray())
        if(HasFactoryValueArgument(element,context))return true;
  return false;
}

std::string FactoryDeclarationTypeKey(const clang::NamedDecl *declaration) {
  if(llvm::isa<clang::NamespaceDecl>(declaration))return {};
  struct References:clang::DynamicRecursiveASTVisitor {
    std::string key;
    bool TraverseStmt(clang::Stmt *) override {return true;}
    bool TraverseDecl(clang::Decl *declaration) override {
      return !declaration || declaration->isImplicit() ||
          clang::DynamicRecursiveASTVisitor::TraverseDecl(declaration);
    }
    void Add(clang::QualType type,const clang::ASTContext &context) {
      const auto origins=FactoryTypeKey(type,context);
      if(!origins.empty())key+=std::to_string(origins.size())+":"+origins;
    }
    bool VisitValueDecl(clang::ValueDecl *declaration) override {
      Add(declaration->getType(),declaration->getASTContext());return true;
    }
    bool VisitTypedefNameDecl(clang::TypedefNameDecl *declaration) override {
      Add(declaration->getUnderlyingType(),declaration->getASTContext());return true;
    }
    bool VisitCXXRecordDecl(clang::CXXRecordDecl *declaration) override {
      if(declaration->isCompleteDefinition())
        for(const auto &base:declaration->bases())
          Add(base.getType(),declaration->getASTContext());
      return true;
    }
  } visitor;
  visitor.TraverseDecl(const_cast<clang::NamedDecl *>(declaration));return visitor.key;
}

std::string UnnamedTemplateDeclarationTypeKey(const clang::NamedDecl *declaration) {
  if(llvm::isa<clang::NamespaceDecl>(declaration))return {};
  struct References:clang::DynamicRecursiveASTVisitor {
    std::string key;
    bool TraverseStmt(clang::Stmt *) override {return true;}
    bool TraverseDecl(clang::Decl *declaration) override {
      return !declaration || declaration->isImplicit() ||
          clang::DynamicRecursiveASTVisitor::TraverseDecl(declaration);
    }
    void Add(clang::QualType type,const clang::ASTContext &context) {
      const auto origins=UnnamedTemplateTypeKey(type,context);
      if(!origins.empty())key+=std::to_string(origins.size())+":"+origins;
    }
    bool VisitValueDecl(clang::ValueDecl *declaration) override {
      // The enclosing function type already binds each parameter. TypeLoc-only
      // parameter declarations are source sugar omitted by ASTImporter.
      if(!llvm::isa<clang::ParmVarDecl>(declaration))Add(declaration->getType(),declaration->getASTContext());
      return true;
    }
    void AddArgument(const clang::TemplateArgument &argument,const clang::ASTContext &context) {
      switch(argument.getKind()) {
      case clang::TemplateArgument::Type:Add(argument.getAsType(),context);break;
      case clang::TemplateArgument::Integral:Add(argument.getIntegralType(),context);break;
      case clang::TemplateArgument::NullPtr:Add(argument.getNullPtrType(),context);break;
      case clang::TemplateArgument::Expression:Add(argument.getAsExpr()->getType(),context);break;
      case clang::TemplateArgument::StructuralValue:Add(argument.getStructuralValueType(),context);break;
      case clang::TemplateArgument::Declaration:Add(argument.getParamTypeForDecl(),context);break;
      case clang::TemplateArgument::Template:case clang::TemplateArgument::TemplateExpansion:
        if(const auto *pattern=argument.getAsTemplateOrTemplatePattern().getAsTemplateDecl()) {
          const auto *record=llvm::dyn_cast<clang::ClassTemplateDecl>(pattern);
          const auto *owner=record?record->getTemplatedDecl():llvm::dyn_cast<clang::CXXRecordDecl>(pattern->getDeclContext());
          const auto origin=UnnamedTemplateRecordKey(owner,context);
          if(!origin.empty())key+=std::to_string(origin.size())+":"+origin;
        }
        break;
      default:break;
      }
    }
    bool VisitTemplateTypeParmDecl(clang::TemplateTypeParmDecl *declaration) override {
      if(declaration->hasDefaultArgument())AddArgument(declaration->getDefaultArgument().getArgument(),declaration->getASTContext());
      return true;
    }
    bool VisitNonTypeTemplateParmDecl(clang::NonTypeTemplateParmDecl *declaration) override {
      if(declaration->hasDefaultArgument())AddArgument(declaration->getDefaultArgument().getArgument(),declaration->getASTContext());
      return true;
    }
    bool VisitTemplateTemplateParmDecl(clang::TemplateTemplateParmDecl *declaration) override {
      if(declaration->hasDefaultArgument())AddArgument(declaration->getDefaultArgument().getArgument(),declaration->getASTContext());
      return true;
    }
    bool VisitObjCMethodDecl(clang::ObjCMethodDecl *declaration) override {
      Add(declaration->getReturnType(),declaration->getASTContext());
      for(const auto *parameter:declaration->parameters())Add(parameter->getType(),declaration->getASTContext());
      return true;
    }
    bool VisitObjCPropertyDecl(clang::ObjCPropertyDecl *declaration) override {
      Add(declaration->getType(),declaration->getASTContext());return true;
    }
    bool VisitTypedefNameDecl(clang::TypedefNameDecl *declaration) override {
      Add(declaration->getUnderlyingType(),declaration->getASTContext());return true;
    }
    bool VisitCXXRecordDecl(clang::CXXRecordDecl *declaration) override {
      if(declaration->isCompleteDefinition())
        for(const auto &base:declaration->bases())
          Add(base.getType(),declaration->getASTContext());
      return true;
    }
  } visitor;
  visitor.TraverseDecl(const_cast<clang::NamedDecl *>(declaration));return visitor.key;
}

bool HasDeclarationFactoryTypeProjection(const clang::NamedDecl *declaration) {
  return !FactoryDeclarationTypeKey(declaration).empty();
}

bool HasDeclarationClosureFactory(const clang::NamedDecl *declaration) {
  return HasPublishedExpression(declaration,[](const clang::Stmt *statement) {
    struct Finder:clang::DynamicRecursiveASTVisitor {
      bool found=false;
      bool VisitLambdaExpr(clang::LambdaExpr *expression) override {
        for (const auto *scope=expression->getLambdaClass()->getDeclContext();scope;scope=scope->getParent())
          if (const auto *owner=llvm::dyn_cast<clang::FunctionDecl>(scope)) {
            found |= PublishedClosureResult(owner)==expression->getLambdaClass();break;
          }
        return true;
      }
    } visitor;
    visitor.TraverseStmt(const_cast<clang::Stmt *>(statement));return visitor.found;
  });
}

bool HasDeclarationEffectfulLambda(const clang::NamedDecl *declaration) {
  return HasPublishedExpression(declaration,[](const clang::Stmt *statement) {
    struct Finder:clang::DynamicRecursiveASTVisitor {
      bool found=false;
      bool VisitLambdaExpr(clang::LambdaExpr *expression) override {
        const auto effects=ReadCanonicalFunctionEffects(expression->getCallOperator()->getType(),expression->getLambdaClass()->getASTContext());
        found |= effects.is_async || effects.is_throwing;return true;
      }
    } visitor;
    visitor.TraverseStmt(const_cast<clang::Stmt *>(statement));return visitor.found;
  });
}

bool HasDeclarationConcept(const clang::NamedDecl *declaration) {
  if (llvm::isa<clang::ConceptDecl>(declaration)) return true;
  struct References : clang::DynamicRecursiveASTVisitor {
    bool found = false;
    bool TraverseConceptReference(clang::ConceptReference *reference) override {
      if (!reference) return true;
      found = true;
      return clang::DynamicRecursiveASTVisitor::TraverseConceptReference(reference);
    }
  } visitor;
  if (llvm::isa<clang::TemplateDecl>(declaration) ||
      (llvm::isa<clang::CXXRecordDecl>(declaration) &&
       HasDirectMemberTemplates(llvm::cast<clang::CXXRecordDecl>(declaration)))) {
    visitor.TraverseDecl(const_cast<clang::NamedDecl *>(declaration));
    if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(declaration))
      for (const auto *specialization:PublishedClassSpecializations(pattern))
        visitor.TraverseDecl(const_cast<clang::ClassTemplateSpecializationDecl *>(specialization));
    if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(declaration))
      for (const auto *partial:PublishedVariablePartials(pattern))
        visitor.TraverseDecl(const_cast<clang::VarTemplatePartialSpecializationDecl *>(partial));
    for (auto *member:ClosedParentMemberSpecializations(declaration)) visitor.TraverseDecl(member);
    for (auto *pattern:ClosedParentPatterns(declaration)) visitor.TraverseDecl(pattern);
    for (auto *definition:DetachedCallableTemplateDefinitions(declaration)) visitor.TraverseDecl(definition);
    for (auto *definition:DetachedStaticDataTemplateDefinitions(declaration)) visitor.TraverseDecl(definition);
    for (auto *definition:DetachedNestedTemplateDefinitions(declaration)) visitor.TraverseDecl(definition);
    for (auto *specialization:ConcreteMemberClassSpecializations(declaration)) visitor.TraverseDecl(specialization);
    for (auto *specialization:ConcreteMemberFunctionSpecializations(declaration))
      if (specialization->isInlined()) visitor.TraverseDecl(specialization);
    for (auto *specialization:ConcreteMemberVariableSpecializations(declaration))
      if (PublishesVariableInitializer(specialization)) visitor.TraverseDecl(specialization);
    return visitor.found;
  }
  return HasPublishedExpression(declaration, [](const clang::Stmt *statement) {
    References references;
    references.TraverseStmt(const_cast<clang::Stmt *>(statement));
    return references.found;
  });
}

bool HasDeclarationGenericLambda(const clang::NamedDecl *declaration) {
  return HasPublishedExpression(declaration,[](const clang::Stmt *statement) {
    LambdaExpressionFinder finder;
    finder.TraverseStmt(const_cast<clang::Stmt *>(statement));
    return finder.generic;
  });
}

class LocalExpressionFinder final : public clang::DynamicRecursiveASTVisitor {
public:
  bool VisitBlockExpr(clang::BlockExpr *) override { blocks = true; return true; }
  bool VisitVarDecl(clang::VarDecl *variable) override {
    blocks |= variable->hasAttr<clang::BlocksAttr>(); return true;
  }
  bool VisitStmtExpr(clang::StmtExpr *) override { statements = true; return true; }
  bool VisitTagDecl(clang::TagDecl *declaration) override {
    const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
    const bool local=IsExpressionLocalTag(declaration) && !(record && record->isLambda());
    types |= local;
    unnamed_types |= local && !declaration->getIdentifier();
    union_types |= local && record && record->isUnion();
    return true;
  }
  bool blocks = false;
  bool statements = false;
  bool types = false;
  bool unnamed_types = false;
  bool union_types = false;
};
bool HasDeclarationLocalType(const clang::NamedDecl *declaration) {
  return HasPublishedExpression(declaration, [](const clang::Stmt *statement) {
    LocalExpressionFinder finder;
    finder.TraverseStmt(const_cast<clang::Stmt *>(statement));
    return finder.types;
  });
}
bool ContainsUnnamedLocalType(const clang::Stmt *statement) {
  LocalExpressionFinder finder;
  finder.TraverseStmt(const_cast<clang::Stmt *>(statement));
  return finder.unnamed_types;
}
bool HasDeclarationUnnamedLocalType(const clang::NamedDecl *declaration) {
  return HasPublishedExpression(declaration,ContainsUnnamedLocalType);
}
bool HasDeclarationUnionValue(const clang::NamedDecl *declaration) {
  if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
      record && record->isUnion()) return true;
  return HasPublishedExpression(declaration, [](const clang::Stmt *statement) {
    LocalExpressionFinder finder;
    finder.TraverseStmt(const_cast<clang::Stmt *>(statement));
    return finder.union_types;
  });
}
bool HasDeclarationLocalExpression(const clang::NamedDecl *declaration, bool block) {
  return HasPublishedExpression(declaration, [=](const clang::Stmt *statement) {
    LocalExpressionFinder finder;
    finder.TraverseStmt(const_cast<clang::Stmt *>(statement));
    return block ? finder.blocks : finder.statements;
  });
}

class ThreadLocalFinder final : public clang::DynamicRecursiveASTVisitor {
public:
  bool VisitVarDecl(clang::VarDecl *variable) override {
    found |= variable->getTLSKind() != clang::VarDecl::TLS_None;
    return true;
  }
  bool found = false;
};
bool HasDeclarationThreadLocal(const clang::NamedDecl *declaration) {
  if (const auto *pattern=llvm::dyn_cast<clang::FunctionTemplateDecl>(declaration))
    return HasDeclarationThreadLocal(pattern->getTemplatedDecl());
  if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(declaration)) {
    for (const auto *partial:PublishedVariablePartials(pattern))
      if (HasDeclarationThreadLocal(partial)) return true;
    return HasDeclarationThreadLocal(pattern->getTemplatedDecl());
  }
  if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(declaration)) {
    for (const auto *specialization:PublishedClassSpecializations(pattern))
      if (HasDeclarationThreadLocal(specialization)) return true;
    return HasDeclarationThreadLocal(pattern->getTemplatedDecl());
  }
  if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration))
    return variable->getTLSKind() != clang::VarDecl::TLS_None;
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration);
      PublishesFunctionBody(function)) {
    if (const auto *definition = function->getDefinition()) {
      ThreadLocalFinder finder;
      finder.TraverseStmt(definition->getBody());
      if (finder.found) return true;
    }
  }
  if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
      record && record->getDefinition())
    for (const auto *member : record->getDefinition()->decls()) {
      if (member->isImplicit() || llvm::isa<clang::VarTemplateSpecializationDecl>(member)) continue;
      if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(member);
          function && function->getTemplateSpecializationKind()==clang::TSK_ImplicitInstantiation) continue;
      if (const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(member);
          specialization && specialization->getSpecializationKind()==clang::TSK_ImplicitInstantiation) continue;
      if (const auto *named=llvm::dyn_cast<clang::NamedDecl>(member))
        if (llvm::isa<clang::VarDecl,clang::FunctionDecl,clang::TemplateDecl,clang::CXXRecordDecl>(named) &&
            HasDeclarationThreadLocal(named)) return true;
    }
  return false;
}

bool HasTemplateTemplateParameters(const clang::TemplateParameterList *parameters) {
  return llvm::any_of(*parameters,[](const auto *parameter) {
    return llvm::isa<clang::TemplateTemplateParmDecl>(parameter);
  });
}
bool HasDeclarationTemplateTemplateParameters(const clang::NamedDecl *declaration) {
  if (!declaration) return false;
  if (const auto *partial=llvm::dyn_cast<clang::VarTemplatePartialSpecializationDecl>(declaration))
    if (HasTemplateTemplateParameters(partial->getTemplateParameters())) return true;
  if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(declaration))
    for (const auto *partial:PublishedVariablePartials(pattern))
      if (HasDeclarationTemplateTemplateParameters(partial)) return true;
  if (const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(declaration))
    if (HasTemplateTemplateParameters(partial->getTemplateParameters())) return true;
  if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(declaration))
    for (const auto *specialization:PublishedClassSpecializations(pattern))
      if (HasDeclarationTemplateTemplateParameters(specialization)) return true;
  if (const auto *pattern=llvm::dyn_cast<clang::TemplateDecl>(declaration)) {
    if (HasTemplateTemplateParameters(pattern->getTemplateParameters())) return true;
    return HasDeclarationTemplateTemplateParameters(pattern->getTemplatedDecl());
  }
  if (const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
      record && record->getDefinition())
    for (const auto *member:record->getDefinition()->decls()) {
      if (member->isImplicit() || llvm::isa<clang::VarTemplateSpecializationDecl>(member)) continue;
      if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(member);
          function && function->getTemplateSpecializationKind()==clang::TSK_ImplicitInstantiation) continue;
      if (const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(member);
          specialization && specialization->getSpecializationKind()==clang::TSK_ImplicitInstantiation) continue;
      if (const auto *named=llvm::dyn_cast<clang::NamedDecl>(member))
        if (HasDeclarationTemplateTemplateParameters(named)) return true;
    }
  return HasPublishedExpression(declaration,[](const clang::Stmt *statement) {
    struct References:clang::DynamicRecursiveASTVisitor {
      bool found=false;
      bool VisitLambdaExpr(clang::LambdaExpr *expression) override {
        if (const auto *pattern=expression->getCallOperator()->getDescribedFunctionTemplate())
          found |= HasTemplateTemplateParameters(pattern->getTemplateParameters());
        return true;
      }
      bool VisitDeclRefExpr(clang::DeclRefExpr *expression) override {
        if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(expression->getDecl()))
          if (const auto *arguments=function->getTemplateSpecializationArgs())
            for (const auto &argument:arguments->asArray())
              found |= argument.getKind()==clang::TemplateArgument::Template;
        return true;
      }
      bool VisitTemplateSpecializationType(clang::TemplateSpecializationType *type) override {
        for (const auto &argument:type->template_arguments())
          found |= argument.getKind()==clang::TemplateArgument::Template;
        return true;
      }
    } visitor;
    visitor.TraverseStmt(const_cast<clang::Stmt *>(statement));return visitor.found;
  });
}

bool HasClassLifecycle(const clang::CXXRecordDecl *record) {
  if (!record || !record->isCompleteDefinition())
    return false;
  if (record->isDynamicClass() || record->hasAttr<clang::FinalAttr>() ||
      record->getNumBases() != 0 ||
      record->hasUserDeclaredConstructor() ||
      record->hasUserDeclaredDestructor() ||
      record->hasUserDeclaredCopyAssignment() ||
      record->hasUserDeclaredMoveAssignment())
    return true;
  for (const auto *member : record->decls())
    if (!member->isImplicit()) {
      if (const auto *field = llvm::dyn_cast<clang::FieldDecl>(member)) {
        if (field->getAccess() != clang::AS_public || field->isMutable())
          return true;
      } else if (const auto *method =
                     llvm::dyn_cast<clang::CXXMethodDecl>(member))
        if (method->getAccess() != clang::AS_public || method->isDeleted() ||
            llvm::isa<clang::CXXConversionDecl>(method))
          return true;
    }
  return false;
}

bool HasClassStructure(const clang::CXXRecordDecl *record) {
  if (!record || !record->isCompleteDefinition())
    return false;
  if (record->isDynamicClass() || record->getNumBases() != 0)
    return true;
  for (const auto *method : record->methods())
    if (!method->isImplicit() && method->getMostRecentDecl()->isDefaulted() &&
        !IsInClassDefaultedComparison(method))
      return true;
  return false;
}

bool IsSupportedDefaultedSpecialMember(const clang::CXXMethodDecl *method) {
  if (!method)
    return false;
  method = method->getMostRecentDecl();
  const auto effects =
      ReadCanonicalFunctionEffects(method->getType(), method->getASTContext());
  return !effects.is_async && !effects.is_throwing &&
         method->isExplicitlyDefaulted() &&
         (llvm::isa<clang::CXXConstructorDecl, clang::CXXDestructorDecl>(
              method) ||
          method->isCopyAssignmentOperator() ||
          method->isMoveAssignmentOperator());
}

bool IsOutOfLineDefaultedDefinition(const clang::NamedDecl *declaration) {
  const auto *method = llvm::dyn_cast<clang::CXXMethodDecl>(declaration);
  return method && method->isExplicitlyDefaulted() && method->isOutOfLine() &&
         IsSupportedDefaultedSpecialMember(method);
}

std::string RecordLayoutIdentity(const clang::RecordDecl *record,
                                const clang::ASTContext &context) {
  std::string text;
  llvm::raw_string_ostream out(text);
  out << "record{";
  if (llvm::isa<clang::CXXRecordDecl>(record) && record->isUnion())
    out << "union=1;";
  if (record->isCompleteDefinition()) {
    const auto &layout = context.getASTRecordLayout(record);
    out << "size=" << layout.getSize().getQuantity() << ";align="
        << layout.getAlignment().getQuantity() << ';';
    unsigned index = 0;
    for (const auto *field : record->fields()) {
      out << "field=" << (field->getIdentifier()
                                ? field->getNameAsString()
                                : std::string("<anonymous>"))
          << ':' << CanonicalType(field->getType(), context) << '@'
          << layout.getFieldOffset(index++);
      if (field->isBitField())
        out << ":bits=" << field->getBitWidthValue();
      out << ';';
    }
  } else {
    out << "incomplete;";
  }
  out << '}';
  if (const auto *cxx = llvm::dyn_cast<clang::CXXRecordDecl>(record);
      HasClassLifecycle(cxx)) {
    out << ";cxx-class-lifecycle:v1;trivial=" << cxx->isTrivial()
        << ";trivially-copyable=" << cxx->isTriviallyCopyable()
        << ";standard-layout=" << cxx->isStandardLayout()
        << ";register-passing=" << cxx->canPassInRegisters()
        << ";nontrivial-dtor-for-call="
        << cxx->hasNonTrivialDestructorForCall();
    for (const auto *field : cxx->fields())
      out << ";field-access=" << static_cast<unsigned>(field->getAccess())
          << ":mutable=" << field->isMutable();
  }
  if (const auto *cxx = llvm::dyn_cast<clang::CXXRecordDecl>(record);
      HasClassStructure(cxx)) {
    out << ";cxx-class-structure:v1;aggregate=" << cxx->isAggregate()
        << ";empty=" << cxx->isEmpty();
    const auto &layout = context.getASTRecordLayout(cxx);
    for (const auto &base : cxx->bases()) {
      const auto *owner = base.getType()->getAsCXXRecordDecl();
      out << ";base=" << CanonicalType(base.getType(), context) << ":access="
          << (base.getAccessSpecifier() == clang::AS_public      ? "public"
              : base.getAccessSpecifier() == clang::AS_protected ? "protected"
                                                                 : "private")
          << ":virtual=" << base.isVirtual() << ":offset="
          << (base.isVirtual() ? layout.getVBaseClassOffset(owner)
                               : layout.getBaseClassOffset(owner))
                 .getQuantity();
    }
  }
  if (const auto *cxx = llvm::dyn_cast<clang::CXXRecordDecl>(record);
      cxx && cxx->isCompleteDefinition() &&
      (cxx->isDynamicClass() || cxx->hasAttr<clang::FinalAttr>())) {
    const auto &layout = context.getASTRecordLayout(cxx);
    out << ";cxx-class-dispatch:v1;dynamic=" << cxx->isDynamicClass()
        << ";rtti=" << context.getLangOpts().RTTI
        << ";polymorphic=" << cxx->isPolymorphic()
        << ";abstract=" << cxx->isAbstract()
        << ";final=" << cxx->hasAttr<clang::FinalAttr>()
        << ";nv-size=" << layout.getNonVirtualSize().getQuantity()
        << ";nv-align=" << layout.getNonVirtualAlignment().getQuantity()
        << ";primary-virtual=" << layout.isPrimaryBaseVirtual();
    if (const auto *primary = layout.getPrimaryBase())
      out << ";primary=" << ReadInterfaceDeclarationStableId(primary);
    // Include every transitive virtual base in native layout order, not only
    // the direct bases. This binds diamond sharing and receiver adjustments.
    for (const auto &base : cxx->vbases())
      out << ";vbase=" << CanonicalType(base.getType(), context) << ":offset="
          << layout.getVBaseClassOffset(base.getType()->getAsCXXRecordDecl())
                 .getQuantity();
    // Declaration order determines new virtual slots. The sidecar's sorted
    // declaration table alone cannot distinguish reordered virtual functions.
    for (const auto *method : cxx->methods())
      if (!method->isImplicit() && method->isVirtual())
        out << ";virtual-slot=" << ReadInterfaceDeclarationStableId(method);
  }
  if (const auto *cxx = llvm::dyn_cast<clang::CXXRecordDecl>(record);
      cxx && cxx->isCompleteDefinition()) {
    std::set<std::string> nested;
    for (const auto *member : cxx->decls())
      if (!member->isImplicit())
        if (const auto *type = llvm::dyn_cast<clang::TypeDecl>(member))
          nested.insert("nested-type=" + ReadInterfaceDeclarationStableId(type) +
                        ":kind=" + type->getDeclKindName() +
                        ":access=" + CxxAccessName(type->getAccess()) + ";");
    // Import dependencies can materialize nested types before their owner.
    // Unlike fields and virtual slots, nested type declaration order is not ABI.
    if (!nested.empty()) out << ";cxx-nested-types:v1;";
    for (const auto &type : nested) out << type;
    std::set<std::string> using_members;
    for (const auto *member:cxx->decls())
      if(const auto *using_decl=llvm::dyn_cast<clang::BaseUsingDecl>(member))
        using_members.insert(ReadInterfaceDeclarationStableId(using_decl)+
                             ":access="+CxxAccessName(using_decl->getAccess()));
    if(!using_members.empty()) out << ";cxx-using:v1;";
    for(const auto &member:using_members) out << "using=" << member << ';';
  }
  if (const auto *cxx = llvm::dyn_cast<clang::CXXRecordDecl>(record);
      cxx && cxx->isCompleteDefinition()) {
    std::set<std::string> expressions;
    for (const auto *field : cxx->fields())
      if (field->hasInClassInitializer())
        expressions.insert("field=" + ReadInterfaceDeclarationStableId(field) + ":" +
            DeclarationExpressionContract(field->getInClassInitializer(), context));
    for (const auto *method : cxx->methods())
      if (!method->isImplicit() && !TemplatePublicationOwner(method)) {
        unsigned index = 0;
        for (const auto *parameter : method->getMostRecentDecl()->parameters()) {
          if (parameter->hasDefaultArg() && !parameter->hasUninstantiatedDefaultArg())
            expressions.insert("method=" + ReadInterfaceDeclarationStableId(method) + ":" +
                std::to_string(index) + ":" +
                DeclarationExpressionContract(parameter->getDefaultArg(), context));
          ++index;
        }
      }
    if (!expressions.empty()) out << ";cxx-declaration-expressions:v1;";
    for (const auto &expression : expressions) out << expression << ';';
    std::set<std::string> definitions;
    for (const auto *method : cxx->methods())
      if (!method->isImplicit() && !TemplatePublicationOwner(method) && PublishesFunctionBody(method))
        definitions.insert(ReadInterfaceDeclarationStableId(method) + ":" +
                           FunctionDefinitionContract(method, context));
    for (const auto *member : cxx->decls())
      if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(member);
          variable && variable->isStaticDataMember() && !TemplatePublicationOwner(variable))
        definitions.insert("static=" + ReadInterfaceDeclarationStableId(variable) +
            ":type=" + CanonicalType(variable->getType(), context) +
            ":inline=" + std::to_string(variable->isInline()) +
            ":constexpr=" + std::to_string(variable->isConstexpr()) +
            ":thread-storage=" + std::to_string(variable->getTSCSpec()) +
            ":access=" + CxxAccessName(variable->getAccess()) +
            ":init=" + (PublishesVariableInitializer(variable) && variable->hasInit()
                ? DeclarationExpressionContract(variable->getInit(), context) : "none"));
    if (!definitions.empty()) out << ";cxx-definitions:v1;";
    for (const auto &definition : definitions) out << "definition=" << definition << ';';
  }
  if (const auto *cxx = llvm::dyn_cast<clang::CXXRecordDecl>(record);
      cxx && cxx->isCompleteDefinition()) {
    std::set<std::string> friends;
    for (const auto *member : cxx->decls())
      if (const auto *friend_decl = llvm::dyn_cast<clang::FriendDecl>(member)) {
        if (const auto *type = friend_decl->getFriendType()) {
          friends.insert("type=" + CanonicalType(type->getType(), context));
        } else if (const auto *function =
                       llvm::dyn_cast_or_null<clang::FunctionDecl>(friend_decl->getFriendDecl());
                   function && !IsInClassDefaultedComparison(function)) {
          function = function->getMostRecentDecl();
          friends.insert("function=" + ReadInterfaceDeclarationStableId(function) +
              ":type=" + CanonicalType(function->getType(), context) +
              ":inline=" + std::to_string(function->isInlined()) +
              ":constexpr=" + std::to_string(function->isConstexpr()) +
              ":consteval=" + std::to_string(function->isConsteval()) +
              ":deleted=" + std::to_string(function->isDeleted()) +
              ":lookup=" + std::to_string(function->getIdentifierNamespace()) +
              ":body=" + (PublishesFunctionBody(function)
                  ? FunctionDefinitionContract(function, context) : "declaration"));
        }
      }
    if (!friends.empty()) out << ";cxx-friends:v1;";
    for (const auto &friend_identity : friends) out << "friend=" << friend_identity << ';';
  }
  if (const auto *cxx = llvm::dyn_cast<clang::CXXRecordDecl>(record);
      cxx && cxx->isCompleteDefinition()) {
    std::set<std::string> comparisons;
    auto add = [&](const clang::FunctionDecl *function) {
      if (IsInClassDefaultedComparison(function))
        comparisons.insert(ReadInterfaceDeclarationStableId(function) +
            ":operator=" + std::to_string(function->getOverloadedOperator()) +
            ":consteval=" + std::to_string(function->isConsteval()));
    };
    for (const auto *member : cxx->decls()) {
      if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(member)) add(function);
      if (const auto *friend_decl = llvm::dyn_cast<clang::FriendDecl>(member))
        add(llvm::dyn_cast_or_null<clang::FunctionDecl>(friend_decl->getFriendDecl()));
    }
    if (!comparisons.empty()) out << ";cxx-defaulted-comparisons:v1;";
    for (const auto &comparison : comparisons) out << "comparison=" << comparison << ';';
  }
  if (const auto *cxx = llvm::dyn_cast<clang::CXXRecordDecl>(record);
      cxx && cxx->isCompleteDefinition()) {
    std::set<std::string> defaulted;
    for (const auto *method : cxx->methods())
      if (!method->isImplicit() && IsOutOfLineDefaultedDefinition(method->getMostRecentDecl()))
        defaulted.insert(ReadInterfaceDeclarationStableId(method));
    if (!defaulted.empty()) out << ";cxx-out-of-line-defaulting:v1;";
    for (const auto &member : defaulted) out << "defaulted=" << member << ';';
  }
  if (const auto *cxx = llvm::dyn_cast<clang::CXXRecordDecl>(record);
      cxx && cxx->isCompleteDefinition()) {
    std::set<std::string> immediate;
    auto add = [&](const clang::FunctionDecl *function) {
      if (function && function->getMostRecentDecl()->isExplicitlyDefaulted() && function->isConsteval())
        immediate.insert(ReadInterfaceDeclarationStableId(function));
    };
    for (const auto *member : cxx->decls()) {
      add(llvm::dyn_cast<clang::FunctionDecl>(member));
      if (const auto *friend_decl = llvm::dyn_cast<clang::FriendDecl>(member))
        add(llvm::dyn_cast_or_null<clang::FunctionDecl>(friend_decl->getFriendDecl()));
    }
    if (!immediate.empty()) out << ";cxx-immediate-defaulting:v1;";
    for (const auto &member : immediate) out << "immediate=" << member << ';';
  }
  out.flush();
  return text;
}

std::string EnumIdentity(const clang::EnumDecl *enumeration,
                         const clang::ASTContext &context) {
  std::string text = "enum{" + CanonicalType(
      context.getCanonicalTagType(enumeration), context);
  text += std::string(";scoped=") +
          (enumeration->isScoped() ? "true" : "false") +
          ";fixed=" + (enumeration->isFixed() ? "true" : "false");
  // Integer representation participates in enum calling/layout ABI even when
  // the enumerator spellings and values are unchanged (for example short enums).
  if (!enumeration->getIntegerType().isNull())
    text += ";underlying=" + CanonicalType(enumeration->getIntegerType(), context);
  for (const auto *constant : enumeration->enumerators()) {
    llvm::SmallString<32> value;
    constant->getInitVal().toString(value, 10);
    text += ";" + constant->getNameAsString() + "=" + value.str().str();
  }
  return text + '}';
}

std::string ObjCMethodIdentity(const clang::ObjCMethodDecl *method,
                               const clang::ASTContext &context) {
  std::string text;
  llvm::raw_string_ostream out(text);
  out << (method->isInstanceMethod() ? "instance" : "class") << ':'
      << method->getSelector().getAsString() << "->"
      << CanonicalType(method->getReturnType(), context) << '(';
  for (unsigned i = 0; i < method->param_size(); ++i) {
    if (i) out << ',';
    out << CanonicalType(method->getParamDecl(i)->getType(), context);
    const auto lifetime = method->getParamDecl(i)->getType().getObjCLifetime();
    if (lifetime != clang::Qualifiers::OCL_None)
      out << "#lifetime=" << static_cast<unsigned>(lifetime);
  }
  out << ")";
  if (method->isVariadic()) out << ";variadic";
  if (method->isOptional()) out << ";optional";
  if (method->isDirectMethod()) out << ";dispatch=direct";
  if (!method->getObjC3FunctionType().isNull())
    out << ";callable=" << CanonicalType(method->getObjC3FunctionType(), context);
  const auto return_lifetime = method->getReturnType().getObjCLifetime();
  if (return_lifetime != clang::Qualifiers::OCL_None)
    out << ";return-lifetime=" << static_cast<unsigned>(return_lifetime);
  out.flush();
  return text;
}

std::string ObjCContainerIdentity(const clang::NamedDecl *declaration,
                                  const clang::ASTContext &context) {
  if (const auto *interface = llvm::dyn_cast<clang::ObjCInterfaceDecl>(declaration)) {
    std::string text = "objc-interface:" + interface->getNameAsString();
    if (const auto *superclass = interface->getSuperClass())
      text += ";super=" + superclass->getNameAsString();
    if (interface->hasAttr<clang::ObjCRootClassAttr>()) text += ";root-class";
    for (const auto *protocol : interface->protocols())
      text += ";protocol=" + protocol->getNameAsString();
    for (const auto *ivar : interface->ivars()) {
      text += ";ivar=" + ivar->getNameAsString() + ':' +
              CanonicalType(ivar->getType(), context) + "@" +
              std::to_string(static_cast<unsigned>(ivar->getAccessControl()));
      text += ivar->isBitField() ? ":bitfield=" + std::to_string(ivar->getBitWidthValue()) : ":ordinary";
    }
    if (const auto *parameters = interface->getTypeParamList()) {
      text += ";objc-generics:v1;count=" + std::to_string(parameters->size());
      for (const auto *parameter : *parameters) {
        text += ";parameter=" + std::to_string(parameter->getIndex()) + ":variance=" +
                std::to_string(static_cast<unsigned>(parameter->getVariance())) + ":bound=" +
                CanonicalType(parameter->getUnderlyingType(), context);
      }
    }
    if (const auto *superclass = interface->getSuperClassType())
      text += ";super-type=" + CanonicalType(clang::QualType(superclass, 0), context);
    return text;
  }
  if (const auto *protocol = llvm::dyn_cast<clang::ObjCProtocolDecl>(declaration)) {
    std::string text = "objc-protocol:" + protocol->getNameAsString();
    for (const auto *adopted : protocol->protocols())
      text += ";protocol=" + adopted->getNameAsString();
    return text;
  }
  if (const auto *category = llvm::dyn_cast<clang::ObjCCategoryDecl>(declaration)) {
    std::string text = "objc-category:" +
           (category->getClassInterface()
                ? category->getClassInterface()->getNameAsString()
                : std::string("<missing-class>")) +
           '(' + category->getNameAsString() + ')';
    for (const auto *protocol : category->protocols())
      text += ";protocol=" + protocol->getNameAsString();
    return text;
  }

  return {};
}

}  // namespace emission_detail

}  // namespace objc3::clang_foundation
