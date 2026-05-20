#!/usr/bin/env python3
"""Package-ecosystem owner contracts shared by summary and validation scripts."""

from __future__ import annotations

import hashlib
import json
from pathlib import Path
from typing import Any, Iterable

PACKAGE_ECOSYSTEM_OWNER_POLICY = {
    "channel_owner": "packaging-channels-source",
    "registry_owner": "package-ecosystem-registry-source",
    "package_authoring_owner": "package-ecosystem-authoring-source",
    "package_validation_owner": "package-ecosystem-validation-gate",
    "blocker_owner": "package-ecosystem-blockers",
    "source_authority": "checked-in-package-ecosystem-contracts",
    "evidence_log_allowed": False,
}

PACKAGE_ECOSYSTEM_OWNER_FIELDS = tuple(PACKAGE_ECOSYSTEM_OWNER_POLICY)
PACKAGE_LOADER_INTEROP_METADATA_REL = (
    "tests/tooling/fixtures/package_ecosystem/mixed_image_interop_loader_metadata.json"
)
PACKAGE_LOADER_INTEROP_CONTRACT_ID = (
    "objc3c.package_ecosystem.mixed_image_interop_loader_metadata.v1"
)
PACKAGE_LOADER_INTEROP_TAMPER_CODE = "O3PKG8054"


def package_ecosystem_owner_payload() -> dict[str, object]:
    return dict(PACKAGE_ECOSYSTEM_OWNER_POLICY)


def require_package_ecosystem_owner_policy(payload: dict[str, Any], *, surface_name: str) -> dict[str, Any]:
    owner_policy = payload.get("owner_policy")
    if not isinstance(owner_policy, dict):
        raise RuntimeError(f"{surface_name} missing owner_policy")
    missing_fields = [field for field in PACKAGE_ECOSYSTEM_OWNER_FIELDS if field not in owner_policy]
    if missing_fields:
        raise RuntimeError(f"{surface_name} owner_policy missing fields: {', '.join(missing_fields)}")
    if owner_policy.get("evidence_log_allowed") is not False:
        raise RuntimeError(f"{surface_name} owner_policy must forbid evidence-log publication")
    expected = package_ecosystem_owner_payload()
    for field, expected_value in expected.items():
        if owner_policy.get(field) != expected_value:
            raise RuntimeError(f"{surface_name} owner_policy drifted for {field}")
    return owner_policy


def require_package_ecosystem_blocker_metadata(
    payload: dict[str, Any],
    *,
    surface_name: str,
    required_blockers: Iterable[str] = (),
) -> dict[str, Any]:
    metadata = payload.get("blocker_metadata")
    if not isinstance(metadata, dict):
        raise RuntimeError(f"{surface_name} missing blocker_metadata")
    if metadata.get("blocker_owner") != PACKAGE_ECOSYSTEM_OWNER_POLICY["blocker_owner"]:
        raise RuntimeError(f"{surface_name} blocker owner drifted")
    blocking_conditions = metadata.get("blocking_conditions")
    if not isinstance(blocking_conditions, list) or not blocking_conditions:
        raise RuntimeError(f"{surface_name} blocker_metadata missing blocking_conditions")
    missing_blockers = [
        blocker
        for blocker in required_blockers
        if blocker not in blocking_conditions
    ]
    if missing_blockers:
        raise RuntimeError(f"{surface_name} blocker_metadata missing blockers: {', '.join(missing_blockers)}")
    return metadata


def require_paths_under_package_ecosystem_tmp(paths: Iterable[str], *, surface_name: str) -> None:
    for raw_path in paths:
        path = Path(raw_path)
        if not path.as_posix().startswith("tmp/artifacts/package-ecosystem/"):
            raise RuntimeError(f"{surface_name} generated path is outside package ecosystem artifact root: {raw_path}")


def package_loader_interop_metadata_path(root: Path) -> Path:
    return root / PACKAGE_LOADER_INTEROP_METADATA_REL


def _stable_payload_digest(payload: object) -> str:
    encoded = json.dumps(
        payload,
        sort_keys=True,
        separators=(",", ":"),
        ensure_ascii=True,
        allow_nan=False,
    ).encode("utf-8")
    return "sha256:" + hashlib.sha256(encoded).hexdigest()


def _require_string(payload: dict[str, Any], key: str, *, surface_name: str) -> str:
    value = payload.get(key)
    if not isinstance(value, str) or not value:
        raise RuntimeError(f"{surface_name} missing string field {key}")
    return value


def _require_list(payload: dict[str, Any], key: str, *, surface_name: str) -> list[Any]:
    value = payload.get(key)
    if not isinstance(value, list) or not value:
        raise RuntimeError(f"{surface_name} missing non-empty list field {key}")
    return value


def _require_string_list(payload: dict[str, Any], key: str, *, surface_name: str) -> list[str]:
    values = _require_list(payload, key, surface_name=surface_name)
    if not all(isinstance(value, str) and value for value in values):
        raise RuntimeError(f"{surface_name} field {key} must contain only non-empty strings")
    return sorted(str(value) for value in values)


def _require_source_path(root: Path, raw_path: str, *, surface_name: str) -> None:
    if not (root / raw_path).is_file():
        raise RuntimeError(f"{surface_name} references missing source path: {raw_path}")


def load_package_loader_interop_metadata(root: Path) -> dict[str, Any]:
    path = package_loader_interop_metadata_path(root)
    if not path.is_file():
        raise RuntimeError(f"missing package loader interop metadata fixture: {PACKAGE_LOADER_INTEROP_METADATA_REL}")
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"package loader interop metadata fixture must be a JSON object: {PACKAGE_LOADER_INTEROP_METADATA_REL}")
    return payload


def package_loader_metadata_by_package(
    payload: dict[str, Any],
    *,
    root: Path,
    package_ids: Iterable[str] | None = None,
) -> dict[str, dict[str, Any]]:
    surface_name = "package loader interop metadata"
    if payload.get("contract_id") != PACKAGE_LOADER_INTEROP_CONTRACT_ID:
        raise RuntimeError(f"{surface_name} contract_id drifted")
    package_id_set = set(package_ids) if package_ids is not None else None
    packages = _require_list(payload, "packages", surface_name=surface_name)
    metadata_by_package: dict[str, dict[str, Any]] = {}
    for raw_entry in packages:
        if not isinstance(raw_entry, dict):
            raise RuntimeError(f"{surface_name} packages must contain objects")
        package_id = _require_string(raw_entry, "package_id", surface_name=surface_name)
        if package_id in metadata_by_package:
            raise RuntimeError(f"{surface_name} has duplicate package_id {package_id}")
        if package_id_set is not None and package_id not in package_id_set:
            raise RuntimeError(f"{surface_name} references unknown package_id {package_id}")
        _require_string(raw_entry, "metadata_id", surface_name=surface_name)
        for source_path in _require_string_list(raw_entry, "source_fixtures", surface_name=surface_name):
            _require_source_path(root, source_path, surface_name=surface_name)
        _require_string_list(raw_entry, "header_imports", surface_name=surface_name)
        _require_string_list(raw_entry, "header_exports", surface_name=surface_name)
        _require_list(raw_entry, "abi_alignment", surface_name=surface_name)
        _require_list(raw_entry, "foreign_types", surface_name=surface_name)
        _require_list(raw_entry, "mixed_images", surface_name=surface_name)

        package_execution = raw_entry.get("package_execution")
        if not isinstance(package_execution, dict):
            raise RuntimeError(f"{surface_name} missing package_execution for {package_id}")
        _require_source_path(
            root,
            _require_string(package_execution, "entry_fixture", surface_name=surface_name),
            surface_name=surface_name,
        )
        _require_string(package_execution, "public_action", surface_name=surface_name)

        for raw_fixture in _require_list(raw_entry, "negative_fixtures", surface_name=surface_name):
            if not isinstance(raw_fixture, dict):
                raise RuntimeError(f"{surface_name} negative_fixtures must contain objects")
            _require_source_path(
                root,
                _require_string(raw_fixture, "fixture", surface_name=surface_name),
                surface_name=surface_name,
            )
            code = _require_string(raw_fixture, "diagnostic_code", surface_name=surface_name)
            if not code.startswith("O3PKG"):
                raise RuntimeError(f"{surface_name} negative diagnostic must use O3PKG code: {code}")
            _require_string(raw_fixture, "source_range", surface_name=surface_name)

        tamper_rejection = raw_entry.get("tamper_rejection")
        if not isinstance(tamper_rejection, dict):
            raise RuntimeError(f"{surface_name} missing tamper_rejection for {package_id}")
        if tamper_rejection.get("diagnostic_code") != PACKAGE_LOADER_INTEROP_TAMPER_CODE:
            raise RuntimeError(f"{surface_name} tamper diagnostic drifted for {package_id}")
        _require_string(tamper_rejection, "source_range", surface_name=surface_name)
        _require_string(tamper_rejection, "message", surface_name=surface_name)
        metadata_by_package[package_id] = raw_entry
    return dict(sorted(metadata_by_package.items()))


def package_loader_metadata_digest_inputs(entry: dict[str, Any]) -> list[str]:
    inputs = [PACKAGE_LOADER_INTEROP_METADATA_REL]
    inputs.extend(_require_string_list(entry, "source_fixtures", surface_name="package loader interop metadata"))
    package_execution = entry.get("package_execution", {})
    if isinstance(package_execution, dict):
        entry_fixture = package_execution.get("entry_fixture")
        if isinstance(entry_fixture, str) and entry_fixture:
            inputs.append(entry_fixture)
    for raw_fixture in entry.get("negative_fixtures", []):
        if isinstance(raw_fixture, dict):
            fixture = raw_fixture.get("fixture")
            if isinstance(fixture, str) and fixture:
                inputs.append(fixture)
    return sorted(set(inputs))


def normalize_package_loader_interop_metadata(entry: dict[str, Any]) -> dict[str, Any]:
    negative_diagnostics = []
    for raw_fixture in entry.get("negative_fixtures", []):
        if not isinstance(raw_fixture, dict):
            continue
        negative_diagnostics.append(
            {
                "fixture": str(raw_fixture.get("fixture")),
                "diagnostic_code": str(raw_fixture.get("diagnostic_code")),
                "source_range": str(raw_fixture.get("source_range")),
            }
        )
    tamper_rejection = entry.get("tamper_rejection", {})
    normalized = {
        "metadata_id": str(entry.get("metadata_id")),
        "source": PACKAGE_LOADER_INTEROP_METADATA_REL,
        "contract_id": PACKAGE_LOADER_INTEROP_CONTRACT_ID,
        "digest": _stable_payload_digest(entry),
        "header_imports": _require_string_list(entry, "header_imports", surface_name="package loader interop metadata"),
        "header_exports": _require_string_list(entry, "header_exports", surface_name="package loader interop metadata"),
        "abi_alignment": sorted(entry.get("abi_alignment", []), key=lambda item: json.dumps(item, sort_keys=True)),
        "foreign_types": sorted(entry.get("foreign_types", []), key=lambda item: json.dumps(item, sort_keys=True)),
        "mixed_images": sorted(entry.get("mixed_images", []), key=lambda item: json.dumps(item, sort_keys=True)),
        "package_execution": entry.get("package_execution"),
        "positive_fixtures": _require_string_list(entry, "source_fixtures", surface_name="package loader interop metadata"),
        "negative_diagnostics": sorted(negative_diagnostics, key=lambda item: (item["fixture"], item["diagnostic_code"])),
        "tamper_rejection": {
            "diagnostic_code": str(tamper_rejection.get("diagnostic_code")),
            "source_range": str(tamper_rejection.get("source_range")),
            "message": str(tamper_rejection.get("message")),
        },
    }
    return normalized


def package_loader_metadata_summary(payload: dict[str, Any], metadata_by_package: dict[str, dict[str, Any]]) -> dict[str, object]:
    return {
        "contract_id": payload.get("contract_id"),
        "source": PACKAGE_LOADER_INTEROP_METADATA_REL,
        "package_count": len(metadata_by_package),
        "package_ids": sorted(metadata_by_package),
        "tamper_rejection_diagnostic": PACKAGE_LOADER_INTEROP_TAMPER_CODE,
    }


__all__ = [
    "PACKAGE_ECOSYSTEM_OWNER_FIELDS",
    "PACKAGE_ECOSYSTEM_OWNER_POLICY",
    "PACKAGE_LOADER_INTEROP_CONTRACT_ID",
    "PACKAGE_LOADER_INTEROP_METADATA_REL",
    "PACKAGE_LOADER_INTEROP_TAMPER_CODE",
    "load_package_loader_interop_metadata",
    "normalize_package_loader_interop_metadata",
    "package_ecosystem_owner_payload",
    "package_loader_interop_metadata_path",
    "package_loader_metadata_by_package",
    "package_loader_metadata_digest_inputs",
    "package_loader_metadata_summary",
    "require_package_ecosystem_blocker_metadata",
    "require_package_ecosystem_owner_policy",
    "require_paths_under_package_ecosystem_tmp",
]
