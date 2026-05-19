"""Release manifest data model construction."""

from __future__ import annotations

from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import repo_rel

from .hashing import sha256_file


@dataclass(frozen=True)
class PayloadEntry:
    path: str
    sha256: str
    byte_count: int
    component_group: str

    def to_json(self) -> dict[str, Any]:
        return {
            "path": self.path,
            "sha256": self.sha256,
            "byte_count": self.byte_count,
            "component_group": self.component_group,
        }


@dataclass(frozen=True)
class PackageAssembly:
    package_root: Path
    manifest_path: Path
    package_manifest: dict[str, Any]
    entries: tuple[PayloadEntry, ...]
    payload_digest: str

    def entries_json(self) -> list[dict[str, Any]]:
        return [entry.to_json() for entry in self.entries]


@dataclass(frozen=True)
class ReleaseValidation:
    repo_superclean_path: Path
    reproducibility_match: bool


def component_group_for_path(rel_path: str) -> str:
    normalized = rel_path.replace("\\", "/")
    if normalized == "package.json":
        return "scripts-and-runbooks"
    top = normalized.split("/", 1)[0]
    return {
        "artifacts": "native-binaries",
        "scripts": "scripts-and-runbooks",
        "docs": "scripts-and-runbooks",
        "showcase": "showcase-and-tutorials",
        "site": "showcase-and-tutorials",
        "stdlib": "stdlib",
        "tests": "conformance-and-evidence",
        "spec": "conformance-and-evidence",
        "schemas": "conformance-and-evidence",
        "native": "runtime-library",
    }.get(top, top)


def build_release_manifest_payload(
    *,
    first: PackageAssembly,
    second: PackageAssembly,
    source_surface: Path,
    reproducibility_policy: dict[str, Any],
    evidence_index_path: Path,
    validation: ReleaseValidation,
    git_commit: str,
    git_tree_dirty: bool,
) -> dict[str, Any]:
    return {
        "contract_id": "objc3c.release.foundation.manifest.v1",
        "schema_version": 1,
        "package_model": first.package_manifest["package_model"],
        "reproducibility_scope": reproducibility_policy["reproducibility_scope"],
        "build_run_count": 2,
        "reproducibility_match": validation.reproducibility_match,
        "source_surface": repo_rel(source_surface),
        "package_runs": [
            {
                "run_id": "run-1",
                "package_root": repo_rel(first.package_root),
                "package_manifest_path": repo_rel(first.manifest_path),
                "package_manifest_sha256": sha256_file(first.manifest_path),
                "copied_file_count": first.package_manifest["copied_file_count"],
                "release_payload_digest_sha256": first.payload_digest,
            },
            {
                "run_id": "run-2",
                "package_root": repo_rel(second.package_root),
                "package_manifest_path": repo_rel(second.manifest_path),
                "package_manifest_sha256": sha256_file(second.manifest_path),
                "copied_file_count": second.package_manifest["copied_file_count"],
                "release_payload_digest_sha256": second.payload_digest,
            },
        ],
        "primary_package_root": repo_rel(first.package_root),
        "primary_package_manifest_path": repo_rel(first.manifest_path),
        "primary_package_manifest_sha256": sha256_file(first.manifest_path),
        "repo_superclean_surface_path": repo_rel(validation.repo_superclean_path),
        "repo_superclean_surface_sha256": sha256_file(validation.repo_superclean_path),
        "release_evidence_index_path": repo_rel(evidence_index_path),
        "release_evidence_index_sha256": sha256_file(evidence_index_path),
        "release_payload_entries": first.entries_json(),
        "release_payload_digest_sha256": first.payload_digest,
        "source_stamps": {
            "generated_at_utc": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
            "git_commit": git_commit,
            "git_tree_dirty": git_tree_dirty,
        },
    }


def build_release_manifest_summary(
    *,
    payload: dict[str, Any],
    source_surface: Path,
    manifest_path: Path,
    first: PackageAssembly,
    validation: ReleaseValidation,
) -> dict[str, Any]:
    return {
        "contract_id": "objc3c.release.foundation.manifest.summary.v1",
        "status": "PASS",
        "source_surface": repo_rel(source_surface),
        "release_manifest_path": repo_rel(manifest_path),
        "reproducibility_match": validation.reproducibility_match,
        "primary_package_root": payload["primary_package_root"],
        "primary_package_manifest_path": payload["primary_package_manifest_path"],
        "primary_package_manifest_sha256": payload["primary_package_manifest_sha256"],
        "release_payload_file_count": len(first.entries),
        "release_payload_digest_sha256": payload["release_payload_digest_sha256"],
        "release_evidence_index_path": payload["release_evidence_index_path"],
        "release_evidence_index_sha256": payload["release_evidence_index_sha256"],
        "repo_superclean_surface_path": payload["repo_superclean_surface_path"],
        "repo_superclean_surface_sha256": payload["repo_superclean_surface_sha256"],
    }
