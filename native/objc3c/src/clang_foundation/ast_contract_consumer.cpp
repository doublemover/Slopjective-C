#include "clang_foundation/ast_contract_consumer.h"
#include "clang_foundation/ast_effect_identity.h"
#include "clang_foundation/interface_emitter.h"

#include <algorithm>
#include "clang/AST/ASTContext.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclObjC.h"
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

  bool VisitObjCMethodDecl(clang::ObjCMethodDecl *decl) {
    if (!context_ || decl->isImplicit() ||
        !context_->getSourceManager().isWrittenInMainFile(decl->getLocation()))
      return true;
    InterfaceDeclarationIdentity identity;
    if (ReadInterfaceDeclarationIdentity(decl, *context_, identity))
      result_.declarations.push_back(std::move(identity));
    return true;
  }

  bool VisitNamedDecl(clang::NamedDecl *decl) {
    if (decl->isImplicit() || !decl->getIdentifier() || !context_ ||
        !context_->getSourceManager().isWrittenInMainFile(decl->getLocation()))
      return true;
    if (auto *function = llvm::dyn_cast<clang::FunctionDecl>(decl)) {
      if (!function->isExternallyVisible()) return true;
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
    result_.declarations.push_back(std::move(identity));
    return true;
  }

  void HandleTranslationUnit(clang::ASTContext &context) override {
    if (context.getDiagnostics().hasErrorOccurred()) return;
    context_ = &context;
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
          request_.interface_exports);
    else if (request_.emit_semantic_interface)
      result_.interface_artifact = EmitSemanticInterface(
          context, request_.module_name, request_.package_identity,
          result_.resolved_interface_dependencies,
          result_.resolved_interface_sources, {}, request_.interface_exports);
  }

 private:
  const CompileRequest &request_;
  CompileResult &result_;
  clang::ASTContext *context_ = nullptr;
};
}  // namespace

std::unique_ptr<clang::ASTConsumer> MakeContractConsumer(
    const CompileRequest &request, CompileResult &result) {
  return std::make_unique<ContractConsumer>(request, result);
}
}  // namespace objc3::clang_foundation
