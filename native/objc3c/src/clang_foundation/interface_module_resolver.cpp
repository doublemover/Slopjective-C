#include "clang_foundation/interface_module_resolver.h"
#include "clang_foundation/runtime_interface_types.h"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <map>
#include <set>
#include <tuple>

#include "clang_foundation/interface_codec.h"
#include "clang_foundation/interface_validation.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/Twine.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/MemoryBuffer.h"

namespace objc3::clang_foundation {
namespace fs = std::filesystem;
namespace {
constexpr std::uint64_t kMaxArtifactBytes = kInterfaceArtifactByteLimit;
constexpr std::size_t kMaxSearchEntries = 50000;
const std::vector<llvm::StringRef> kSupportedSemantics = {
    "objc3.canonical-function-effects.v1",
    "objc3.native-carrier-types.v1",
    "objc3.native-return-ownership.v1",
    "objc3.native-ownership-contracts.v1",
    "objc3.isolation-contracts.v1",
    "objc3.typed-keypaths.v1",
    "objc3.keypath-subscripts.v1",
    kNullableKeyPathWriteSemantic,
    "objc3.async-entry-abi.v1",
    "objc3.direct-dispatch.v1",
    "objc3.cxx-data-interfaces.v1",
    "objc3.cxx-member-interfaces.v1",
    "objc3.cxx-class-lifecycles.v1",
    "objc3.cxx-class-structure.v1",
    "objc3.cxx-class-dispatch.v1",
    "objc3.cxx-namespaces.v1",
    "objc3.cxx-nested-types.v1",
    "objc3.cxx-using.v1",
    "objc3.cxx-declaration-expressions.v1",
    "objc3.cxx-definitions.v1",
    "objc3.cxx-defaulted-comparisons.v1",
    "objc3.cxx-out-of-line-defaulting.v1",
    "objc3.cxx-immediate-defaulting.v1",
    "objc3.cxx-namespace-attributes.v1",
    "objc3.cxx-lambda-expressions.v1",
    "objc3.cxx-thread-local.v1",
    "objc3.cxx-friends.v1",
    "objc3.cxx-block-expressions.v1",
    "objc3.cxx-statement-expressions.v1",
    "objc3.cxx-local-types.v1",
    "objc3.cxx-unnamed-local-types.v1",
    "objc3.cxx-generic-local-lambdas.v2",
    "objc3.cxx-union-values.v1",
    "objc3.cxx-concepts.v1",
    kCxxFunctionTemplateSemantic,
    kCxxAliasTemplateSemantic,
    kCxxVariableTemplateSemantic,
    kCxxClassTemplateSemantic,
    kCxxTemplateStructureSemantic,
    kCxxTemplateDeductionSemantic,
    kCxxMemberTemplateSemantic,
    kCxxMemberAliasTemplateSemantic,
    kCxxNestedTemplateSemantic,
    kCxxVariadicTemplateSemantic,
    kCxxConcreteMemberTemplateSemantic,
    kCxxConcreteNestedTemplateSemantic,
    kCxxTemplateTemplateSemantic,
    kCxxNominalAliasTemplateSemantic,
    kCxxFunctionSpecializationSemantic,
    kCxxVariableSpecializationSemantic,
    kCxxClassSpecializationSemantic,
    kCxxClassPartialSpecializationSemantic,
    kCxxVariablePartialSpecializationSemantic,
    kCxxVariableSpecializationStorageSemantic,
    kCxxDetachedCallableTemplateSemantic,
    kCxxDetachedStaticDataTemplateSemantic,
    kCxxDetachedNestedTemplateSemantic,
    kCxxConcreteMemberClassSpecializationSemantic,
    kCxxConcreteMemberFunctionSpecializationSemantic,
    kCxxConcreteMemberVariableSpecializationSemantic,
    kCxxConcreteSpecialMemberSpecializationSemantic,
    kCxxDependentMemberPartialSemantic,
    kCxxClosedParentMemberSemantic,
    kCxxClosedParentPatternSemantic,
    kCxxEffectfulFunctionTemplateSemantic,
    kCxxEffectfulTemplateMemberSemantic,
    kCxxTemplateCallableProjectionSemantic,
    kCxxAddressTemplateArgumentSemantic,
    kNativeDeclarationAttributeSemantic,
    kCxxCoroutineBodySemantic,
    kCxxUnnamedTemplateProjectionSemantic,
    kCxxStandardCoroutineSemantic,
    kForeignImportAttributeSemantic,
    kCxxNestedFactoryTypeSemantic,
    kCxxLambdaFactoryResultSemantic,
    kCxxClosureResultProjectionSemantic,
    kCxxFactoryValueArgumentSemantic,
    kCxxGenericFactoryProjectionSemantic,
    kCxxFactoryTemplateArgumentSemantic,
    kCxxFactoryTypeProjectionSemantic,
    kCxxLocalFactoryResultSemantic,
    kCxxClosureFactorySemantic,
    kCxxEffectfulLambdaSemantic,
    "objc3.cxx-async-definitions.v1",
    "objc3.sdk-runtime-declarations.v1",
    "objc3.ownership-projection.v1",
    "objc3.api-availability.v1",
    "objc3.api-partitions.v1",
    "objc3.module-owned-names.v1",
    RuntimeInterfaceSemantic()};

void Add(InterfaceModuleResolution &result, llvm::StringRef code,
         const llvm::Twine &message, llvm::StringRef subject = {}) {
  result.diagnostics.push_back({code.str(), message.str(), subject.str()});
}

bool ReadBounded(llvm::StringRef path, std::string &text,
                 InterfaceModuleResolution &result, llvm::StringRef kind,
                 const InterfaceResourceLimits &limits, bool selected = false) {
  std::uint64_t size = 0;
  if (auto error = llvm::sys::fs::file_size(path, size)) {
    Add(result, "O3IFC130", llvm::Twine("cannot stat ") + kind +
        " artifact: " + error.message(), path);
    return false;
  }
  if (size > kMaxArtifactBytes) {
    Add(result, "O3IFC130", llvm::Twine(kind) +
        " artifact exceeds the 32 MiB import bound", path);
    return false;
  }
  if (!ChargeInterfaceResource(result.resource_usage.read_bytes, size, limits.read_bytes)) {
    Add(result, kInterfaceResourceDiagnostic, "interface artifacts exceed the aggregate read-work budget");
    return false;
  }
  if (selected && !ChargeInterfaceResource(result.resource_usage.selected_bytes, size,
                                             limits.selected_bytes)) {
    Add(result, kInterfaceResourceDiagnostic, "selected interface artifacts exceed their aggregate byte budget");
    return false;
  }
  // Allocate only the already admitted byte count. A concurrent replacement or
  // growth must not make MemoryBuffer allocate/map an unbounded new file.
  std::ifstream input(path.str(), std::ios::binary);
  if (!input) {
    Add(result, "O3IFC130", llvm::Twine("cannot open ") + kind + " artifact", path);
    return false;
  }
  text.resize(size);
  input.read(text.data(), static_cast<std::streamsize>(size));
  const auto actual = static_cast<std::uint64_t>(input.gcount());
  if (input.bad() || (actual == size && input.peek() != std::char_traits<char>::eof())) {
    Add(result, "O3IFC130", llvm::Twine(kind) + " artifact changed or failed during bounded reading", path);
    return false;
  }
  if (actual < size) {
    result.resource_usage.read_bytes -= size - actual;
    if (selected) result.resource_usage.selected_bytes -= size - actual;
    text.resize(actual);
  }
  return true;
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

bool CompatibleTarget(const InterfaceModuleIdentity &identity,
                      const InterfaceImportTarget &target) {
  return identity.target_triple == target.target_triple &&
         identity.runtime_abi == target.runtime_abi &&
         identity.extension_abi == target.extension_abi &&
         identity.language_revision == target.language_revision &&
         identity.language_profile == target.language_profile;
}

struct Candidate {
  std::string source_path;
  std::string sidecar_path;
  InterfaceModuleIdentity identity;
  std::string source_sha256;
  std::string sidecar_sha256;
  std::uint64_t stringBytes() const {
    return source_path.size() + sidecar_path.size() + source_sha256.size() + sidecar_sha256.size() +
        2 * identity.canonical_name.size() + identity.semantic_fingerprint.size() +
        identity.target_triple.size() + identity.runtime_abi.size() + identity.extension_abi.size() +
        identity.language_revision.size() + identity.language_profile.size() + identity.package_identity.size();
  }
};

bool Resource(InterfaceModuleResolution &result, std::uint64_t &used,
               std::uint64_t amount, std::uint64_t limit, llvm::StringRef name) {
  if (ChargeInterfaceResource(used, amount, limit)) return true;
  Add(result, kInterfaceResourceDiagnostic, llvm::Twine("interface resolution exceeds its ") + name + " budget");
  return false;
}

} // namespace

std::string BuildInterfaceIncludePath(llvm::StringRef canonical_module_name) {
  static constexpr char Hex[] = "0123456789abcdef";
  std::string path = "__objc3_imports/m-";
  for (unsigned char byte : canonical_module_name.bytes()) {
    path.push_back(Hex[byte >> 4]);
    path.push_back(Hex[byte & 0x0f]);
  }
  path += ".objc3i";
  return path;
}

InterfaceModuleResolution ResolveInterfaceModules(
    const std::vector<std::string> &search_roots,
    const std::vector<InterfaceModuleRequest> &requested_modules,
    const InterfaceImportTarget &target) {
  InterfaceModuleResolution result;
  // Compact index: no source bodies, JSON buffers or unselected declarations
  // survive one candidate iteration. Identical copies share one chosen path.
  std::map<std::string, Candidate> index;
  const auto &limits = target.resource_limits;
  auto &usage = result.resource_usage;
  std::set<std::string> canonical_roots;
  std::set<std::string> candidate_paths;
  std::size_t visited = 0;

  for (const auto &root_text : search_roots) {
    std::error_code error;
    fs::path root = fs::canonical(fs::path(root_text), error);
    if (error || !fs::is_directory(root, error) || error) {
      Add(result, "O3IFC145", "interface search root is missing or is not a directory",
          root_text);
      continue;
    }
    if (!canonical_roots.insert(root.generic_string()).second) continue;
    fs::recursive_directory_iterator it(root,
        fs::directory_options::skip_permission_denied, error), end;
    for (; !error && it != end; it.increment(error)) {
      if (++visited > kMaxSearchEntries) {
        Add(result, "O3IFC145", "interface search root exceeds the 50,000-entry scan bound",
            root_text);
        break;
      }
      const auto status = it->symlink_status(error);
      if (error) { error.clear(); continue; }
      if (fs::is_symlink(status)) {
        if (it->is_directory(error)) it.disable_recursion_pending();
        error.clear();
        continue;
      }
      if (!fs::is_regular_file(status)) continue;
      const auto sidecar_path = it->path();
      const auto name = sidecar_path.filename().string();
      constexpr llvm::StringLiteral suffix = ".objc3i.json";
      if (!llvm::StringRef(name).ends_with(suffix)) continue;
      fs::path source_path = sidecar_path;
      source_path.replace_filename(name.substr(0, name.size() - 5));
      if (source_path.extension() != ".objc3i") continue;

      const auto path = sidecar_path.string();
      if (!candidate_paths.count(path)) {
        if (!Resource(result, usage.scan_path_bytes, path.size(),
                      limits.scan_path_bytes, "scan path string")) return result;
        candidate_paths.insert(path);
      }
    }
    if (error)
      Add(result, "O3IFC145", llvm::Twine("failed while scanning interface search root: ") +
          error.message(), root_text);
  }
  if (!result.diagnostics.empty()) return result;

  // Decode in canonical path order so duplicates, selected paths and budget
  // boundaries do not depend on directory iteration or caller root order.
  for (const auto &sidecar_path : candidate_paths) {
      Candidate candidate;
      candidate.source_path = sidecar_path.substr(0, sidecar_path.size() - 5);
      candidate.sidecar_path = sidecar_path;
      std::string source_bytes, sidecar_bytes;
      if (!ReadBounded(candidate.source_path, source_bytes, result, "source", limits) ||
          !ReadBounded(candidate.sidecar_path, sidecar_bytes, result, "sidecar", limits)) {
        if (!result.diagnostics.empty() && result.diagnostics.back().code == kInterfaceResourceDiagnostic)
          return result;
        continue;
      }
      ++usage.candidate_pairs;
      auto decoded = DecodeSemanticInterfaceSidecar(
          sidecar_bytes, source_bytes, llvm::ArrayRef<llvm::StringRef>(kSupportedSemantics));
      if (!decoded) {
        Add(result, "O3IFC133", llvm::Twine("invalid interface artifact: ") +
            llvm::toString(decoded.takeError()), candidate.sidecar_path);
        continue;
      }
      if (!Resource(result, usage.decoded_declarations, decoded->declarations.size(),
                    limits.decoded_declarations, "decoded declaration work")) return result;
      candidate.identity = decoded->module;
      candidate.source_sha256 = decoded->canonical_source_sha256;
      candidate.sidecar_sha256 = ComputeInterfaceSourceSha256(sidecar_bytes);
      auto existing = index.find(candidate.identity.canonical_name);
      if (existing != index.end()) {
        ++usage.coalesced_duplicates;
        if (!SameIdentity(existing->second.identity, candidate.identity) ||
            existing->second.source_sha256 != candidate.source_sha256) {
          Add(result, "O3IFC146", "search roots resolve one canonical module name to conflicting artifacts",
              candidate.identity.canonical_name);
          continue;
        }
        if (candidate.source_path < existing->second.source_path) {
          const auto old_size = existing->second.stringBytes();
          const auto new_size = candidate.stringBytes();
          if (new_size > old_size && !Resource(result, usage.index_string_bytes,
                new_size - old_size, limits.index_string_bytes, "compact index string")) return result;
          if (new_size < old_size) usage.index_string_bytes -= old_size - new_size;
          existing->second = std::move(candidate);
        }
      } else {
        if (!Resource(result, usage.index_modules, 1, limits.index_modules, "indexed module") ||
            !Resource(result, usage.index_string_bytes, candidate.stringBytes(),
                      limits.index_string_bytes, "compact index string")) return result;
        const auto name = candidate.identity.canonical_name;
        index.emplace(name, std::move(candidate));
      }
  }
  if (!result.diagnostics.empty()) return result;
  candidate_paths.clear(); // release scan strings before retaining selected bodies

  std::map<std::string, SemanticInterfaceSidecar> selected;
  std::map<std::string, InterfaceImportInput> inputs;
  auto select = [&](const InterfaceModuleIdentity &identity,
                    bool direct, bool public_import, bool reexported) -> bool {
    auto found = index.find(identity.canonical_name);
    if (found == index.end()) {
      Add(result, "O3IFC147", "required canonical interface artifact is absent from all search roots",
          identity.canonical_name);
      return false;
    }
    Candidate &candidate = found->second;
    if (!SameIdentity(candidate.identity, identity)) {
      Add(result, "O3IFC134", "artifact identity does not match the dependency lock",
          identity.canonical_name);
      return false;
    }
    if (!CompatibleTarget(identity, target)) {
      Add(result, "O3IFC135",
          "interface target, runtime, extension ABI, language revision, or profile differs from the consumer",
          identity.canonical_name);
      return false;
    }
    auto existing = inputs.find(identity.canonical_name);
    if (existing != inputs.end()) {
      if (!SameIdentity(existing->second.locked_identity, identity)) {
        Add(result, "O3IFC136",
            "the same canonical module name was selected with conflicting lock identities",
            identity.canonical_name);
        return false;
      }
      if (direct) {
        if (existing->second.direct_import) {
          existing->second.public_import |= public_import;
          existing->second.reexported |= reexported;
        } else {
          existing->second.direct_import = true;
          existing->second.public_import = public_import;
          existing->second.reexported = reexported;
        }
      }
      return true;
    }
    if (!Resource(result, usage.selected_modules, 1, limits.selected_modules, "selected module"))
      return false;
    std::string source_bytes, sidecar_bytes;
    if (!ReadBounded(candidate.source_path, source_bytes, result, "source", limits, true) ||
        !ReadBounded(candidate.sidecar_path, sidecar_bytes, result, "sidecar", limits, true)) return false;
    if (ComputeInterfaceSourceSha256(source_bytes) != candidate.source_sha256 ||
        ComputeInterfaceSourceSha256(sidecar_bytes) != candidate.sidecar_sha256) {
      Add(result, "O3IFC134", "indexed interface bytes changed before selected artifact loading",
          identity.canonical_name);
      return false;
    }
    auto decoded = DecodeSemanticInterfaceSidecar(sidecar_bytes, source_bytes,
        llvm::ArrayRef<llvm::StringRef>(kSupportedSemantics));
    if (!decoded) {
      Add(result, "O3IFC133", llvm::Twine("invalid selected interface artifact: ") +
          llvm::toString(decoded.takeError()), candidate.sidecar_path);
      return false;
    }
    if (!SameIdentity(decoded->module, identity)) {
      Add(result, "O3IFC134", "selected interface identity changed after indexing", identity.canonical_name);
      return false;
    }
    if (!Resource(result, usage.decoded_declarations, decoded->declarations.size(), limits.decoded_declarations, "decoded declaration work") ||
        !Resource(result, usage.selected_declarations, decoded->declarations.size(), limits.selected_declarations, "selected declaration") ||
        !Resource(result, usage.dependency_edges, decoded->dependencies.size(), limits.dependency_edges, "selected dependency-edge"))
      return false;
    InterfaceImportInput input;
    input.source_path = candidate.source_path;
    input.sidecar_path = candidate.sidecar_path;
    input.locked_identity = identity;
    input.direct_import = direct;
    input.public_import = public_import;
    input.reexported = reexported;
    inputs.emplace(identity.canonical_name, std::move(input));
    result.requires_coroutine_header |= std::find(decoded->required_semantics.begin(),
        decoded->required_semantics.end(), kCxxCoroutineBodySemantic) != decoded->required_semantics.end() ||
        std::find(decoded->required_semantics.begin(), decoded->required_semantics.end(), kCxxStandardCoroutineSemantic) != decoded->required_semantics.end();
    selected.emplace(identity.canonical_name, std::move(*decoded));
    return true;
  };

  std::vector<std::string> worklist;
  for (const auto &request : requested_modules) {
    if (request.canonical_name.empty() ||
        request.canonical_name != request.locked_identity.canonical_name) {
      Add(result, "O3IFC131", "module request name and locked canonical identity must match",
          request.canonical_name);
      continue;
    }
    if (select(request.locked_identity, request.initially_visible,
               request.public_import,
               request.reexported))
      worklist.push_back(request.canonical_name);
    if (inputs.count(request.canonical_name)) {
      auto &groups = inputs.at(request.canonical_name).spi_groups;
      groups.insert(groups.end(), request.spi_groups.begin(),
                    request.spi_groups.end());
    }
  }
  if (!result.diagnostics.empty()) return result;

  // Resolve exact sidecar dependency locks recursively. Paths only identify
  // candidate bytes and never substitute for dependency identity.
  for (std::size_t cursor = 0; cursor < worklist.size(); ++cursor) {
    const auto current_name = worklist[cursor];
    const auto &candidate = selected.at(current_name);
    for (const auto &dependency : candidate.dependencies) {
      if (!select(dependency.module, false, dependency.public_import,
                  dependency.reexported))
        continue;
      if (std::find(worklist.begin(), worklist.end(),
                    dependency.module.canonical_name) == worklist.end())
        worklist.push_back(dependency.module.canonical_name);
    }
  }
  if (!result.diagnostics.empty()) return result;

  std::vector<InterfaceDependencyNode> graph_nodes;
  for (const auto &entry : selected)
    graph_nodes.push_back({entry.second.module,
                           entry.second.dependencies});
  const auto graph = ResolveInterfaceDependencyGraph(graph_nodes, limits);
  for (const auto &diagnostic : graph.diagnostics)
    Add(result, diagnostic.code, diagnostic.message,
        llvm::join(diagnostic.modules, ", "));
  if (!graph.ok()) return result;

  // Direct imports are visible. A dependency is visible to the consumer only
  // when every edge from one of its direct imports is explicitly re-exported.
  std::set<std::string> visible;
  std::vector<std::string> visibility_worklist;
  for (const auto &input : inputs)
    if (input.second.direct_import) {
      visible.insert(input.first);
      visibility_worklist.push_back(input.first);
    }
  for (std::size_t cursor = 0; cursor < visibility_worklist.size(); ++cursor) {
    const auto &candidate = selected.at(visibility_worklist[cursor]);
    for (const auto &dependency : candidate.dependencies) {
      if (!dependency.reexported || !dependency.public_import) continue;
      if (visible.insert(dependency.module.canonical_name).second)
        visibility_worklist.push_back(dependency.module.canonical_name);
    }
  }

  for (const auto &name : graph.topological_order) {
    result.dependency_order.push_back(name);
    result.artifacts.push_back(std::move(inputs.at(name)));
  }
  result.visible_module_names.assign(visible.begin(), visible.end());
  result.success = true;
  return result;
}

} // namespace objc3::clang_foundation
