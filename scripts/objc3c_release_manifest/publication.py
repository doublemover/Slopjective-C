"""SBOM and attestation publication for release-foundation artifacts."""

from __future__ import annotations

from collections import defaultdict
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel

from .hashing import sha256_file
from .paths import (
    ATTESTATION_PATH,
    MANIFEST_PATH,
    PROVENANCE_POLICY,
    PUBLICATION_SUMMARY_PATH,
    SBOM_PATH,
)

PUBLICATION_SUMMARY_CONTRACT_ID = "objc3c.release.foundation.publication.summary.v1"


def utc_stamp() -> str:
    return datetime.now(timezone.utc).isoformat().replace("+00:00", "Z")


def _require_manifest(manifest_path: Path) -> dict[str, Any]:
    if not manifest_path.is_file():
        raise RuntimeError(f"missing release manifest {repo_rel(manifest_path)}")
    manifest = load_json(manifest_path)
    if manifest.get("contract_id") != "objc3c.release.foundation.manifest.v1":
        raise RuntimeError("release manifest contract drifted")
    return manifest


def _payload_entries(manifest: dict[str, Any]) -> list[dict[str, Any]]:
    entries = manifest.get("release_payload_entries")
    if not isinstance(entries, list) or not entries:
        raise RuntimeError("release manifest did not contain release_payload_entries")
    for entry in entries:
        if not isinstance(entry, dict):
            raise RuntimeError("release manifest contained non-object payload entry")
        for field_name in ("path", "sha256", "byte_count", "component_group"):
            if field_name not in entry:
                raise RuntimeError(f"release payload entry missed {field_name}")
    return entries


def _component_groups(entries: list[dict[str, Any]]) -> list[dict[str, Any]]:
    grouped_entries: dict[str, list[dict[str, Any]]] = defaultdict(list)
    for entry in entries:
        grouped_entries[str(entry["component_group"])].append(
            {
                "path": entry["path"],
                "sha256": entry["sha256"],
                "byte_count": entry["byte_count"],
            }
        )

    component_groups = []
    for group_id in sorted(grouped_entries):
        group_entries = sorted(grouped_entries[group_id], key=lambda item: item["path"])
        component_groups.append(
            {
                "group_id": group_id,
                "file_count": len(group_entries),
                "entries": group_entries,
            }
        )
    return component_groups


def build_sbom(
    *,
    manifest: dict[str, Any],
    provenance_policy: dict[str, Any],
    generated_at_utc: str,
) -> dict[str, Any]:
    entries = _payload_entries(manifest)
    component_groups = _component_groups(entries)
    actual_groups = {group["group_id"] for group in component_groups}
    required_groups = set(provenance_policy["required_sbom_component_groups"])
    missing_groups = sorted(required_groups - actual_groups)
    if missing_groups:
        raise RuntimeError(
            "release SBOM missed required component groups: " + ", ".join(missing_groups)
        )
    return {
        "contract_id": provenance_policy["sbom_contract_id"],
        "schema_version": 1,
        "generated_at_utc": generated_at_utc,
        "release_manifest_path": repo_rel(MANIFEST_PATH),
        "release_payload_digest_sha256": manifest["release_payload_digest_sha256"],
        "component_groups": component_groups,
    }


def build_attestation(
    *,
    manifest: dict[str, Any],
    provenance_policy: dict[str, Any],
    release_manifest_sha256: str,
    sbom_sha256: str,
    generated_at_utc: str,
) -> dict[str, Any]:
    attested_digests = {
        "release_manifest_sha256": release_manifest_sha256,
        "release_payload_digest_sha256": manifest["release_payload_digest_sha256"],
        "package_manifest_sha256": manifest["primary_package_manifest_sha256"],
        "sbom_sha256": sbom_sha256,
        "repo_superclean_surface_sha256": manifest["repo_superclean_surface_sha256"],
        "release_evidence_index_sha256": manifest["release_evidence_index_sha256"],
        "abi_api_drift_summary_sha256": manifest["abi_api_drift_summary_sha256"],
    }
    missing_bindings = sorted(
        set(provenance_policy["required_attestation_bindings"]) - set(attested_digests)
    )
    if missing_bindings:
        raise RuntimeError(
            "release attestation missed required digest bindings: "
            + ", ".join(missing_bindings)
        )

    source_stamps = {
        "git_commit": manifest["source_stamps"]["git_commit"],
        "git_tree_dirty": manifest["source_stamps"]["git_tree_dirty"],
        "package_root": manifest["primary_package_root"],
        "package_manifest_path": manifest["primary_package_manifest_path"],
        "release_manifest_path": repo_rel(MANIFEST_PATH),
        "sbom_path": repo_rel(SBOM_PATH),
        "attestation_path": repo_rel(ATTESTATION_PATH),
    }
    missing_source_stamps = sorted(
        set(provenance_policy["required_source_stamp_fields"]) - set(source_stamps)
    )
    if missing_source_stamps:
        raise RuntimeError(
            "release attestation missed required source stamps: "
            + ", ".join(missing_source_stamps)
        )

    return {
        "contract_id": provenance_policy["attestation_contract_id"],
        "schema_version": 1,
        "generated_at_utc": generated_at_utc,
        "release_manifest_path": repo_rel(MANIFEST_PATH),
        "sbom_path": repo_rel(SBOM_PATH),
        "attested_digests": attested_digests,
        "source_stamps": source_stamps,
    }


def publish_release_provenance() -> dict[str, Any]:
    manifest = _require_manifest(MANIFEST_PATH)
    provenance_policy = load_json(PROVENANCE_POLICY)
    generated_at_utc = utc_stamp()

    sbom = build_sbom(
        manifest=manifest,
        provenance_policy=provenance_policy,
        generated_at_utc=generated_at_utc,
    )
    SBOM_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SBOM_PATH, sbom)

    release_manifest_sha256 = sha256_file(MANIFEST_PATH)
    sbom_sha256 = sha256_file(SBOM_PATH)
    attestation = build_attestation(
        manifest=manifest,
        provenance_policy=provenance_policy,
        release_manifest_sha256=release_manifest_sha256,
        sbom_sha256=sbom_sha256,
        generated_at_utc=generated_at_utc,
    )
    ATTESTATION_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(ATTESTATION_PATH, attestation)

    PUBLICATION_SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "contract_id": PUBLICATION_SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "release_manifest_path": repo_rel(MANIFEST_PATH),
        "release_manifest_sha256": release_manifest_sha256,
        "sbom_path": repo_rel(SBOM_PATH),
        "sbom_sha256": sbom_sha256,
        "attestation_path": repo_rel(ATTESTATION_PATH),
        "attestation_sha256": sha256_file(ATTESTATION_PATH),
        "component_group_count": len(sbom["component_groups"]),
        "attested_digest_keys": sorted(attestation["attested_digests"]),
        "source_stamp_keys": sorted(attestation["source_stamps"]),
    }
    write_json_file(PUBLICATION_SUMMARY_PATH, summary)
    return summary


def main() -> int:
    summary = publish_release_provenance()
    print(f"summary_path: {repo_rel(PUBLICATION_SUMMARY_PATH)}")
    print(f"published_manifest: {summary['release_manifest_path']}")
    print(f"published_sbom: {summary['sbom_path']}")
    print(f"published_attestation: {summary['attestation_path']}")
    print("objc3c-release-provenance: PASS")
    return 0
