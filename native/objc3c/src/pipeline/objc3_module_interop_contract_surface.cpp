#include "pipeline/objc3_module_interop_contract_surface.h"

#include <algorithm>
#include <cctype>
#include <set>
#include <sstream>
#include <string>
#include <vector>

std::string Objc3ModuleImportVisibilityName(
    Objc3ModuleImportVisibility visibility) {
  switch (visibility) {
    case Objc3ModuleImportVisibility::kPublic:
      return "public";
    case Objc3ModuleImportVisibility::kPrivate:
      return "private";
    case Objc3ModuleImportVisibility::kInternal:
      return "internal";
  }
  return "private";
}

std::string BuildObjc3ModuleImportIdentity(
    const Objc3ModuleImportEdgeContract &import_edge) {
  return import_edge.module_name + "@" + import_edge.metadata_version + ":" +
         import_edge.abi_identity;
}

namespace {

bool HasAnyRebuildEffect(const Objc3ModuleImportEdgeContract &edge) {
  return edge.rebuild_affects_semantic || edge.rebuild_affects_abi ||
         edge.rebuild_affects_link || edge.rebuild_affects_package_lock;
}

bool IsStableDiagnosticCode(const std::string &code) {
  return code.rfind("O3", 0) == 0 && code.size() > 2;
}

bool IsPowerOfTwo(std::size_t value) {
  return value != 0u && (value & (value - 1u)) == 0u;
}

bool IsSha256HexDigest(const std::string &digest) {
  return digest.size() == 64u &&
         std::all_of(digest.begin(), digest.end(), [](unsigned char ch) {
           return std::isxdigit(ch) != 0;
         });
}

bool InsertUnique(const std::string &value,
                  std::set<std::string> &seen,
                  const char *label,
                  std::string &error) {
  if (value.empty()) {
    error = std::string(label) + " must not be empty";
    return false;
  }
  if (!seen.insert(value).second) {
    error = std::string(label) + " must be unique: " + value;
    return false;
  }
  return true;
}

std::string JoinSortedStrings(std::vector<std::string> values) {
  std::sort(values.begin(), values.end());
  std::ostringstream out;
  for (std::size_t index = 0; index < values.size(); ++index) {
    if (index != 0u) {
      out << ",";
    }
    out << values[index];
  }
  return out.str();
}

}  // namespace

std::string BuildObjc3ModuleInteropRebuildKey(
    const Objc3ModuleInteropContractSurface &surface) {
  std::vector<Objc3ModuleImportEdgeContract> imports =
      surface.import_edges_source_order;
  std::sort(imports.begin(), imports.end(), [](const auto &left,
                                               const auto &right) {
    return left.module_name < right.module_name;
  });

  std::ostringstream import_part;
  for (std::size_t index = 0; index < imports.size(); ++index) {
    const Objc3ModuleImportEdgeContract &edge = imports[index];
    if (index != 0u) {
      import_part << ",";
    }
    import_part << BuildObjc3ModuleImportIdentity(edge) << ":"
                << Objc3ModuleImportVisibilityName(edge.visibility) << ":"
                << (edge.reexported ? "reexport" : "import");
  }

  std::vector<Objc3ModuleInteropForeignLaneContract> lanes =
      surface.foreign_lane_contracts;
  std::sort(lanes.begin(), lanes.end(), [](const auto &left,
                                           const auto &right) {
    return left.language < right.language;
  });

  std::ostringstream foreign_part;
  for (std::size_t index = 0; index < lanes.size(); ++index) {
    const Objc3ModuleInteropForeignLaneContract &lane = lanes[index];
    if (index != 0u) {
      foreign_part << ",";
    }
    foreign_part << lane.language << ":" << lane.surface_id << ":"
                 << lane.symbol_owner << ":" << lane.abi_alignment << ":"
                 << (lane.state == Objc3ModuleInteropLaneState::kSupported
                         ? "supported"
                         : "reserved");
  }

  return "module=" + surface.module_name + ";metadata=" +
         surface.module_metadata_version + ";abi=" +
         surface.module_abi_identity + ";source_digest=" +
         surface.module_source_digest + ";package=" +
         surface.package_lock_module_identity + ";imports=[" +
         import_part.str() + "];exports=[" +
         JoinSortedStrings(surface.public_exports_source_order) +
         "];bridge_metadata_digest=" + surface.bridge_metadata_digest +
         ";mixed_image_loader_metadata_digest=" +
         surface.mixed_image_loader_metadata_digest + ";foreign=[" +
         foreign_part.str() + "]";
}

bool ValidateObjc3ModuleInteropContractSurface(
    const Objc3ModuleInteropContractSurface &surface,
    std::string &error) {
  if (surface.module_name.empty() ||
      surface.module_metadata_version.empty() ||
      surface.module_abi_identity.empty() ||
      surface.module_source_digest.empty()) {
    error = "module identity fields must be present";
    return false;
  }
  if (!IsSha256HexDigest(surface.module_source_digest) ||
      !IsSha256HexDigest(surface.bridge_metadata_digest) ||
      !IsSha256HexDigest(surface.mixed_image_loader_metadata_digest)) {
    error = "module interop rebuild digests must be stable sha256 hex values";
    return false;
  }
  if (!IsStableDiagnosticCode(surface.stale_metadata_diagnostic_code) ||
      !IsStableDiagnosticCode(surface.abi_mismatch_diagnostic_code)) {
    error = "module interop stale metadata and ABI mismatch diagnostics must "
            "be stable";
    return false;
  }
  if (!surface.deterministic_rebuild_identity ||
      surface.module_identity_rebuild_key !=
          BuildObjc3ModuleInteropRebuildKey(surface)) {
    error = "module interop rebuild key must be deterministic";
    return false;
  }
  if (!surface.visibility_fail_closed ||
      !surface.package_identity_matches_module_identity ||
      !surface.bridge_metadata_digest_participates_in_rebuild_key) {
    error =
        "module interop visibility, package identity, and bridge metadata must fail closed";
    return false;
  }

  std::size_t public_imports = 0;
  std::size_t private_imports = 0;
  std::size_t reexports = 0;
  std::set<std::string> import_names;
  std::set<std::string> exported_symbols;
  std::vector<std::string> expected_package_identities;
  for (const Objc3ModuleImportEdgeContract &edge :
       surface.import_edges_source_order) {
    if (!InsertUnique(edge.module_name, import_names,
                      "imported module name", error)) {
      return false;
    }
    if (edge.module_name == surface.module_name) {
      error = "module cannot import itself";
      return false;
    }
    if (!HasAnyRebuildEffect(edge)) {
      error = "import edge must participate in rebuild invalidation: " +
              edge.module_name;
      return false;
    }
    if (edge.visibility == Objc3ModuleImportVisibility::kPublic) {
      ++public_imports;
    } else {
      ++private_imports;
    }
    if (edge.reexported) {
      if (edge.visibility != Objc3ModuleImportVisibility::kPublic) {
        error = "reexported import edge must be public: " + edge.module_name;
        return false;
      }
      ++reexports;
    }
    expected_package_identities.push_back(BuildObjc3ModuleImportIdentity(edge));
    for (const std::string &symbol : edge.exported_symbols_source_order) {
      if (!InsertUnique(symbol, exported_symbols,
                        "exported import symbol", error)) {
        return false;
      }
    }
  }

  for (const std::string &symbol : surface.public_exports_source_order) {
    if (!InsertUnique(symbol, exported_symbols, "public export", error)) {
      return false;
    }
  }
  for (const std::string &symbol : surface.private_exports_source_order) {
    if (!InsertUnique(symbol, exported_symbols, "private export", error)) {
      return false;
    }
  }

  std::sort(expected_package_identities.begin(),
            expected_package_identities.end());
  std::vector<std::string> package_identities =
      surface.package_imported_module_identities_source_order;
  std::sort(package_identities.begin(), package_identities.end());
  if (package_identities != expected_package_identities) {
    error = "package imported module identities must match import graph";
    return false;
  }
  if (surface.public_import_edge_count != public_imports ||
      surface.private_import_edge_count != private_imports ||
      surface.reexported_import_edge_count != reexports ||
      surface.public_export_count != surface.public_exports_source_order.size() ||
      surface.private_export_count !=
          surface.private_exports_source_order.size()) {
    error = "module interop import/export counts drifted from source graph";
    return false;
  }

  for (const Objc3ModuleVisibilityAccessContract &access :
       surface.visibility_access_source_order) {
    if (access.symbol.empty() || access.provided_by.empty()) {
      error = "module visibility access contract must name symbol and provider";
      return false;
    }
    const bool public_access =
        access.visibility == Objc3ModuleImportVisibility::kPublic;
    if (access.allowed != public_access) {
      error = "module visibility access allowance drifted: " + access.symbol;
      return false;
    }
    if (!public_access && !IsStableDiagnosticCode(access.diagnostic_code)) {
      error = "hidden declaration access must fail closed with a stable "
              "diagnostic: " +
              access.symbol;
      return false;
    }
  }

  std::size_t supported_lanes = 0;
  std::size_t reserved_lanes = 0;
  for (const Objc3ModuleInteropForeignLaneContract &lane :
       surface.foreign_lane_contracts) {
    if (lane.language.empty() || lane.surface_id.empty() ||
        lane.symbol_owner != surface.module_name ||
        !IsPowerOfTwo(lane.abi_alignment) || !lane.fail_closed ||
        !lane.ownership_policy_explicit || !lane.error_policy_explicit ||
        !lane.async_policy_explicit ||
        !lane.object_identity_policy_explicit) {
      error = "foreign lane contract must be explicit and fail closed";
      return false;
    }
    if (lane.state == Objc3ModuleInteropLaneState::kSupported) {
      ++supported_lanes;
      if (lane.evidence_anchors_source_order.empty()) {
        error = "supported foreign lane must carry executable evidence: " +
                lane.language;
        return false;
      }
    } else {
      ++reserved_lanes;
      if (!IsStableDiagnosticCode(lane.diagnostic_code)) {
        error = "reserved foreign lane must publish a stable diagnostic: " +
                lane.language;
        return false;
      }
    }
  }
  if (surface.supported_foreign_lane_count != supported_lanes ||
      surface.reserved_foreign_lane_count != reserved_lanes ||
      (reserved_lanes != 0u && !surface.reserved_lanes_have_stable_diagnostics)) {
    error = "foreign lane support/reservation counts drifted";
    return false;
  }
  return true;
}

bool IsReadyObjc3ModuleInteropContractSurface(
    const Objc3ModuleInteropContractSurface &surface) {
  std::string error;
  return ValidateObjc3ModuleInteropContractSurface(surface, error);
}
