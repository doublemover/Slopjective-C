"""Subprocess boundaries for package channel publication."""

from __future__ import annotations

import hashlib
from pathlib import Path

from objc3c_tooling.json_io import load_json_object
from objc3c_tooling.paths import repo_rel, resolve_repo_path_inside
from objc3c_tooling.subprocesses import python_script_command, run_completed
from scripts.objc3c_workflow.command_powershell_policy import powershell_file_command

from .paths import (
    PACKAGE_PS1,
    PLATFORM_SUPPORT_MATRIX_BUILD,
    PWSH,
    RELEASE_MANIFEST_PY,
    RELEASE_PROVENANCE_PY,
    RELEASE_FOUNDATION_ATTESTATION,
    RELEASE_FOUNDATION_MANIFEST,
    RELEASE_FOUNDATION_SBOM,
    ROOT,
)

RELEASE_FOUNDATION_MANIFEST_SUMMARY = (
    ROOT / "tmp" / "reports" / "release-foundation" / "release-manifest-summary.json"
)
RELEASE_FOUNDATION_ABI_API_DRIFT_SUMMARY = (
    ROOT / "tmp" / "reports" / "release-foundation" / "abi-api-drift-summary.json"
)
RELEASE_FOUNDATION_PUBLICATION_SUMMARY = (
    ROOT / "tmp" / "reports" / "release-foundation" / "publication-summary.json"
)
RELEASE_FOUNDATION_SUMMARY_CONTRACTS = {
    "manifest summary": "objc3c.release.foundation.manifest.summary.v1",
    "ABI/API drift summary": "objc3c.release.foundation.abi_api_drift.summary.v1",
    "publication summary": "objc3c.release.foundation.publication.summary.v1",
}
RELEASE_FOUNDATION_ARTIFACT_CONTRACTS = {
    "manifest": "objc3c.release.foundation.manifest.v1",
    "SBOM": "objc3c.release.foundation.sbom.v1",
    "attestation": "objc3c.release.foundation.attestation.v1",
}


def run(command: list[str]) -> None:
    result = run_completed(command, cwd=ROOT, capture_output=False)
    if result.returncode != 0:
        raise RuntimeError(f"command failed with exit code {result.returncode}: {' '.join(command)}")


def build_support_matrix() -> None:
    run(python_script_command(PLATFORM_SUPPORT_MATRIX_BUILD))


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def require_pass_summary(path: Path, *, label: str) -> dict[str, object]:
    payload = load_json_object(path)
    expected_contract = RELEASE_FOUNDATION_SUMMARY_CONTRACTS[label]
    if payload.get("contract_id") != expected_contract:
        raise RuntimeError(
            f"release foundation reuse requested but {label} contract drifted from {expected_contract}"
        )
    if payload.get("status") != "PASS":
        raise RuntimeError(f"release foundation reuse requested but {label} did not pass")
    return payload


def require_artifact_contract(path: Path, *, label: str) -> dict[str, object]:
    payload = load_json_object(path)
    expected_contract = RELEASE_FOUNDATION_ARTIFACT_CONTRACTS[label]
    if payload.get("contract_id") != expected_contract:
        raise RuntimeError(
            f"release foundation reuse requested but {label} contract drifted from {expected_contract}"
        )
    return payload


def require_reported_file_digest(
    summary: dict[str, object],
    *,
    path_field: str,
    digest_field: str,
    label: str,
    expected_path: Path | None = None,
) -> Path:
    reported_path = summary.get(path_field)
    if not isinstance(reported_path, str) or not reported_path:
        raise RuntimeError(
            f"release foundation reuse requested but {label} missed {path_field}"
        )
    resolved_path = resolve_repo_path_inside(reported_path)
    if expected_path is not None and repo_rel(resolved_path) != repo_rel(expected_path):
        raise RuntimeError(
            f"release foundation reuse requested but {label} {path_field} drifted "
            f"from {repo_rel(expected_path)}"
        )
    if not resolved_path.is_file():
        raise RuntimeError(
            f"release foundation reuse requested but {label} artifact is missing: "
            f"{reported_path}"
        )
    if summary.get(digest_field) != sha256_file(resolved_path):
        raise RuntimeError(
            f"release foundation reuse requested but {label} {digest_field} drifted "
            f"from {reported_path}"
        )
    return resolved_path


def require_release_foundation_manifest_summary(summary: dict[str, object]) -> None:
    if summary.get("reproducibility_match") is not True:
        raise RuntimeError(
            "release foundation reuse requested but manifest reproducibility did not pass"
        )
    if summary.get("release_manifest_path") != repo_rel(RELEASE_FOUNDATION_MANIFEST):
        raise RuntimeError(
            "release foundation reuse requested but manifest summary release_manifest_path "
            f"drifted from {repo_rel(RELEASE_FOUNDATION_MANIFEST)}"
        )
    for path_field, digest_field, label in (
        (
            "primary_package_manifest_path",
            "primary_package_manifest_sha256",
            "primary package manifest",
        ),
        (
            "release_evidence_index_path",
            "release_evidence_index_sha256",
            "release evidence index",
        ),
        (
            "abi_api_drift_summary_path",
            "abi_api_drift_summary_sha256",
            "ABI/API drift summary",
        ),
        (
            "repo_superclean_surface_path",
            "repo_superclean_surface_sha256",
            "repo superclean surface",
        ),
    ):
        require_reported_file_digest(
            summary,
            path_field=path_field,
            digest_field=digest_field,
            label=label,
        )


def require_release_foundation_publication_summary(summary: dict[str, object]) -> None:
    for path_field, digest_field, expected_path, label in (
        (
            "release_manifest_path",
            "release_manifest_sha256",
            RELEASE_FOUNDATION_MANIFEST,
            "release manifest",
        ),
        ("sbom_path", "sbom_sha256", RELEASE_FOUNDATION_SBOM, "SBOM"),
        (
            "attestation_path",
            "attestation_sha256",
            RELEASE_FOUNDATION_ATTESTATION,
            "attestation",
        ),
    ):
        require_reported_file_digest(
            summary,
            path_field=path_field,
            digest_field=digest_field,
            label=label,
            expected_path=expected_path,
        )


def require_release_foundation_artifact_parity() -> None:
    manifest = require_artifact_contract(RELEASE_FOUNDATION_MANIFEST, label="manifest")
    sbom = require_artifact_contract(RELEASE_FOUNDATION_SBOM, label="SBOM")
    attestation = require_artifact_contract(
        RELEASE_FOUNDATION_ATTESTATION,
        label="attestation",
    )

    if manifest.get("reproducibility_match") is not True:
        raise RuntimeError(
            "release foundation reuse requested but release manifest reproducibility did not pass"
        )
    release_payload_digest = manifest.get("release_payload_digest_sha256")
    if sbom.get("release_payload_digest_sha256") != release_payload_digest:
        raise RuntimeError(
            "release foundation reuse requested but SBOM release payload digest drifted"
        )

    attested_digests = attestation.get("attested_digests")
    if not isinstance(attested_digests, dict):
        raise RuntimeError(
            "release foundation reuse requested but attestation missed attested_digests"
        )
    expected_attested_digests = {
        "release_manifest_sha256": sha256_file(RELEASE_FOUNDATION_MANIFEST),
        "release_payload_digest_sha256": release_payload_digest,
        "package_manifest_sha256": manifest.get("primary_package_manifest_sha256"),
        "sbom_sha256": sha256_file(RELEASE_FOUNDATION_SBOM),
    }
    for field_name, expected_value in expected_attested_digests.items():
        if attested_digests.get(field_name) != expected_value:
            raise RuntimeError(
                f"release foundation reuse requested but attestation {field_name} drifted"
            )


def require_existing_release_foundation_artifacts() -> None:
    missing_paths = [
        path
        for path in (
            RELEASE_FOUNDATION_MANIFEST_SUMMARY,
            RELEASE_FOUNDATION_ABI_API_DRIFT_SUMMARY,
            RELEASE_FOUNDATION_PUBLICATION_SUMMARY,
            RELEASE_FOUNDATION_MANIFEST,
            RELEASE_FOUNDATION_SBOM,
            RELEASE_FOUNDATION_ATTESTATION,
        )
        if not path.is_file()
    ]
    if missing_paths:
        formatted = ", ".join(str(path) for path in missing_paths)
        raise RuntimeError(f"release foundation reuse requested but artifacts are missing: {formatted}")

    manifest_summary = require_pass_summary(
        RELEASE_FOUNDATION_MANIFEST_SUMMARY,
        label="manifest summary",
    )
    require_pass_summary(
        RELEASE_FOUNDATION_ABI_API_DRIFT_SUMMARY,
        label="ABI/API drift summary",
    )
    publication_summary = require_pass_summary(
        RELEASE_FOUNDATION_PUBLICATION_SUMMARY,
        label="publication summary",
    )
    require_release_foundation_manifest_summary(manifest_summary)
    require_release_foundation_publication_summary(publication_summary)
    require_release_foundation_artifact_parity()
    print("package-channels: reused validated release-foundation artifacts")


def build_release_foundation_artifacts(*, reuse_existing: bool = False) -> None:
    if reuse_existing:
        require_existing_release_foundation_artifacts()
        return
    run(python_script_command(RELEASE_MANIFEST_PY))
    run(python_script_command(RELEASE_PROVENANCE_PY))


def build_runnable_package(
    package_root: Path,
    manifest_relative_path: str,
    sanitizer_variant: str = "release",
) -> None:
    run(
        powershell_file_command(
            PWSH,
            PACKAGE_PS1,
            "-PackageRoot",
            str(package_root),
            "-ManifestRelativePath",
            manifest_relative_path,
            "-SanitizerVariant",
            sanitizer_variant,
        )
    )
