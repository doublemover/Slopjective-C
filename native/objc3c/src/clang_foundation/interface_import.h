#pragma once

#include <memory>
#include <map>
#include <string>
#include <vector>

#include "clang_foundation/interface_contract.h"

namespace llvm {
template <typename T> class ArrayRef;
class StringRef;
}

namespace clang {
class ASTContext;
class ASTImporterSharedState;
class DiagnosticsEngine;
class FileManager;
class NamedDecl;
}

namespace objc3::clang_foundation {

// The lock identity is supplied by the build/package resolver. Artifact paths
// locate bytes but never stand in for this canonical provider identity.
struct InterfaceImportInput {
  std::string source_path;
  std::string sidecar_path; // Empty means source_path + ".json".
  InterfaceModuleIdentity locked_identity;
  // Controls publication to this module's consumers; every direct import is
  // available for lookup while compiling the importing module itself.
  bool public_import = false;
  bool direct_import = true;
  bool reexported = false;
  // Explicit grants to this exact provider; never inherited by dependencies.
  std::vector<std::string> spi_groups;
};

struct InterfaceImportTarget {
  std::string target_triple;
  std::string runtime_abi;
  std::string extension_abi;
  std::string language_revision;
  std::string language_profile;
  std::string objc_runtime;
  std::string objc3_profile;
  bool objc = false;
  bool objc3 = false;
  bool arc = false;
  bool blocks = false;
  bool strict_concurrency = false;
  std::vector<std::string> ast_arguments;
  bool allow_function_overloads = false;
};

struct InterfaceImportReport {
  bool success = false;
  std::vector<InterfaceContractDiagnostic> diagnostics;
  std::vector<InterfaceDependency> direct_dependencies;
  std::vector<std::string> dependency_order;
  std::vector<InterfaceModuleIdentity> resolved_modules;
  std::vector<std::string> visible_module_names;
  // These identities are recorded only after ASTImporter has inserted the
  // corresponding declarations in the consumer ASTContext.
  std::vector<InterfaceDeclarationIdentity> imported_declarations;
  // Verified inline definitions require backend registration as well as Sema
  // insertion. Provider-only code cannot supply consumer-only ODR uses.
  std::vector<clang::NamedDecl *> inline_definitions;
  std::vector<clang::NamedDecl *> visible_lookup_declarations;
  // Imported TU lookup declarations (including unscoped enum constants)
  // grouped by their originating module. Each vector
  // is filtered by that module's own sidecar declaration identities; types
  // re-emitted through a dependency include do not become exports of the
  // including module.
  std::map<std::string, std::vector<clang::NamedDecl *>> declarations_by_module;
};

class InterfaceImportSession {
public:
  // Reads, decodes, reparses, identity-checks and dependency-orders all inputs
  // before any declaration is inserted into a consumer AST.
  static std::unique_ptr<InterfaceImportSession> Prepare(
      const std::vector<InterfaceImportInput> &inputs,
      const InterfaceImportTarget &target,
      std::vector<InterfaceContractDiagnostic> &diagnostics);

  ~InterfaceImportSession();
  InterfaceImportSession(InterfaceImportSession &&) noexcept;
  InterfaceImportSession &operator=(InterfaceImportSession &&) noexcept;
  InterfaceImportSession(const InterfaceImportSession &) = delete;
  InterfaceImportSession &operator=(const InterfaceImportSession &) = delete;

  // Imports validated Clang declarations into the supplied target translation
  // unit before its source is parsed, so ordinary Sema lookup sees them.
  [[nodiscard]] InterfaceImportReport ImportInto(
      clang::ASTContext &target_context,
      clang::FileManager &target_file_manager,
      bool source_scoped_visibility = false,
      std::shared_ptr<clang::ASTImporterSharedState> shared_state = {});

  [[nodiscard]] std::vector<InterfaceSourceArtifact>
  GetSourceArtifacts() const;
  [[nodiscard]] bool RequiresRuntimeHeader() const;
  [[nodiscard]] bool RequiresTaskHeader() const;

private:
  struct Impl;
  explicit InterfaceImportSession(std::unique_ptr<Impl> impl);
  std::unique_ptr<Impl> impl_;
};

// Filtered command-line subset for reparsing canonical interfaces. The caller
// appends the exact language/target/runtime/effect mode and imports never reuse
// the provider's filesystem working directory implicitly.
[[nodiscard]] std::vector<std::string> BuildInterfaceASTArguments(
    llvm::ArrayRef<std::string> user_arguments,
    llvm::StringRef language, llvm::StringRef standard,
    const InterfaceImportTarget &target);

} // namespace objc3::clang_foundation
