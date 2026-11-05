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

class TypeDependencyVisitor final
    : public clang::RecursiveASTVisitor<TypeDependencyVisitor> {
 public:
   TypeDependencyVisitor(const clang::ASTContext &context,
                         InterfaceEmissionArtifact &result,
                         const std::set<std::string> &resolved_ids,
                         const std::vector<std::string> &exports,
                         std::string spi_group, const std::string &source_root)
       : context_(context), result_(result), resolved_ids_(resolved_ids),
         exports_(exports), spi_group_(std::move(spi_group)),
         source_root_(source_root) {}

   void CheckDeclarationAttributes(const clang::NamedDecl *declaration) {
     if (!declaration) return;
     for (const auto *attribute:declaration->attrs())
       if (llvm::isa<clang::AlignedAttr,clang::AssumeAlignedAttr,clang::RestrictAttr>(attribute))
         TraverseAttr(const_cast<clang::Attr *>(attribute));
       else if(llvm::isa<clang::DiagnoseIfAttr,clang::EnableIfAttr>(attribute)) {
         const auto *diagnostic=llvm::dyn_cast<clang::DiagnoseIfAttr>(attribute);
         const auto *enabled=llvm::dyn_cast<clang::EnableIfAttr>(attribute);
         const auto locals=lambda_locals_;
         auto restore=llvm::scope_exit([&]{lambda_locals_=locals;});
         if(const auto *function=llvm::dyn_cast<clang::FunctionDecl>(declaration))
           for(const auto *redecl:function->redecls())
             for(const auto *parameter:redecl->parameters())lambda_locals_.insert(parameter->getCanonicalDecl());
         if(const auto *method=llvm::dyn_cast<clang::ObjCMethodDecl>(declaration))
           for(const auto *parameter:method->parameters())lambda_locals_.insert(parameter->getCanonicalDecl());
         TraverseStmt(diagnostic?diagnostic->getCond():enabled->getCond());
       }
     if (const auto *pattern=llvm::dyn_cast<clang::TemplateDecl>(declaration))
       CheckDeclarationAttributes(pattern->getTemplatedDecl());
   }

   void AllowPublishedFactoryResult(const clang::FunctionDecl *function) {
     for (const auto *tag:PublishedFactoryResults(function)) {
       factory_results_.insert(tag->getCanonicalDecl());
       for (const auto *scope=tag->getDeclContext();scope && !scope->isFunctionOrMethod();scope=scope->getParent())
         if (const auto *parent=llvm::dyn_cast<clang::TagDecl>(scope))
           factory_results_.insert(parent->getCanonicalDecl());
     }
   }

   void CheckVisibility(const clang::NamedDecl *declaration) {
     if (declaration) {
       const auto group = ReadInterfaceSPIGroup(declaration);
       if (!group.empty() &&
           (group != spi_group_ ||
            !IsInMainFile(context_, declaration->getLocation())))
         AddDiagnostic(
             result_, "O3IFC158",
             "exported signature references SPI outside its local group",
             declaration->getQualifiedNameAsString());
     }
     if (declaration && IsInMainFile(context_, declaration->getLocation()) &&
         !SelectedExport(declaration, exports_))
       AddDiagnostic(
           result_, "O3IFC156",
           "public API references a declaration excluded by its export policy",
           declaration->getQualifiedNameAsString());
   }

   bool IsResolved(const clang::NamedDecl *declaration) {
     if (IsStandardCoroutineDeclaration(declaration,context_)) return true;
     if (IsQualifiedRuntimeFrameType(declaration, context_)) {
       UsesRuntimeFrame = true;
       return true;
     }
     if (IsQualifiedRuntimeServiceDeclaration(declaration,context_)) {
       UsesRuntimeFrame=true;
       return true;
     }
     if (IsQualifiedRuntimeTaskDeclaration(declaration,context_)) {
       UsesCxxTaskRuntime=true;
       UsesRuntimeFrame=true;
       return true;
     }
     const auto stable_id = ReadInterfaceDeclarationStableId(declaration);
     return !stable_id.empty() && resolved_ids_.count(stable_id);
   }

  bool TraverseRecordType(clang::RecordType *type, bool traverse_qualifier) {
    clang::ObjC3KeyPathInfo keypath;
    if (clang::getObjC3KeyPathInfo(clang::QualType(type, 0), keypath))
      return TraverseType(keypath.Root) && TraverseType(keypath.Value);
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::TraverseRecordType(type, traverse_qualifier);
  }

  void CheckExpressionReference(const clang::NamedDecl *target) {
    if (!target) return;
    target = target->getUnderlyingDecl();
    if (IsStandardCoroutineDeclaration(target,context_)) return;
    if (IsStandardCoroutineTraitsSpecialization(target)) {
      CheckVisibility(target);
      if (!IsInMainFile(context_,target->getLocation()) && !IsResolved(target))
        AddDiagnostic(result_,"O3IFC112","coroutine traits requires its resolved public specialization",target->getQualifiedNameAsString());
      return;
    }
    // C++ default-initialization of the ABI's va_list array can reference its
    // implicit record constructor. It belongs to the same compiler-owned type.
    if (const auto *method=llvm::dyn_cast<clang::CXXMethodDecl>(target);
        method && method->isImplicit())
      if (const auto *builtin=context_.getVaListTagDecl();
          builtin && method->getParent()->getCanonicalDecl()==builtin->getCanonicalDecl()) return;
    // Inherited defaults can retain the prototype's parameter declarations.
    // Template parameters are lexical bindings even when their DeclContext
    // is a namespace rather than the definition currently being checked.
    if (llvm::isa<clang::TemplateTypeParmDecl, clang::NonTypeTemplateParmDecl,
                  clang::TemplateTemplateParmDecl>(target)) return;
    if (const auto *specialization = llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(target)) {
      if (IsExplicitNamespaceVariableSpecialization(specialization)) {
        CheckVisibility(specialization);
        if (!IsInMainFile(context_,specialization->getLocation()) && !IsResolved(specialization))
          AddDiagnostic(result_,"O3IFC112","variable specialization requires its resolved public declaration",
                        specialization->getQualifiedNameAsString());
      }
      CheckExpressionReference(specialization->getSpecializedTemplate());
      for (const auto &argument : specialization->getTemplateArgs().asArray())
        if (!CheckGenericArgument(argument)) UnsupportedExpression();
      return;
    }
    if (const auto *pattern = TemplatePublicationOwner(target);
        pattern && pattern!=target &&
        (IsInMainFile(context_,pattern->getLocation()) ||
         resolved_ids_.count(ReadInterfaceDeclarationStableId(pattern)))) {
      CheckExpressionReference(pattern);
      CheckEnclosingClassArguments(target);
      if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(target))
        if (const auto *arguments = function->getTemplateSpecializationArgs())
          for (const auto &argument : arguments->asArray())
            if (!CheckGenericArgument(argument)) UnsupportedExpression();
      return;
    }
    if (const auto *pattern = llvm::dyn_cast<clang::ClassTemplateDecl>(target)) {
      CheckVisibility(pattern);
      if (!IsTopLevel(pattern) ||
          (!IsInMainFile(context_,pattern->getLocation()) && !IsResolved(pattern)))
        AddDiagnostic(result_, "O3IFC112", "class template requires a public resolved definition",
                      pattern->getQualifiedNameAsString());
      return;
    }
    if (const auto *pattern = llvm::dyn_cast<clang::VarTemplateDecl>(target)) {
      CheckVisibility(pattern);
      if (!IsTopLevel(pattern) || pattern->getTemplatedDecl()->getStorageClass()==clang::SC_Static ||
          (!IsInMainFile(context_,pattern->getLocation()) && !IsResolved(pattern)))
        AddDiagnostic(result_,"O3IFC112","variable template requires a public resolved definition",
                      pattern->getQualifiedNameAsString());
      return;
    }
    if (llvm::isa<clang::TypeAliasTemplateDecl>(target)) {
      CheckVisibility(target);
      if (!IsTopLevel(target) ||
          (!IsInMainFile(context_, target->getLocation()) && !IsResolved(target)))
        AddDiagnostic(result_, "O3IFC112", "alias template requires a public resolved definition",
                      target->getQualifiedNameAsString());
      return;
    }

    if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(target))
      if (const auto *pattern = function->getPrimaryTemplate()) {
        if (IsExplicitNamespaceFunctionSpecialization(function)) {
          CheckVisibility(function);
          if (!IsInMainFile(context_,function->getLocation()) && !IsResolved(function))
            AddDiagnostic(result_,"O3IFC112","function specialization requires its resolved public declaration",
                          function->getQualifiedNameAsString());
        }
        CheckExpressionReference(pattern);
        if (const auto *arguments = function->getTemplateSpecializationArgs())
          for (const auto &argument : arguments->asArray())
            if (!CheckGenericArgument(argument)) UnsupportedExpression();
        return;
      }
    if (llvm::isa<clang::ConceptDecl>(target)) {
      CheckVisibility(target);
      if (!IsTopLevel(target) ||
          (!IsInMainFile(context_, target->getLocation()) && !IsResolved(target)))
        AddDiagnostic(result_, "O3IFC112", "concept reference requires a public resolved definition",
                      target->getQualifiedNameAsString());
      return;
    }
    if (const auto *parameter = llvm::dyn_cast<clang::ParmVarDecl>(target))
      if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(parameter->getDeclContext());
          function && (function->getDescribedFunctionTemplate() || function->getPrimaryTemplate()))
        return;
    if (const auto *parameter = llvm::dyn_cast<clang::ParmVarDecl>(target))
      if (const auto *method = llvm::dyn_cast<clang::CXXMethodDecl>(parameter->getDeclContext());
          method && method->getParent()->isLambda()) return;
    if (const auto *object = llvm::dyn_cast<clang::TemplateParamObjectDecl>(target)) {
      TraverseType(object->getType());
      unsigned budget = 4096;
      std::string key;
      std::vector<const clang::NamedDecl *> references;
      if (!StructuralArgumentKey(object->getValue(),context_,key,budget,0,&references))
        AddDiagnostic(result_, "O3IFC101", "unsupported structural template object value");
      for(const auto *reference:references)CheckExpressionReference(reference);
      return;
    }
    // Reserved compiler builtins are supplied by the pinned consumer compiler,
    // not an exported declaration of the publishing package. Ordinary libc
    // declarations retain the normal visibility and dependency checks.
    if (const auto *builtin = llvm::dyn_cast<clang::FunctionDecl>(target);
        builtin && builtin->getBuiltinID() &&
        builtin->getName().starts_with("__builtin_") &&
        builtin->getDeclContext()->getRedeclContext()->isTranslationUnit() && !builtin->hasBody())
      return;
    if (const auto *method = llvm::dyn_cast<clang::CXXMethodDecl>(target);
        method && method->isImplicit()) {
      clang::ObjC3KeyPathInfo info;
      if (clang::getObjC3KeyPathInfo(context_.getCanonicalTagType(method->getParent()), info))
        return;
    }
    if (const auto *owner = llvm::dyn_cast<clang::CXXRecordDecl>(target->getDeclContext());
        owner && owner->isLambda()) return;
    if (in_expression_ && IsLocalExpressionContext(target->getDeclContext())) return;
    if (lambda_locals_.count(target->getCanonicalDecl())) return;
    if (lambda_contexts_.count(
            clang::Decl::castFromDeclContext(target->getDeclContext())->getCanonicalDecl())) return;
    if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(target->getDeclContext());
        function && body_function_ && function->getCanonicalDecl() == body_function_->getCanonicalDecl())
      return;
    CheckVisibility(target);
    if (target->getDeclContext()->isFunctionOrMethod() ||
        (!target->isExternallyVisible() && IsTopLevel(target)) ||
        (!IsInMainFile(context_, target->getLocation()) && !IsResolved(target)))
      AddDiagnostic(result_, "O3IFC112",
                    "declaration expression references a non-public or unresolved dependency",
                    target->getQualifiedNameAsString());
  }
  void CheckCxxCoroutine(const clang::FunctionDecl *function) {
    if (const auto *body=PublishedCxxCoroutine(function)) {
      if (!body->hasDependentPromiseType()) TraverseType(context_.getCanonicalType(body->getPromiseDecl()->getType()));
      for (const auto *reference:CxxCoroutineReferences(function)) CheckExpressionReference(reference);
    }
  }
  void CheckBody(const clang::FunctionDecl *function) {
    const auto *definition = function->getDefinition();
    if (!definition) return;
    if (IsDetachedCallableTemplateDefinition(definition) && !IsInMainFile(context_,definition->getLocation())) {
      AddDiagnostic(result_,"O3IFC112","detached template body requires a module-owned definition",
                    definition->getQualifiedNameAsString());
      return;
    }
    body_function_ = definition;
    CheckDeclarationAttributes(definition);
    CheckCxxCoroutine(definition);
    const auto *type=clang::getObjC3CallableFunctionType(definition);
    UsesCxxTaskRuntime |= type && type->getObjC3FunctionEffects().Async &&
                         !type->getObjC3FunctionEffects().AsyncFrame;
    if (const auto *constructor = llvm::dyn_cast<clang::CXXConstructorDecl>(definition))
      for (const auto *initializer : constructor->inits())
        if (initializer->isWritten()) TraverseStmt(initializer->getInit());
    TraverseStmt(const_cast<clang::Stmt *>(PublishedFunctionBody(definition)));
    body_function_ = nullptr;
  }
  bool TraverseTemplateName(clang::TemplateName name) {
    if (auto *declaration = name.getAsTemplateDecl()) {
      if (llvm::isa<clang::TypeAliasTemplateDecl>(declaration))
        CheckExpressionReference(declaration);
      else if (llvm::isa<clang::ClassTemplateDecl>(declaration) &&
               (IsInMainFile(context_,declaration->getLocation()) ||
                resolved_ids_.count(ReadInterfaceDeclarationStableId(declaration)) ||
                !ReadNativeSymbolName(declaration).empty()))
        CheckExpressionReference(declaration);
    }
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::TraverseTemplateName(name);
  }
  void CheckAliasTemplate(const clang::TypeAliasTemplateDecl *pattern) {
    const auto locals = lambda_locals_;
    const bool previous = public_template_type_;
    public_template_type_=true;
    auto restore = llvm::scope_exit([&] { lambda_locals_ = locals;public_template_type_=previous; });
    for (const auto *parameter : *pattern->getTemplateParameters())
      lambda_locals_.insert(parameter->getCanonicalDecl());
    for (auto *parameter : *pattern->getTemplateParameters()) TraverseDecl(parameter);
    TraverseStmt(pattern->getTemplateParameters()->getRequiresClause());
    TraverseType(pattern->getTemplatedDecl()->getUnderlyingType());
  }
  void CheckClassTemplate(const clang::ClassTemplateDecl *pattern) {
    const auto locals = lambda_locals_;
    const bool previous = public_template_type_;
    public_template_type_ = true;
    auto restore = llvm::scope_exit([&] {lambda_locals_=locals;public_template_type_=previous;});
    struct Bindings : clang::DynamicRecursiveASTVisitor {
      std::set<const clang::Decl *> &locals;
      explicit Bindings(std::set<const clang::Decl *> &locals) : locals(locals) {}
      bool VisitNamedDecl(clang::NamedDecl *decl) override {locals.insert(decl->getCanonicalDecl());return true;}
      bool TraverseStmt(clang::Stmt *) override {return true;}
    } bindings(lambda_locals_);
    bindings.TraverseDecl(const_cast<clang::ClassTemplateDecl *>(pattern));
    for (auto *parameter : *pattern->getTemplateParameters()) TraverseDecl(parameter);
    TraverseStmt(pattern->getTemplateParameters()->getRequiresClause());
    CheckTemplateRecord(pattern->getTemplatedDecl());
    for (const auto *specialization:PublishedClassSpecializations(pattern)) {
      bindings.TraverseDecl(const_cast<clang::ClassTemplateSpecializationDecl *>(specialization));
      const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(specialization);
      if (partial) {
        for (auto *parameter:*partial->getTemplateParameters()) TraverseDecl(parameter);
        TraverseStmt(partial->getTemplateParameters()->getRequiresClause());
      }
      for (const auto &argument:specialization->getTemplateArgs().asArray()) {
        if (!(partial?CheckPatternArgument(argument):CheckGenericArgument(argument))) UnsupportedExpression();
      }
      CheckTemplateRecord(specialization);
    }
  }
  void CheckCoroutineTraits(const clang::ClassTemplateSpecializationDecl *record) {
    const auto locals=lambda_locals_;const bool previous=public_template_type_;
    auto restore=llvm::scope_exit([&]{lambda_locals_=locals;public_template_type_=previous;});
    public_template_type_=true;
    const auto *owner=CoroutineTraitsReturnOwner(record);
    if (!owner || (!IsInMainFile(context_,owner->getLocation()) && !IsResolved(owner))) {
      AddDiagnostic(result_,"O3IFC112","coroutine traits return type requires a public module owner",record->getQualifiedNameAsString());return;
    }
    CheckExpressionReference(owner);
    if (const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(record)) {
      for (auto *parameter:*partial->getTemplateParameters()) lambda_locals_.insert(parameter->getCanonicalDecl());
      for (auto *parameter:*partial->getTemplateParameters()) TraverseDecl(parameter);
      TraverseStmt(partial->getTemplateParameters()->getRequiresClause());
    }
    for (const auto &argument:record->getTemplateArgs().asArray())
      if (!CheckPatternArgument(argument)) UnsupportedExpression();
    CheckTemplateRecord(record);
  }
  void CheckTemplateRecord(const clang::CXXRecordDecl *record) {
    if (const auto *definition=record->getDefinition()) record=definition;
    // A materialized member primary may still be a forward declaration when
    // only one of its partials has been defined. Its origin and each published
    // partial are checked separately; base/layout access requires a definition.
    if (!record->isCompleteDefinition()) return;
    CheckDeclarationAttributes(record);
    if (IsDetachedNestedTemplateDefinition(record) && !IsInMainFile(context_,record->getLocation())) {
      AddDiagnostic(result_,"O3IFC112","detached nested template requires a module-owned definition",
                    record->getQualifiedNameAsString());return;
    }
    for (const auto &base : record->bases()) TraverseType(base.getType());
    for (auto *member : record->decls()) {
      if (member->isImplicit()) continue;
      CheckDeclarationAttributes(llvm::dyn_cast<clang::NamedDecl>(member));
      if (const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(member);
          specialization && specialization->getSpecializationKind()==clang::TSK_ImplicitInstantiation) continue;
      if (const auto *function=llvm::dyn_cast<clang::FunctionTemplateDecl>(member)) CheckFunctionTemplate(function);
      else if (const auto *alias=llvm::dyn_cast<clang::TypeAliasTemplateDecl>(member)) CheckAliasTemplate(alias);
      else if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(member)) CheckClassTemplate(pattern);
      else if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(member)) CheckVariableTemplate(pattern);
      else if (const auto *inner=llvm::dyn_cast<clang::CXXRecordDecl>(member)) CheckTemplateRecord(inner);
      else if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(member)) {
        const auto previous_results=factory_results_;
        auto restore_results=llvm::scope_exit([&]{factory_results_=previous_results;});
        AllowPublishedFactoryResult(function);
        TraverseType(function->getType());
        TraverseStmt(const_cast<clang::Expr *>(function->getTrailingRequiresClause().ConstraintExpr));
        for (const auto *parameter : function->parameters())
          if (const auto *value=WrittenDefaultArgument(parameter)) TraverseStmt(const_cast<clang::Expr *>(value));
        if (!IsConcreteMemberFunctionSpecialization(function) || function->isInlined()) CheckBody(function);
      } else if (const auto *variable=llvm::dyn_cast<clang::VarDecl>(member);
                 variable && IsDetachedStaticDataTemplateDefinition(variable->getDefinition()))
        CheckDetachedStaticData(variable->getDefinition());
      else TraverseDecl(member);
    }
    for (const auto *specialization:ConcreteMemberFunctionSpecializations(record))
      CheckConcreteMemberFunctionSpecialization(specialization);
    for (const auto *specialization:ConcreteMemberVariableSpecializations(record))
      if (const auto *partial=llvm::dyn_cast<clang::VarTemplatePartialSpecializationDecl>(specialization))
        CheckVariablePartial(partial);
      else {
        for (const auto &argument:specialization->getTemplateArgs().asArray())
          if (!CheckGenericArgument(argument)) UnsupportedExpression();
        TraverseType(specialization->getType());
        if (PublishesVariableInitializer(specialization) && specialization->hasInit())
          TraverseStmt(const_cast<clang::Expr *>(specialization->getInit()));
      }
  }
  void CheckConcreteMemberFunctionSpecialization(const clang::FunctionDecl *function) {
    const auto previous_results=factory_results_;
    auto restore_results=llvm::scope_exit([&]{factory_results_=previous_results;});
    AllowPublishedFactoryResult(function);
    CheckExpressionReference(function->getPrimaryTemplate());
    for (const auto &argument:function->getTemplateSpecializationArgs()->asArray())
      if (!CheckGenericArgument(argument)) UnsupportedExpression();
    TraverseType(function->getType());
    if (function->isInlined()) CheckBody(function);
  }
  void CheckDetachedStaticData(const clang::VarDecl *definition) {
    if (!IsInMainFile(context_,definition->getLocation())) {
      AddDiagnostic(result_,"O3IFC112","detached static template data requires a module-owned definition",
                    definition->getQualifiedNameAsString());return;
    }
    const auto locals=lambda_locals_;
    const bool previous=public_template_type_;
    auto restore=llvm::scope_exit([&]{lambda_locals_=locals;public_template_type_=previous;});
    public_template_type_=true;
    for (const auto *head:definition->getTemplateParameterLists())
      for (auto *parameter:*head) {lambda_locals_.insert(parameter->getCanonicalDecl());TraverseDecl(parameter);}
    TraverseType(definition->getType());public_template_type_=previous;
    if (definition->hasInit()) TraverseStmt(const_cast<clang::Expr *>(definition->getInit()));
  }
  void CheckVariableTemplate(const clang::VarTemplateDecl *pattern) {
    const auto locals=lambda_locals_;
    const bool previous=public_template_type_;public_template_type_=true;
    auto restore=llvm::scope_exit([&]{lambda_locals_=locals;public_template_type_=previous;});
    for (const auto *parameter:*pattern->getTemplateParameters()) lambda_locals_.insert(parameter->getCanonicalDecl());
    for (auto *parameter:*pattern->getTemplateParameters()) TraverseDecl(parameter);
    TraverseStmt(pattern->getTemplateParameters()->getRequiresClause());
    const auto *variable=pattern->getTemplatedDecl();
    TraverseType(variable->getType());
    public_template_type_=previous;
    if (IsDetachedStaticDataTemplateDefinition(variable->getDefinition()))
      CheckDetachedStaticData(variable->getDefinition());
    else if(variable->hasInit()) TraverseStmt(const_cast<clang::Expr *>(variable->getInit()));
    for (const auto *partial:PublishedVariablePartials(pattern)) CheckVariablePartial(partial);
  }
  void CheckVariablePartial(const clang::VarTemplatePartialSpecializationDecl *partial) {
    const auto locals=lambda_locals_;const bool previous=public_template_type_;
    auto restore=llvm::scope_exit([&]{lambda_locals_=locals;public_template_type_=previous;});
    public_template_type_=true;
    for (const auto *parameter:*partial->getTemplateParameters()) lambda_locals_.insert(parameter->getCanonicalDecl());
    for (auto *parameter:*partial->getTemplateParameters()) TraverseDecl(parameter);
    TraverseStmt(partial->getTemplateParameters()->getRequiresClause());
    for (const auto &argument:partial->getTemplateArgs().asArray())
      if (!CheckPatternArgument(argument)) UnsupportedExpression();
    TraverseType(partial->getType());public_template_type_=previous;
    if(partial->hasInit()) TraverseStmt(const_cast<clang::Expr *>(partial->getInit()));
  }
  void CheckFunctionTemplate(const clang::FunctionTemplateDecl *pattern) {
    const auto previous_results=factory_results_;
    auto restore_results=llvm::scope_exit([&]{factory_results_=previous_results;});
    AllowPublishedFactoryResult(pattern->getTemplatedDecl());
    const auto locals = lambda_locals_;
    const bool previous = public_template_type_;
    public_template_type_=true;
    auto restore = llvm::scope_exit([&] { lambda_locals_ = locals;public_template_type_=previous; });
    for (const auto *parameter : *pattern->getTemplateParameters())
      lambda_locals_.insert(parameter->getCanonicalDecl());
    for (auto *parameter : *pattern->getTemplateParameters()) TraverseDecl(parameter);
    const auto *function = pattern->getTemplatedDecl();
    for (const auto *parameter : function->parameters())
      lambda_locals_.insert(parameter->getCanonicalDecl());
    TraverseType(function->getType());
    TraverseStmt(pattern->getTemplateParameters()->getRequiresClause());
    TraverseStmt(const_cast<clang::Expr *>(function->getTrailingRequiresClause().ConstraintExpr));
    for (const auto *parameter : function->parameters())
      TraverseStmt(const_cast<clang::Expr *>(WrittenDefaultArgument(parameter)));
    if (const auto *definition = function->getDefinition()) {
      if (const auto *definition_pattern = definition->getDescribedFunctionTemplate())
        for (const auto *parameter : *definition_pattern->getTemplateParameters())
          lambda_locals_.insert(parameter->getCanonicalDecl());
      for (const auto *parameter : definition->parameters())
        lambda_locals_.insert(parameter->getCanonicalDecl());
    }
    public_template_type_=previous;
    CheckBody(function);
  }
  void CheckConcept(const clang::ConceptDecl *declaration) {
    const auto locals = lambda_locals_;
    auto restore = llvm::scope_exit([&] { lambda_locals_ = locals; });
    for (const auto *parameter : *declaration->getTemplateParameters())
      lambda_locals_.insert(parameter->getCanonicalDecl());
    for (auto *parameter : *declaration->getTemplateParameters())
      TraverseDecl(parameter);
    TraverseStmt(declaration->getTemplateParameters()->getRequiresClause());
    TraverseStmt(const_cast<clang::Expr *>(declaration->getConstraintExpr()));
  }
  bool VisitVarDecl(clang::VarDecl *variable) {
    if (in_expression_) TraverseType(variable->getType());
    if (body_function_)
      for (const auto *attribute : variable->attrs())
        if (!attribute->isImplicit() && !llvm::isa<clang::BlocksAttr>(attribute))
          UnsupportedExpression("local variable attribute has no publication contract");
    return true;
  }
  bool VisitTagDecl(clang::TagDecl *declaration) {
    if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
        record && record->isLambda()) return true;
    if (IsExpressionLocalTag(declaration) &&
        (!in_expression_ ||
         !IsBoundedCxxDeclaration(declaration, result_, exports_)))
      return UnsupportedExpression();
    return true;
  }
  bool IsLocalExpressionContext(const clang::DeclContext *scope) const {
    for (; scope; scope = scope->getParent()) {
      if (const auto *tag = llvm::dyn_cast<clang::TagDecl>(scope);
          tag && IsExpressionLocalTag(tag)) return true;
      if (body_function_ &&
          clang::Decl::castFromDeclContext(scope)->getCanonicalDecl() ==
              body_function_->getCanonicalDecl()) return true;
      if (lambda_contexts_.count(clang::Decl::castFromDeclContext(scope)->getCanonicalDecl()))
        return true;
    }
    return false;
  }
  bool VisitFieldDecl(clang::FieldDecl *declaration) {
    if (in_expression_) TraverseType(declaration->getType());
    return true;
  }
  bool VisitFunctionDecl(clang::FunctionDecl *declaration) {
    if (in_expression_ && IsLocalExpressionContext(declaration->getDeclContext()))
      TraverseType(declaration->getType());
    return true;
  }
  bool VisitExpr(clang::Expr *expression) {
    TraverseType(expression->getType());
    return true;
  }
  bool VisitDeclRefExpr(clang::DeclRefExpr *expression) {
    CheckExpressionReference(expression->getDecl()); return true;
  }
  bool VisitUnresolvedLookupExpr(clang::UnresolvedLookupExpr *expression) {
    for (const auto *declaration : expression->decls())
      CheckExpressionReference(declaration);
    return true;
  }
  bool VisitUnresolvedMemberExpr(clang::UnresolvedMemberExpr *expression) {
    for (const auto *declaration : expression->decls())
      CheckExpressionReference(declaration);
    return true;
  }
  bool VisitMemberExpr(clang::MemberExpr *expression) {
    CheckExpressionReference(expression->getMemberDecl()); return true;
  }
  bool VisitCXXConstructExpr(clang::CXXConstructExpr *expression) {
    CheckExpressionReference(expression->getConstructor()); return true;
  }
  bool VisitCXXNewExpr(clang::CXXNewExpr *expression) {
    CheckExpressionReference(expression->getOperatorNew());
    CheckExpressionReference(expression->getOperatorDelete());
    return true;
  }
  bool VisitCXXDeleteExpr(clang::CXXDeleteExpr *expression) {
    CheckExpressionReference(expression->getOperatorDelete()); return true;
  }
  bool VisitObjCMessageExpr(clang::ObjCMessageExpr *expression) {
    CheckExpressionReference(expression->getMethodDecl()); return true;
  }
  bool TraverseConceptReference(clang::ConceptReference *reference) {
    if (!reference) return true;
    CheckExpressionReference(reference->getNamedConcept());
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::TraverseConceptReference(reference);
  }
  bool TraverseRequiresExpr(clang::RequiresExpr *expression, DataRecursionQueue * = nullptr) {
    const auto locals = lambda_locals_;
    auto restore = llvm::scope_exit([&] { lambda_locals_ = locals; });
    for (const auto *parameter : expression->getLocalParameters())
      lambda_locals_.insert(parameter->getCanonicalDecl());
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::TraverseRequiresExpr(expression, nullptr);
  }
  void CheckEnclosingClassArguments(const clang::NamedDecl *declaration) {
    for (const clang::NamedDecl *owner=declaration; owner;
         owner=llvm::dyn_cast<clang::NamedDecl>(owner->getDeclContext()))
      if (const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(owner))
        for (const auto &argument : specialization->getTemplateArgs().asArray())
          if (!CheckGenericArgument(argument)) UnsupportedExpression();
  }
  bool CheckPatternArgument(const clang::TemplateArgument &argument) {
    if (argument.getKind()==clang::TemplateArgument::Expression) return TraverseStmt(argument.getAsExpr());
    if (argument.getKind()==clang::TemplateArgument::TemplateExpansion)
      return TraverseTemplateName(argument.getAsTemplateOrTemplatePattern());
    if (argument.getKind()==clang::TemplateArgument::Pack) {
      for (const auto &element:argument.pack_elements()) if (!CheckPatternArgument(element)) return false;
      return true;
    }
    return CheckGenericArgument(argument);
  }
  bool CheckGenericArgument(const clang::TemplateArgument &argument) {
    switch (argument.getKind()) {
    case clang::TemplateArgument::Type:
      return TraverseType(argument.getAsType());
    case clang::TemplateArgument::Integral:
      return TraverseType(argument.getIntegralType());
    case clang::TemplateArgument::NullPtr:
      return TraverseType(argument.getNullPtrType());
    case clang::TemplateArgument::StructuralValue: {
      unsigned budget=4096;std::string key;std::vector<const clang::NamedDecl *> references;
      if(!StructuralArgumentKey(argument.getAsStructuralValue(),context_,key,budget,0,&references))return false;
      for(const auto *reference:references)CheckExpressionReference(reference);
      return TraverseType(argument.getStructuralValueType());
    }
    case clang::TemplateArgument::Declaration: {
      const auto *declaration=argument.getAsDecl();std::string key;
      if(const auto *object=llvm::dyn_cast<clang::TemplateParamObjectDecl>(declaration)) {
        unsigned budget=4096;std::vector<const clang::NamedDecl *> references;
        if(!StructuralArgumentKey(object->getValue(),context_,key,budget,0,&references))return false;
        for(const auto *reference:references)CheckExpressionReference(reference);
        return TraverseType(object->getType());
      }
      if(!AddressDeclarationKey(declaration,context_,key))return false;
      CheckExpressionReference(declaration);return TraverseType(argument.getParamTypeForDecl());
    }
    case clang::TemplateArgument::Template: {
      const auto *pattern=argument.getAsTemplate().getAsTemplateDecl();
      if (!pattern) return false;
      if (llvm::isa<clang::BuiltinTemplateDecl,clang::TemplateTemplateParmDecl>(pattern)) return true;
      if (!llvm::isa<clang::ClassTemplateDecl,clang::TypeAliasTemplateDecl>(pattern)) return false;
      CheckExpressionReference(pattern);CheckEnclosingClassArguments(pattern);return true;
    }
    case clang::TemplateArgument::Pack:
      for (const auto &element : argument.pack_elements())
        if (!CheckGenericArgument(element)) return false;
      return true;
    default:
      return false;
    }
  }
  bool TraverseStmt(clang::Stmt *statement, DataRecursionQueue * = nullptr) {
    const auto *previous_function=body_function_;
    const clang::Decl *enclosing_context=nullptr;
    bool inserted_context=false;
    auto restore_function=llvm::scope_exit([&]{
      body_function_=previous_function;
      if(inserted_context)lambda_contexts_.erase(enclosing_context);
    });
    if(auto found=lambda_bodies_.find(statement);found!=lambda_bodies_.end()) {
      if(previous_function) {
        enclosing_context=previous_function->getCanonicalDecl();
        inserted_context=lambda_contexts_.insert(enclosing_context).second;
      }
      body_function_=found->second;
      CheckCxxCoroutine(body_function_);
      statement=const_cast<clang::Stmt *>(PublishedFunctionBody(body_function_));
    }
    const bool previous = in_expression_; in_expression_ = true;
    auto restore = llvm::scope_exit([&] { in_expression_ = previous; });
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::TraverseStmt(statement, nullptr);
  }
  bool TraverseCoreturnStmt(clang::CoreturnStmt *statement,
                            DataRecursionQueue * = nullptr) {
    const auto *type=clang::getObjC3CallableFunctionType(body_function_);
    if ((type && type->getObjC3FunctionEffects().Async) || PublishedCxxCoroutine(body_function_))
      return WalkUpFromCoreturnStmt(statement) && TraverseStmt(statement->getOperand());
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::
        TraverseCoreturnStmt(statement,nullptr);
  }
  bool TraverseCoroutineBodyStmt(clang::CoroutineBodyStmt *,
                                 DataRecursionQueue * = nullptr) {
    // The enclosing published function has already been projected to its
    // written body. Nested coroutine callables need their own body contract.
    return UnsupportedExpression("nested coroutine body has no published callable context");
  }
  bool TraverseCoyieldExpr(clang::CoyieldExpr *expression,
                           DataRecursionQueue * = nullptr) {
    if (PublishedCxxCoroutine(body_function_)) {
      const auto *operand=expression->getWrittenOperand();
      if (!operand) return UnsupportedExpression("coroutine yield has no recoverable written operand");
      return WalkUpFromCoyieldExpr(expression) && TraverseStmt(const_cast<clang::Expr *>(operand));
    }
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::TraverseCoyieldExpr(expression,nullptr);
  }
  bool TraverseCoawaitExpr(clang::CoawaitExpr *expression,
                           DataRecursionQueue * = nullptr) {
    if (PublishedCxxCoroutine(body_function_))
      return WalkUpFromCoawaitExpr(expression) && TraverseStmt(expression->getOperand());
    if (const auto *source=PublishedAwaitSource(expression,body_function_,context_)) {
      const auto *type=clang::getObjC3AsyncCalleeFunctionType(source);
      UsesCxxTaskRuntime |= !clang::getObjC3AwaitSourceExpression(expression) ||
                           (type && type->getObjC3FunctionEffects().Async &&
                            !type->getObjC3FunctionEffects().AsyncFrame);
      return WalkUpFromCoawaitExpr(expression) &&
             TraverseStmt(const_cast<clang::Expr *>(source));
    }
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::
        TraverseCoawaitExpr(expression,nullptr);
  }
  bool TraversePseudoObjectExpr(clang::PseudoObjectExpr *expression,
                                DataRecursionQueue * = nullptr) {
    if (const auto *source=clang::getObjC3IsolationSourceSyntax(expression))
      return TraverseStmt(const_cast<clang::Expr *>(source));
    clang::ObjC3KeyPathApplicationInfo application;
    if (clang::getObjC3KeyPathApplication(expression, application)) {
      UsesTypedKeyPaths = true;
      for (auto *argument : application.Syntax->arguments())
        if (!TraverseStmt(const_cast<clang::Expr *>(argument))) return false;
      return true;
    }
    if (auto *syntax=KeyPathSubscriptSyntax(expression)) {
      UsesKeyPathSubscripts = UsesTypedKeyPaths = true;
      return TraverseStmt(syntax);
    }
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::
        TraversePseudoObjectExpr(expression,nullptr);
  }
  bool TraverseOpaqueValueExpr(clang::OpaqueValueExpr *expression,
                               DataRecursionQueue * = nullptr) {
    return TraverseStmt(expression->getSourceExpr());
  }
  bool TraverseCompoundLiteralExpr(clang::CompoundLiteralExpr *expression,
                                   DataRecursionQueue * = nullptr) {
    if (clang::getObjC3KeyPathLiteralPath(expression)) {
      // The public spelling is @keypath(Root, path). Its private descriptor
      // and accessors are reconstructed by the pinned consumer compiler.
      UsesTypedKeyPaths = true;
      return TraverseType(expression->getType());
    }
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::
        TraverseCompoundLiteralExpr(expression,nullptr);
  }
  bool TraverseLambdaExpr(clang::LambdaExpr *expression, DataRecursionQueue * = nullptr) {
    std::vector<GenericLambdaBody> bodies;
    if (!GenericLambdaBodies(expression,context_,bodies)) {
      AddDiagnostic(result_,"O3IFC101",
          "generic local lambdas require complete instantiations with stable named types and supported structural, integral, null or pack values");
      return false;
    }
    lambda_bodies_[expression->getBody()]=expression->getCallOperator();
    const auto contexts = lambda_contexts_;
    const auto locals = lambda_locals_;
    lambda_contexts_.insert(expression->getCallOperator()->getCanonicalDecl());
    for (const auto &capture : expression->captures())
      if (capture.capturesVariable() && expression->isInitCapture(&capture))
        lambda_locals_.insert(capture.getCapturedVar()->getCanonicalDecl());
    auto restore = llvm::scope_exit([&] { lambda_contexts_ = contexts; lambda_locals_ = locals; });
    TraverseType(expression->getCallOperator()->getType());
    if (!clang::RecursiveASTVisitor<TypeDependencyVisitor>::TraverseLambdaExpr(expression,nullptr)) return false;
    for (const auto &body:bodies) {
      lambda_contexts_.insert(body.function->getCanonicalDecl());
      for (const auto &argument : body.function->getTemplateSpecializationArgs()->asArray())
        if (!CheckGenericArgument(argument)) return false;
      for (const auto *parameter:body.function->parameters()) {
        lambda_locals_.insert(parameter->getCanonicalDecl());
        TraverseType(parameter->getType());
      }
      TraverseType(body.function->getType());
      auto *statement=const_cast<clang::Stmt *>(PublishedFunctionBody(body.function));
      lambda_bodies_[statement]=body.function;
      if (!TraverseStmt(statement)) return false;
    }
    return true;
  }
  bool TraverseBlockExpr(clang::BlockExpr *expression, DataRecursionQueue * = nullptr) {
    const auto contexts = lambda_contexts_;
    lambda_contexts_.insert(expression->getBlockDecl());
    auto restore = llvm::scope_exit([&] { lambda_contexts_ = contexts; });
    for (const auto &capture : expression->getBlockDecl()->captures()) {
      CheckExpressionReference(capture.getVariable());
      TraverseType(capture.getCaptureType());
    }
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::TraverseBlockExpr(expression, nullptr);
  }
  bool UnsupportedExpression(llvm::StringRef reason={}) {
    std::string message="declaration expressions with local declarations or captures require additional contracts";
    if(!reason.empty())message+="; "+reason.str();
    AddDiagnostic(result_, "O3IFC101",message,
                  body_function_?body_function_->getQualifiedNameAsString():std::string());
    return false;
  }

  void CheckFactoryReference(const clang::FunctionDecl *function) {
    if(const auto *method=llvm::dyn_cast<clang::CXXMethodDecl>(function);method && (method->getParent()->isLambda() || IsExpressionLocalTag(method->getParent()))) {
      if(const auto *owner=FactoryResultOrigin(method->getParent()))CheckFactoryReference(owner);
      if(const auto *arguments=function->getTemplateSpecializationArgs())
        for(const auto &argument:arguments->asArray())
          if(!CheckGenericArgument(argument))UnsupportedExpression();
      for(const auto *parameter:function->parameters())TraverseType(parameter->getType());
      return;
    }
    CheckExpressionReference(function);
    CheckEnclosingClassArguments(function);
  }

  bool VisitTagType(clang::TagType *type) {
    const auto *declaration = type->getDecl();
    if (IsStandardCoroutineDeclaration(declaration,context_)) {
      if (const auto *record=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(declaration))
        for (const auto &argument:record->getTemplateArgs().asArray())
          if (!CheckPatternArgument(argument)) UnsupportedExpression();
      return true;
    }
    // The target ABI owns this implicit record; it is supplied by the consumer
    // compiler rather than a package. Never recognize a user type by spelling.
    if (const auto *builtin=context_.getVaListTagDecl();
        builtin && declaration->getCanonicalDecl()==builtin->getCanonicalDecl()) return true;
    if (const auto *pattern = TemplatePublicationOwner(declaration);
        !IsExpressionLocalTag(declaration) && pattern && (IsInMainFile(context_,pattern->getLocation()) ||
                    resolved_ids_.count(ReadInterfaceDeclarationStableId(pattern)))) {
      CheckExpressionReference(pattern);
      CheckEnclosingClassArguments(declaration);
      return true;
    }
    if (const auto *factory=FactoryResultOrigin(declaration)) {
      CheckFactoryReference(factory);
      return true;
    }
    if (public_template_type_ && !in_expression_ && !IsPublishedType(declaration) &&
        !IsResolved(declaration))
      AddDiagnostic(result_,"O3IFC112","template type references a non-public declaration",
                    declaration->getQualifiedNameAsString());
    if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
        record && record->isLambda())
      return in_expression_ || factory_results_.count(record->getCanonicalDecl())
          ? true : UnsupportedExpression("closure type escapes its expression context: "+
              ReadInterfaceDeclarationStableId(llvm::dyn_cast<clang::NamedDecl>(declaration->getDeclContext())));
    if (IsExpressionLocalTag(declaration))
      return (in_expression_ && IsLocalExpressionContext(declaration->getDeclContext())) ||
             factory_results_.count(declaration->getCanonicalDecl())
                 ? true : UnsupportedExpression("local type escapes its callable context");
    CheckVisibility(declaration);
    if (declaration && !IsInMainFile(context_, declaration->getLocation()) &&
        !IsResolved(declaration))
      AddDiagnostic(result_, "O3IFC112",
                    "public declaration references a record or enum from an unresolved external dependency",
                    declaration->getQualifiedNameAsString());
    return true;
  }

  bool VisitTypedefType(clang::TypedefType *type) {
    const auto *declaration = type->getDecl();
    if (IsStandardCoroutineDeclaration(declaration,context_)) return true;
    if (const auto *owner=TemplatePublicationOwner(declaration);IsStandardCoroutineTraitsSpecialization(owner)) {
      CheckExpressionReference(owner);CheckEnclosingClassArguments(declaration);
      TraverseType(declaration->getUnderlyingType());return true;
    }
    if (declaration->getCanonicalDecl()==context_.getBuiltinVaListDecl()->getCanonicalDecl()) return true;
    // Runtime headers redeclare these compiler-provided language types. Accept
    // only the same native name and type, without additional typedef contracts.
    if (context_.getLangOpts().ObjC && declaration) {
      for (const auto *builtin :
           {context_.getObjCIdDecl(), context_.getObjCClassDecl(),
            context_.getObjCSelDecl(),
            const_cast<clang::ASTContext &>(context_).getObjCInstanceTypeDecl()}) {
        if (declaration == builtin ||
            (builtin && !declaration->hasAttrs() &&
             declaration->getIdentifier() == builtin->getIdentifier() &&
             context_.hasSameType(declaration->getUnderlyingType(),
                                  builtin->getUnderlyingType())))
          return true;
      }
    }
    const auto contract = clang::getObjC3ResourceTypeContract(
        context_, clang::QualType(type, 0));
    if (const auto *cleanup = contract.getCleanupFunction()) {
      CheckVisibility(cleanup);
      if (!cleanup->isExternallyVisible() ||
          (!IsInMainFile(context_, cleanup->getLocation()) && !IsResolved(cleanup)))
        AddDiagnostic(result_, "O3IFC170",
                      "exported resource cleanup needs an externally visible, resolved function",
                      cleanup->getQualifiedNameAsString());
    }
    if (contract.Invalid) {
      const auto *invalid = contract.Invalid->getInvalid();
      if (NativeResourceSentinelIdentity(invalid, context_) == "!unmodeled")
        AddDiagnostic(result_, "O3IFC170",
                      "resource sentinel has no stable exported constant identity",
                      declaration->getQualifiedNameAsString());
      clang::Expr::EvalResult value;
      if (invalid->EvaluateAsRValue(value, context_) && value.Val.isLValue())
        if (const auto *base = value.Val.getLValueBase().dyn_cast<const clang::ValueDecl *>()) {
          CheckVisibility(base);
          if (!IsInMainFile(context_, base->getLocation()) && !IsResolved(base))
            AddDiagnostic(result_, "O3IFC170", "resource sentinel dependency is unresolved",
                          base->getQualifiedNameAsString());
        }
    }
    CheckVisibility(declaration);
    if (declaration && !IsInMainFile(context_, declaration->getLocation()) &&
        !IsResolved(declaration))
      AddDiagnostic(result_, "O3IFC112",
                    "public declaration references a typedef from an unresolved external dependency",
                    declaration->getQualifiedNameAsString());
    return true;
  }

  bool VisitObjCInterfaceType(clang::ObjCInterfaceType *type) {
    const auto *declaration = type->getDecl();
    CheckVisibility(declaration);
    if (declaration && !IsInMainFile(context_, declaration->getLocation()) &&
        !IsResolved(declaration))
      AddDiagnostic(result_, "O3IFC112",
                    "public declaration references an Objective-C class from an unresolved external dependency",
                    declaration->getNameAsString());
    return true;
  }

  bool VisitObjCObjectType(clang::ObjCObjectType *type) {
    for (const auto *protocol : type->quals()) {
      CheckVisibility(protocol);
      if (!IsInMainFile(context_, protocol->getLocation()) && !IsResolved(protocol))
        AddDiagnostic(result_, "O3IFC112",
                      "public declaration references a protocol from an unresolved external dependency",
                      protocol->getNameAsString());
    }
    return true;
  }

  bool VisitFunctionProtoType(clang::FunctionProtoType *type) {
    const auto effects = type->getObjC3FunctionEffects();
    // The canonical function type explicitly selects its physical ABI. Never
    // infer the return carrier from the importing translation unit's language.
    UsesRuntimeFrame |= effects.Async && effects.AsyncFrame;
    UsesCanonicalFunctionEffects |=
        effects.Async || effects.Throws != clang::ObjC3ThrowsKind::None;
    if (!effects.ErrorType.isNull()) TraverseType(effects.ErrorType);
    return true;
  }

  bool UsesCanonicalFunctionEffects = false;
  bool UsesRuntimeFrame = false;
  bool UsesCxxTaskRuntime = false;
  bool UsesTypedKeyPaths = false;
  bool UsesKeyPathSubscripts = false;

private:
  bool public_template_type_ = false;
  bool in_expression_ = false;
  std::set<const clang::Decl *> factory_results_;
  std::set<const clang::Decl *> lambda_contexts_;
  std::set<const clang::Decl *> lambda_locals_;
  const clang::FunctionDecl *body_function_ = nullptr;
  std::map<const clang::Stmt *,const clang::FunctionDecl *> lambda_bodies_;
  const clang::ASTContext &context_;
  InterfaceEmissionArtifact &result_;
  const std::set<std::string> &resolved_ids_;
  const std::vector<std::string> &exports_;
  std::string spi_group_;
  std::string source_root_;
  bool IsInMainFile(const clang::ASTContext &context,
                    clang::SourceLocation location) const {
    return IsModuleOwnedSource(context, location, source_root_);
  }
};

bool CheckTypeDependencies(const clang::NamedDecl *declaration,
                           const clang::ASTContext &context,
                           InterfaceEmissionArtifact &result,
                           const std::set<std::string> &resolved_ids,
                           bool &uses_runtime_frame,
                           bool &uses_cxx_task_runtime,
                           const std::vector<std::string> &exports,
                           const std::string &source_root,
                           bool *uses_typed_keypaths,
                           bool *uses_keypath_subscripts) {
  const auto first_diagnostic=result.diagnostics.size();
  auto qualify_diagnostics=llvm::scope_exit([&] {
    for(auto index=first_diagnostic;index<result.diagnostics.size();++index)
      if(result.diagnostics[index].subject.empty())
        result.diagnostics[index].subject=declaration->getQualifiedNameAsString();
  });
  TypeDependencyVisitor visitor(context, result, resolved_ids, exports,
                                ReadInterfaceSPIGroup(declaration),
                                source_root);
  visitor.AllowPublishedFactoryResult(llvm::dyn_cast<clang::FunctionDecl>(declaration));
  visitor.CheckDeclarationAttributes(declaration);
  auto check = [&](clang::QualType type) {
    if (type.isNull()) return;
    visitor.TraverseType(type);
    // Clang's type visitor deliberately stops at typedef sugar. Canonical
    // traversal exposes nested callable contracts behind those aliases.
    if (type != type.getCanonicalType())
      visitor.TraverseType(type.getCanonicalType());
  };
  if (const auto *value = llvm::dyn_cast<clang::ValueDecl>(declaration))
    check(value->getType());
  if (const auto *type = llvm::dyn_cast<clang::TypeDecl>(declaration))
    check(context.getTypeDeclType(type));
  if (const auto *concept_decl = llvm::dyn_cast<clang::ConceptDecl>(declaration))
    visitor.CheckConcept(concept_decl);
  if (const auto *pattern = llvm::dyn_cast<clang::FunctionTemplateDecl>(declaration))
    visitor.CheckFunctionTemplate(pattern);
  if (const auto *specialization=llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(declaration);
      IsExplicitNamespaceVariableSpecialization(specialization)) {
    visitor.CheckExpressionReference(specialization->getSpecializedTemplate());
    for (const auto &argument:specialization->getTemplateArgs().asArray())
      if (!visitor.CheckGenericArgument(argument))
        AddDiagnostic(result,"O3IFC101","variable specialization argument requires additional publication support",
                      specialization->getQualifiedNameAsString());
  }
  if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(declaration);
      IsExplicitNamespaceFunctionSpecialization(function)) {
    visitor.CheckExpressionReference(function->getPrimaryTemplate());
    for (const auto &argument:function->getTemplateSpecializationArgs()->asArray())
      if (!visitor.CheckGenericArgument(argument))
        AddDiagnostic(result,"O3IFC101","function specialization argument requires additional publication support",
                      function->getQualifiedNameAsString());
  }
  if (const auto *pattern = llvm::dyn_cast<clang::TypeAliasTemplateDecl>(declaration))
    visitor.CheckAliasTemplate(pattern);
  if (const auto *pattern = llvm::dyn_cast<clang::VarTemplateDecl>(declaration))
    visitor.CheckVariableTemplate(pattern);
  if (const auto *pattern = llvm::dyn_cast<clang::ClassTemplateDecl>(declaration))
    visitor.CheckClassTemplate(pattern);
  if (const auto *record=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(declaration);
      IsStandardCoroutineTraitsSpecialization(record)) visitor.CheckCoroutineTraits(record);
  if (const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
      HasDirectMemberTemplates(record)) visitor.CheckTemplateRecord(record);
  for (const auto *pattern:ClosedParentPatterns(declaration)) {
    if (const auto *record=llvm::dyn_cast<clang::ClassTemplateDecl>(pattern)) visitor.CheckClassTemplate(record);
    else if (const auto *function=llvm::dyn_cast<clang::FunctionTemplateDecl>(pattern)) visitor.CheckFunctionTemplate(function);
    else if (const auto *variable=llvm::dyn_cast<clang::VarTemplateDecl>(pattern)) visitor.CheckVariableTemplate(variable);
    else if (const auto *partial=llvm::dyn_cast<clang::VarTemplatePartialSpecializationDecl>(pattern)) visitor.CheckVariablePartial(partial);
    else if (const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(pattern)) visitor.CheckClassTemplate(partial->getSpecializedTemplate());
  }
  for (const auto *specialization:ClosedParentMemberSpecializations(declaration)) {
    const auto *parent=llvm::cast<clang::ClassTemplateSpecializationDecl>(specialization->getDeclContext());
    for (const auto &argument:parent->getTemplateArgs().asArray())
      if (!visitor.CheckGenericArgument(argument))
        AddDiagnostic(result,"O3IFC101","closed parent argument requires additional publication support",specialization->getQualifiedNameAsString());
    if (const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(specialization)) visitor.CheckTemplateRecord(record);
    else if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(specialization))
      visitor.CheckConcreteMemberFunctionSpecialization(function);
    else if (const auto *variable=llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(specialization)) {
      for (const auto &argument:variable->getTemplateArgs().asArray())
        if (!visitor.CheckGenericArgument(argument))
          AddDiagnostic(result,"O3IFC101","closed member argument requires additional publication support",variable->getQualifiedNameAsString());
      visitor.TraverseType(variable->getType());
      if (PublishesVariableInitializer(variable) && variable->hasInit())
        visitor.TraverseStmt(const_cast<clang::Expr *>(variable->getInit()));
    }
  }
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration))
    for (const auto *parameter : function->parameters())
      if (parameter->hasDefaultArg() && !parameter->hasUninstantiatedDefaultArg())
        visitor.TraverseStmt(const_cast<clang::Expr *>(parameter->getDefaultArg()));
  if (const auto *field = llvm::dyn_cast<clang::FieldDecl>(declaration);
      field && field->hasInClassInitializer())
    visitor.TraverseStmt(field->getInClassInitializer());
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration);
      PublishesFunctionBody(function)) visitor.CheckBody(function);
  if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration);
      PublishesVariableInitializer(variable) && variable->hasInit())
    visitor.TraverseStmt(const_cast<clang::Expr *>(variable->getInit()));
  auto check_reference=[&](const clang::NamedDecl *target) {
    visitor.CheckVisibility(target);
    if (!IsInMainFile(context,target->getLocation(),source_root) && !visitor.IsResolved(target))
      AddDiagnostic(result,"O3IFC112","using target has no resolved interface identity",
                    target->getQualifiedNameAsString());
  };
  if(const auto *using_enum=llvm::dyn_cast<clang::UsingEnumDecl>(declaration))
    check_reference(using_enum->getEnumDecl());
  if(const auto *using_decl=llvm::dyn_cast<clang::UsingDecl>(declaration))
    for(const auto *shadow:using_decl->shadows()) check_reference(shadow->getTargetDecl());
  if(const auto *directive=llvm::dyn_cast<clang::UsingDirectiveDecl>(declaration))
    check_reference(directive->getNominatedNamespaceAsWritten());
  if (const auto *alias = llvm::dyn_cast<clang::NamespaceAliasDecl>(declaration)) {
    const auto *target = alias->getAliasedNamespace();
    visitor.CheckVisibility(target);
    if (!IsInMainFile(context, target->getLocation(), source_root) &&
        !visitor.IsResolved(target))
      AddDiagnostic(result, "O3IFC112", "namespace alias target has no resolved interface identity",
                    target->getQualifiedNameAsString());
  }
  if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
      record && record->isCompleteDefinition())
    for (const auto &base : record->bases())
      check(base.getType());
  if (const auto *property = llvm::dyn_cast<clang::ObjCPropertyDecl>(declaration))
    check(property->getType());
  if (const auto *method = llvm::dyn_cast<clang::ObjCMethodDecl>(declaration)) {
    check(method->getObjC3FunctionType());
    check(method->getReturnType());
    for (unsigned i = 0; i < method->param_size(); ++i)
      check(method->getParamDecl(i)->getType());
  }
  if (const auto *interface = llvm::dyn_cast<clang::ObjCInterfaceDecl>(declaration)) {
    visitor.CheckVisibility(interface->getSuperClass());
    for (const auto *protocol : interface->protocols())
      visitor.CheckVisibility(protocol);
    if (const auto *superclass = interface->getSuperClass())
      if (!IsInMainFile(context, superclass->getLocation(), source_root) &&
          !visitor.IsResolved(superclass))
        AddDiagnostic(result, "O3IFC112",
                      "Objective-C superclass dependency has no resolved interface identity",
                      superclass->getNameAsString());
    for (const auto *protocol : interface->protocols())
      if (!IsInMainFile(context, protocol->getLocation(), source_root) &&
          !visitor.IsResolved(protocol))
        AddDiagnostic(result, "O3IFC112",
                      "adopted Objective-C protocol dependency has no resolved interface identity",
                      protocol->getNameAsString());
    for (const auto *ivar : interface->ivars()) check(ivar->getType());
    if (const auto *parameters = interface->getTypeParamList())
      for (const auto *parameter : *parameters) check(parameter->getUnderlyingType());
    if (const auto *superclass = interface->getSuperClassType())
      check(clang::QualType(superclass, 0));
  }
  if (const auto *protocol = llvm::dyn_cast<clang::ObjCProtocolDecl>(declaration))
    for (const auto *adopted : protocol->protocols()) {
      visitor.CheckVisibility(adopted);
      if (!IsInMainFile(context, adopted->getLocation(), source_root) &&
          !visitor.IsResolved(adopted))
        AddDiagnostic(result, "O3IFC112",
                      "adopted Objective-C protocol dependency has no resolved interface identity",
                      adopted->getNameAsString());
    }
  if (const auto *category =
          llvm::dyn_cast<clang::ObjCCategoryDecl>(declaration)) {
    visitor.CheckVisibility(category->getClassInterface());
    for (const auto *protocol : category->protocols()) {
      visitor.CheckVisibility(protocol);
      if (!IsInMainFile(context, protocol->getLocation(), source_root) &&
          !visitor.IsResolved(protocol))
        AddDiagnostic(result, "O3IFC112",
                      "category-adopted Objective-C protocol dependency has no resolved interface identity",
                      protocol->getNameAsString());
    }
  }
  uses_runtime_frame |= visitor.UsesRuntimeFrame;
  uses_cxx_task_runtime |= visitor.UsesCxxTaskRuntime;
  if (uses_typed_keypaths) *uses_typed_keypaths |= visitor.UsesTypedKeyPaths;
  if (uses_keypath_subscripts) *uses_keypath_subscripts |= visitor.UsesKeyPathSubscripts;
  if (visitor.UsesCxxTaskRuntime && !IsQualifiedRuntimeTaskHeader(context))
    AddDiagnostic(result, "O3IFC112",
                  "published bodies using C++ tasks require the exact SDK task runtime header",
                  declaration->getQualifiedNameAsString());
  return visitor.UsesCanonicalFunctionEffects;
}

// Member patterns are owned and printed with the enclosing primary class.
// Their native specialization mangling inherits that class's module ABI tag.
// Authored detached callables and static data use explicit definition-set
// contracts; member specializations require separate publication support.


bool IsBoundedClassMemberAlias(const clang::TypeAliasTemplateDecl *pattern) {
  if (!IsBoundedNestedTemplateParameters(pattern->getTemplateParameters())) return false;
  for (const auto *attribute : pattern->getTemplatedDecl()->attrs())
    if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
  return true;
}

bool IsBoundedClassMemberTemplate(const clang::FunctionTemplateDecl *pattern) {
  const auto *method=llvm::dyn_cast<clang::CXXMethodDecl>(pattern->getTemplatedDecl());
  if (!method || (!method->doesThisDeclarationHaveABody() &&
                  !IsDetachedCallableTemplateDefinition(method->getDefinition()))) return false;
  if (!IsBoundedNestedTemplateParameters(pattern->getTemplateParameters())) return false;
  for (const auto *attribute : method->attrs())
    if (const auto *tag=llvm::dyn_cast<clang::AbiTagAttr>(attribute)) {
      if (tag->tags_size()!=1 || !tag->tags_begin()->starts_with("__objc3_m_") ||
          ReadNativeSymbolName(pattern)!=*tag->tags_begin()) return false;
    } else if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
  for (const auto *parameter : method->parameters())
    for (const auto *attribute : parameter->attrs()) if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
  return true;
}

bool IsBoundedNestedTemplateParameters(const clang::TemplateParameterList *parameters,
                                        unsigned depth) {
  if (depth>64) return false;
  for (const auto *parameter:*parameters) {
    for (const auto *attribute:parameter->attrs()) if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
    if (const auto *nested=llvm::dyn_cast<clang::TemplateTemplateParmDecl>(parameter)) {
      if (nested->templateParameterKind()!=clang::TNK_Type_template ||
          !IsBoundedNestedTemplateParameters(nested->getTemplateParameters(),depth+1)) return false;
    }
  }
  return true;
}

bool IsBoundedVariablePartial(const clang::VarTemplatePartialSpecializationDecl *partial) {
  if (!IsBoundedNestedTemplateParameters(partial->getTemplateParameters()) ||
      (partial->isStaticDataMember() && !IsConcreteMemberVariableSpecialization(partial) && !IsDependentMemberPartial(partial) && !IsClosedParentPattern(partial)) ||
      (partial->getStorageClass()==clang::SC_Static && !IsConcreteMemberVariableSpecialization(partial) && !IsDependentMemberPartial(partial) && !IsClosedParentPattern(partial)) ||
      partial->getStorageClass()==clang::SC_PrivateExtern ||
      partial->isThisDeclarationADefinition()==clang::VarDecl::DeclarationOnly) return false;
  for (const auto *attribute:partial->attrs())
    if (const auto *tag=llvm::dyn_cast<clang::AbiTagAttr>(attribute)) {
      if(tag->tags_size()!=1 || ReadNativeSymbolName(partial)!=*tag->tags_begin()) return false;
    } else if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
  return true;
}

bool IsBoundedClassMemberVariable(const clang::VarTemplateDecl *pattern) {
  const auto *variable=pattern->getTemplatedDecl();
  if (!IsBoundedNestedTemplateParameters(pattern->getTemplateParameters()) ||
      !variable->isStaticDataMember() ||
      (((!variable->isInline() && !variable->isConstexpr()) || !variable->hasInit()) &&
       !IsDetachedStaticDataTemplateDefinition(variable->getDefinition()))) return false;
  for (const auto *attribute:variable->attrs())
    if (const auto *tag=llvm::dyn_cast<clang::AbiTagAttr>(attribute)) {
      if (tag->tags_size()!=1 || !tag->tags_begin()->starts_with("__objc3_m_") ||
          ReadNativeSymbolName(pattern)!=*tag->tags_begin()) return false;
    } else if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
  return true;
}


bool IsBoundedTemplateRecord(const clang::CXXRecordDecl *record,
                            const clang::ClassTemplateDecl *pattern) {
  if (pattern && !IsBoundedNestedTemplateParameters(pattern->getTemplateParameters())) return false;
  const auto *body=record->getDefinition();
  if (!body || !body->isCompleteDefinition() || body->isUnion() ||
      (!body->getIdentifier() && UnnamedTemplateRecordKey(body,body->getASTContext()).empty())) return false;
  if (record->getDeclContext()->isRecord() && !body->getLexicalDeclContext()->isRecord() &&
      !IsDetachedNestedTemplateDefinition(body) &&
      !IsConcreteMemberClassSpecialization(llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(body)) &&
      !IsDependentMemberPartial(body) && !IsClosedParentMemberSpecialization(body) && !IsClosedParentPattern(body)) return false;
  if ((body->getNumBases() || body->isPolymorphic()) && !body->getASTContext().getLangOpts().RTTI) return false;
  for (const auto *attribute : body->attrs()) {
    if (const auto *tag=llvm::dyn_cast<clang::AbiTagAttr>(attribute)) {
      if (!pattern || tag->tags_size()!=1 || !tag->tags_begin()->starts_with("__objc3_m_") ||
          ReadNativeSymbolName(pattern)!=*tag->tags_begin()) return false;
    } else if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute) && !llvm::isa<clang::FinalAttr>(attribute)) return false;
  }
  for (const auto *member : body->decls()) {
    if (member->isImplicit() || llvm::isa<clang::AccessSpecDecl>(member)) continue;
      if (const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(member);
          specialization && specialization->getSpecializationKind()==clang::TSK_ImplicitInstantiation) continue;
    if (const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(member);
        (IsDependentMemberPartial(partial) || IsClosedParentPattern(partial))) {
      if (!IsBoundedTemplateRecord(partial,partial->getSpecializedTemplate()) ||
          !IsBoundedNestedTemplateParameters(partial->getTemplateParameters())) return false;
      continue;
    }
    if (const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(member);
        IsClosedParentMemberSpecialization(specialization)) {
      std::string key;
      if (!IsBoundedTemplateRecord(specialization,specialization->getSpecializedTemplate()) ||
          !ClassSpecializationArgumentKey(specialization,specialization->getASTContext(),key)) return false;
      continue;
    }
    if (llvm::isa<clang::ClassTemplateSpecializationDecl>(member)) return false;
    if (const auto *function=llvm::dyn_cast<clang::FunctionTemplateDecl>(member)) {
      if (!IsBoundedClassMemberTemplate(function)) return false;
    } else if (const auto *alias=llvm::dyn_cast<clang::TypeAliasTemplateDecl>(member)) {
      if (!IsBoundedClassMemberAlias(alias)) return false;
    } else if (const auto *inner=llvm::dyn_cast<clang::ClassTemplateDecl>(member)) {
      if (!IsBoundedPublishedClassTemplate(inner)) return false;
    } else if (const auto *inner=llvm::dyn_cast<clang::CXXRecordDecl>(member)) {
      if (!IsBoundedTemplateRecord(inner)) return false;
    } else if (const auto *value=llvm::dyn_cast<clang::VarTemplateDecl>(member)) {
      const auto *variable=value->getTemplatedDecl();
      if (!IsBoundedNestedTemplateParameters(value->getTemplateParameters()) ||
          !variable->isStaticDataMember() ||
          (((!variable->isInline() && !variable->isConstexpr()) || !variable->hasInit()) &&
           !IsDetachedStaticDataTemplateDefinition(variable->getDefinition()))) return false;
      for (const auto *attribute : variable->attrs()) if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
      for (const auto *partial:PublishedVariablePartials(value))
        if (!IsBoundedVariablePartial(partial)) return false;
    } else if (const auto *method=llvm::dyn_cast<clang::CXXMethodDecl>(member)) {
      if (IsClosedParentMemberSpecialization(method)) continue;
      if (method->getTemplateSpecializationKind()==clang::TSK_ExplicitSpecialization ||
          (!method->doesThisDeclarationHaveABody() &&
           !IsDetachedCallableTemplateDefinition(method->getDefinition()) && !method->isDefaulted() &&
           !method->isDeleted() && !method->isPureVirtual())) return false;
    } else if (const auto *variable=llvm::dyn_cast<clang::VarDecl>(member)) {
      if (!variable->isStaticDataMember() ||
          (!variable->isInline() && !variable->isConstexpr() &&
           !IsDetachedStaticDataTemplateDefinition(variable->getDefinition()))) return false;
    } else if (!llvm::isa<clang::FieldDecl,clang::TypedefNameDecl,clang::EnumDecl,clang::BaseUsingDecl>(member)) return false;
    for (const auto *attribute : member->attrs())
      if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute) && !llvm::isa<clang::OverrideAttr,clang::FinalAttr>(attribute)) return false;
  }
  return true;
}

bool IsBoundedPublishedClassTemplate(const clang::ClassTemplateDecl *pattern) {
  if (!IsBoundedNestedTemplateParameters(pattern->getTemplateParameters()) ||
      !IsBoundedTemplateRecord(pattern->getTemplatedDecl(),pattern)) return false;
  for (const auto *specialization:PublishedClassSpecializations(pattern)) {
    if (!IsBoundedTemplateRecord(specialization,pattern)) return false;
    if (const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(specialization))
      if (!IsBoundedNestedTemplateParameters(partial->getTemplateParameters())) return false;
    std::string key;
    if (!ClassSpecializationArgumentKey(specialization,specialization->getASTContext(),key)) return false;
  }
  return true;
}

bool IsBoundedConcreteMemberFunctionSpecialization(const clang::FunctionDecl *function) {
  if (!IsConcreteMemberFunctionSpecialization(function) ||
      (function->isInlined() && !function->getDefinition())) return false;
  for (const auto *attribute:function->attrs())
    if (const auto *tag=llvm::dyn_cast<clang::AbiTagAttr>(attribute)) {
      if (tag->tags_size()!=1 || ReadNativeSymbolName(function->getPrimaryTemplate())!=*tag->tags_begin()) return false;
    } else if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
  for (const auto *parameter:function->parameters())
    for (const auto *attribute:parameter->attrs()) if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
  std::string key;
  for (const auto &argument:function->getTemplateSpecializationArgs()->asArray())
    if (!GenericArgumentKey(argument,function->getASTContext(),key)) return false;
  return true;
}

bool IsBoundedConcreteMemberVariableSpecialization(const clang::VarTemplateSpecializationDecl *variable) {
  if (!IsConcreteMemberVariableSpecialization(variable)) return false;
  if (const auto *partial=llvm::dyn_cast<clang::VarTemplatePartialSpecializationDecl>(variable))
    return IsBoundedVariablePartial(partial);
  if (variable->getType()->isDependentType()) return false;
  for (const auto *attribute:variable->attrs())
    if (const auto *tag=llvm::dyn_cast<clang::AbiTagAttr>(attribute)) {
      if (tag->tags_size()!=1 || ReadNativeSymbolName(variable->getSpecializedTemplate())!=*tag->tags_begin()) return false;
    } else if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
  std::string key;
  for (const auto &argument:variable->getTemplateArgs().asArray())
    if (!GenericArgumentKey(argument,variable->getASTContext(),key)) return false;
  return true;
}

bool IsBoundedClosedParentMembers(const clang::NamedDecl *owner) {
  for (const auto *specialization:ClosedParentMemberSpecializations(owner)) {
    const auto *parent=llvm::cast<clang::ClassTemplateSpecializationDecl>(specialization->getDeclContext());
    std::string arguments;
    if (!ClassSpecializationArgumentKey(parent,parent->getASTContext(),arguments)) return false;
    if (const auto *record=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(specialization)) {
      if (!IsBoundedTemplateRecord(record,record->getSpecializedTemplate()) ||
          !ClassSpecializationArgumentKey(record,record->getASTContext(),arguments)) return false;
    } else if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(specialization)) {
      if (function->isInlined() && !function->getDefinition()) return false;
      for (const auto *attribute:function->attrs()) if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
      for (const auto &argument:function->getTemplateSpecializationArgs()->asArray())
        if (!GenericArgumentKey(argument,function->getASTContext(),arguments)) return false;
    } else if (const auto *variable=llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(specialization)) {
      if (variable->getType()->isDependentType()) return false;
      for (const auto *attribute:variable->attrs()) if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
      for (const auto &argument:variable->getTemplateArgs().asArray())
        if (!GenericArgumentKey(argument,variable->getASTContext(),arguments)) return false;
    }
  }
  return true;
}

bool IsBoundedClosedParentPatterns(const clang::NamedDecl *owner) {
  for (const auto *pattern:ClosedParentPatterns(owner)) {
    if (const auto *record=llvm::dyn_cast<clang::ClassTemplateDecl>(pattern)) {
      if (!IsBoundedPublishedClassTemplate(record)) return false;
    } else if (const auto *function=llvm::dyn_cast<clang::FunctionTemplateDecl>(pattern)) {
      if (!IsBoundedClassMemberTemplate(function)) return false;
    } else if (const auto *variable=llvm::dyn_cast<clang::VarTemplateDecl>(pattern)) {
      if (!IsBoundedClassMemberVariable(variable)) return false;
    } else if (const auto *partial=llvm::dyn_cast<clang::VarTemplatePartialSpecializationDecl>(pattern)) {
      if (!IsBoundedVariablePartial(partial)) return false;
    } else if (const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(pattern)) {
      std::string key;
      if (!IsBoundedTemplateRecord(partial,partial->getSpecializedTemplate()) ||
          !IsBoundedNestedTemplateParameters(partial->getTemplateParameters()) ||
          !ClassSpecializationArgumentKey(partial,partial->getASTContext(),key)) return false;
    }
  }
  return true;
}

bool IsBoundedCxxDeclaration(const clang::NamedDecl *declaration,
                             InterfaceEmissionArtifact &result,
                             const std::vector<std::string> &exports) {
  const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
  if (IsStandardCoroutineTraitsSpecialization(record)) {
    const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(record);
    if (record->isCompleteDefinition() && CoroutineTraitsReturnOwner(record) &&
        IsBoundedTemplateRecord(record) && (!partial || IsBoundedNestedTemplateParameters(partial->getTemplateParameters()))) return true;
    AddDiagnostic(result,"O3IFC101","coroutine traits specialization requires a complete bounded definition and named return owner",record->getQualifiedNameAsString());return false;
  }
  bool supported = IsBoundedClosedParentMembers(declaration) && IsBoundedClosedParentPatterns(declaration);
  if (record && record->isCompleteDefinition()) {
    bool dependent_factory_local=false;
    if (IsExpressionLocalTag(record))
      for (const auto *scope=record->getDeclContext();scope;scope=scope->getParent())
        if (const auto *owner=llvm::dyn_cast<clang::FunctionDecl>(scope)) {
          dependent_factory_local=IsPublishedDeducedFactory(owner);break;
        }
    supported &=
        (!record->isDependentContext() || dependent_factory_local) && !record->getDescribedClassTemplate() &&
        (record->getTemplateSpecializationKind() == clang::TSK_Undeclared ||
         (record->getTemplateSpecializationKind() == clang::TSK_ImplicitInstantiation &&
          dependent_factory_local && FactoryResultOrigin(record))) &&
        (!record->isDynamicClass() || record->getASTContext().getLangOpts().RTTI);
    for (const auto *member : record->decls()) {
      if (member->isImplicit() || llvm::isa<clang::AccessSpecDecl, clang::BlockDecl>(member))
        continue;
      if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(member)) {
        supported &= IsBoundedPublishedClassTemplate(pattern);continue;
      }
      if (const auto *pattern=llvm::dyn_cast<clang::FunctionTemplateDecl>(member)) {
        supported &= IsBoundedClassMemberTemplate(pattern);
        for (const auto *specialization:pattern->specializations())
          if (specialization->getTemplateSpecializationKind()==clang::TSK_ExplicitSpecialization)
            supported &= IsBoundedConcreteMemberFunctionSpecialization(specialization);
        continue;
      }
      if (const auto *pattern=llvm::dyn_cast<clang::TypeAliasTemplateDecl>(member)) {
        supported &= IsBoundedClassMemberAlias(pattern);continue;
      }
      if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(member)) {
        supported &= IsBoundedClassMemberVariable(pattern);
        for (const auto *partial:PublishedVariablePartials(pattern))
          supported &= IsBoundedConcreteMemberVariableSpecialization(partial);
        for (const auto *specialization:pattern->specializations())
          if (specialization->getSpecializationKind()==clang::TSK_ExplicitSpecialization)
            supported &= IsBoundedConcreteMemberVariableSpecialization(specialization);
        continue;
      }
      if (const auto *method=llvm::dyn_cast<clang::CXXMethodDecl>(member);
          method && TemplatePublicationOwner(method)) continue;
      if (const auto *variable=llvm::dyn_cast<clang::VarDecl>(member);
          variable && TemplatePublicationOwner(variable)) continue;
      const auto *field = llvm::dyn_cast<clang::FieldDecl>(member);
      if (field) continue;
      if (llvm::isa<clang::BaseUsingDecl>(member)) continue;
      if (const auto *friend_decl = llvm::dyn_cast<clang::FriendDecl>(member)) {
        supported &= IsConcreteFriend(friend_decl);
        if (const auto *function = llvm::dyn_cast_or_null<clang::FunctionDecl>(friend_decl->getFriendDecl()))
          for (const auto *redecl : function->redecls())
            if (const auto *owner = llvm::dyn_cast<clang::CXXRecordDecl>(redecl->getLexicalDeclContext());
                owner && redecl->getFriendObjectKind() != clang::Decl::FOK_None) {
              if (!SelectedExport(owner, exports))
                AddDiagnostic(result, "O3IFC156", "friend binding references an excluded owning record",
                              owner->getQualifiedNameAsString());
              if (ReadInterfaceSPIGroup(owner) != ReadInterfaceSPIGroup(record))
                AddDiagnostic(result, "O3IFC157", "shared friend owners require the same SPI policy",
                              function->getQualifiedNameAsString());
            }
        continue;
      }
      if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(member)) {
        supported &= variable->isStaticDataMember() && !variable->getDescribedVarTemplate();
        continue;
      }
      if (const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(member);
          IsConcreteMemberClassSpecialization(specialization)) {
        supported &= IsBoundedTemplateRecord(specialization,specialization->getSpecializedTemplate());continue;
      }
      if (const auto *type = llvm::dyn_cast<clang::TypeDecl>(member)) {
        if (const auto *tag = llvm::dyn_cast<clang::TagDecl>(type))
          supported &= (tag->getIdentifier() || tag->getTypedefNameForAnonDecl() || IsExpressionLocalTag(tag));
        supported &= IsBoundedCxxDeclaration(type, result, exports);
        continue;
      }
      const auto *method = llvm::dyn_cast<clang::CXXMethodDecl>(member);
      supported &=
          method && (!method->isTemplated() ||
              (dependent_factory_local && !method->getDescribedFunctionTemplate() && !method->getPrimaryTemplate())) &&
          (!method->isDefaulted() || IsSupportedDefaultedSpecialMember(method) ||
           IsInClassDefaultedComparison(method)) &&
          !method->getTrailingRequiresClause();
      if (const auto *ctor =
              llvm::dyn_cast_or_null<clang::CXXConstructorDecl>(method))
        supported &= ctor->getExplicitSpecifier().getExpr() == nullptr;
      if (const auto *conversion =
              llvm::dyn_cast_or_null<clang::CXXConversionDecl>(method))
        supported &= conversion->getExplicitSpecifier().getExpr() == nullptr;
    }
  }
  if (const auto *method = llvm::dyn_cast<clang::CXXMethodDecl>(declaration))
    if (method->getMostRecentDecl()->isDefaulted())
      supported &= IsSupportedDefaultedSpecialMember(method) || IsInClassDefaultedComparison(method);
  if (IsTopLevel(declaration)) {
    supported &=
        !llvm::isa<clang::TemplateDecl>(declaration) ||
        llvm::isa<clang::ConceptDecl, clang::FunctionTemplateDecl, clang::TypeAliasTemplateDecl,
                   clang::VarTemplateDecl, clang::ClassTemplateDecl>(declaration);
    if (const auto *pattern=llvm::dyn_cast<clang::TemplateDecl>(declaration))
      supported &= IsBoundedNestedTemplateParameters(pattern->getTemplateParameters());
    if (const auto *specialization=llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(declaration)) {
      supported &= IsExplicitNamespaceVariableSpecialization(specialization) &&
          specialization->isExternallyVisible() && !specialization->getType()->isDependentType();
      std::string key;
      for (const auto &argument:specialization->getTemplateArgs().asArray())
        supported &= GenericArgumentKey(argument,specialization->getASTContext(),key);
    }
    if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(declaration)) {
      supported &= IsBoundedTemplateRecord(pattern->getTemplatedDecl(),pattern);
      for (const auto *specialization:PublishedClassSpecializations(pattern)) {
        supported &= IsBoundedTemplateRecord(specialization,pattern);
        std::string key;
        supported &= ClassSpecializationArgumentKey(specialization,declaration->getASTContext(),key);
        if (const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(specialization))
          supported &= IsBoundedNestedTemplateParameters(partial->getTemplateParameters());
      }
    }
    if (const auto *pattern = llvm::dyn_cast<clang::VarTemplateDecl>(declaration)) {
      const auto *definition=pattern->getTemplatedDecl()->getDefinition();
      const auto *variable=definition ? definition : pattern->getTemplatedDecl();
      supported &= variable->isThisDeclarationADefinition()!=clang::VarDecl::DeclarationOnly &&
          !variable->isStaticDataMember();
      for (const auto *partial:PublishedVariablePartials(pattern)) supported &= IsBoundedVariablePartial(partial);
      for (const auto *attribute:variable->attrs())
        if (const auto *tag=llvm::dyn_cast<clang::AbiTagAttr>(attribute))
          supported &= tag->tags_size()==1 && tag->tags_begin()->starts_with("__objc3_m_") &&
              ReadNativeSymbolName(pattern)==*tag->tags_begin();
        else supported &= (attribute->isImplicit() || IsPublishedNativeAttribute(attribute) || IsForeignImportAttribute(attribute));
    }
    if (const auto *pattern = llvm::dyn_cast<clang::TypeAliasTemplateDecl>(declaration)) {
      const auto *alias = pattern->getTemplatedDecl();
      for (const auto *attribute : alias->attrs())
        if (const auto *annotation=llvm::dyn_cast<clang::AnnotateAttr>(attribute);
            annotation && annotation->getAnnotation().starts_with("objc3.alias-owner:"))
          supported &= annotation->args_size()==0 &&
              annotation->getAnnotation().drop_front(sizeof("objc3.alias-owner:")-1)==ReadNativeSymbolName(pattern);
        else supported &= (attribute->isImplicit() || IsPublishedNativeAttribute(attribute) || IsForeignImportAttribute(attribute));
    }
    if (const auto *pattern = llvm::dyn_cast<clang::FunctionTemplateDecl>(declaration)) {
      const auto *function = pattern->getTemplatedDecl();
      supported &= function->getDefinition() != nullptr &&
          !llvm::isa<clang::CXXMethodDecl>(function) && !function->isDeleted() &&
          !function->isDefaulted();
      for (const auto *attribute : function->attrs()) {
        if (const auto *tag = llvm::dyn_cast<clang::AbiTagAttr>(attribute))
          supported &= tag->tags_size() == 1 &&
              tag->tags_begin()->starts_with("__objc3_m_") &&
              ReadNativeSymbolName(pattern) == *tag->tags_begin();
        else
          supported &= (attribute->isImplicit() || IsPublishedNativeAttribute(attribute) || IsForeignImportAttribute(attribute)) ||
              llvm::isa<clang::DeprecatedAttr, clang::UnavailableAttr,
                        clang::AvailabilityAttr>(attribute);
      }
      for (const auto *parameter : function->parameters())
        for (const auto *attribute : parameter->attrs()) supported &= (attribute->isImplicit() || IsPublishedNativeAttribute(attribute) || IsForeignImportAttribute(attribute));
    }
    if (const auto *space = llvm::dyn_cast<clang::NamespaceDecl>(declaration)) {
      const auto original = NamespaceAttributes(space);
      for (const auto *attribute : space->attrs())
        if (!attribute->isImplicit()) {
          supported &= IsModeledNamespaceAttribute(attribute);
          // Namespace defaults must be stable across extensions. Applying a
          // later attribute retroactively can change earlier symbol visibility.
          if (!attribute->isInherited())
            supported &= original.count(NamespaceAttributeText(attribute, space->getASTContext()));
        }
    }
    if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration);
        function && function->isExternallyVisible()) {
      bool specialization=IsExplicitNamespaceFunctionSpecialization(function);
      if (specialization) {
        std::string key;
        specialization &= !function->isDeleted() && !function->isDefaulted() &&
            function->getTemplateSpecializationArgs();
        if (specialization)
          for (const auto &argument:function->getTemplateSpecializationArgs()->asArray())
            specialization &= GenericArgumentKey(argument,function->getASTContext(),key);
      }
      supported &= ((!function->getPrimaryTemplate() && !function->isTemplated()) || specialization) &&
                   !function->getTrailingRequiresClause() &&
                   (!function->isDefaulted() || IsInClassDefaultedComparison(function));
    }
  }
  if (!supported)
    AddDiagnostic(result, "O3IFC101",
                  "C++ declaration exceeds the implemented semantic interface contract; "
                  "public dependent templates and unnamed nested types require "
                  "dedicated publication support",
                  declaration->getQualifiedNameAsString());
  return supported;
}

}  // namespace emission_detail

}  // namespace objc3::clang_foundation
