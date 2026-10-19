#include "clang_foundation/interface_validation.h"

#include "clang_foundation/interface_source_import_bridge.h"

#include <algorithm>
#include <utility>

#include "clang/AST/ASTImporterSharedState.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/Lookup.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/ErrorHandling.h"

namespace objc3::clang_foundation {
namespace {
bool SameIdentity(const InterfaceModuleIdentity &left,
                  const InterfaceModuleIdentity &right) {
  return left.canonical_name == right.canonical_name &&
      left.semantic_fingerprint == right.semantic_fingerprint &&
      left.target_triple == right.target_triple &&
      left.runtime_abi == right.runtime_abi &&
      left.extension_abi == right.extension_abi &&
      left.language_revision == right.language_revision &&
      left.language_profile == right.language_profile &&
      left.package_identity == right.package_identity;
}

bool ComputeVisibleClosure(
    const std::map<std::string, InterfaceSourceArtifact> &artifacts,
    llvm::StringRef canonical_name, std::set<std::string> &visible) {
  auto candidate = visible;
  std::set<std::string> visited;
  std::vector<std::string> worklist{canonical_name.str()};
  for (std::size_t cursor = 0; cursor < worklist.size(); ++cursor) {
    const auto current = worklist[cursor];
    if (!visited.insert(current).second) continue;
    auto artifact = artifacts.find(current);
    if (artifact == artifacts.end()) return false;
    candidate.insert(current);
    for (const auto &dependency : artifact->second.dependencies) {
      if (!dependency.public_import || !dependency.reexported) continue;
      auto target = artifacts.find(dependency.module.canonical_name);
      if (target == artifacts.end() ||
          !SameIdentity(target->second.module, dependency.module))
        return false;
      worklist.push_back(dependency.module.canonical_name);
    }
  }
  visible = std::move(candidate);
  return true;
}
} // namespace

InterfaceSourceImportBridge::InterfaceSourceImportBridge(
    const std::vector<InterfaceModuleRequest> &registry,
    const InterfaceImportReport &imported,
    const std::vector<InterfaceSourceArtifact> &artifacts) {
  ConfigureSourceRegistry(registry, imported, artifacts);
}

void InterfaceSourceImportBridge::ConfigureSourceRegistry(
    const std::vector<InterfaceModuleRequest> &registry,
    const InterfaceImportReport &imported,
    const std::vector<InterfaceSourceArtifact> &artifacts) {
  Reset();
  declarations_ = imported.declarations_by_module;
  valid_ = imported.success;
  diagnostics_ = imported.diagnostics;
  if (!valid_)
    last_failure_ = "source-import declarations were not successfully imported";
  for (const auto &entry : registry) {
    if (entry.canonical_name.empty() ||
        entry.locked_identity.canonical_name != entry.canonical_name ||
        !registry_.insert(entry.canonical_name).second) {
      valid_ = false;
      last_failure_ = "source-import registry contains an empty, mismatched, or duplicate canonical module identity";
    }
    requests_.emplace(entry.canonical_name, entry);
  }
  for (const auto &artifact : artifacts) {
    artifacts_.emplace(artifact.module.canonical_name, artifact);
    auto &ids = declaration_ids_[artifact.module.canonical_name];
    for (const auto &declaration : artifact.declarations)
      ids.insert(declaration.stable_id);
  }
  direct_dependencies_ = imported.direct_dependencies;
  dependency_order_ = imported.dependency_order;
  visible_identities_ = imported.resolved_modules;
  imported_identities_ = imported.imported_declarations;
  for (const auto &identity : imported_identities_)
    recorded_identities_.insert(identity.stable_id);
  if (valid_)
    for (const auto &entry : registry)
      if (entry.initially_visible) {
        auto artifact = artifacts_.find(entry.canonical_name);
        if (artifact == artifacts_.end() ||
            !SameIdentity(artifact->second.module, entry.locked_identity) ||
            !AddVisibleClosure(entry.canonical_name)) {
          valid_ = false;
          last_failure_ = "initial source import has an incomplete re-export graph for '" +
              entry.canonical_name + "'";
          visible_.clear();
          directly_imported_.clear();
          break;
        }
        RecordDirectImport(entry.canonical_name);
      }
}

void InterfaceSourceImportBridge::Reset() {
  registry_.clear();
  requests_.clear();
  declarations_.clear();
  declaration_ids_.clear();
  artifacts_.clear();
  directly_imported_.clear();
  visible_.clear();
  initially_visible_declarations_.clear();
  visible_identities_.clear();
  imported_identities_.clear();
  direct_dependencies_.clear();
  dependency_order_.clear();
  input_paths_.clear();
  recorded_identities_.clear();
  diagnostics_.clear();
  last_failure_.clear();
  sessions_.clear();
  search_roots_.clear();
  target_ = {};
  file_manager_ = nullptr;
  sema_ = nullptr;
  import_context_ = nullptr;
  importer_state_.reset();
  on_demand_ = false;
  valid_ = false;
}

void InterfaceSourceImportBridge::ConfigureOnDemand(
    const std::vector<std::string> &search_roots,
    const std::vector<InterfaceModuleRequest> &registry,
    const InterfaceImportTarget &target, clang::FileManager &file_manager) {
  Reset();
  search_roots_ = search_roots;
  target_ = target;
  file_manager_ = &file_manager;
  on_demand_ = true;
  valid_ = true;
  for (const auto &entry : registry) {
    if (entry.canonical_name.empty() ||
        entry.locked_identity.canonical_name != entry.canonical_name ||
        !registry_.insert(entry.canonical_name).second) {
      valid_ = false;
      last_failure_ = "source-import registry contains an empty, mismatched, or duplicate canonical module identity";
    }
    requests_.emplace(entry.canonical_name, entry);
  }
}

void InterfaceSourceImportBridge::InitializeSema(clang::Sema &sema) {
  if (import_context_ && import_context_ != &sema.getASTContext()) {
    valid_ = false;
    last_failure_ = "source-import bridge must be reconfigured for a different consumer ASTContext";
    visible_.clear();
    initially_visible_declarations_.clear();
    importer_state_.reset();
  }
  sema_ = &sema;
}

bool InterfaceSourceImportBridge::IsRegistered(
    llvm::StringRef canonical_name) const {
  return registry_.count(canonical_name.str());
}

bool InterfaceSourceImportBridge::AddVisibleClosure(
    llvm::StringRef canonical_name) {
  return ComputeVisibleClosure(
      artifacts_, canonical_name, visible_);
}

bool InterfaceSourceImportBridge::ValidateVisibleExports(
    llvm::StringRef importing_module,
    const std::map<std::string, InterfaceSourceArtifact> &artifacts,
    const std::set<std::string> &visible, std::string &reason) const {
  struct Export {
    std::string module;
    const InterfaceDeclarationIdentity *identity = nullptr;
  };
  std::map<std::string, std::vector<Export>> names;
  for (const auto &module_name : visible) {
    auto artifact = artifacts.find(module_name);
    if (artifact == artifacts.end()) continue;
    for (const auto &decl : artifact->second.declarations) {
      const bool tag = decl.kind == "Record" || decl.kind == "CXXRecord" ||
                       decl.kind == "Enum";
      const std::string key = (tag ? "tag:" : "ordinary:") + decl.qualified_name;
      auto &prior_exports = names[key];
      for (const auto &prior : prior_exports) {
        const auto &old = *prior.identity;
        if (SameInterfaceDeclarationIdentity(old, decl))
          continue;
        if (target_.allow_function_overloads && old.kind == "Function" &&
            decl.kind == "Function" && old.stable_id != decl.stable_id)
          continue;
        reason = "source import '" + importing_module.str() +
            "' conflicts with visible declaration '" + decl.qualified_name +
            "' from module '" + prior.module + "'";
        return false;
      }
      prior_exports.push_back({module_name, &decl});
    }
  }
  return true;
}

void InterfaceSourceImportBridge::RecordDirectImport(
    llvm::StringRef canonical_name) {
  directly_imported_.insert(canonical_name.str());
  const auto &request = requests_.at(canonical_name.str());
  InterfaceDependency direct{request.locked_identity, request.reexported,
                             request.public_import};
  auto prior = std::find_if(direct_dependencies_.begin(), direct_dependencies_.end(),
      [&](const auto &dependency) {
        return dependency.module.canonical_name == canonical_name;
      });
  if (prior == direct_dependencies_.end())
    direct_dependencies_.push_back(std::move(direct));
  else
    *prior = std::move(direct);
  std::sort(direct_dependencies_.begin(), direct_dependencies_.end(),
      [](const auto &left, const auto &right) {
        return left.module.canonical_name < right.module.canonical_name;
      });
}

SourceModuleImportStatus InterfaceSourceImportBridge::Activate(
    llvm::StringRef canonical_name) {
  if (on_demand_)
    return ImportObjC3Module(canonical_name);
  if (!IsRegistered(canonical_name)) {
    last_failure_.clear();
    return SourceModuleImportStatus::NotRegistered;
  }
  if (!valid_) {
    if (last_failure_.empty())
      last_failure_ = "source-import declarations were not successfully configured";
    return SourceModuleImportStatus::Rejected;
  }
  last_failure_.clear();
  auto artifact = artifacts_.find(canonical_name.str());
  if (artifact != artifacts_.end() &&
      !SameIdentity(artifact->second.module,
                    requests_.at(canonical_name.str()).locked_identity)) {
    last_failure_ = "source import lock conflicts with the imported identity for '" +
        canonical_name.str() + "'";
    return SourceModuleImportStatus::Rejected;
  }
  if (visible_.count(canonical_name.str())) {
    RecordDirectImport(canonical_name);
    return SourceModuleImportStatus::AlreadyVisible;
  }
  if (!AddVisibleClosure(canonical_name)) {
    last_failure_ = "source import has an incomplete re-export graph for '" +
        canonical_name.str() + "'";
    return SourceModuleImportStatus::Rejected;
  }
  RecordDirectImport(canonical_name);
  return SourceModuleImportStatus::Imported;
}

bool InterfaceSourceImportBridge::IsVisible(
    llvm::StringRef canonical_name) const {
  return visible_.count(canonical_name.str()) != 0;
}

void InterfaceSourceImportBridge::AppendModuleDeclarations(
    llvm::StringRef canonical_name, llvm::StringRef identifier,
    unsigned identifier_namespace,
    std::vector<clang::NamedDecl *> &out) const {
  auto found = declarations_.find(canonical_name.str());
  if (found == declarations_.end()) return;
  for (auto *declaration : found->second) {
    if (!declaration || !declaration->getIdentifier() ||
        declaration->getIdentifier()->getName() != identifier)
      continue;
    if (identifier_namespace &&
        !(declaration->getIdentifierNamespace() & identifier_namespace))
      continue;
    if (std::none_of(out.begin(), out.end(), [&](clang::NamedDecl *prior) {
          return prior->getCanonicalDecl() == declaration->getCanonicalDecl();
        }))
      out.push_back(declaration);
  }
}

std::vector<clang::NamedDecl *> InterfaceSourceImportBridge::LookupUnqualified(
    llvm::StringRef identifier, unsigned identifier_namespace) const {
  std::vector<clang::NamedDecl *> result;
  for (const auto &module : visible_)
    AppendModuleDeclarations(module, identifier, identifier_namespace, result);
  return result;
}

QualifiedModuleLookup InterfaceSourceImportBridge::LookupQualified(
    llvm::StringRef canonical_module_name, llvm::StringRef identifier,
    unsigned identifier_namespace) const {
  QualifiedModuleLookup result;
  result.canonical_module_name = canonical_module_name.str();
  result.identifier = identifier.str();
  AppendModuleDeclarations(canonical_module_name, identifier,
                          identifier_namespace, result.declarations);
  if (result.declarations.size() > 1) {
    const auto *first =
        llvm::dyn_cast<clang::TypeDecl>(result.declarations.front());
    if (first) {
      const auto &context = first->getASTContext();
      const auto type = context.getTypeDeclType(first);
      const bool same_type = std::all_of(
          result.declarations.begin(), result.declarations.end(),
          [&](const auto *declaration) {
            const auto *named_type =
                llvm::dyn_cast<clang::TypeDecl>(declaration);
            return named_type && &named_type->getASTContext() == &context &&
                   context.hasSameType(type,
                                       context.getTypeDeclType(named_type));
          });
      if (same_type) {
        auto preferred = std::find_if(
            result.declarations.begin(), result.declarations.end(),
            [](const auto *declaration) {
              return llvm::isa<clang::TypedefNameDecl>(declaration);
            });
        auto *declaration = preferred == result.declarations.end()
                                ? result.declarations.front()
                                : *preferred;
        result.declarations = {declaration};
      }
    }
  }
  if (!IsVisible(canonical_module_name)) {
    // An unloaded registry entry has no export list yet. Once its artifact
    // is known, only an actual export should trigger the import diagnostic.
    result.status = !result.declarations.empty() ||
            (IsRegistered(canonical_module_name) &&
             !artifacts_.count(canonical_module_name.str()))
        ? QualifiedModuleLookupStatus::ModuleNotVisible
        : QualifiedModuleLookupStatus::NotFound;
    result.declarations.clear();
    return result;
  }
  if (result.declarations.empty())
    result.status = QualifiedModuleLookupStatus::NotFound;
  else if (result.declarations.size() == 1)
    result.status = QualifiedModuleLookupStatus::Found;
  else if (target_.allow_function_overloads &&
           std::all_of(result.declarations.begin(), result.declarations.end(),
               [](clang::NamedDecl *declaration) {
                 return llvm::isa<clang::FunctionDecl>(declaration);
               }))
    result.status = QualifiedModuleLookupStatus::Found;
  else
    result.status = QualifiedModuleLookupStatus::Ambiguous;
  return result;
}

QualifiedModuleLookup InterfaceSourceImportBridge::LookupQualifiedSpelling(
    llvm::ArrayRef<llvm::StringRef> path,
    unsigned identifier_namespace) const {
  QualifiedModuleLookup result;
  if (path.size() < 2) return result;
  std::string module_name;
  for (std::size_t i = 0; i + 1 < path.size(); ++i) {
    if (i) module_name.push_back('.');
    module_name += path[i].str();
  }
  if (IsRegistered(module_name) || artifacts_.count(module_name))
    return LookupQualified(module_name, path.back(), identifier_namespace);
  result.status = QualifiedModuleLookupStatus::NotFound;
  result.canonical_module_name = std::move(module_name);
  result.identifier = path.back().str();
  return result;
}

std::vector<std::string> InterfaceSourceImportBridge::GetVisibleModules() const {
  return {visible_.begin(), visible_.end()};
}

SourceModuleImportStatus InterfaceSourceImportBridge::ImportObjC3Module(
    llvm::StringRef canonical_name, clang::SourceLocation import_location) {
  if (on_demand_) {
    if (!IsRegistered(canonical_name))
      return SourceModuleImportStatus::NotRegistered;
    auto reject = [&](std::string reason, llvm::StringRef code = {}) {
      last_failure_ = std::move(reason);
      if (!code.empty())
        diagnostics_.push_back({code.str(), last_failure_, canonical_name.str()});
      DiagnoseImportFailure(import_location);
      return SourceModuleImportStatus::Rejected;
    };
    auto reject_diagnostics = [&](std::string reason,
        const std::vector<InterfaceContractDiagnostic> &diagnostics) {
      diagnostics_.insert(diagnostics_.end(), diagnostics.begin(), diagnostics.end());
      if (!diagnostics.empty())
        reason += "; " + diagnostics.front().code + ": " +
            diagnostics.front().message;
      return reject(std::move(reason));
    };
    if (!valid_ || !sema_ || !file_manager_) {
      return reject(valid_ ? "source-import bridge was not initialized by Clang Sema"
                           : last_failure_, "O3IFC148");
    }
    last_failure_.clear();
    if (directly_imported_.count(canonical_name.str()))
      return SourceModuleImportStatus::AlreadyVisible;

    const auto prior_artifact = artifacts_.find(canonical_name.str());
    if (prior_artifact != artifacts_.end()) {
      const auto &request = requests_.at(canonical_name.str());
      if (!SameIdentity(prior_artifact->second.module,
                        request.locked_identity)) {
        return reject("source import lock conflicts with the already imported dependency identity for '" +
            canonical_name.str() + "'", "O3IFC136");
      }
      auto candidate_visible = visible_;
      if (!ComputeVisibleClosure(
              artifacts_, canonical_name, candidate_visible))
        return reject("direct source import has an incomplete re-export graph for '" +
            canonical_name.str() + "'", "O3IFC149");
      std::string conflict;
      if (!ValidateVisibleExports(canonical_name, artifacts_, candidate_visible, conflict))
        return reject(std::move(conflict), "O3IFC132");
      const bool already_visible = IsVisible(canonical_name);
      visible_ = std::move(candidate_visible);
      RecordDirectImport(canonical_name);
      return already_visible ? SourceModuleImportStatus::AlreadyVisible
                             : SourceModuleImportStatus::Imported;
    }

    auto request = requests_.at(canonical_name.str());
    request.initially_visible = true;
    auto resolution = ResolveInterfaceModules(search_roots_, {request}, target_);
    if (!resolution.success)
      return reject_diagnostics("source import resolution failed for '" +
          canonical_name.str() + "'", resolution.diagnostics);
    // Preparation reports into a fresh vector. Diagnostics from a rejected
    // earlier directive must not invalidate a later, independent import.
    std::vector<InterfaceContractDiagnostic> preparation_diagnostics;
    auto session = InterfaceImportSession::Prepare(
        resolution.artifacts, target_, preparation_diagnostics);
    if (!session)
      return reject_diagnostics("source import validation failed for '" +
          canonical_name.str() + "'", preparation_diagnostics);

    // Validate the combined identities, graph and visibility before changing
    // the consumer AST or any published bridge metadata.
    const auto sources = session->GetSourceArtifacts();
    auto candidate_artifacts = artifacts_;
    for (const auto &source : sources) {
      auto prior = candidate_artifacts.find(source.module.canonical_name);
      if (prior != candidate_artifacts.end() &&
          !SameIdentity(prior->second.module, source.module))
        return reject("source import '" + canonical_name.str() +
            "' conflicts with the already imported module identity for '" +
            source.module.canonical_name + "'", "O3IFC136");
      candidate_artifacts.emplace(source.module.canonical_name, source);
    }
    std::vector<InterfaceDependencyNode> graph_nodes;
    graph_nodes.reserve(candidate_artifacts.size());
    for (const auto &entry : candidate_artifacts)
      graph_nodes.push_back({entry.second.module, entry.second.dependencies});
    const auto graph = ResolveInterfaceDependencyGraph(graph_nodes);
    if (!graph.ok()) {
      std::vector<InterfaceContractDiagnostic> graph_diagnostics;
      for (const auto &diagnostic : graph.diagnostics)
        graph_diagnostics.push_back({diagnostic.code, diagnostic.message,
                                     canonical_name.str()});
      return reject_diagnostics("combined source-import dependency graph is invalid",
                                graph_diagnostics);
    }
    auto candidate_visible = visible_;
    if (!ComputeVisibleClosure(
            candidate_artifacts, canonical_name, candidate_visible))
      return reject("source import re-export graph is incomplete for '" +
          canonical_name.str() + "'", "O3IFC149");
    std::string conflict;
    if (!ValidateVisibleExports(canonical_name, candidate_artifacts,
                                candidate_visible, conflict))
      return reject(std::move(conflict), "O3IFC132");

    if (!importer_state_) {
      import_context_ = &sema_->getASTContext();
      importer_state_ = std::make_shared<clang::ASTImporterSharedState>(
          *import_context_->getTranslationUnitDecl());
    }
    auto imported = session->ImportInto(sema_->getASTContext(), *file_manager_,
        /*source_scoped_visibility=*/true, importer_state_);
    if (!imported.success) {
      // ASTImporter can already have inserted part of the module. Keep the
      // provider session alive and stop further AST mutations after failure.
      valid_ = false;
      sessions_.push_back(std::move(session));
      return reject_diagnostics("source declarations could not be imported for '" +
          canonical_name.str() + "'", imported.diagnostics);
    }

    for (const auto &entry : imported.declarations_by_module) {
      auto &destination = declarations_[entry.first];
      for (auto *declaration : entry.second)
        if (declaration &&
            std::find(destination.begin(), destination.end(), declaration) ==
                destination.end())
          destination.push_back(declaration);
    }
    for (const auto &source : sources) {
      auto &ids = declaration_ids_[source.module.canonical_name];
      for (const auto &declaration : source.declarations)
        ids.insert(declaration.stable_id);
    }

    artifacts_ = std::move(candidate_artifacts);
    visible_ = std::move(candidate_visible);
    RecordDirectImport(canonical_name);
    for (const auto &identity : imported.resolved_modules)
      if (std::none_of(visible_identities_.begin(), visible_identities_.end(),
              [&](const auto &prior) {
                return prior.canonical_name == identity.canonical_name;
              }))
        visible_identities_.push_back(identity);
    std::sort(visible_identities_.begin(), visible_identities_.end(),
        [](const auto &left, const auto &right) {
          return left.canonical_name < right.canonical_name;
        });
    for (const auto &identity : imported.imported_declarations)
      if (recorded_identities_.insert(identity.stable_id).second)
        imported_identities_.push_back(identity);
    dependency_order_ = graph.topological_order;
    for (const auto &input : resolution.artifacts) {
      input_paths_.push_back(input.source_path);
      input_paths_.push_back(input.sidecar_path.empty()
          ? input.source_path + ".json" : input.sidecar_path);
    }
    sessions_.push_back(std::move(session));
    return SourceModuleImportStatus::Imported;
  }
  const auto status = Activate(canonical_name);
  if (status == SourceModuleImportStatus::Rejected)
    DiagnoseImportFailure(import_location);
  return status;
}

void InterfaceSourceImportBridge::DiagnoseImportFailure(
    clang::SourceLocation import_location) {
  if (!sema_ || last_failure_.empty()) return;
  const auto id = sema_->getDiagnostics().getCustomDiagID(
      clang::DiagnosticsEngine::Error, "%0");
  sema_->Diag(import_location, id) << last_failure_;
}

QualifiedModuleLookup InterfaceSourceImportBridge::LookupObjC3QualifiedName(
    llvm::StringRef canonical_module_name, llvm::StringRef identifier,
    unsigned identifier_namespace) const {
  return LookupQualified(canonical_module_name, identifier,
                        identifier_namespace);
}

clang::ObjC3SourceImportResult
InterfaceSourceImportBridge::HandleObjC3SourceImport(
    llvm::StringRef canonical_module_name,
    clang::SourceLocation import_location) {
  switch (ImportObjC3Module(canonical_module_name, import_location)) {
  case SourceModuleImportStatus::Imported:
  case SourceModuleImportStatus::AlreadyVisible:
    return clang::ObjC3SourceImportResult::Handled;
  case SourceModuleImportStatus::NotRegistered:
    return clang::ObjC3SourceImportResult::NotHandled;
  case SourceModuleImportStatus::Rejected:
    return clang::ObjC3SourceImportResult::Error;
  }
  llvm_unreachable("unhandled source module import status");
}

clang::ObjC3QualifiedNameLookupStatus
InterfaceSourceImportBridge::LookupObjC3QualifiedName(
    llvm::ArrayRef<clang::IdentifierInfo *> components,
    unsigned identifier_namespace,
    llvm::SmallVectorImpl<clang::NamedDecl *> &declarations) {
  if (components.size() < 2)
    return clang::ObjC3QualifiedNameLookupStatus::NotHandled;
  llvm::SmallVector<llvm::StringRef, 8> spelling;
  spelling.reserve(components.size());
  for (const auto *component : components) {
    if (!component)
      return clang::ObjC3QualifiedNameLookupStatus::NotHandled;
    spelling.push_back(component->getName());
  }
  auto found = LookupQualifiedSpelling(spelling, identifier_namespace);
  if (!IsRegistered(found.canonical_module_name) &&
      !artifacts_.count(found.canonical_module_name))
    return clang::ObjC3QualifiedNameLookupStatus::NotHandled;
  declarations.append(found.declarations.begin(), found.declarations.end());
  switch (found.status) {
  case QualifiedModuleLookupStatus::Found:
    return clang::ObjC3QualifiedNameLookupStatus::Found;
  case QualifiedModuleLookupStatus::ModuleNotVisible:
    return clang::ObjC3QualifiedNameLookupStatus::ModuleNotVisible;
  case QualifiedModuleLookupStatus::NotFound:
    return clang::ObjC3QualifiedNameLookupStatus::NotFound;
  case QualifiedModuleLookupStatus::Ambiguous:
    return clang::ObjC3QualifiedNameLookupStatus::Ambiguous;
  }
  llvm_unreachable("unhandled qualified module lookup status");
}

bool InterfaceSourceImportBridge::LookupUnqualified(
    clang::LookupResult &result, clang::Scope *) {
  auto *identifier = result.getLookupName().getAsIdentifierInfo();
  if (!identifier) return false;
  bool found = false;
  std::set<const clang::Decl *> added;
  for (auto *declaration : result)
    added.insert(declaration->getCanonicalDecl());
  auto add = [&](clang::NamedDecl *declaration) {
    if (declaration &&
        (!result.getIdentifierNamespace() ||
         (declaration->getIdentifierNamespace() &
          result.getIdentifierNamespace())) &&
        added.insert(declaration->getCanonicalDecl()).second) {
      result.addDecl(declaration);
      found = true;
    }
  };
  auto initial = initially_visible_declarations_.find(identifier);
  if (initial != initially_visible_declarations_.end())
    for (auto *declaration : initial->second) add(declaration);
  for (auto *declaration : LookupUnqualified(
           identifier->getName(), result.getIdentifierNamespace()))
    add(declaration);
  if (found) result.resolveKind();
  return found;
}

void InterfaceSourceImportBridge::AddInitiallyVisible(
    clang::NamedDecl *declaration) {
  if (!declaration || !declaration->getIdentifier()) return;
  auto &declarations =
      initially_visible_declarations_[declaration->getIdentifier()];
  if (std::find(declarations.begin(), declarations.end(), declaration) ==
      declarations.end())
    declarations.push_back(declaration);
}

std::vector<InterfaceModuleIdentity>
InterfaceSourceImportBridge::GetImportedModuleIdentities() const {
  return visible_identities_;
}

std::vector<InterfaceSourceArtifact>
InterfaceSourceImportBridge::GetResolvedSourceArtifacts() const {
  std::vector<InterfaceSourceArtifact> result;
  for (const auto &name : dependency_order_) {
    auto found = artifacts_.find(name);
    if (found != artifacts_.end()) result.push_back(found->second);
  }
  return result;
}

std::vector<std::string>
InterfaceSourceImportBridge::GetResolvedInputPaths() const {
  return input_paths_;
}

std::vector<InterfaceDeclarationIdentity>
InterfaceSourceImportBridge::GetVisibleDeclarationIdentities() const {
  std::vector<InterfaceDeclarationIdentity> result;
  for (const auto &identity : imported_identities_)
    if (std::any_of(declaration_ids_.begin(), declaration_ids_.end(),
            [&](const auto &entry) {
              return visible_.count(entry.first) &&
                  entry.second.count(identity.stable_id);
            }))
      result.push_back(identity);
  return result;
}

std::vector<InterfaceDependency>
InterfaceSourceImportBridge::GetDirectDependencies() const {
  std::vector<InterfaceDependency> result;
  for (const auto &dependency : direct_dependencies_)
    if (directly_imported_.count(dependency.module.canonical_name))
      result.push_back(dependency);
  return result;
}

std::vector<std::string> InterfaceSourceImportBridge::GetDependencyOrder() const {
  return dependency_order_;
}

} // namespace objc3::clang_foundation
