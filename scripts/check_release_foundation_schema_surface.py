#!/usr/bin/env python3
"""Validate the checked-in release-foundation schema surface."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

from objc3c_shared.json_io import load_json_object as load_json
from objc3c_shared.json_io import write_report_json
from objc3c_shared.schema_registry import load_schema, schema_path
from objc3c_tooling.paths import repo_rel

ROOT = Path(__file__).resolve().parents[1]
SCHEMA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_foundation" / "schema_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "release-foundation" / "schema-surface-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.release.foundation.schema.surface.summary.v1"
JSON_SCHEMA_DRAFT = "https://json-schema.org/draft/2020-12/schema"

EXPECTED_SCHEMAS = {
    "release_manifest_schema": (
        "objc3c-release-manifest-v1",
        "https://objc3c.dev/schemas/objc3c-release-manifest-v1.schema.json",
        "objc3c.release.foundation.manifest.v1",
        (
            "source_surface",
            "primary_package_root",
            "primary_package_manifest_path",
            "primary_package_manifest_sha256",
            "repo_superclean_surface_path",
            "repo_superclean_surface_sha256",
            "release_evidence_index_path",
            "release_evidence_index_sha256",
            "release_payload_entries",
            "release_payload_digest_sha256",
            "source_stamps",
        ),
    ),
    "release_sbom_schema": (
        "objc3c-release-sbom-v1",
        "https://objc3c.dev/schemas/objc3c-release-sbom-v1.schema.json",
        "objc3c.release.foundation.sbom.v1",
        (
            "generated_at_utc",
            "release_manifest_path",
            "release_payload_digest_sha256",
            "component_groups",
        ),
    ),
    "release_attestation_schema": (
        "objc3c-release-attestation-v1",
        "https://objc3c.dev/schemas/objc3c-release-attestation-v1.schema.json",
        "objc3c.release.foundation.attestation.v1",
        (
            "generated_at_utc",
            "release_manifest_path",
            "sbom_path",
            "attested_digests",
            "source_stamps",
        ),
    ),
}

EXPECTED_ABI_MANIFEST_SCHEMAS = (
    (
        "objc3-abi-2025Q4",
        "https://objc3c.dev/schemas/objc3-abi-2025Q4.schema.json",
        ("properties", "manifest_schema"),
        "objc3-abi-2025Q4",
        (
            "manifest_type",
            "manifest_schema",
            "manifest_version",
            "issue_ref",
            "producer",
            "target_triples",
            "symbol_policy_id",
            "mangling_policy_id",
            "metadata_contract",
            "artifacts",
        ),
    ),
    (
        "objc3-runtime-2025Q4-manifest",
        "https://objc3c.dev/schemas/objc3-runtime-2025Q4.manifest.schema.json",
        ("$defs", "runtimeManifest", "properties", "artifact_id"),
        "objc3-runtime-2025Q4",
        (
            "schema_id",
            "artifact_id",
            "release_stamp",
            "manifest_version",
            "producer",
            "target",
            "normative_reference",
            "artifacts",
            "invariants",
            "capabilities",
        ),
    ),
)

EXPECTED_PACKAGE_ATTESTATION_SCHEMAS = (
    (
        "objc3c-package-channels-manifest-v1",
        "https://objc3c.dev/schemas/objc3c-package-channels-manifest-v1.schema.json",
        "objc3c.packaging.channels.summary.v1",
        (),
        (
            "platform_support_matrix",
            "implemented_channels",
            "portable_archive",
            "installer_archive",
            "offline_archive",
        ),
    ),
    (
        "objc3c-package-install-receipt-v1",
        "https://objc3c.dev/schemas/objc3c-package-install-receipt-v1.schema.json",
        "objc3c.packaging.channels.install-receipt.v1",
        (),
        (
            "install_root",
            "install_home",
            "bootstrap_entrypoint",
            "installed_at_utc",
        ),
    ),
    (
        "objc3c-update-manifest-v1",
        "https://objc3c.dev/schemas/objc3c-update-manifest-v1.schema.json",
        "objc3c.release.operations.update-manifest.v1",
        ("properties", "channels", "items", "properties", "artifacts"),
        (
            "package_channels_manifest",
            "release_manifest",
            "platform_support_matrix",
            "portable_archive",
            "installer_archive",
            "offline_archive",
        ),
    ),
)


def fail(message: str) -> int:
    print(f"release-foundation-schema-surface: {message}", file=sys.stderr)
    return 1


def property_const(schema_payload: dict[str, Any], property_name: str) -> Any:
    properties = schema_payload.get("properties")
    if not isinstance(properties, dict):
        return None
    property_payload = properties.get(property_name)
    if not isinstance(property_payload, dict):
        return None
    return property_payload.get("const")


def required_properties(schema_payload: dict[str, Any]) -> set[str]:
    required = schema_payload.get("required")
    if not isinstance(required, list):
        return set()
    return {field_name for field_name in required if isinstance(field_name, str)}


def nested_object(
    payload: dict[str, Any],
    path: tuple[str, ...],
) -> dict[str, Any] | None:
    current: Any = payload
    for segment in path:
        if not isinstance(current, dict):
            return None
        current = current.get(segment)
    return current if isinstance(current, dict) else None


def nested_const(payload: dict[str, Any], path: tuple[str, ...]) -> Any:
    target = nested_object(payload, path)
    if target is None:
        return None
    return target.get("const")


def required_properties_at_path(
    schema_payload: dict[str, Any],
    path: tuple[str, ...],
) -> set[str]:
    target = nested_object(schema_payload, path) if path else schema_payload
    if target is None:
        return set()
    return required_properties(target)


def validate_abi_manifest_schemas() -> (
    tuple[dict[str, str], dict[str, list[str]]] | int
):
    schema_refs: dict[str, str] = {}
    schema_required_fields: dict[str, list[str]] = {}
    for (
        registry_id,
        expected_schema_id,
        const_path,
        expected_const,
        expected_required_fields,
    ) in EXPECTED_ABI_MANIFEST_SCHEMAS:
        expected_path = repo_rel(schema_path(registry_id))
        payload = load_schema(registry_id)
        if payload.get("$schema") != JSON_SCHEMA_DRAFT:
            return fail(f"{expected_path} drifted from draft 2020-12")
        if payload.get("$id") != expected_schema_id:
            return fail(
                f"{expected_path} drifted from expected schema id {expected_schema_id}"
            )
        if nested_const(payload, const_path) != expected_const:
            return fail(f"{expected_path} versioned ABI manifest identity drifted")
        required_field_path = (
            const_path[:-2]
            if const_path[-2:] == ("properties", const_path[-1])
            else ()
        )
        missing_required_fields = sorted(
            set(expected_required_fields)
            - required_properties_at_path(payload, required_field_path)
        )
        if missing_required_fields:
            return fail(
                f"{expected_path} missed required ABI manifest fields: "
                + ", ".join(missing_required_fields)
            )
        schema_refs[registry_id] = expected_path
        schema_required_fields[registry_id] = list(expected_required_fields)
    return schema_refs, schema_required_fields


def validate_package_attestation_schemas() -> (
    tuple[dict[str, str], dict[str, list[str]]] | int
):
    schema_refs: dict[str, str] = {}
    schema_required_fields: dict[str, list[str]] = {}
    for (
        registry_id,
        expected_schema_id,
        expected_contract_id,
        required_path,
        expected_required_fields,
    ) in EXPECTED_PACKAGE_ATTESTATION_SCHEMAS:
        expected_path = repo_rel(schema_path(registry_id))
        payload = load_schema(registry_id)
        if payload.get("$schema") != JSON_SCHEMA_DRAFT:
            return fail(f"{expected_path} drifted from draft 2020-12")
        if payload.get("$id") != expected_schema_id:
            return fail(
                f"{expected_path} drifted from expected schema id {expected_schema_id}"
            )
        if property_const(payload, "contract_id") != expected_contract_id:
            return fail(f"{expected_path} package attestation contract identity drifted")
        missing_required_fields = sorted(
            set(expected_required_fields)
            - required_properties_at_path(payload, required_path)
        )
        if missing_required_fields:
            return fail(
                f"{expected_path} missed required package attestation fields: "
                + ", ".join(missing_required_fields)
            )
        schema_refs[registry_id] = expected_path
        schema_required_fields[registry_id] = list(expected_required_fields)
    return schema_refs, schema_required_fields


def main() -> int:
    if not SCHEMA_SURFACE.is_file():
        return fail(f"missing schema surface {repo_rel(SCHEMA_SURFACE)}")
    surface = load_json(SCHEMA_SURFACE)
    if surface.get("contract_id") != "objc3c.release.foundation.schema.surface.v1":
        return fail("unexpected schema surface contract_id")
    if surface.get("schema_version") != 1:
        return fail("unexpected schema surface version")
    if surface.get("schema_check_script") != "scripts/check_release_foundation_schema_surface.py":
        return fail("schema_check_script drifted")

    schema_paths: list[str] = []
    schema_ids: list[str] = []
    schema_refs: dict[str, str] = {}
    schema_required_fields: dict[str, list[str]] = {}
    for field_name, (
        registry_id,
        expected_schema_id,
        expected_contract_id,
        expected_required_fields,
    ) in EXPECTED_SCHEMAS.items():
        raw_path = surface.get(field_name)
        expected_path = repo_rel(schema_path(registry_id))
        if raw_path != expected_path:
            return fail(f"{field_name} drifted from registered schema path {expected_path}")
        payload = load_schema(registry_id)
        if payload.get("$schema") != JSON_SCHEMA_DRAFT:
            return fail(f"{expected_path} drifted from draft 2020-12")
        if payload.get("$id") != expected_schema_id:
            return fail(f"{field_name} drifted from expected schema id {expected_schema_id}")
        if property_const(payload, "contract_id") != expected_contract_id:
            return fail(f"{field_name} contract identity drifted")
        missing_required_fields = sorted(
            set(expected_required_fields) - required_properties(payload)
        )
        if missing_required_fields:
            return fail(
                f"{field_name} missed required release fields: "
                + ", ".join(missing_required_fields)
            )

        schema_paths.append(expected_path)
        schema_ids.append(expected_schema_id)
        schema_refs[field_name] = expected_path
        schema_required_fields[field_name] = list(expected_required_fields)

    abi_manifest_validation = validate_abi_manifest_schemas()
    if isinstance(abi_manifest_validation, int):
        return abi_manifest_validation
    abi_manifest_schema_refs, abi_manifest_required_fields = abi_manifest_validation

    package_attestation_validation = validate_package_attestation_schemas()
    if isinstance(package_attestation_validation, int):
        return package_attestation_validation
    package_attestation_schema_refs, package_attestation_required_fields = (
        package_attestation_validation
    )

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "schema_surface": repo_rel(SCHEMA_SURFACE),
        "release_manifest_schema": schema_refs["release_manifest_schema"],
        "release_sbom_schema": schema_refs["release_sbom_schema"],
        "release_attestation_schema": schema_refs["release_attestation_schema"],
        "schemas": schema_paths,
        "schema_ids": schema_ids,
        "schema_required_fields": schema_required_fields,
        "abi_manifest_schemas": abi_manifest_schema_refs,
        "abi_manifest_required_fields": abi_manifest_required_fields,
        "package_attestation_schemas": package_attestation_schema_refs,
        "package_attestation_required_fields": package_attestation_required_fields,
    }
    write_report_json(SUMMARY_PATH, summary, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("release-foundation-schema-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
