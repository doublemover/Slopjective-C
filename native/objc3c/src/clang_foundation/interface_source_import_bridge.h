#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "clang_foundation/interface_contract.h"
#include "clang_foundation/interface_import.h"
#include "clang_foundation/interface_module_resolver.h"
#include "clang/Sema/ExternalSemaSource.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "clang/Basic/SourceLocation.h"

namespace clang {
class LookupResult;
class FileManager;
class IdentifierInfo;
class NamedDecl;
class Scope;
class Sema;
}

namespace objc3::clang_foundation {

enum class SourceModuleImportStatus { Imported, AlreadyVisible, NotRegistered, Rejected };
enum class QualifiedModuleLookupStatus { Found, ModuleNotVisible, NotFound, Ambiguous };

struct QualifiedModuleLookup {
  QualifiedModuleLookupStatus status = QualifiedModuleLookupStatus::NotFound;
  std::string canonical_module_name;
  std::string identifier;
  std::vector<clang::NamedDecl *> declarations;
};

// Position-sensitive lookup adapter for ObjC3 source `@import` and
// `@Module.Path.Identifier`. The declarations are already imported by Clang's
// ASTImporter into the consumer ASTContext. This class only controls which
// imported declarations Sema may see at each source location.
class InterfaceSourceImportBridge final : public clang::ExternalSemaSource {
public:
  InterfaceSourceImportBridge() = default;
  InterfaceSourceImportBridge(
      const std::vector<InterfaceModuleRequest> &registry,
      const InterfaceImportReport &imported,
      const std::vector<InterfaceSourceArtifact> &artifacts);

  void ConfigureSourceRegistry(
      const std::vector<InterfaceModuleRequest> &registry,
      const InterfaceImportReport &imported,
      const std::vector<InterfaceSourceArtifact> &artifacts);
  void ConfigureOnDemand(
      const std::vector<std::string> &search_roots,
      const std::vector<InterfaceModuleRequest> &registry,
      const InterfaceImportTarget &target, clang::FileManager &file_manager);
  void InitializeSema(clang::Sema &sema) override;

  [[nodiscard]] bool IsRegistered(llvm::StringRef canonical_name) const;
  [[nodiscard]] SourceModuleImportStatus Activate(
      llvm::StringRef canonical_name);
  [[nodiscard]] bool IsVisible(llvm::StringRef canonical_name) const;

  [[nodiscard]] std::vector<clang::NamedDecl *> LookupUnqualified(
      llvm::StringRef identifier, unsigned identifier_namespace) const;
  [[nodiscard]] QualifiedModuleLookup LookupQualified(
      llvm::StringRef canonical_module_name, llvm::StringRef identifier,
      unsigned identifier_namespace) const;
  [[nodiscard]] QualifiedModuleLookup LookupQualifiedSpelling(
      llvm::ArrayRef<llvm::StringRef> path,
      unsigned identifier_namespace) const;

  [[nodiscard]] std::vector<std::string> GetVisibleModules() const;
  [[nodiscard]] std::vector<InterfaceModuleIdentity>
  GetImportedModuleIdentities() const;
  [[nodiscard]] std::vector<InterfaceSourceArtifact>
  GetResolvedSourceArtifacts() const;
  [[nodiscard]] std::vector<InterfaceDeclarationIdentity>
  GetVisibleDeclarationIdentities() const;
  [[nodiscard]] std::vector<InterfaceDependency> GetDirectDependencies() const;
  [[nodiscard]] std::vector<std::string> GetDependencyOrder() const;
  [[nodiscard]] const std::vector<InterfaceContractDiagnostic> &
  GetDiagnostics() const { return diagnostics_; }
  [[nodiscard]] llvm::StringRef GetLastFailure() const { return last_failure_; }

  // These entry points are called by the narrow ObjC3 Parser/Sema hook. A
  // NotRegistered import must continue through Clang's native ModuleLoader;
  // Rejected means an extension artifact was selected but failed validation.
  [[nodiscard]] SourceModuleImportStatus ImportObjC3Module(
      llvm::StringRef canonical_name,
      clang::SourceLocation import_location = {});
  void DiagnoseImportFailure(clang::SourceLocation import_location);
  [[nodiscard]] QualifiedModuleLookup LookupObjC3QualifiedName(
      llvm::StringRef canonical_module_name, llvm::StringRef identifier,
      unsigned identifier_namespace) const;
  clang::ObjC3SourceImportResult HandleObjC3SourceImport(
      llvm::StringRef canonical_module_name,
      clang::SourceLocation import_location) override;

  // Clang's parser/Sema callback passes one candidate split at a time: all
  // components except the final identifier form the canonical module path.
  clang::ObjC3QualifiedNameLookupStatus LookupObjC3QualifiedName(
      llvm::ArrayRef<clang::IdentifierInfo *> components,
      unsigned identifier_namespace,
      llvm::SmallVectorImpl<clang::NamedDecl *> &declarations) override;

  // C IdentifierResolver does not see ASTImporter insertions. This fallback
  // supplies only currently visible imported names.
  bool LookupUnqualified(clang::LookupResult &result,
                         clang::Scope *scope) override;

  void AddInitiallyVisible(clang::NamedDecl *declaration);

private:
  void Reset();
  void RecordDirectImport(llvm::StringRef canonical_name);
  bool AddVisibleClosure(llvm::StringRef canonical_name);
  void AppendModuleDeclarations(llvm::StringRef canonical_name,
                               llvm::StringRef identifier,
                               unsigned identifier_namespace,
                               std::vector<clang::NamedDecl *> &out) const;
  bool ValidateVisibleExports(llvm::StringRef importing_module,
                              const std::map<std::string, InterfaceSourceArtifact> &artifacts,
                              const std::set<std::string> &visible,
                              std::string &reason) const;

  std::set<std::string> registry_;
  std::map<std::string, InterfaceModuleRequest> requests_;
  std::map<std::string, std::vector<clang::NamedDecl *>> declarations_;
  std::map<std::string, std::set<std::string>> declaration_ids_;
  std::map<std::string, InterfaceSourceArtifact> artifacts_;
  std::set<std::string> directly_imported_;
  std::set<std::string> visible_;
  std::map<const clang::IdentifierInfo *, std::vector<clang::NamedDecl *>>
      initially_visible_declarations_;
  bool valid_ = false;
  bool on_demand_ = false;
  std::vector<std::string> search_roots_;
  InterfaceImportTarget target_;
  clang::FileManager *file_manager_ = nullptr;
  clang::Sema *sema_ = nullptr;
  clang::ASTContext *import_context_ = nullptr;
  std::shared_ptr<clang::ASTImporterSharedState> importer_state_;
  std::vector<std::unique_ptr<InterfaceImportSession>> sessions_;
  std::vector<InterfaceModuleIdentity> visible_identities_;
  std::vector<InterfaceDeclarationIdentity> imported_identities_;
  std::vector<InterfaceDependency> direct_dependencies_;
  std::vector<std::string> dependency_order_;
  std::set<std::string> recorded_identities_;
  std::vector<InterfaceContractDiagnostic> diagnostics_;
  std::string last_failure_;
};

} // namespace objc3::clang_foundation
