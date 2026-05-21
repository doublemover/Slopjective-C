#!/usr/bin/env python3
"""Validate the Objective-C 3 interop surface policy for issue #8165."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

from check_objc3c_module_interop_contracts import build_summary as build_module_interop_summary
from check_runtime_import_interop_bridge_metadata import (
    CANONICAL_BRIDGE,
    CANONICAL_HEADER,
    CANONICAL_MODULE,
    validate_import_surface,
)
from objc3c_shared.json_io import (
    JsonSchemaValidationError,
    load_json_object,
    validate_json_schema,
)
from objc3c_tooling.paths import repo_rel
from package_ecosystem_contracts import (
    PACKAGE_LOADER_INTEROP_TAMPER_CODE,
    load_package_loader_interop_metadata,
    package_loader_metadata_by_package,
    package_loader_metadata_channel_summary,
)


ROOT = Path(__file__).resolve().parents[1]
POLICY_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "interop_surface_policy"
    / "objc3c_interop_surface_policy.json"
)
SCHEMA_PATH = ROOT / "schemas" / "objc3c-interop-surface-policy-v1.schema.json"

REQUIRED_LANE_IDS = {
    "c.header-import-export",
    "objc2.metadata-only-migration",
    "swift.annotation-metadata",
    "cpp.annotation-metadata",
    "package.mixed-image-loader-metadata",
    "swift.full-abi-callable-import",
    "cpp.template-and-abi-import",
    "objc2.retired-source-compatibility",
    "package.unchecked-abi-alignment-fallback",
}
SUPPORTED_LANE_IDS = {
    "c.header-import-export",
    "objc2.metadata-only-migration",
    "swift.annotation-metadata",
    "cpp.annotation-metadata",
    "package.mixed-image-loader-metadata",
}
RESERVED_LANE_IDS = {
    "swift.full-abi-callable-import",
    "cpp.template-and-abi-import",
}
REJECTED_LANE_IDS = {
    "objc2.retired-source-compatibility",
    "package.unchecked-abi-alignment-fallback",
}
REQUIRED_LANGUAGES = {"c", "objc2", "swift", "cpp"}
SUPPORTED_METADATA_KINDS = {
    "header-import-export",
    "metadata-only-migration",
    "annotation-metadata",
    "package-loader-metadata",
}
SUPPORTED_SUPPORT_CLAIMS = {
    "objc3c.behavior.runtime.interop.package-loader-bridge",
    "objc3c.behavior.runtime.interop.mixed-image-replay",
}
REQUIRED_PUBLIC_COMMANDS = {
    "npm run objc3c -- validate-module-interop-contracts",
    "npm run objc3c -- validate-interop-conformance",
    "npm run objc3c -- validate-migration-workflow",
    "npm run objc3c -- validate-package-ecosystem",
}


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def _as_list(value: object) -> list[Any]:
    return value if isinstance(value, list) else []


def _as_object(value: object) -> dict[str, Any]:
    return value if isinstance(value, dict) else {}


def _is_portable_relative_path(value: object) -> bool:
    if not isinstance(value, str) or not value:
        return False
    path = Path(value)
    if path.is_absolute() or "\\" in value:
        return False
    return all(part not in {"", ".", ".."} for part in path.parts)


def _require_existing_path(raw_path: object, failures: list[str], *, label: str) -> None:
    expect(_is_portable_relative_path(raw_path), f"{label} path is not portable-relative: {raw_path}", failures)
    if isinstance(raw_path, str) and _is_portable_relative_path(raw_path):
        expect((ROOT / raw_path).is_file(), f"{label} path is missing: {raw_path}", failures)


def _validate_schema(payload: dict[str, Any], failures: list[str]) -> None:
    try:
        validate_json_schema(payload, load_json_object(SCHEMA_PATH), label=repo_rel(POLICY_PATH))
    except JsonSchemaValidationError as exc:
        failures.append(str(exc))


def _validate_lanes(payload: dict[str, Any], failures: list[str]) -> None:
    lanes = [_as_object(lane) for lane in _as_list(payload.get("lanes")) if isinstance(lane, dict)]
    lanes_by_id = {str(lane.get("lane_id")): lane for lane in lanes}
    expect(set(lanes_by_id) == REQUIRED_LANE_IDS, "interop lane set drifted from the issue #8165 policy", failures)
    expect(
        {str(lane.get("language")) for lane in lanes if str(lane.get("language")) != "package"} >= REQUIRED_LANGUAGES,
        "interop policy must cover C, ObjC2, Swift, and C++ lanes",
        failures,
    )

    for lane_id, lane in lanes_by_id.items():
        state = str(lane.get("public_state"))
        language = str(lane.get("language"))
        kind = str(lane.get("interop_kind"))
        support_claim = str(lane.get("support_claim", ""))

        expect(lane.get("complete_language_compatibility_claimed") is False, f"{lane_id} must not claim complete language compatibility", failures)
        expect(lane.get("source_syntax_compatibility_claimed") is False, f"{lane_id} must not claim source syntax compatibility", failures)
        expect(lane.get("fail_closed") is True, f"{lane_id} must fail closed", failures)

        for policy_name, policy_value in _as_object(lane.get("safety_policies")).items():
            expect(
                isinstance(policy_value, str) and policy_value and policy_value != "implicit",
                f"{lane_id} {policy_name} policy must be explicit",
                failures,
            )

        for raw_path in _as_list(lane.get("policy_anchors")):
            _require_existing_path(raw_path, failures, label=f"{lane_id} policy anchor")

        if lane_id in SUPPORTED_LANE_IDS:
            expect(state == "supported", f"{lane_id} must remain supported", failures)
            expect(kind in SUPPORTED_METADATA_KINDS, f"{lane_id} support is wider than the metadata/header/package slice", failures)
            expect(support_claim in SUPPORTED_SUPPORT_CLAIMS, f"{lane_id} supported lane must use a supported runtime interop claim", failures)
            expect(_as_list(lane.get("public_commands")) != [], f"{lane_id} supported lane needs replayable public command evidence", failures)
            expect(_as_list(lane.get("evidence_anchors")) != [], f"{lane_id} supported lane needs checked evidence anchors", failures)
            for raw_path in _as_list(lane.get("evidence_anchors")):
                _require_existing_path(raw_path, failures, label=f"{lane_id} evidence anchor")
        else:
            expect(state in {"reserved", "rejected"}, f"{lane_id} unsupported lane must be reserved or rejected", failures)
            expect(support_claim == "", f"{lane_id} reserved/rejected lane must not publish a support claim", failures)
            expect(_as_list(lane.get("public_commands")) == [], f"{lane_id} reserved/rejected lane must not publish command evidence", failures)
            expect(_as_list(lane.get("diagnostics")) != [], f"{lane_id} reserved/rejected lane needs a stable diagnostic", failures)

        if lane_id in RESERVED_LANE_IDS:
            expect(state == "reserved", f"{lane_id} must remain reserved", failures)
        if lane_id in REJECTED_LANE_IDS:
            expect(state == "rejected", f"{lane_id} must remain rejected", failures)
        if language == "objc2" and kind == "source-compatibility":
            expect(state == "rejected", "Objective-C 2 source compatibility must stay rejected", failures)
        if language == "swift" and kind == "full-abi-callable-import":
            expect(state == "reserved", "Swift full ABI callable import must stay reserved", failures)
        if language == "cpp" and kind == "full-abi-callable-import":
            expect(state == "reserved", "C++ ABI/template import must stay reserved", failures)


def _validate_bridge_metadata_policy(payload: dict[str, Any], failures: list[str]) -> None:
    policy = _as_object(payload.get("bridge_metadata_policy"))
    _require_existing_path(policy.get("checker"), failures, label="bridge metadata checker")
    valid_fixture = policy.get("runtime_import_surface_fixture")
    tampered_fixture = policy.get("tampered_runtime_import_surface_fixture")
    _require_existing_path(valid_fixture, failures, label="valid bridge fixture")
    _require_existing_path(tampered_fixture, failures, label="tampered bridge fixture")

    canonical_paths = _as_object(policy.get("canonical_artifact_paths"))
    expect(canonical_paths.get("header") == CANONICAL_HEADER, "canonical bridge header path drifted", failures)
    expect(canonical_paths.get("modulemap") == CANONICAL_MODULE, "canonical bridge modulemap path drifted", failures)
    expect(canonical_paths.get("bridge") == CANONICAL_BRIDGE, "canonical bridge metadata path drifted", failures)

    if isinstance(valid_fixture, str) and (ROOT / valid_fixture).is_file():
        valid_payload = load_json_object(ROOT / valid_fixture)
        expect(validate_import_surface(valid_payload) == [], "valid interop bridge fixture no longer validates", failures)
    if isinstance(tampered_fixture, str) and (ROOT / tampered_fixture).is_file():
        tampered_payload = load_json_object(ROOT / tampered_fixture)
        tampered_failures = validate_import_surface(tampered_payload)
        for expected_message in _as_list(policy.get("fail_closed_messages")):
            expect(
                any(str(expected_message) in failure for failure in tampered_failures),
                f"tampered bridge fixture no longer proves fail-closed message: {expected_message}",
                failures,
            )


def _validate_package_loader_policy(payload: dict[str, Any], failures: list[str]) -> None:
    policy = _as_object(payload.get("package_loader_policy"))
    _require_existing_path(policy.get("metadata_fixture"), failures, label="package loader metadata")
    expect(policy.get("tamper_diagnostic") == PACKAGE_LOADER_INTEROP_TAMPER_CODE, "package loader tamper diagnostic drifted", failures)

    package_ids = {str(package_id) for package_id in _as_list(policy.get("required_package_ids"))}
    metadata = load_package_loader_interop_metadata(ROOT)
    metadata_by_package = package_loader_metadata_by_package(metadata, root=ROOT, package_ids=package_ids)
    summary = package_loader_metadata_channel_summary(metadata, metadata_by_package)

    expect(set(summary["package_ids"]) == package_ids, "package loader metadata package ids drifted", failures)
    for bridge_kind in _as_list(policy.get("required_bridge_kinds")):
        summary_key = f"{bridge_kind}_bridge_surface_count"
        expect(int(summary.get(summary_key, 0)) > 0, f"package loader metadata missing {bridge_kind} bridge surfaces", failures)

    unsupported = set(str(surface) for surface in _as_list(policy.get("unsupported_surfaces")))
    expect(unsupported <= set(summary["unsupported_surfaces"]), "package loader unsupported interop surfaces drifted", failures)


def _validate_module_interop_contract(failures: list[str]) -> None:
    summary = build_module_interop_summary()
    expect(summary["status"] == "PASS", "module interop contract summary no longer passes", failures)
    expect(summary["supported_bridge_surface_count"] == 2, "module contract supported bridge surface count drifted", failures)
    expect(summary["reserved_bridge_surface_count"] == 2, "module contract reserved bridge surface count drifted", failures)


def validate_policy_payload(payload: dict[str, Any]) -> list[str]:
    failures: list[str] = []
    _validate_schema(payload, failures)
    expect(payload.get("issue_refs") == [8165], "interop policy must own issue #8165", failures)
    expect(payload.get("evidence_log_allowed") is False, "interop policy cannot use evidence logs as source authority", failures)
    expect(REQUIRED_PUBLIC_COMMANDS <= set(str(command) for command in _as_list(payload.get("public_commands"))), "interop policy public commands are incomplete", failures)
    _require_existing_path(_as_object(payload.get("docs")).get("runbook"), failures, label="interop runbook")
    _validate_lanes(payload, failures)
    _validate_bridge_metadata_policy(payload, failures)
    _validate_package_loader_policy(payload, failures)
    _validate_module_interop_contract(failures)
    return failures


def main(argv: list[str] | None = None) -> int:
    path = Path(argv[0]) if argv else POLICY_PATH
    payload = load_json_object(path)
    failures = validate_policy_payload(payload)
    if failures:
        print("objc3c-interop-surface-policy: FAIL")
        for failure in failures:
            print(f"- {failure}")
        return 1
    print("objc3c-interop-surface-policy: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
