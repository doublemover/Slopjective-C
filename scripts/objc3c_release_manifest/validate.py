"""Release manifest policy and reproducibility validation."""

from __future__ import annotations

import os
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.paths import repo_rel

from .model import PackageAssembly, ReleaseValidation

ABI_API_DRIFT_SUMMARY_CONTRACT_ID = "objc3c.release.foundation.abi_api_drift.summary.v1"


def validate_release_inputs(
    *,
    first: PackageAssembly,
    second: PackageAssembly,
    payload_policy: dict[str, Any],
    abi_api_drift_summary_path: Path,
) -> ReleaseValidation:
    required_prefixes = payload_policy["required_payload_prefixes"]
    for prefix in required_prefixes:
        if not any(entry.path.startswith(prefix) for entry in first.entries):
            raise RuntimeError(f"release payload did not include required prefix {prefix}")

    repo_superclean_rel = first.package_manifest["repo_superclean_surface"]
    repo_superclean_path = first.package_root / repo_superclean_rel.replace("/", os.sep)
    if not repo_superclean_path.is_file():
        raise RuntimeError(f"missing packaged repo superclean surface {repo_superclean_rel}")
    if not abi_api_drift_summary_path.is_file():
        raise RuntimeError(
            f"missing ABI/API drift gate summary {repo_rel(abi_api_drift_summary_path)}"
        )
    abi_api_drift_summary = load_json(abi_api_drift_summary_path)
    if abi_api_drift_summary.get("contract_id") != ABI_API_DRIFT_SUMMARY_CONTRACT_ID:
        raise RuntimeError("ABI/API drift gate summary contract drifted")
    if abi_api_drift_summary.get("status") != "PASS":
        raise RuntimeError("ABI/API drift gate did not pass")
    if abi_api_drift_summary.get("failure_count") != 0:
        raise RuntimeError("ABI/API drift gate reported release blockers")

    reproducibility_match = (
        first.payload_digest == second.payload_digest
        and first.entries == second.entries
        and first.package_manifest["copied_file_count"] == second.package_manifest["copied_file_count"]
    )
    if not reproducibility_match:
        raise RuntimeError("repeated runnable package assembly drifted across release payload digests")

    return ReleaseValidation(
        repo_superclean_path=repo_superclean_path,
        abi_api_drift_summary_path=abi_api_drift_summary_path,
        reproducibility_match=reproducibility_match,
    )
