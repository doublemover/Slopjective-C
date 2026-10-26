#include "clang_foundation/module_names.h"
#include "clang_foundation/interface_codec.h"
#include "clang_foundation/interface_emitter.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Attr.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/DeclCXX.h"
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
  if (declaration.native_symbol_name.empty())
    return true;
  if (declaration.kind != "Function" &&
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
        (!decl->getIdentifier() && !llvm::isa<clang::CXXMethodDecl>(decl)) ||
        !IsModuleOwnedSource(context_, decl->getLocation(), source_root_))
      return true;
    const auto *owner = decl->getDeclContext();
    while (owner && llvm::isa<clang::LinkageSpecDecl>(owner))
      owner = owner->getParent();
    if (!owner || (!owner->isTranslationUnit() &&
                   !llvm::isa<clang::NamespaceDecl, clang::CXXRecordDecl>(owner)))
      return true;
    if (llvm::isa<clang::ObjCCompatibleAliasDecl>(decl))
      return Error(
          decl,
          "runtime aliases cannot be introduced by a module with owned names");
    const bool category = llvm::isa<clang::ObjCCategoryDecl>(decl);
    if (!category && !decl->isExternallyVisible())
      return true;
    const bool value = llvm::isa<clang::FunctionDecl, clang::VarDecl>(decl);
    const bool runtime =
        llvm::isa<clang::ObjCInterfaceDecl, clang::ObjCProtocolDecl,
                  clang::ObjCCategoryDecl>(decl);
    auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(decl);
    if (!value && !runtime && !record)
      return true;
    if (record && (record->isDependentContext() ||
                   record->getDescribedClassTemplate() ||
                   record->getTemplateSpecializationKind() !=
                       clang::TSK_Undeclared))
      return true;
    if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(decl))
      if (function->isDeleted() || function->getDescribedFunctionTemplate() ||
          function->getType()->isDependentType())
        return true;
    if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(decl))
      if (variable->getDescribedVarTemplate() ||
          variable->getType()->isDependentType())
        return true;
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
