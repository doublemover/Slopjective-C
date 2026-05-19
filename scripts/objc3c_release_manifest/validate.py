"""Release manifest policy and reproducibility validation."""

from __future__ import annotations

import os
from typing import Any

from .model import PackageAssembly, ReleaseValidation


def validate_release_inputs(
    *,
    first: PackageAssembly,
    second: PackageAssembly,
    payload_policy: dict[str, Any],
) -> ReleaseValidation:
    required_prefixes = payload_policy["required_payload_prefixes"]
    for prefix in required_prefixes:
        if not any(entry.path.startswith(prefix) for entry in first.entries):
            raise RuntimeError(f"release payload did not include required prefix {prefix}")

    repo_superclean_rel = first.package_manifest["repo_superclean_surface"]
    repo_superclean_path = first.package_root / repo_superclean_rel.replace("/", os.sep)
    if not repo_superclean_path.is_file():
        raise RuntimeError(f"missing packaged repo superclean surface {repo_superclean_rel}")

    reproducibility_match = (
        first.payload_digest == second.payload_digest
        and first.entries == second.entries
        and first.package_manifest["copied_file_count"] == second.package_manifest["copied_file_count"]
    )
    if not reproducibility_match:
        raise RuntimeError("repeated runnable package assembly drifted across release payload digests")

    return ReleaseValidation(
        repo_superclean_path=repo_superclean_path,
        reproducibility_match=reproducibility_match,
    )
