from __future__ import annotations

from pathlib import Path
from typing import Any, Sequence

from conformance_evidence_index.constants import (
    ARTIFACT_AUTHENTICITY_SCHEMA_ID,
    EVIDENCE_INDEX_ARTIFACT_FAMILY_ID,
    EVIDENCE_INDEX_REPORT_FAMILY_ID,
    EVIDENCE_INDEX_SURFACE_ID,
    GENERATOR_COMMAND,
    GENERATOR_PATH,
    INDEX_VERSION,
    SCHEMA_ID,
)
from conformance_evidence_index.manifest import infer_profile_release
from conformance_evidence_index.model import ArtifactRecord
from conformance_evidence_index.paths import (
    detect_media_type,
    file_sha256,
    load_json_object,
    normalize_repo_path,
)


def build_artifact_records(
    *,
    artifact_paths: Sequence[Path],
    release_retired_route: str | None,
    strict_generated_at: bool = False,
) -> list[ArtifactRecord]:
    records: list[ArtifactRecord] = []

    for path in artifact_paths:
        rel_path = normalize_repo_path(path)
        payload = load_json_object(path)
        (
            profile_id,
            release_id,
            artifact_id,
            manifest_kind,
            schema_ref,
            source_generated_at,
            issue_ref,
        ) = infer_profile_release(
            rel_path=rel_path,
            payload=payload,
            release_retired_route=release_retired_route,
            strict_generated_at=strict_generated_at,
        )

        records.append(
            ArtifactRecord(
                artifact_path=rel_path,
                file_sha256=file_sha256(path),
                size_bytes=path.stat().st_size,
                media_type=detect_media_type(path),
                profile_id=profile_id,
                release_id=release_id,
                artifact_id=artifact_id,
                manifest_kind=manifest_kind,
                schema_ref=schema_ref,
                source_generated_at=source_generated_at,
                issue_ref=issue_ref,
            )
        )

    return sorted(
        records,
        key=lambda record: (
            record.profile_id.casefold(),
            record.release_id.casefold(),
            record.artifact_path.casefold(),
        ),
    )


def build_profiles_index(records: Sequence[ArtifactRecord]) -> list[dict[str, Any]]:
    by_profile: dict[str, dict[str, list[str]]] = {}
    for record in records:
        release_map = by_profile.setdefault(record.profile_id, {})
        release_map.setdefault(record.release_id, []).append(record.artifact_path)

    profiles: list[dict[str, Any]] = []
    for profile_id in sorted(by_profile.keys(), key=str.casefold):
        release_map = by_profile[profile_id]
        releases: list[dict[str, Any]] = []
        artifact_total = 0
        for release_id in sorted(release_map.keys(), key=str.casefold):
            artifact_paths = sorted(release_map[release_id], key=str.casefold)
            artifact_total += len(artifact_paths)
            releases.append(
                {
                    "release_id": release_id,
                    "artifact_count": len(artifact_paths),
                    "artifact_paths": artifact_paths,
                }
            )
        profiles.append(
            {
                "profile_id": profile_id,
                "artifact_count": artifact_total,
                "releases": releases,
            }
        )
    return profiles


def build_releases_index(records: Sequence[ArtifactRecord]) -> list[dict[str, Any]]:
    by_release: dict[str, dict[str, list[str]]] = {}
    for record in records:
        profile_map = by_release.setdefault(record.release_id, {})
        profile_map.setdefault(record.profile_id, []).append(record.artifact_path)

    releases: list[dict[str, Any]] = []
    for release_id in sorted(by_release.keys(), key=str.casefold):
        profile_map = by_release[release_id]
        profiles: list[dict[str, Any]] = []
        artifact_total = 0
        for profile_id in sorted(profile_map.keys(), key=str.casefold):
            artifact_paths = sorted(profile_map[profile_id], key=str.casefold)
            artifact_total += len(artifact_paths)
            profiles.append(
                {
                    "profile_id": profile_id,
                    "artifact_count": len(artifact_paths),
                    "artifact_paths": artifact_paths,
                }
            )
        releases.append(
            {
                "release_id": release_id,
                "artifact_count": artifact_total,
                "profiles": profiles,
            }
        )
    return releases


def build_index_payload(
    *,
    records: Sequence[ArtifactRecord],
    input_root: Path,
    output_path: Path | None,
    release_label: str | None,
    generated_at: str | None,
) -> dict[str, Any]:
    profiles = build_profiles_index(records)
    releases = build_releases_index(records)
    payload = {
        "schema_id": SCHEMA_ID,
        "index_version": INDEX_VERSION,
        "release_label": release_label,
        "generated_at": generated_at,
        "input_root": normalize_repo_path(input_root),
        "artifact_count": len(records),
        "profile_count": len(profiles),
        "release_count": len(releases),
        "artifacts": [record.as_dict() for record in records],
        "profiles": profiles,
        "releases": releases,
    }
    if output_path is not None:
        output_rel = normalize_repo_path(output_path)
        payload["artifact_authenticity"] = {
            "authenticity_schema_id": ARTIFACT_AUTHENTICITY_SCHEMA_ID,
            "provenance_class": "genuine_generated_output",
            "provenance_mode": "generator_replayable",
            "content_role": "conformance_evidence_index",
            "surface_id": EVIDENCE_INDEX_SURFACE_ID,
            "artifact_family_id": EVIDENCE_INDEX_ARTIFACT_FAMILY_ID,
            "report_family_id": EVIDENCE_INDEX_REPORT_FAMILY_ID,
            "generator_or_compile_path": GENERATOR_PATH,
            "input_root": normalize_repo_path(input_root),
            "output_path": output_rel,
        }
        payload["replay"] = {
            "cwd": ".",
            "command": [
                *GENERATOR_COMMAND,
                "--input-root",
                normalize_repo_path(input_root),
                "--output",
                output_rel,
            ],
            "notes": [
                "re-run this command from the repository root to regenerate the evidence index",
                "artifacts under reports/conformance remain tracked inputs; the index itself is a genuine generated output under tmp/reports",
            ],
        }
        if release_label is not None:
            payload["replay"]["command"].extend(["--release-label", release_label])
        if generated_at is not None:
            payload["replay"]["command"].extend(["--generated-at", generated_at])
    return payload
