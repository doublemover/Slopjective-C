#pragma once

#include <cstddef>
#include <string>
#include <vector>

enum class Objc3ModuleInteropLaneState {
  kSupported,
  kReserved,
};

struct Objc3ModuleInteropForeignLaneContract {
  std::string language;
  std::string surface_id;
  Objc3ModuleInteropLaneState state = Objc3ModuleInteropLaneState::kReserved;
  std::string diagnostic_code;
  std::vector<std::string> evidence_anchors_source_order;
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
  std::string package_lock_module_identity;
  std::string module_identity_rebuild_key;
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
