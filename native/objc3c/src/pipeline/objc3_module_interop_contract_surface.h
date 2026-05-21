#pragma once

#include <cstddef>
#include <string>
#include <vector>

enum class Objc3ModuleImportVisibility {
  kPublic,
  kPrivate,
  kInternal,
};

enum class Objc3ModuleInteropLaneState {
  kSupported,
  kReserved,
};

struct Objc3ModuleImportEdgeContract {
  std::string module_name;
  std::string metadata_version;
  std::string abi_identity;
  Objc3ModuleImportVisibility visibility = Objc3ModuleImportVisibility::kPrivate;
  bool reexported = false;
  std::vector<std::string> exported_symbols_source_order;
  bool rebuild_affects_semantic = false;
  bool rebuild_affects_abi = false;
  bool rebuild_affects_link = false;
  bool rebuild_affects_package_lock = false;
};

struct Objc3ModuleVisibilityAccessContract {
  std::string symbol;
  std::string provided_by;
  Objc3ModuleImportVisibility visibility = Objc3ModuleImportVisibility::kPrivate;
  bool allowed = false;
  std::string diagnostic_code;
};

struct Objc3ModuleInteropForeignLaneContract {
  std::string language;
  std::string surface_id;
  std::string symbol_owner;
  Objc3ModuleInteropLaneState state = Objc3ModuleInteropLaneState::kReserved;
  std::string diagnostic_code;
  std::vector<std::string> evidence_anchors_source_order;
  std::size_t abi_alignment = 0;
  bool fail_closed = true;
  bool ownership_policy_explicit = true;
  bool error_policy_explicit = true;
  bool async_policy_explicit = true;
  bool object_identity_policy_explicit = true;
};

struct Objc3ModuleInteropContractSurface {
  std::string module_name;
  std::string module_metadata_version;
  std::string module_abi_identity;
  std::string module_source_digest;
  std::string package_lock_module_identity;
  std::string module_identity_rebuild_key;
  std::vector<Objc3ModuleImportEdgeContract> import_edges_source_order;
  std::vector<std::string> public_exports_source_order;
  std::vector<std::string> private_exports_source_order;
  std::vector<std::string> package_imported_module_identities_source_order;
  std::vector<Objc3ModuleVisibilityAccessContract>
      visibility_access_source_order;
  std::size_t public_import_edge_count = 0;
  std::size_t private_import_edge_count = 0;
  std::size_t reexported_import_edge_count = 0;
  std::size_t public_export_count = 0;
  std::size_t private_export_count = 0;
  bool deterministic_rebuild_identity = false;
  bool visibility_fail_closed = false;
  bool package_identity_matches_module_identity = false;
  std::string bridge_header_relative_path;
  std::string bridge_modulemap_relative_path;
  std::string bridge_metadata_relative_path;
  std::string bridge_metadata_digest;
  std::string mixed_image_loader_metadata_digest;
  std::string stale_metadata_diagnostic_code;
  std::string abi_mismatch_diagnostic_code;
  std::size_t c_foreign_type_contract_count = 0;
  std::size_t objc2_bridge_metadata_only_count = 0;
  bool objc2_bridge_metadata_only_retired_syntax_rejected = false;
  std::size_t swift_callable_metadata_count = 0;
  std::size_t cpp_callable_metadata_count = 0;
  std::size_t supported_foreign_lane_count = 0;
  std::size_t reserved_foreign_lane_count = 0;
  std::size_t abi_alignment_contract_count = 0;
  std::vector<Objc3ModuleInteropForeignLaneContract> foreign_lane_contracts;
  bool foreign_symbol_ownership_fail_closed = false;
  bool bridge_metadata_digest_participates_in_rebuild_key = false;
  bool reserved_lanes_have_stable_diagnostics = false;
};

std::string Objc3ModuleImportVisibilityName(Objc3ModuleImportVisibility visibility);
std::string BuildObjc3ModuleImportIdentity(
    const Objc3ModuleImportEdgeContract &import_edge);
std::string BuildObjc3ModuleInteropRebuildKey(
    const Objc3ModuleInteropContractSurface &surface);
bool ValidateObjc3ModuleInteropContractSurface(
    const Objc3ModuleInteropContractSurface &surface,
    std::string &error);
bool IsReadyObjc3ModuleInteropContractSurface(
    const Objc3ModuleInteropContractSurface &surface);
