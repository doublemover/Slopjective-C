#include "ir/objc3_ir_runtime_artifact_emission.h"

#include <cstddef>
#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_runtime_bootstrap_global_emission.h"
#include "lower/contracts/runtime_metadata_layout_policy_record_contracts.h"
#include "lower/contracts/runtime_metadata_object_format_contracts.h"
#include "lower/contracts/runtime_metadata_source_record_contracts.h"

namespace {

void EmitRetained(std::vector<std::string> &retained_globals,
                  const std::string &symbol) {
  retained_globals.push_back(symbol);
}

void EmitObjc3IRCanonicalPoolSection(
    const std::map<std::string, std::string> &pool_globals,
    const std::string &aggregate_symbol, const std::string &logical_section,
    std::ostringstream &out, std::vector<std::string> &retained_globals) {
  const std::string emitted_section_name =
      Objc3RuntimeMetadataHostSectionForLogicalName(logical_section);
  for (const auto &entry : pool_globals) {
    const std::string &value = entry.first;
    const std::string &symbol = entry.second;
    out << symbol << " = private unnamed_addr constant ["
        << (value.size() + 1u) << " x i8] c\"" << EscapeCStringLiteral(value)
        << "\\00\", section \"" << emitted_section_name << "\", align 1\n";
  }
  out << aggregate_symbol << " = internal "
      << (pool_globals.empty() ? "constant " : "global ");
  if (pool_globals.empty()) {
    out << "{ i64 } { i64 0 }";
  } else {
    out << "{ i64, [" << pool_globals.size()
        << " x ptr] } { i64 " << pool_globals.size() << ", ["
        << pool_globals.size() << " x ptr] [";
    std::size_t index = 0;
    for (const auto &entry : pool_globals) {
      if (index++ != 0) {
        out << ", ";
      }
      out << "ptr " << entry.second;
    }
    out << "] }";
  }
  out << ", section \"" << emitted_section_name << "\", align 8\n";
  EmitRetained(retained_globals, aggregate_symbol);
}

bool EmitObjc3IRTypedKeypathArtifacts(
    const Objc3IRRuntimeArtifactEmissionOptions &options, std::ostringstream &out,
    std::vector<std::string> &retained_globals, std::string &error) {
  if (options.typed_keypath_artifacts.empty()) {
    return true;
  }

  const std::string emitted_section_name =
      Objc3RuntimeMetadataHostSectionForLogicalName(
          kObjc3RuntimeKeypathDescriptorLogicalSection);
  const std::string generic_metadata_replay_key_symbol =
      options.frontend_metadata.lowering_generic_metadata_abi_replay_key.empty()
          ? "null"
          : options.runtime_string_pool_globals
                .find(options.frontend_metadata
                          .lowering_generic_metadata_abi_replay_key)
                ->second;
  std::vector<std::string> descriptor_symbols;
  descriptor_symbols.reserve(options.typed_keypath_artifacts.size());
  for (const auto &entry : options.typed_keypath_artifacts) {
    const TypedKeyPathArtifact &artifact = entry.second;
    const auto root_it =
        options.runtime_string_pool_globals.find(artifact.root_name);
    const auto component_it =
        options.runtime_string_pool_globals.find(artifact.component_path);
    const auto profile_it =
        options.runtime_string_pool_globals.find(artifact.profile);
    if (root_it == options.runtime_string_pool_globals.end() ||
        component_it == options.runtime_string_pool_globals.end() ||
        profile_it == options.runtime_string_pool_globals.end()) {
      error = "typed key-path artifact string-pool registration failed";
      return false;
    }
    descriptor_symbols.push_back(artifact.descriptor_symbol);
    out << artifact.descriptor_symbol
        << " = private global { i64, ptr, ptr, ptr, ptr, i1 } { i64 "
        << static_cast<unsigned long long>(artifact.ordinal + 1u) << ", ptr "
        << root_it->second << ", ptr " << component_it->second << ", ptr "
        << profile_it->second << ", ptr " << generic_metadata_replay_key_symbol
        << ", i1 " << (artifact.root_is_self ? 1 : 0) << " }, section \""
        << emitted_section_name << "\", align 8\n";
  }
  out << "@__objc3_sec_keypath_descriptors = internal global { i64, ["
      << descriptor_symbols.size() << " x ptr] } { i64 "
      << descriptor_symbols.size() << ", [" << descriptor_symbols.size()
      << " x ptr] [";
  for (std::size_t i = 0; i < descriptor_symbols.size(); ++i) {
    if (i != 0) {
      out << ", ";
    }
    out << "ptr " << descriptor_symbols[i];
  }
  out << "] }, section \"" << emitted_section_name << "\", align 8\n";
  EmitRetained(retained_globals, "@__objc3_sec_keypath_descriptors");
  return true;
}

}  // namespace

bool EmitObjc3IRRuntimeArtifacts(
    const Objc3IRRuntimeArtifactEmissionOptions &options, std::ostringstream &out,
    std::vector<std::string> &retained_globals, std::string &error) {
  const bool emit_selector_string_pools =
      !options.selector_pool_globals.empty() ||
      !options.runtime_string_pool_globals.empty();
  if (emit_selector_string_pools) {
    EmitObjc3IRCanonicalPoolSection(
        options.selector_pool_globals, "@__objc3_sec_selector_pool",
        kObjc3RuntimeSelectorPoolLogicalSection, out, retained_globals);
    EmitObjc3IRCanonicalPoolSection(
        options.runtime_string_pool_globals, "@__objc3_sec_string_pool",
        kObjc3RuntimeStringPoolLogicalSection, out, retained_globals);
  }

  const bool emit_typed_keypath_artifacts =
      !options.typed_keypath_artifacts.empty();
  if (!EmitObjc3IRTypedKeypathArtifacts(options, out, retained_globals,
                                        error)) {
    return false;
  }

  std::vector<std::string> discovery_root_targets;
  discovery_root_targets.reserve(options.layout_policy.families.size() + 3);
  discovery_root_targets.push_back(options.image_info_symbol);
  for (const auto &family : options.layout_policy.families) {
    discovery_root_targets.push_back("@" + family.aggregate_symbol_name);
  }
  if (emit_selector_string_pools) {
    discovery_root_targets.push_back("@__objc3_sec_selector_pool");
    discovery_root_targets.push_back("@__objc3_sec_string_pool");
  }
  if (emit_typed_keypath_artifacts) {
    discovery_root_targets.push_back("@__objc3_sec_keypath_descriptors");
  }

  const std::string discovery_root_symbol =
      "@" + options.discovery_root_symbol;
  const std::string linker_anchor_symbol = "@" + options.linker_anchor_symbol;
  out << discovery_root_symbol << " = dso_local constant { i64, ["
      << discovery_root_targets.size() << " x ptr] } { i64 "
      << discovery_root_targets.size() << ", ["
      << discovery_root_targets.size() << " x ptr] [";
  for (std::size_t i = 0; i < discovery_root_targets.size(); ++i) {
    if (i != 0) {
      out << ", ";
    }
    out << "ptr " << discovery_root_targets[i];
  }
  out << "] }, section \""
      << Objc3RuntimeMetadataHostSectionForLogicalName(
             kObjc3RuntimeLinkerDiscoveryRootLogicalSection)
      << "\", align 8\n";
  out << linker_anchor_symbol << " = dso_local global ptr "
      << discovery_root_symbol << ", section \""
      << Objc3RuntimeMetadataHostSectionForLogicalName(
             kObjc3RuntimeLinkerAnchorLogicalSection)
      << "\", align 8\n";
  EmitRetained(retained_globals, discovery_root_symbol);
  EmitRetained(retained_globals, linker_anchor_symbol);

  if (options.emit_runtime_bootstrap_lowering) {
    Objc3IRRuntimeBootstrapGlobalEmissionOptions bootstrap_options;
    bootstrap_options.module_name = options.module_name;
    bootstrap_options.discovery_root_symbol = discovery_root_symbol;
    bootstrap_options.linker_anchor_symbol = linker_anchor_symbol;
    bootstrap_options.emit_selector_string_pools = emit_selector_string_pools;
    bootstrap_options.emit_typed_keypath_artifacts =
        emit_typed_keypath_artifacts;
    bootstrap_options.emit_registration_descriptor_image_root =
        options.emit_runtime_bootstrap_registration_descriptor_image_root;
    EmitObjc3IRRuntimeBootstrapGlobals(
        options.frontend_metadata, options.runtime_metadata_symbols,
        bootstrap_options, out, retained_globals);
  }

  out << "@llvm.used = appending global [" << retained_globals.size()
      << " x ptr] [";
  for (std::size_t i = 0; i < retained_globals.size(); ++i) {
    if (i != 0) {
      out << ", ";
    }
    out << "ptr " << retained_globals[i];
  }
  out << "], section \"llvm.metadata\"\n\n";
  return true;
}
