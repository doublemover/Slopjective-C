#include "io/objc3_process_internal.h"

std::vector<std::filesystem::path>
BuildObjc3RetiredClaimSidecarPaths(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return {
      out_dir / (emit_prefix + ".objc3-release-runtime-claim-matrix.json"),
      out_dir / (emit_prefix + ".objc3-dashboard-ready-summary.json"),
      out_dir /
          (emit_prefix + ".objc3-toolchain-runtime-ga-operations-scaffold.json"),
  };
}

bool DiagnoseObjc3RetiredClaimSidecars(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    std::string &error) {
  error.clear();
  std::vector<std::string> retired_sidecars;
  for (const auto &path :
       BuildObjc3RetiredClaimSidecarPaths(out_dir, emit_prefix)) {
    if (std::filesystem::exists(path)) {
      retired_sidecars.push_back(path.filename().string());
    }
  }
  if (retired_sidecars.empty()) {
    return true;
  }

  std::ostringstream out;
  out << "retired claim sidecar(s) detected next to the active release artifacts: ";
  for (std::size_t index = 0; index < retired_sidecars.size(); ++index) {
    if (index != 0u) {
      out << ", ";
    }
    out << retired_sidecars[index];
  }
  out << " (remove them; the canonical release surface is the integrated "
         ".objc3-conformance-publication.json, "
         ".objc3-conformance-validation.json, "
         ".objc3-release-evidence-operation.json, "
         ".objc3-dashboard-status.json, "
         ".objc3-advanced-feature-gate.json, and "
         ".objc3-release-candidate-matrix.json artifacts in the active output "
         "directory, while reports and other transient evidence belong under "
         "tmp/)";
  error = out.str();
  return false;
}
