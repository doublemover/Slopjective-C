#include "clang_foundation/module_names.h"
#include "clang_foundation/interface_codec.h"
#include "clang_foundation/interface_emitter.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Attr.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Expr.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/Support/VirtualFileSystem.h"
#include <filesystem>

namespace objc3::clang_foundation {
std::string BuildModuleOwnedName(const std::string &module,
                                 const std::string &package,
                                 const std::string &stable_id) {
  auto field = [](const std::string &value) {
    return std::to_string(value.size()) + ":" + value;
  };
  return "__objc3_m_" +
         ComputeInterfaceSourceSha256(field(module) + field(package)) + "_" +
         ComputeInterfaceSourceSha256(stable_id);
}
bool HasValidModuleOwnedName(const InterfaceModuleIdentity &module,
                             const InterfaceDeclarationIdentity &declaration) {
  if (declaration.kind == "Concept") {
    std::string owner = module.canonical_name;
    if (owner.ends_with(".__ownership_producer"))
      owner.resize(owner.size() - std::string(".__ownership_producer").size());
    return declaration.native_symbol_name.empty() &&
        declaration.canonical_type_identity.starts_with(
            "concept{owner=" + BuildModuleOwnedName(owner, module.package_identity,
                                                    declaration.stable_id) + ";");
  }
  if (declaration.native_symbol_name.empty())
    return true;
  if (declaration.kind != "Function" && declaration.kind != "FunctionTemplate" &&
      declaration.kind != "VarTemplate" && declaration.kind != "VarTemplateSpecialization" && declaration.kind != "ClassTemplate" &&
      declaration.kind != "TypeAliasTemplate" &&
      !IsCxxMemberDeclarationKind(declaration.kind) &&
      declaration.kind != "Var" && declaration.kind != "CXXRecord" &&
      declaration.kind != "ObjCInterface" &&
      declaration.kind != "ObjCProtocol" && declaration.kind != "ObjCCategory")
    return false;
  std::string owner = module.canonical_name;
  if (owner.ends_with(".__ownership_producer"))
    owner.resize(owner.size() - std::string(".__ownership_producer").size());
  return declaration.native_symbol_name ==
         BuildModuleOwnedName(owner, module.package_identity,
                              declaration.stable_id);
}
namespace {
std::filesystem::path SourceFile(const clang::ASTContext &context,
                                 clang::SourceLocation location) {
  const auto &manager = context.getSourceManager();
  const auto file = manager.getFileEntryRefForID(
      manager.getFileID(manager.getExpansionLoc(location)));
  if (!file)
    return {};
  const auto real = file->getFileEntry().tryGetRealPathName();
  return std::filesystem::path((real.empty() ? file->getName() : real).str())
      .lexically_normal();
}
} // namespace
std::string NormalizeModuleSourceRoot(const clang::ASTContext &context,
                                      const std::string &root) {
  namespace fs = std::filesystem;
  fs::path path(root);
  if (root.empty())
    path = SourceFile(context, context.getSourceManager().getLocForStartOfFile(
                                   context.getSourceManager().getMainFileID()))
               .parent_path();
  if (path.is_relative()) {
    auto cwd = context.getSourceManager()
                   .getFileManager()
                   .getVirtualFileSystem()
                   .getCurrentWorkingDirectory();
    if (cwd)
      path = fs::path(*cwd) / path;
  }
  std::error_code error;
  auto canonical = fs::weakly_canonical(path, error);
  return (error ? path.lexically_normal() : canonical).string();
}
bool IsModuleOwnedSource(const clang::ASTContext &context,
                         clang::SourceLocation location,
                         const std::string &root) {
  if (!location.isValid())
    return false;
  const auto &manager = context.getSourceManager();
  location = manager.getExpansionLoc(location);
  if (root.empty())
    return manager.isWrittenInMainFile(location);
  const auto file = SourceFile(context, location);
  if (file.empty())
    return false;
  if (!manager.isWrittenInMainFile(location) && file.extension() == ".objc3i")
    return false;
  const auto relative = file.lexically_relative(std::filesystem::path(root));
  return !relative.empty() && !relative.is_absolute() &&
         *relative.begin() != "..";
}
std::string ReadNativeSymbolName(const clang::NamedDecl *declaration) {
  if (const auto *specialization = llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(declaration))
    return ReadNativeSymbolName(specialization->getSpecializedTemplate());
  if (const auto *pattern = llvm::dyn_cast<clang::ClassTemplateDecl>(declaration))
    declaration = pattern->getTemplatedDecl();
  if (const auto *pattern = llvm::dyn_cast<clang::TypeAliasTemplateDecl>(declaration))
    declaration = pattern->getTemplatedDecl();
  if (const auto *alias = llvm::dyn_cast<clang::TypeAliasDecl>(declaration);
      alias && alias->getDescribedAliasTemplate()) {
    for (const auto *annotation:alias->specific_attrs<clang::AnnotateAttr>())
      if (annotation->args_size()==0 && annotation->getAnnotation().starts_with("objc3.alias-owner:"))
        return annotation->getAnnotation().drop_front(sizeof("objc3.alias-owner:")-1).str();
    // Member alias templates inherit the nominal owner and enclosing template
    // arguments from their record context; they have no independent tag.
    for (const auto *scope=alias->getDeclContext();scope;scope=scope->getParent())
      if (const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(scope))
        if (auto owner=ReadNativeSymbolName(record);!owner.empty()) return owner;
  }
  if (const auto *specialization = llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(declaration)) {
    if (specialization->getSpecializationKind()==clang::TSK_ExplicitSpecialization &&
        !specialization->isStaticDataMember()) {
      if (const auto *label=specialization->getAttr<clang::AsmLabelAttr>()) return label->getLabel().str();
      return {};
    }
    return ReadNativeSymbolName(specialization->getSpecializedTemplate());
  }
  if (const auto *pattern = llvm::dyn_cast<clang::VarTemplateDecl>(declaration))
    declaration = pattern->getTemplatedDecl();
  if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration))
    if (variable->getDescribedVarTemplate())
      if (const auto *tag = variable->getAttr<clang::AbiTagAttr>();
          tag && tag->tags_size()==1 && tag->tags_begin()->starts_with("__objc3_m_"))
        return tag->tags_begin()->str();
  if (const auto *pattern = llvm::dyn_cast<clang::FunctionTemplateDecl>(declaration))
    declaration = pattern->getTemplatedDecl();
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration))
    if (const auto *pattern = function->getPrimaryTemplate()) {
      if (function->getTemplateSpecializationKind()==clang::TSK_ExplicitSpecialization &&
          !llvm::isa<clang::CXXMethodDecl>(function)) {
        if (const auto *label=function->getAttr<clang::AsmLabelAttr>()) return label->getLabel().str();
        return {};
      }
      return ReadNativeSymbolName(pattern);
    }
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration))
    if (function->getDescribedFunctionTemplate())
      if (const auto *tag = function->getAttr<clang::AbiTagAttr>();
          tag && tag->tags_size() == 1 && tag->tags_begin()->starts_with("__objc3_m_"))
        return tag->tags_begin()->str();
  if (const auto *label = declaration->getAttr<clang::AsmLabelAttr>())
    return label->getLabel().str();
  if (const auto *name = declaration->getAttr<clang::ObjCRuntimeNameAttr>())
    return name->getMetadataName().str();
  if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration))
    if (const auto *tag = record->getCanonicalDecl()->getAttr<clang::AbiTagAttr>();
        tag && tag->tags_size() == 1 &&
        tag->tags_begin()->starts_with("__objc3_m_"))
      return tag->tags_begin()->str();
  if (llvm::isa<clang::ObjCCategoryDecl>(declaration))
    for (const auto *a : declaration->specific_attrs<clang::AnnotateAttr>())
      if (a->getAnnotation().starts_with("objc3.module-name:"))
        return a->getAnnotation().drop_front(18).str();
  return {};
}
std::string ReadCxxConceptOwner(const clang::NamedDecl *declaration) {
  for (const auto *attribute : declaration->specific_attrs<clang::AnnotateAttr>())
    if (attribute->getAnnotation().starts_with("objc3.concept-owner:"))
      return attribute->getAnnotation().drop_front(sizeof("objc3.concept-owner:") - 1).str();
  return {};
}
bool ApplyCxxConceptOwners(clang::ASTContext &context,
                          const std::string &module,
                          const std::string &package,
                          const std::string &source_root) {
  struct Concepts : clang::RecursiveASTVisitor<Concepts> {
    clang::ASTContext &context;
    const std::string &module, &package, &source_root;
    Concepts(clang::ASTContext &context, const std::string &module,
             const std::string &package, const std::string &source_root)
        : context(context), module(module), package(package), source_root(source_root) {}
    bool VisitConceptDecl(clang::ConceptDecl *declaration) {
      if (!IsModuleOwnedSource(context, declaration->getLocation(), source_root)) return true;
      const auto expected = BuildModuleOwnedName(module, package,
                                                ReadInterfaceDeclarationStableId(declaration));
      unsigned count = 0;
      for (const auto *attribute : declaration->specific_attrs<clang::AnnotateAttr>())
        if (attribute->getAnnotation().starts_with("objc3.concept-owner:")) {
          if (++count > 1 || attribute->args_size() ||
              attribute->getAnnotation().drop_front(sizeof("objc3.concept-owner:") - 1) != expected) {
            return false;
          }
        }
      if (!count)
        declaration->addAttr(clang::AnnotateAttr::Create(
            context, "objc3.concept-owner:" + expected, nullptr, 0,
            declaration->getLocation()));
      return true;
    }
  };
  std::string nominal_module = module;
  if (nominal_module.ends_with(".__ownership_producer"))
    nominal_module.resize(nominal_module.size() - std::string(".__ownership_producer").size());
  Concepts visitor(context, nominal_module, package, source_root);
  return visitor.TraverseDecl(context.getTranslationUnitDecl());
}
namespace {
class NamingVisitor : public clang::RecursiveASTVisitor<NamingVisitor> {
public:
  NamingVisitor(clang::ASTContext &context, const std::string &module,
                const std::string &package, const std::string &source_root)
      : context_(context), module_(module), package_(package),
        source_root_(source_root) {}
  bool VisitDeclRefExpr(clang::DeclRefExpr *expression) {
    return VisitNamedDecl(expression->getDecl());
  }
  bool VisitNamedDecl(clang::NamedDecl *decl) {
    if (decl->isImplicit() ||
        (!decl->getIdentifier() && !llvm::isa<clang::FunctionDecl>(decl)) ||
        !IsModuleOwnedSource(context_, decl->getLocation(), source_root_))
      return true;
    const auto *owner = decl->getDeclContext();
    while (owner && llvm::isa<clang::LinkageSpecDecl>(owner))
      owner = owner->getParent();
    // Closure and local-class members have lexical ABI discriminators, not
    // independent public USRs. The native mangler inherits module ownership
    // from their enclosing function; a direct asm label can collapse distinct
    // closure layouts with the same call-operator signature.
    if (const auto *record = llvm::dyn_cast_or_null<clang::CXXRecordDecl>(owner);
        record && (record->isLambda() || record->isLocalClass()))
      return true;
    if (!owner || (!owner->isTranslationUnit() &&
                   !llvm::isa<clang::NamespaceDecl, clang::CXXRecordDecl>(owner)))
      return true;
    if (llvm::isa<clang::ObjCCompatibleAliasDecl>(decl))
      return Error(
          decl,
          "runtime aliases cannot be introduced by a module with owned names");
    const bool category = llvm::isa<clang::ObjCCategoryDecl>(decl);
    const auto *variable_pattern = llvm::dyn_cast<clang::VarDecl>(decl);
    const bool variable_template = variable_pattern &&
        (variable_pattern->getDescribedVarTemplate() ||
         llvm::isa<clang::VarTemplateSpecializationDecl>(decl));
    auto *alias=llvm::dyn_cast<clang::TypeAliasDecl>(decl);
    const auto *alias_pattern=alias?alias->getDescribedAliasTemplate():nullptr;
    if (!category && !decl->isExternallyVisible() && !variable_template && !alias_pattern)
      return true;
    if (alias_pattern) {
      if (llvm::isa<clang::CXXRecordDecl>(alias->getDeclContext())) return true;
      const auto expected=BuildModuleOwnedName(module_,package_,ReadInterfaceDeclarationStableId(alias_pattern));
      for (const auto *redecl:alias_pattern->redecls()) {
        auto *definition=redecl->getTemplatedDecl();
        if (!IsModuleOwnedSource(context_,definition->getLocation(),source_root_))
          return Error(decl,"module-owned alias template redeclares a foreign ABI");
        unsigned count=0;
        for (const auto *annotation:definition->specific_attrs<clang::AnnotateAttr>())
          if (annotation->getAnnotation().starts_with("objc3.alias-owner:"))
            if (++count!=1 || annotation->args_size()!=0 ||
                annotation->getAnnotation().drop_front(sizeof("objc3.alias-owner:")-1)!=expected)
              return Error(decl,"alias template ABI owner conflicts with its module/package");
        if (!count) definition->addAttr(clang::AnnotateAttr::Create(
            context_,"objc3.alias-owner:"+expected,nullptr,0,definition->getLocation()));
      }
      return true;
    }
    const bool value = llvm::isa<clang::FunctionDecl, clang::VarDecl>(decl);
    const bool runtime =
        llvm::isa<clang::ObjCInterfaceDecl, clang::ObjCProtocolDecl,
                  clang::ObjCCategoryDecl>(decl);
    auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(decl);
    if (!value && !runtime && !record)
      return true;
    // Members and specializations retain native template mangling. A concrete
    // asm label on a pattern member would alias every specialization.
    for (const auto *scope = decl->getDeclContext(); scope; scope = scope->getParent())
      if (const auto *parent = llvm::dyn_cast<clang::CXXRecordDecl>(scope);
          parent && (parent->getDescribedClassTemplate() ||
                     llvm::isa<clang::ClassTemplateSpecializationDecl>(parent)))
        return true;
    if (record && record->getDescribedClassTemplate()) {
      const auto expected = BuildModuleOwnedName(module_, package_,
          ReadInterfaceDeclarationStableId(record->getDescribedClassTemplate()));
      for (auto *redecl : record->redecls()) {
        if (!redecl->isImplicit() && !IsModuleOwnedSource(context_, redecl->getLocation(), source_root_))
          return Error(decl, "module-owned class template redeclares a foreign ABI");
        if (const auto *tag = redecl->getAttr<clang::AbiTagAttr>()) {
          if (tag->tags_size()!=1 || *tag->tags_begin()!=expected)
            return Error(decl, "class template ABI tag conflicts with its owning module");
        } else {
          llvm::StringRef owned_tag = context_.Idents.get(expected).getName();
          redecl->addAttr(clang::AbiTagAttr::Create(context_, &owned_tag, 1, redecl->getSourceRange()));
        }
      }
      return true;
    }
    if (auto *specialization=llvm::dyn_cast_or_null<clang::ClassTemplateSpecializationDecl>(record);
        specialization && (llvm::isa<clang::ClassTemplatePartialSpecializationDecl>(specialization) ||
        specialization->getSpecializationKind()==clang::TSK_ExplicitSpecialization) &&
        !specialization->getDeclContext()->isRecord()) {
      const auto expected=BuildModuleOwnedName(module_,package_,
          ReadInterfaceDeclarationStableId(specialization->getSpecializedTemplate()));
      for (auto *redecl:specialization->redecls()) {
        if (!redecl->isImplicit() && !IsModuleOwnedSource(context_,redecl->getLocation(),source_root_))
          return Error(decl,"explicit class specialization redeclares a foreign ABI");
        if (const auto *tag=redecl->getAttr<clang::AbiTagAttr>()) {
          if (tag->tags_size()!=1 || *tag->tags_begin()!=expected)
            return Error(decl,"explicit class specialization conflicts with its primary owner");
        } else {
          llvm::StringRef owned=context_.Idents.get(expected).getName();
          redecl->addAttr(clang::AbiTagAttr::Create(context_,&owned,1,redecl->getSourceRange()));
        }
      }
      return true;
    }
    if (record && (record->isDependentContext() ||
                   record->getDescribedClassTemplate() ||
                   record->getTemplateSpecializationKind() !=
                       clang::TSK_Undeclared))
      return true;
    if (auto *function = llvm::dyn_cast<clang::FunctionDecl>(decl)) {
      if (function->getPrimaryTemplate()) {
        const bool explicit_namespace=!llvm::isa<clang::CXXMethodDecl>(function) &&
            function->getTemplateSpecializationKind()==clang::TSK_ExplicitSpecialization;
        if (!explicit_namespace) return true;
        if (ReadInterfaceDeclarationStableId(function).empty())
          return Error(decl,"explicit function specialization exceeds the module ABI argument contract");
      }
      if (const auto *pattern = function->getDescribedFunctionTemplate()) {
        for (const auto *prior : function->redecls())
          if (!prior->isImplicit() &&
              !IsModuleOwnedSource(context_, prior->getLocation(), source_root_))
            return Error(decl, "module-owned function template redeclares a foreign ABI");
        const auto expected = BuildModuleOwnedName(module_, package_,
            ReadInterfaceDeclarationStableId(pattern));
        for (auto *redecl : function->redecls()) {
          if (redecl->hasAttr<clang::AsmLabelAttr>())
            return Error(decl, "function template cannot carry a concrete asm label");
          if (const auto *tag = redecl->getAttr<clang::AbiTagAttr>()) {
            if (tag->tags_size() != 1 || *tag->tags_begin() != expected)
              return Error(decl, "function template ABI tag conflicts with its owning module");
          } else {
            llvm::StringRef owned_tag = context_.Idents.get(expected).getName();
            redecl->addAttr(clang::AbiTagAttr::Create(
                context_, &owned_tag, 1, redecl->getSourceRange()));
          }
        }
        return true;
      }
      if (function->isDeleted() || function->getType()->isDependentType()) return true;
    }
    if (auto *variable = llvm::dyn_cast<clang::VarDecl>(decl)) {
      if (const auto *specialization=llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(variable)) {
        if (llvm::isa<clang::VarTemplatePartialSpecializationDecl>(specialization) ||
            specialization->getSpecializationKind()!=clang::TSK_ExplicitSpecialization ||
            specialization->isStaticDataMember()) return true;
        if (ReadInterfaceDeclarationStableId(specialization).empty())
          return Error(decl,"explicit variable specialization exceeds the module ABI argument contract");
      }
      if (const auto *pattern = variable->getDescribedVarTemplate()) {
        if (variable->getStorageClass()==clang::SC_Static) return true;
        const auto expected = BuildModuleOwnedName(module_, package_, ReadInterfaceDeclarationStableId(pattern));
        for (const auto *prior : variable->redecls())
          if (!prior->isImplicit() && !IsModuleOwnedSource(context_,prior->getLocation(),source_root_))
            return Error(decl,"module-owned variable template redeclares a foreign ABI");
        if (variable->hasAttr<clang::AsmLabelAttr>())
          return Error(decl,"variable template cannot carry a concrete asm label");
        if (const auto *tag = variable->getAttr<clang::AbiTagAttr>()) {
          if (tag->tags_size()!=1 || *tag->tags_begin()!=expected)
            return Error(decl,"variable template ABI tag conflicts with its owning module");
        } else {
          llvm::StringRef owned_tag = context_.Idents.get(expected).getName();
          variable->addAttr(clang::AbiTagAttr::Create(context_, &owned_tag, 1, variable->getSourceRange()));
        }
        return true;
      }
      if (variable->getType()->isDependentType()) return true;
    }
    if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(decl);
        function && function->isMain())
      return Error(
          decl,
          "a module with owned names cannot define the platform entry point");
    const auto stable_id = ReadInterfaceDeclarationStableId(decl);
    if (stable_id.empty())
      return Error(decl,
                   "module-owned declaration has no stable native identity");
    const auto expected = BuildModuleOwnedName(module_, package_, stable_id);
    // A local declaration may refine an SDK ABI (for example a retained-return
    // annotation on class_createInstance). Keep that foreign binding. Defining
    // it here would claim another owner's ABI and must fail.
    bool foreign = false;
    if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(decl))
      for (const auto *prior : function->redecls())
        foreign |=
            prior != decl && !prior->isImplicit() &&
            !IsModuleOwnedSource(context_, prior->getLocation(), source_root_);
    if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(decl))
      for (const auto *prior : variable->redecls())
        foreign |=
            prior != decl && !prior->isImplicit() &&
            !IsModuleOwnedSource(context_, prior->getLocation(), source_root_);
    if (record)
      for (const auto *prior : record->redecls())
        foreign |= prior != decl && !prior->isImplicit() &&
                   !IsModuleOwnedSource(context_, prior->getLocation(),
                                        source_root_);
    if (foreign) {
      const bool definition =
          (record && record->isThisDeclarationADefinition()) ||
          (llvm::isa<clang::FunctionDecl>(decl) &&
           llvm::cast<clang::FunctionDecl>(decl)
               ->doesThisDeclarationHaveABody()) ||
          (llvm::isa<clang::VarDecl>(decl) &&
           llvm::cast<clang::VarDecl>(decl)->isThisDeclarationADefinition() !=
               clang::VarDecl::DeclarationOnly);
      if (!definition)
        return true;
      return Error(decl, "module-owned definition redeclares a foreign ABI; "
                         "include its owning header in --module-source-root");
    }
    if (category) {
      unsigned count = 0;
      for (const auto *annotation : decl->specific_attrs<clang::AnnotateAttr>())
        if (annotation->getAnnotation().starts_with("objc3.module-name:")) {
          if (++count != 1 || annotation->args_size() != 0)
            return Error(decl, "module-owned category requires one native name "
                               "without arguments");
        }
    }
    if (record) {
      for (auto *redecl : record->redecls()) {
        if (const auto *tag = redecl->getAttr<clang::AbiTagAttr>()) {
          if (tag->tags_size() != 1 || *tag->tags_begin() != expected)
            return Error(decl, "explicit type ABI tag conflicts with its owning module");
        } else {
          llvm::StringRef owned_tag = context_.Idents.get(expected).getName();
          redecl->addAttr(clang::AbiTagAttr::Create(
              context_, &owned_tag, 1, redecl->getSourceRange()));
        }
      }
      return true;
    }
    const auto prior = ReadNativeSymbolName(decl);
    if (!prior.empty()) {
      if (prior != expected)
        return Error(decl,
                     "explicit native name conflicts with its owning module");
      return true;
    }
    if (value)
      decl->addAttr(
          clang::AsmLabelAttr::Create(context_, expected, decl->getLocation()));
    else if (llvm::isa<clang::ObjCCategoryDecl>(decl))
      decl->addAttr(
          clang::AnnotateAttr::Create(context_, "objc3.module-name:" + expected,
                                      nullptr, 0, decl->getLocation()));
    else
      decl->addAttr(clang::ObjCRuntimeNameAttr::Create(context_, expected,
                                                       decl->getLocation()));
    return true;
  }

private:
  bool Error(clang::NamedDecl *decl, const char *message) {
    const auto id = context_.getDiagnostics().getCustomDiagID(
        clang::DiagnosticsEngine::Error, "%0");
    context_.getDiagnostics().Report(decl->getLocation(), id) << message;
    return false;
  }
  clang::ASTContext &context_;
  const std::string &module_;
  const std::string &package_;
  const std::string &source_root_;
};
} // namespace
bool ApplyModuleOwnedNames(clang::ASTContext &context, clang::Decl *declaration,
                           const std::string &module,
                           const std::string &package,
                           const std::string &source_root) {
  NamingVisitor visitor(context, module, package, source_root);
  return visitor.TraverseDecl(declaration);
}
} // namespace objc3::clang_foundation
