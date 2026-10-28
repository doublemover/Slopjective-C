#include "clang_foundation/native_contract_identity.h"
#include "clang_foundation/ast_contract_consumer.h"
#include "clang_foundation/ast_effect_identity.h"
#include "clang_foundation/interface_emitter.h"
#include "clang_foundation/module_names.h"

#include <algorithm>
#include "clang/AST/ASTContext.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceManager.h"
#include "clang/UnifiedSymbolResolution/USRGeneration.h"
#include "llvm/ADT/SmallString.h"

namespace objc3::clang_foundation {
namespace {
class ContractConsumer final : public clang::ASTConsumer,
                               public clang::RecursiveASTVisitor<ContractConsumer> {
 public:
  ContractConsumer(const CompileRequest &request, CompileResult &result)
      : request_(request), result_(result) {}

  void Initialize(clang::ASTContext &context) override {
    context_ = &context;
    if (request_.module_owned_names) {
      source_root_ =
          NormalizeModuleSourceRoot(context, request_.module_source_root);
      const auto main = context.getSourceManager().getLocForStartOfFile(
          context.getSourceManager().getMainFileID());
      if (!IsModuleOwnedSource(context, main, source_root_)) {
        const auto id = context.getDiagnostics().getCustomDiagID(
            clang::DiagnosticsEngine::Error, "%0");
        context.getDiagnostics().Report(main, id)
            << "module source root does not contain the compilation input";
      }
    }
  }
  void
  HandleInlineFunctionDefinition(clang::FunctionDecl *declaration) override {
    if (request_.module_owned_names)
      (void)ApplyModuleOwnedNames(*context_, declaration, request_.module_name,
                                  request_.package_identity, source_root_);
  }
  void HandleCXXImplicitFunctionInstantiation(
      clang::FunctionDecl *declaration) override {
    HandleInlineFunctionDefinition(declaration);
  }
  bool HandleTopLevelDecl(clang::DeclGroupRef group) override {
    if (request_.module_owned_names)
      for (auto *declaration : group)
        if (!ApplyModuleOwnedNames(*context_, declaration, request_.module_name,
                                   request_.package_identity, source_root_))
          return false;
    return true;
  }
  bool VisitObjCMethodDecl(clang::ObjCMethodDecl *decl) {
    if (!context_ || decl->isImplicit() ||
        !IsModuleOwnedSource(*context_, decl->getLocation(), source_root_))
      return true;
    InterfaceDeclarationIdentity identity;
    if (ReadInterfaceDeclarationIdentity(decl, *context_, identity))
      result_.declarations.push_back(std::move(identity));
    return true;
  }

  bool VisitNamedDecl(clang::NamedDecl *decl) {
    if (decl->isImplicit() || !decl->getIdentifier() || !context_ ||
        !IsModuleOwnedSource(*context_, decl->getLocation(), source_root_))
      return true;
    if (const auto *alias = llvm::dyn_cast<clang::TypeAliasDecl>(decl);
        alias && alias->getDescribedAliasTemplate()) return true;
    if (const auto *variable=llvm::dyn_cast<clang::VarDecl>(decl);
        variable && variable->getDescribedVarTemplate()) return true;
    if (llvm::isa<clang::ConceptDecl, clang::FunctionTemplateDecl, clang::TypeAliasTemplateDecl,
                  clang::VarTemplateDecl>(decl)) {
      InterfaceDeclarationIdentity identity;
      if (ReadInterfaceDeclarationIdentity(decl, *context_, identity))
        result_.declarations.push_back(std::move(identity));
      return true;
    }
    if (auto *function = llvm::dyn_cast<clang::FunctionDecl>(decl)) {
      if (function->getDescribedFunctionTemplate() || !function->isExternallyVisible()) return true;
      InterfaceDeclarationIdentity identity;
      if (ReadInterfaceDeclarationIdentity(function, *context_, identity))
        result_.declarations.push_back(std::move(identity));
      return true;
    } else if (auto *variable = llvm::dyn_cast<clang::VarDecl>(decl)) {
      if (!variable->hasGlobalStorage() || !variable->isExternallyVisible()) return true;
    } else if (!llvm::isa<clang::TypeDecl, clang::ObjCContainerDecl>(decl)) {
      return true;
    }
    llvm::SmallString<128> usr;
    if (clang::index::generateUSRForDecl(decl, usr)) return true;
    InterfaceDeclarationIdentity identity;
    identity.stable_id = usr.str().str();
    identity.kind = decl->getDeclKindName();
    identity.qualified_name = decl->getQualifiedNameAsString();
    identity.export_root = ReadInterfaceExportRoot(decl);
    clang::QualType type;
    if (auto *value = llvm::dyn_cast<clang::ValueDecl>(decl)) type = value->getType();
    if (auto *type_decl = llvm::dyn_cast<clang::TypeDecl>(decl))
      type = context_->getTypeDeclType(type_decl);
    if (!type.isNull()) {
      identity.effects = ReadCanonicalFunctionEffects(type, *context_);
      clang::PrintingPolicy policy(context_->getLangOpts());
      identity.canonical_type_identity = type.getCanonicalType().getAsString(policy);
      if (type.getObjCLifetime() != clang::Qualifiers::OCL_None)
        identity.effects.ownership_identity = std::to_string(type.getObjCLifetime());
    }
    const auto ownership = ReadNativeReturnOwnership(decl);
    if (!ownership.empty()) identity.effects.ownership_identity = ownership;
    ReadNativeDeclarationContracts(decl, *context_, identity.effects);
    identity.native_symbol_name = ReadNativeSymbolName(decl);
    identity.spi_group = ReadInterfaceSPIGroup(decl);
    result_.declarations.push_back(std::move(identity));
    return true;
  }

  void HandleTranslationUnit(clang::ASTContext &context) override {
    if (context.getDiagnostics().hasErrorOccurred()) return;
    context_ = &context;
    if (request_.emit_semantic_interface && context.getLangOpts().CPlusPlus &&
        !ApplyCxxConceptOwners(context, request_.module_name, request_.package_identity, source_root_)) {
      const auto id = context.getDiagnostics().getCustomDiagID(
          clang::DiagnosticsEngine::Error, "concept owner conflicts with its semantic module");
      context.getDiagnostics().Report(id);
      return;
    }
    TraverseDecl(context.getTranslationUnitDecl());
    auto &declarations = result_.declarations;
    std::sort(declarations.begin(), declarations.end(), [](const auto &a, const auto &b) {
      return a.stable_id < b.stable_id;
    });
    declarations.erase(std::unique(declarations.begin(), declarations.end(),
        [](const auto &a, const auto &b) { return a.stable_id == b.stable_id; }),
        declarations.end());
    if (request_.emit_semantic_interface && request_.projected_arc.has_value())
      result_.interface_artifact = EmitOwnershipProjection(
          context, request_.module_name, request_.package_identity,
          *request_.projected_arc, result_.resolved_interface_dependencies,
          request_.interface_exports, source_root_);
    else if (request_.emit_semantic_interface)
      result_.interface_artifact = EmitSemanticInterface(
          context, request_.module_name, request_.package_identity,
          result_.resolved_interface_dependencies,
          result_.resolved_interface_sources, {}, request_.interface_exports,
          source_root_);
  }

 private:
  const CompileRequest &request_;
  CompileResult &result_;
  clang::ASTContext *context_ = nullptr;
  std::string source_root_;
};
}  // namespace

std::unique_ptr<clang::ASTConsumer> MakeContractConsumer(
    const CompileRequest &request, CompileResult &result) {
  return std::make_unique<ContractConsumer>(request, result);
}
}  // namespace objc3::clang_foundation
