#include "clang_foundation/interface_import.h"
#include "clang_foundation/interface_module_resolver.h"

#include <algorithm>
#include <cstdint>
#include <map>
#include <memory>
#include <set>
#include <tuple>
#include <utility>

#include "clang_foundation/interface_codec.h"
#include "clang_foundation/interface_emitter.h"
#include "clang_foundation/interface_validation.h"
#include "clang_foundation/runtime_interface_types.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Attr.h"
#include "clang/AST/ASTImporter.h"
#include "clang/AST/ASTImporterSharedState.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclContextInternals.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Frontend/ASTUnit.h"
#include "clang/Tooling/Tooling.h"
#include "clang/UnifiedSymbolResolution/USRGeneration.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/VirtualFileSystem.h"

namespace objc3::clang_foundation {
namespace {
constexpr std::uint64_t kMaxInterfaceBytes = 32ull * 1024ull * 1024ull;
constexpr llvm::StringLiteral kCarrierSemantics = "objc3.native-carrier-types.v1";
constexpr llvm::StringLiteral kEffectSemantics = "objc3.canonical-function-effects.v1";
const std::vector<llvm::StringRef> kSupportedSemantics = {
    kCarrierSemantics,
    kEffectSemantics,
    "objc3.native-return-ownership.v1",
    "objc3.native-ownership-contracts.v1",
    "objc3.isolation-contracts.v1",
    "objc3.typed-keypaths.v1",
    "objc3.keypath-subscripts.v1",
    "objc3.async-entry-abi.v1",
    "objc3.direct-dispatch.v1",
    "objc3.cxx-data-interfaces.v1",
    "objc3.cxx-member-interfaces.v1",
    kCxxClassLifecycleSemantic,
    kCxxClassStructureSemantic,
    kCxxClassDispatchSemantic,
    kCxxNamespaceSemantic,
    kCxxNestedTypeSemantic,
    kCxxUsingSemantic,
    kCxxDeclarationExpressionSemantic,
    kCxxDefinitionSemantic,
    kCxxDefaultedComparisonSemantic,
    kCxxOutOfLineDefaultingSemantic,
    kCxxImmediateDefaultingSemantic,
    kCxxNamespaceAttributeSemantic,
    kCxxLambdaExpressionSemantic,
    kCxxThreadLocalSemantic,
    kCxxFriendSemantic,
    kCxxBlockExpressionSemantic,
    kCxxStatementExpressionSemantic,
    kCxxLocalTypeSemantic,
    kCxxUnnamedLocalTypeSemantic,
    kCxxGenericLocalLambdaSemantic,
    kCxxAsyncDefinitionSemantic,
    kSdkRuntimeDeclarationSemantic,
    "objc3.ownership-projection.v1",
    "objc3.api-availability.v1",
    "objc3.api-partitions.v1",
    "objc3.module-owned-names.v1",
    RuntimeInterfaceSemantic()};

void Add(std::vector<InterfaceContractDiagnostic> &out, const llvm::Twine &code,
         const llvm::Twine &message, const llvm::Twine &subject = llvm::Twine()) {
  out.push_back({code.str(), message.str(), subject.str()});
}

bool SameIdentity(const InterfaceModuleIdentity &a,
                  const InterfaceModuleIdentity &b) {
  return std::tie(a.canonical_name, a.semantic_fingerprint, a.target_triple,
                  a.runtime_abi, a.extension_abi, a.language_revision,
                  a.language_profile, a.package_identity) ==
         std::tie(b.canonical_name, b.semantic_fingerprint, b.target_triple,
                  b.runtime_abi, b.extension_abi, b.language_revision,
                  b.language_profile, b.package_identity);
}

bool CompatibleTarget(const InterfaceModuleIdentity &module,
                      const InterfaceImportTarget &target) {
  return module.target_triple == target.target_triple &&
         module.runtime_abi == target.runtime_abi &&
         module.extension_abi == target.extension_abi &&
         module.language_revision == target.language_revision &&
         module.language_profile == target.language_profile;
}

bool ReadBoundedFile(llvm::StringRef path, std::string &contents,
                     std::vector<InterfaceContractDiagnostic> &diagnostics,
                     llvm::StringRef kind) {
  std::uint64_t size = 0;
  if (auto error = llvm::sys::fs::file_size(path, size)) {
    Add(diagnostics, "O3IFC130",
        llvm::Twine("cannot stat ") + kind + " artifact: " + error.message(), path);
    return false;
  }
  if (size > kMaxInterfaceBytes) {
    Add(diagnostics, "O3IFC130",
        llvm::Twine(kind) + " artifact exceeds the 32 MiB import bound", path);
    return false;
  }
  auto buffer = llvm::MemoryBuffer::getFile(path);
  if (!buffer) {
    Add(diagnostics, "O3IFC130",
        llvm::Twine("cannot read ") + kind + " artifact: " +
            buffer.getError().message(), path);
    return false;
  }
  if ((*buffer)->getBufferSize() > kMaxInterfaceBytes) {
    Add(diagnostics, "O3IFC130",
        llvm::Twine(kind) + " artifact grew beyond the 32 MiB import bound while being read", path);
    return false;
  }
  contents.assign((*buffer)->getBufferStart(), (*buffer)->getBufferEnd());
  return true;
}

class ExportedDeclarationCollector final
    : public clang::RecursiveASTVisitor<ExportedDeclarationCollector> {
public:
  ExportedDeclarationCollector(clang::ASTContext &context,
                                const std::set<std::string> &expected_ids)
      : context_(context), expected_ids_(expected_ids) {}

  bool VisitNamedDecl(clang::NamedDecl *decl) {
    if (decl->isImplicit() || !decl->getLocation().isValid() ||
        !context_.getSourceManager().isWrittenInMainFile(decl->getLocation()))
      return true;
    const auto stable_id = ReadInterfaceDeclarationStableId(decl);
    if (stable_id.empty()) return true;
    if (expected_ids_.count(stable_id)) Declarations[stable_id] = decl;
    return true;
  }

  std::map<std::string, clang::NamedDecl *> Declarations;
private:
  clang::ASTContext &context_;
  const std::set<std::string> &expected_ids_;
};

bool IsTranslationUnitLookupDeclaration(const clang::NamedDecl *decl) {
  if (!decl->getIdentifier() && !llvm::isa<clang::UsingDirectiveDecl>(decl)) return false;
  const auto *context = decl->getDeclContext();
  while (context && context->isTransparentContext()) context = context->getParent();
  return context && context->isTranslationUnit();
}

class NamedDeclarationSnapshot final
    : public clang::RecursiveASTVisitor<NamedDeclarationSnapshot> {
public:
  bool VisitNamedDecl(clang::NamedDecl *decl) {
    Declarations.insert(decl);
    return true;
  }
  std::set<clang::NamedDecl *> Declarations;
};

// Native header templates may already have concrete definitions in the
// consumer. Only newly imported executable definitions need backend delivery.
class FunctionDefinitionSnapshot final
    : public clang::RecursiveASTVisitor<FunctionDefinitionSnapshot> {
public:
  bool shouldVisitImplicitCode() const { return true; }
  bool shouldVisitTemplateInstantiations() const { return true; }
  bool VisitFunctionDecl(clang::FunctionDecl *function) {
    if (function->doesThisDeclarationHaveABody())
      Definitions.insert(function->getCanonicalDecl());
    return true;
  }
  std::set<const clang::FunctionDecl *> Definitions;
};

class DefinitionTrackingImporter final : public clang::ASTImporter {
public:
  using clang::ASTImporter::ASTImporter;
  const std::set<const clang::FunctionDecl *> *Existing = nullptr;
  std::vector<clang::NamedDecl *> Definitions;
  void Imported(clang::Decl *from, clang::Decl *to) override {
    auto *function=llvm::dyn_cast<clang::FunctionDecl>(to);
    const auto *source=llvm::dyn_cast<clang::FunctionDecl>(from);
    if (function && source && source->doesThisDeclarationHaveABody() &&
        function->doesThisDeclarationHaveABody() &&
        !function->isDependentContext() &&
        (function->isInlined() ||
         function->getTemplateSpecializationKind()==clang::TSK_ImplicitInstantiation ||
         function->hasAttr<clang::ObjC3IsolationThunkAttr>()) &&
        !Existing->count(function->getCanonicalDecl()))
      Definitions.push_back(function);
  }
};

bool CheckExportCollisions(
    const std::vector<InterfaceDeclarationIdentity> &declarations,
    bool allow_function_overloads,
    std::vector<InterfaceContractDiagnostic> &diagnostics) {
  std::map<std::string, std::vector<InterfaceDeclarationIdentity>> by_name;
  for (const auto &decl : declarations) {
    if (decl.kind == "ObjCMethod" || decl.kind == "ObjCProperty" ||
        decl.kind == "ObjCIvar" || decl.kind == "Field" ||
        (decl.kind == "ObjCCategory" || decl.kind == "UsingDirective"))
      continue;
    const bool is_tag = decl.kind == "Record" || decl.kind == "CXXRecord" ||
                        decl.kind == "Enum";
    const std::string lookup_key = (decl.kind == "ObjCProtocol" ? "protocol:"
                                    : is_tag                    ? "tag:"
                                                                : "ordinary:") +
                                   decl.qualified_name;
    auto &prior_declarations = by_name[lookup_key];
    bool repeated = false;
    for (const auto &prior : prior_declarations) {
      if (SameInterfaceDeclarationIdentity(prior, decl)) {
        repeated = true;
        continue; // The same canonical declaration can be re-exported.
      }
      if (allow_function_overloads && prior.stable_id != decl.stable_id &&
          (IsCxxOverloadDeclaration(prior) && IsCxxOverloadDeclaration(decl)))
        continue;
      Add(diagnostics, "O3IFC132",
          "multiple imported modules provide incompatible declarations with "
          "the same lookup name",
          decl.qualified_name);
      return false;
    }
    if (!repeated)
      prior_declarations.push_back(decl);
  }
  return true;
}

std::string ModuleImportFailure(const InterfaceModuleIdentity &module,
                                const llvm::Twine &reason) {
  return (llvm::Twine("interface '") + module.canonical_name + "' " + reason).str();
}
} // namespace

struct InterfaceImportSession::Impl {
  struct Module {
    InterfaceImportInput input;
    SemanticInterfaceSidecar sidecar;
    std::string source_bytes;
    // Tooling's virtual-file MemoryBuffers borrow these strings. Keep them
    // alive until after the AST, including deferred source location imports.
    clang::tooling::FileContentMappings virtual_files;
    std::unique_ptr<clang::ASTUnit> ast;
  };
  InterfaceImportTarget target;
  std::vector<Module> modules;
  std::vector<std::size_t> import_order;
  std::vector<InterfaceDependency> direct_dependencies;
  std::set<std::string> visible_modules;
};

InterfaceImportSession::InterfaceImportSession(std::unique_ptr<Impl> impl)
    : impl_(std::move(impl)) {}
InterfaceImportSession::~InterfaceImportSession() = default;
InterfaceImportSession::InterfaceImportSession(InterfaceImportSession &&) noexcept = default;
InterfaceImportSession &InterfaceImportSession::operator=(InterfaceImportSession &&) noexcept = default;

std::vector<std::string> BuildInterfaceASTArguments(
    llvm::ArrayRef<std::string> user_arguments, llvm::StringRef language,
    llvm::StringRef standard, const InterfaceImportTarget &target) {
  std::vector<std::string> args = {"-fsyntax-only", "-x", language.str(),
                                   "-std=" + standard.str()};
  for (std::size_t i = 0; i < user_arguments.size(); ++i) {
    const auto &arg = user_arguments[i];
    if (arg == "-c" || arg == "-S" || arg == "-emit-llvm" ||
        arg == "-fsyntax-only")
      continue;
    if (arg == "-o" || arg == "-x" || arg == "-target" ||
        arg == "--target") {
      ++i;
      continue;
    }
    if (arg == "-I" || arg == "-isystem" || arg == "-iquote" ||
        arg == "-idirafter" || arg == "-include" || arg == "-imacros" ||
        arg == "-D" || arg == "-U" || arg == "-F" ||
        arg == "-resource-dir" || arg == "-working-directory") {
      if (i + 1 < user_arguments.size()) {
        args.push_back(arg);
        args.push_back(user_arguments[++i]);
      }
      continue;
    }
    if (arg.empty()) continue;
    if (arg.front() == '-') args.push_back(arg);
  }
  if (!target.target_triple.empty())
    args.push_back("--target=" + target.target_triple);
  if (target.objc && !target.objc_runtime.empty())
    args.push_back("-fobjc-runtime=" + target.objc_runtime);
  if (target.objc && target.arc) args.push_back("-fobjc-arc");
  if (target.blocks) args.push_back("-fblocks");
  if (target.objc3) {
    args.push_back("-fobjc-version=3");
    if (!target.objc3_profile.empty())
      args.push_back("-fobjc3-profile=" + target.objc3_profile);
    if (target.strict_concurrency)
      args.push_back("-fobjc3-strict-concurrency");
  }
  return args;
}
std::unique_ptr<InterfaceImportSession> InterfaceImportSession::Prepare(
    const std::vector<InterfaceImportInput> &inputs,
    const InterfaceImportTarget &target,
    std::vector<InterfaceContractDiagnostic> &diagnostics) {
  auto impl = std::make_unique<Impl>();
  impl->target = target;
  if (inputs.empty()) return std::unique_ptr<InterfaceImportSession>(
      new InterfaceImportSession(std::move(impl)));

  std::map<std::string, std::size_t> module_index;
  for (const auto &input : inputs) {
    if (input.source_path.empty() || input.locked_identity.canonical_name.empty()) {
      Add(diagnostics, "O3IFC131",
          "interface source path and locked canonical module identity are required",
          input.source_path);
      continue;
    }
    std::string sidecar_path = input.sidecar_path.empty()
        ? input.source_path + ".json" : input.sidecar_path;
    std::string source_bytes, sidecar_json;
    if (!ReadBoundedFile(input.source_path, source_bytes, diagnostics, "source") ||
        !ReadBoundedFile(sidecar_path, sidecar_json, diagnostics, "sidecar"))
      continue;
    auto sidecar = DecodeSemanticInterfaceSidecar(
        sidecar_json, source_bytes,
        llvm::ArrayRef<llvm::StringRef>(kSupportedSemantics));
    if (!sidecar) {
      Add(diagnostics, "O3IFC133",
          llvm::Twine("invalid interface sidecar: ") + llvm::toString(sidecar.takeError()),
          input.locked_identity.canonical_name);
      continue;
    }
    if (!SameIdentity(sidecar->module, input.locked_identity)) {
      Add(diagnostics, "O3IFC134",
          "sidecar identity does not match the dependency lock",
          input.locked_identity.canonical_name);
      continue;
    }
    if (!CompatibleTarget(sidecar->module, target)) {
      Add(diagnostics, "O3IFC135",
          "interface target, runtime, extension ABI, language revision, or profile differs from the consumer",
          input.locked_identity.canonical_name);
      continue;
    }
    std::set<std::string> groups;
    for (const auto &group : input.spi_groups) {
      if (input.spi_groups.size() > 64 || !IsValidSPIGroup(group) ||
          !groups.insert(group).second ||
          std::none_of(sidecar->declarations.begin(),
                       sidecar->declarations.end(),
                       [&](const auto &d) { return d.spi_group == group; }))
        Add(diagnostics, "O3IFC159",
            "SPI grants require distinct existing groups on the exact provider",
            group);
    }
    if (!input.direct_import && !input.spi_groups.empty())
      Add(diagnostics, "O3IFC159", "SPI grants require a direct import",
          sidecar->module.canonical_name);
    const auto existing = module_index.find(sidecar->module.canonical_name);
    if (existing != module_index.end()) {
      auto &prior = impl->modules[existing->second];
      if (!SameIdentity(prior.sidecar.module, sidecar->module) ||
          prior.source_bytes != source_bytes)
        Add(diagnostics, "O3IFC136",
            "the same canonical module name was resolved to conflicting artifacts",
            sidecar->module.canonical_name);
      prior.input.spi_groups.insert(prior.input.spi_groups.end(),
                                    input.spi_groups.begin(),
                                    input.spi_groups.end());
      std::sort(prior.input.spi_groups.begin(), prior.input.spi_groups.end());
      prior.input.spi_groups.erase(std::unique(prior.input.spi_groups.begin(),
                                               prior.input.spi_groups.end()),
                                   prior.input.spi_groups.end());
      if (input.direct_import) {
        if (prior.input.direct_import) {
          prior.input.public_import |= input.public_import;
          prior.input.reexported |= input.reexported;
        } else {
          prior.input.direct_import = true;
          prior.input.public_import = input.public_import;
          prior.input.reexported = input.reexported;
        }
      }
      continue;
    }

    Impl::Module module;
    module.input = input;
    module.sidecar = std::move(*sidecar);
    module.source_bytes = std::move(source_bytes);
    module_index.emplace(module.sidecar.module.canonical_name, impl->modules.size());
    impl->modules.push_back(std::move(module));
  }
  if (!diagnostics.empty()) return nullptr;

  std::vector<InterfaceDependencyNode> graph_nodes;
  graph_nodes.reserve(impl->modules.size());
  for (const auto &module : impl->modules)
    graph_nodes.push_back({module.sidecar.module, module.sidecar.dependencies});
  auto graph = ResolveInterfaceDependencyGraph(graph_nodes);
  for (const auto &d : graph.diagnostics)
    Add(diagnostics, d.code, d.message,
        llvm::join(d.modules, ", "));
  if (!graph.ok()) return nullptr;

  // Parse in dependency-first order. A dependent canonical interface can
  // include prior emitted interfaces, so parsing every source standalone
  // before graph resolution is both order-dependent and semantically wrong.
  for (const auto &name : graph.topological_order) {
    const auto index = module_index.at(name);
    auto &module = impl->modules[index];
    auto &virtual_files=module.virtual_files;
    llvm::SmallString<256> include_directory(module.input.source_path);
    llvm::sys::path::remove_filename(include_directory);
    std::vector<InterfaceSourceArtifact> dependency_sources;
    std::set<std::string> dependency_closure;
    std::vector<std::string> dependency_worklist;
    for (const auto &dependency : module.sidecar.dependencies)
      dependency_worklist.push_back(dependency.module.canonical_name);
    for (std::size_t cursor = 0; cursor < dependency_worklist.size(); ++cursor) {
      // Appending nested dependencies can reallocate this worklist.
      const auto dependency_name = dependency_worklist[cursor];
      if (!dependency_closure.insert(dependency_name).second) continue;
      const auto dep_it = module_index.find(dependency_name);
      if (dep_it == module_index.end() || !impl->modules[dep_it->second].ast) {
        Add(diagnostics, "O3IFC140",
            "dependency was not parsed before its importing module",
            dependency_name);
        continue;
      }
      const auto &provider = impl->modules[dep_it->second];
      for (const auto &nested : provider.sidecar.dependencies)
        dependency_worklist.push_back(nested.module.canonical_name);
      llvm::SmallString<256> include_path(include_directory);
      llvm::sys::path::append(include_path, BuildInterfaceIncludePath(dependency_name));
      virtual_files.emplace_back(include_path.str().str(), provider.source_bytes);
      InterfaceSourceArtifact source;
      source.module = provider.sidecar.module;
      source.virtual_include_path = BuildInterfaceIncludePath(dependency_name);
      source.source_bytes = provider.source_bytes;
      source.declarations = provider.sidecar.declarations;
      source.dependencies = provider.sidecar.dependencies;
      dependency_sources.push_back(std::move(source));
    }
    if (!diagnostics.empty()) return nullptr;
    auto ast_arguments = target.ast_arguments;
    AddRuntimeInterfaceFiles(virtual_files, ast_arguments,
        module.source_bytes.find("#include <objc3c/runtime/objc3_task.hpp>") !=
            std::string::npos);
    ast_arguments.push_back("-iquote");
    ast_arguments.push_back(include_directory.empty() ? "." : include_directory.str().str());
    module.ast = clang::tooling::buildASTFromCodeWithArgs(
        module.source_bytes, ast_arguments,
        (include_directory.str() + "/" +
         BuildInterfaceIncludePath(module.sidecar.module.canonical_name))
            .str(),
        "clang-tool", std::make_shared<clang::PCHContainerOperations>(),
        clang::tooling::getClangStripDependencyFileAdjuster(), virtual_files,
        nullptr, llvm::vfs::createPhysicalFileSystem());
    if (!module.ast || module.ast->getDiagnostics().hasErrorOccurred()) {
      Add(diagnostics, "O3IFC137",
          "Clang rejected the generated interface source in the consumer language and target mode",
          module.sidecar.module.canonical_name);
      return nullptr;
    }
    auto regenerated = EmitSemanticInterface(
        module.ast->getASTContext(), module.sidecar.module.canonical_name,
        module.sidecar.module.package_identity, module.sidecar.dependencies,
        dependency_sources, module.sidecar.ownership_projection);
    if (!regenerated.success || regenerated.source_bytes != module.source_bytes) {
      Add(diagnostics, regenerated.diagnostics.empty()
              ? "O3IFC138" : regenerated.diagnostics.front().code,
          regenerated.diagnostics.empty()
              ? "interface source is not the canonical Clang-emitted declaration set"
              : regenerated.diagnostics.front().message,
          module.sidecar.module.canonical_name);
      return nullptr;
    }
    auto regenerated_sidecar = DecodeSemanticInterfaceSidecar(
        regenerated.sidecar_json, regenerated.source_bytes,
        llvm::ArrayRef<llvm::StringRef>(kSupportedSemantics));
    if (!regenerated_sidecar ||
        !SameIdentity(regenerated.identity, module.sidecar.module) ||
        BuildSemanticFingerprintInput(*regenerated_sidecar) !=
            BuildSemanticFingerprintInput(module.sidecar)) {
      if (!regenerated_sidecar) llvm::consumeError(regenerated_sidecar.takeError());
      Add(diagnostics, "O3IFC139",
          "parsed Clang declarations do not reproduce the sidecar type/effect identities",
          module.sidecar.module.canonical_name);
      return nullptr;
    }
  }

  std::vector<InterfaceDeclarationIdentity> visible_declarations;
  std::map<std::string, InterfaceDeclarationIdentity> by_stable_id;
  for (const auto &module : impl->modules) {
    if (module.input.direct_import)
      impl->direct_dependencies.push_back({
          module.sidecar.module, module.input.reexported, module.input.public_import});
  }
  // The root-visible set includes direct imports and only dependencies reached
  // through an explicitly re-exported edge. Private implementation dependencies
  // remain available to type reconstruction but do not enter collision checks.
  std::vector<std::string> visibility_worklist;
  for (const auto &module : impl->modules)
    if (module.input.direct_import) {
      impl->visible_modules.insert(module.sidecar.module.canonical_name);
      visibility_worklist.push_back(module.sidecar.module.canonical_name);
    }
  for (std::size_t cursor = 0; cursor < visibility_worklist.size(); ++cursor) {
    const auto current = module_index.at(visibility_worklist[cursor]);
    for (const auto &dependency : impl->modules[current].sidecar.dependencies) {
      if (!dependency.reexported || !dependency.public_import) continue;
      if (impl->visible_modules.insert(dependency.module.canonical_name).second)
        visibility_worklist.push_back(dependency.module.canonical_name);
    }
  }
  for (const auto &module : impl->modules) {
    for (const auto &decl : module.sidecar.declarations) {
      if (!IsGrantedSPI(decl, module.input.spi_groups))
        continue;
      const auto [it, inserted] = by_stable_id.emplace(decl.stable_id, decl);
      if (!inserted && !SameInterfaceDeclarationIdentity(it->second, decl)) {
        Add(diagnostics, "O3IFC132",
            "one declaration identity has conflicting canonical signatures across providers",
            decl.stable_id);
        return nullptr;
      }
      if (impl->visible_modules.count(module.sidecar.module.canonical_name))
        visible_declarations.push_back(decl);
    }
  }
  if (!CheckExportCollisions(visible_declarations,
                             target.allow_function_overloads, diagnostics))
    return nullptr;
  for (const auto &name : graph.topological_order) {
    const auto found = module_index.find(name);
    if (found == module_index.end()) {
      Add(diagnostics, "O3IFC140",
          "resolved dependency graph contains a module without a validated artifact", name);
      return nullptr;
    }
    impl->import_order.push_back(found->second);
  }
  std::sort(impl->direct_dependencies.begin(), impl->direct_dependencies.end(),
      [](const auto &a, const auto &b) {
        return a.module.canonical_name < b.module.canonical_name;
      });
  return std::unique_ptr<InterfaceImportSession>(
      new InterfaceImportSession(std::move(impl)));
}

InterfaceImportReport InterfaceImportSession::ImportInto(
    clang::ASTContext &target_context, clang::FileManager &target_file_manager,
    bool source_scoped_visibility,
    std::shared_ptr<clang::ASTImporterSharedState> shared_state) {
  InterfaceImportReport report;
  if (!impl_) {
    Add(report.diagnostics, "O3IFC141", "import session was not prepared");
    return report;
  }
  const auto &language = target_context.getLangOpts();
  const auto runtime =
      (language.ObjC ? language.ObjCRuntime.getAsString() : "no-objc-runtime") +
      std::string(language.ObjCAutoRefCount ? ";arc"
                                            : ";manual-reference-counting");
  if (ReadInterfaceLanguageProfile(language) !=
          impl_->target.language_profile ||
      runtime != impl_->target.runtime_abi ||
      target_context.getTargetInfo().getTriple().str() !=
          impl_->target.target_triple) {
    Add(report.diagnostics, "O3IFC135",
        "target AST language, standard, ownership, runtime, or target differs "
        "from the prepared import session");
    return report;
  }
  report.direct_dependencies = impl_->direct_dependencies;
  report.visible_module_names.assign(impl_->visible_modules.begin(),
                                     impl_->visible_modules.end());
  for (auto index : impl_->import_order)
    report.resolved_modules.push_back(impl_->modules[index].sidecar.module);
  // The importer lookup table retains hidden dependency declarations across
  // source-directed sessions without exposing them to consumer Sema lookup.
  if (!shared_state)
    shared_state = std::make_shared<clang::ASTImporterSharedState>(
        *target_context.getTranslationUnitDecl());
  NamedDeclarationSnapshot preexisting;
  preexisting.TraverseDecl(target_context.getTranslationUnitDecl());
  FunctionDefinitionSnapshot existing_definitions;
  existing_definitions.TraverseDecl(target_context.getTranslationUnitDecl());
  std::set<std::string> recorded_ids;
  std::vector<clang::NamedDecl *> auxiliary_definitions;
  std::vector<std::unique_ptr<clang::ASTImporter>> importers;
  for (auto index : impl_->import_order) {
    auto &module = impl_->modules[index];
    // Keep canonical origins as FileEntries in the consumer. ASTImporter's
    // buffer-only fallback loses the filename needed by native C typedef USRs
    // and by the owning-source boundary for imported declarations.
    auto &source_manager = module.ast->getSourceManager();
    for (unsigned i = 0; i < source_manager.local_sloc_entry_size(); ++i) {
      const auto &entry = source_manager.getLocalSLocEntry(i);
      if (!entry.isFile()) continue;
      const auto &cache = entry.getFile().getContentCache();
      if (!cache.OrigEntry ||
          !cache.OrigEntry->getName().ends_with(".objc3i"))
        continue;
      auto buffer = cache.getBufferOrNone(module.ast->getDiagnostics(),
                                           module.ast->getFileManager(), {});
      if (!buffer) {
        Add(report.diagnostics, "O3IFC143",
            "canonical interface source buffer is unavailable",
            module.sidecar.module.canonical_name);
        return report;
      }
      auto file = target_file_manager.getVirtualFileRef(
          cache.OrigEntry->getName(), buffer->getBufferSize(), 0);
      target_context.getSourceManager().overrideFileContents(
          file, llvm::MemoryBuffer::getMemBufferCopy(
                    buffer->getBuffer(), cache.OrigEntry->getName()));
    }
    std::set<std::string> expected_ids;
    for (const auto &identity : module.sidecar.declarations)
      expected_ids.insert(identity.stable_id);
    auto importer = std::make_unique<DefinitionTrackingImporter>(
        target_context, target_file_manager,
        module.ast->getASTContext(), module.ast->getFileManager(),
        /*MinimalImport=*/false, shared_state);
    importer->Existing=&existing_definitions.Definitions;
    auto *from_tu = module.ast->getASTContext().getTranslationUnitDecl();
    // Use the importer's actual source-to-target mappings. A matching USR in
    // another module or in the consumer does not prove this export imported.
    ExportedDeclarationCollector exported(module.ast->getASTContext(), expected_ids);
    exported.TraverseDecl(from_tu);
    if (exported.Declarations.size() != expected_ids.size()) {
      Add(report.diagnostics, "O3IFC144",
          "the canonical source has no declaration for every sidecar export identity",
          module.sidecar.module.canonical_name);
      return report;
    }
    for (const auto &expected : module.sidecar.declarations) {
      if (!IsGrantedSPI(expected, module.input.spi_groups))
        continue;
      auto imported = importer->Import(exported.Declarations.at(expected.stable_id));
      if (!imported) {
        Add(report.diagnostics, "O3IFC142",
            ModuleImportFailure(module.sidecar.module,
                llvm::Twine("could not import exported declaration '") +
                    expected.qualified_name + "': " +
                    llvm::toString(imported.takeError())), expected.stable_id);
        return report;
      }
      auto *named = llvm::dyn_cast_or_null<clang::NamedDecl>(*imported);
      if (!named || &named->getASTContext() != &target_context) {
        Add(report.diagnostics, "O3IFC143",
            "ASTImporter did not produce a declaration owned by the consumer ASTContext",
            expected.stable_id);
        return report;
      }
      InterfaceDeclarationIdentity actual;
      if (!ReadInterfaceDeclarationIdentity(named, target_context, actual) ||
          !SameInterfaceDeclarationIdentity(expected, actual)) {
        Add(report.diagnostics, "O3IFC144",
            "ASTImporter changed an exported declaration's canonical type, "
            "layout, or effects: " +
                expected.canonical_type_identity + " -> " +
                actual.canonical_type_identity,
            expected.stable_id);
        return report;
      }
      if (recorded_ids.insert(actual.stable_id).second)
        report.imported_declarations.push_back(std::move(actual));
      if (auto *function = llvm::dyn_cast<clang::FunctionDecl>(named);
          function && function->isInlined()) {
        if (auto *definition = function->getDefinition();
            definition && definition->doesThisDeclarationHaveABody())
          report.inline_definitions.push_back(definition);
      }
      if (auto *variable = llvm::dyn_cast<clang::VarDecl>(named);
          variable && variable->isInline())
        if (auto *definition = variable->getDefinition())
          report.inline_definitions.push_back(definition);
      if (IsTranslationUnitLookupDeclaration(named)) {
        report.declarations_by_module[module.sidecar.module.canonical_name]
            .push_back(named);
        if (!source_scoped_visibility &&
            impl_->visible_modules.count(module.sidecar.module.canonical_name) &&
            std::find(report.visible_lookup_declarations.begin(),
                      report.visible_lookup_declarations.end(), named) ==
                report.visible_lookup_declarations.end())
          report.visible_lookup_declarations.push_back(named);
      }
    }
    report.dependency_order.push_back(module.sidecar.module.canonical_name);
    auxiliary_definitions.insert(auxiliary_definitions.end(),
        importer->Definitions.begin(),importer->Definitions.end());
    importers.push_back(std::move(importer));
  }
  // Dependencies required only to spell a visible provider's public types
  // remain alive in AST nodes but are removed from TU lookup when they are
  // neither directly imported nor re-exported to this consumer.
  std::set<std::string> hidden_ids;
  std::set<std::string> visible_ids;
  std::set<std::string> hidden_namespace_ids;
  std::set<std::string> visible_namespace_ids;
  for (const auto &module : impl_->modules) {
    for (const auto &identity : module.sidecar.declarations) {
      auto &destination =
          !source_scoped_visibility &&
                  impl_->visible_modules.count(
                      module.sidecar.module.canonical_name) &&
                  IsGrantedSPI(identity, module.input.spi_groups)
              ? visible_ids
              : hidden_ids;
      destination.insert(identity.stable_id);
      auto &namespace_destination =
          impl_->visible_modules.count(module.sidecar.module.canonical_name) &&
                  IsGrantedSPI(identity, module.input.spi_groups)
              ? visible_namespace_ids : hidden_namespace_ids;
      namespace_destination.insert(identity.stable_id);
    }
  }
  for (const auto &id : visible_namespace_ids) hidden_namespace_ids.erase(id);
  for (const auto &id : visible_ids) hidden_ids.erase(id);
  auto *target_tu = target_context.getTranslationUnitDecl();
  // Keep the lexical declarations and redeclaration chains intact. Imported
  // C++ forward declarations can share a lookup entry with a later definition;
  // removeDecl would erase that entry before removing the other redeclaration.
  // Visibility is controlled only by lookup, including transparent enum
  // constants.
  auto *lookup = target_tu->buildLookup();
  NamedDeclarationSnapshot all_declarations;
  all_declarations.TraverseDecl(target_tu);
  std::vector<clang::NamedDecl *> hide;
  for (auto *named : all_declarations.Declarations) {
    if (named->isImplicit() || preexisting.Declarations.count(named) ||
        !IsTranslationUnitLookupDeclaration(named))
      continue;
    const auto stable_id = ReadInterfaceDeclarationStableId(named);
    if (!stable_id.empty() && hidden_ids.count(stable_id))
      hide.push_back(named);
  }
  for (auto *decl : hide) {
    if (lookup) {
      auto found = lookup->find(decl->getDeclName());
      if (found != lookup->end() && !found->second.isNull()) {
        const auto entries = found->second.getLookupResult();
        if (std::find(entries.begin(), entries.end(), decl) != entries.end())
          found->second.remove(decl);
      }
    }
  }

  // Importing a namespace recursively imports its contents. Its public name
  // must not expose ungranted SPI or members of a private dependency. Keep
  // lexical declarations for ABI/type references and control namespace lookup.
  // A later direct import or SPI grant can promote a previously hidden member.
  for (auto *named : all_declarations.Declarations) {
    if (named->isImplicit() || (!named->getIdentifier() && !llvm::isa<clang::UsingDirectiveDecl>(named))) continue;
    auto *scope = named->getDeclContext();
    while (scope && scope->isTransparentContext()) scope=scope->getParent();
    if (!scope || !scope->isNamespace()) continue;
    scope=scope->getPrimaryContext();
    const auto id = ReadInterfaceDeclarationStableId(named);
    if (id.empty()) continue;
    auto *table=scope->buildLookup();
    auto found=table ? table->find(named->getDeclName()) : clang::StoredDeclsMap::iterator();
    if (visible_namespace_ids.count(id)) {
      bool present=false;
      if (table && found!=table->end() && !found->second.isNull())
        for (auto *entry : found->second.getLookupResult())
          present |= entry->getCanonicalDecl()==named->getCanonicalDecl();
      if (!present) scope->makeDeclVisibleInContext(named);
    } else if (!preexisting.Declarations.count(named) && hidden_namespace_ids.count(id) &&
               table && found!=table->end() && !found->second.isNull()) {
      const auto entries=found->second.getLookupResult();
      if (std::find(entries.begin(),entries.end(),named)!=entries.end())
        found->second.remove(named);
    }
  }
  const auto &manager = target_context.getSourceManager();
  std::stable_sort(report.inline_definitions.begin(), report.inline_definitions.end(),
      [&](const auto *left, const auto *right) {
        return manager.isBeforeInTranslationUnit(left->getLocation(), right->getLocation());
      });
  // Header/template instantiations can have independent SourceManager roots.
  // Keep their dependency import order instead of comparing unrelated roots.
  report.inline_definitions.insert(report.inline_definitions.begin(),
      auxiliary_definitions.begin(),auxiliary_definitions.end());
  std::set<const clang::Decl *> registered_definitions;
  std::erase_if(report.inline_definitions, [&](const auto *declaration) {
    return !registered_definitions.insert(declaration->getCanonicalDecl()).second;
  });
  report.success = true;
  return report;
}

bool InterfaceImportSession::RequiresTaskHeader() const {
  if (!impl_) return false;
  return std::any_of(impl_->modules.begin(), impl_->modules.end(), [](const auto &module) {
    return module.source_bytes.find("#include <objc3c/runtime/objc3_task.hpp>") !=
           std::string::npos;
  });
}

bool InterfaceImportSession::RequiresKeyPathHeader() const {
  if (!impl_) return false;
  return std::any_of(impl_->modules.begin(), impl_->modules.end(), [](const auto &module) {
    return std::find(module.sidecar.required_semantics.begin(),
        module.sidecar.required_semantics.end(), "objc3.typed-keypaths.v1") !=
        module.sidecar.required_semantics.end();
  });
}

bool InterfaceImportSession::RequiresRuntimeHeader() const {
  if (!impl_) return false;
  return std::any_of(impl_->modules.begin(), impl_->modules.end(), [](const auto &module) {
    return std::find(module.sidecar.required_semantics.begin(),
        module.sidecar.required_semantics.end(), RuntimeInterfaceSemantic()) !=
        module.sidecar.required_semantics.end();
  });
}

std::vector<InterfaceSourceArtifact>
InterfaceImportSession::GetSourceArtifacts() const {
  std::vector<InterfaceSourceArtifact> result;
  if (!impl_) return result;
  result.reserve(impl_->import_order.size());
  for (auto index : impl_->import_order) {
    const auto &module = impl_->modules[index];
    InterfaceSourceArtifact source;
    source.module = module.sidecar.module;
    source.virtual_include_path = BuildInterfaceIncludePath(
        module.sidecar.module.canonical_name);
    source.source_bytes = module.source_bytes;
    source.declarations = module.sidecar.declarations;
    source.dependencies = module.sidecar.dependencies;
    result.push_back(std::move(source));
  }
  return result;
}

} // namespace objc3::clang_foundation
