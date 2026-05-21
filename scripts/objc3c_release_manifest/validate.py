"""Release manifest policy and reproducibility validation."""

from __future__ import annotations

import os
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.paths import repo_rel

from .model import PackageAssembly, ReleaseValidation

ABI_API_DRIFT_SUMMARY_CONTRACT_ID = "objc3c.release.foundation.abi_api_drift.summary.v1"


def _entry_by_path(assembly: PackageAssembly) -> dict[str, Any]:
    return {entry.path: entry for entry in assembly.entries}


def _reproducibility_drift_detail(
    *,
    first: PackageAssembly,
    second: PackageAssembly,
) -> str:
    first_entries = _entry_by_path(first)
    second_entries = _entry_by_path(second)
    first_paths = set(first_entries)
    second_paths = set(second_entries)

    added = sorted(second_paths - first_paths)
    removed = sorted(first_paths - second_paths)
    changed = [
        path
        for path in sorted(first_paths & second_paths)
        if first_entries[path] != second_entries[path]
    ]
    if added:
        return f"added payload entries after first package run: {', '.join(added[:5])}"
    if removed:
        return f"removed payload entries after first package run: {', '.join(removed[:5])}"
    if changed:
        return f"changed payload entry digests after first package run: {', '.join(changed[:5])}"
    if first.package_manifest["copied_file_count"] != second.package_manifest["copied_file_count"]:
        return (
            "copied_file_count drifted "
            f"{first.package_manifest['copied_file_count']} -> "
            f"{second.package_manifest['copied_file_count']}"
        )
    return "release payload digest drifted without entry-level mismatch"


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
        detail = _reproducibility_drift_detail(first=first, second=second)
        raise RuntimeError(
            "repeated runnable package assembly drifted across release payload digests: "
            + detail
        )

    return ReleaseValidation(
        repo_superclean_path=repo_superclean_path,
        abi_api_drift_summary_path=abi_api_drift_summary_path,
        reproducibility_match=reproducibility_match,
    )
