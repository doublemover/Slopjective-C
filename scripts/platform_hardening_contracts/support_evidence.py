"""Issue-owned platform/toolchain support evidence validation."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any, Iterable

from objc3c_shared.json_io import validate_json_schema
from objc3c_tooling.paths import repo_rel, resolve_repo_path

from .constants import PLATFORM_HARDENING_OWNER_POLICY
from .contract_predicates import expect
from .source_surface_catalog import (
    PLATFORM_TOOLCHAIN_SUPPORT_EVIDENCE_PATH,
    PLATFORM_TOOLCHAIN_SUPPORT_EVIDENCE_SCHEMA_PATH,
)

REQUIRED_SUPPORTED_EVIDENCE_CLASSES: tuple[str, ...] = (
    "build",
    "package",
    "install",
    "execution",
)
CLEAN_INSTALL_SUMMARY_PATH = (
    "tmp/reports/package-ecosystem/install-distribution-credibility-summary.json"
)
CLEAN_INSTALL_CHECKER_PATH = (
    "scripts/check_objc3c_package_install_distribution_credibility.py"
)


def load_json_object(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"{repo_rel(path)} did not contain a JSON object")
    return payload


def load_platform_toolchain_support_evidence() -> dict[str, Any]:
    schema = load_json_object(PLATFORM_TOOLCHAIN_SUPPORT_EVIDENCE_SCHEMA_PATH)
    payload = load_json_object(PLATFORM_TOOLCHAIN_SUPPORT_EVIDENCE_PATH)
    validate_json_schema(payload, schema, label=repo_rel(PLATFORM_TOOLCHAIN_SUPPORT_EVIDENCE_PATH))
    return payload


def evidence_records_by_id(payload: dict[str, Any]) -> dict[str, dict[str, Any]]:
    records = payload.get("evidence_records", [])
    expect(isinstance(records, list), "platform support evidence missing evidence_records")
    by_id: dict[str, dict[str, Any]] = {}
    for record in records:
        expect(isinstance(record, dict), "platform support evidence record must be an object")
        evidence_id = str(record.get("evidence_id", ""))
        expect(evidence_id, "platform support evidence record missing evidence_id")
        expect(evidence_id not in by_id, f"duplicate platform support evidence id: {evidence_id}")
        by_id[evidence_id] = record
    return by_id


def _owner_policy_matches(payload: dict[str, Any]) -> None:
    owner_policy = payload.get("owner_policy")
    expect(isinstance(owner_policy, dict), "platform support evidence missing owner_policy")
    for field_name, expected_value in PLATFORM_HARDENING_OWNER_POLICY.items():
        expect(owner_policy.get(field_name) == expected_value, f"platform support evidence owner_policy drifted for {field_name}")


def _required_source_paths_exist(record: dict[str, Any]) -> None:
    evidence_id = str(record["evidence_id"])
    source_paths = record.get("source_paths", [])
    expect(isinstance(source_paths, list) and source_paths, f"{evidence_id} missing checked-in source paths")
    for raw_path in source_paths:
        path_text = str(raw_path)
        expect(not path_text.startswith(("tmp/", "artifacts/")), f"{evidence_id} used generated artifact as source path: {path_text}")
        expect(resolve_repo_path(path_text).is_file(), f"{evidence_id} source path does not exist: {path_text}")


def _supporting_record_is_claimable(record: dict[str, Any], platform_id: str, evidence_class: str) -> None:
    evidence_id = str(record["evidence_id"])
    expect(record.get("claim_weight") == "supporting", f"{evidence_id} is not supporting evidence")
    expect(record.get("evidence_class") == evidence_class, f"{evidence_id} evidence class drifted from {evidence_class}")
    expect(record.get("requires_network") is False, f"{evidence_id} requires network for a support claim")
    expect(record.get("unsupported_host_behavior") == "fail-closed", f"{evidence_id} does not fail closed for unsupported hosts")
    expect(platform_id in record.get("supports_platform_ids", []), f"{evidence_id} does not support {platform_id}")
    expect(record.get("replay_commands"), f"{evidence_id} missing replay command")
    _required_source_paths_exist(record)


def _policy_record_is_fail_closed(record: dict[str, Any]) -> None:
    evidence_id = str(record["evidence_id"])
    expect(record.get("claim_weight") == "policy", f"{evidence_id} must remain policy-only evidence")
    expect(record.get("requires_network") is False, f"{evidence_id} policy evidence requires network")
    expect(record.get("unsupported_host_behavior") == "fail-closed", f"{evidence_id} policy evidence is not fail-closed")
    _required_source_paths_exist(record)


def _clean_room_record_proves_from_nothing_install(record: dict[str, Any]) -> None:
    evidence_id = str(record["evidence_id"])
    expect(record.get("evidence_class") == "clean_room", f"{evidence_id} is not clean-room evidence")
    source_paths = [str(path) for path in record.get("source_paths", [])]
    replay_commands = [str(command) for command in record.get("replay_commands", [])]
    generated_report_paths = [str(path) for path in record.get("generated_report_paths", [])]
    expect(
        CLEAN_INSTALL_CHECKER_PATH in source_paths,
        f"{evidence_id} did not cite the clean install checker",
    )
    expect(
        any(
            (
                "validate-package-install-distribution" in command
                or CLEAN_INSTALL_CHECKER_PATH in command
            )
            and "--from-nothing" in command
            for command in replay_commands
        ),
        f"{evidence_id} did not replay the from-nothing package install path",
    )
    expect(
        CLEAN_INSTALL_SUMMARY_PATH in generated_report_paths,
        f"{evidence_id} did not publish clean install summary evidence",
    )


def _supported_tier_platforms(tier_policy: dict[str, Any]) -> set[str]:
    supported: set[str] = set()
    for tier in tier_policy.get("tiers", []):
        if tier.get("claim_class") in {"supported", "supported-but-not-default"}:
            supported.update(str(platform_id) for platform_id in tier.get("platform_ids", []))
    return supported


def _unsupported_failure_ids(unsupported_host_policy: dict[str, Any]) -> set[str]:
    return {
        str(check.get("failure_id"))
        for check in unsupported_host_policy.get("synthetic_unsupported_host_checks", [])
        if check.get("failure_id")
    }


def _require_no_claims_outside_boundary(
    records: Iterable[dict[str, Any]],
    *,
    boundary_supported_platform_ids: set[str],
) -> None:
    for record in records:
        evidence_id = str(record["evidence_id"])
        for platform_id in record.get("supports_platform_ids", []):
            expect(
                str(platform_id) in boundary_supported_platform_ids,
                f"{evidence_id} widened support outside the checked-in boundary: {platform_id}",
            )


def validate_platform_toolchain_support_evidence(
    payload: dict[str, Any],
    *,
    boundary: dict[str, Any],
    supported_platforms: dict[str, Any],
    tier_policy: dict[str, Any],
    unsupported_host_policy: dict[str, Any],
) -> None:
    _owner_policy_matches(payload)

    network_policy = payload.get("network_policy", {})
    expect(network_policy.get("support_claims_require_live_network") is False, "support evidence cannot require live network")
    expect(network_policy.get("unsupported_host_result") == "fail-closed", "unsupported hosts must fail closed")
    expect(network_policy.get("network_unavailable_result") == "skip-no-support-claim", "network loss must not publish support claims")

    records_by_id = evidence_records_by_id(payload)
    boundary_supported_ids = {str(platform_id) for platform_id in boundary.get("supported_platform_ids", [])}
    supported_platform_ids = {
        str(platform.get("platform_id"))
        for platform in supported_platforms.get("supported_platforms", [])
        if platform.get("platform_id")
    }
    tier_supported_ids = _supported_tier_platforms(tier_policy)
    unsupported_failure_ids = _unsupported_failure_ids(unsupported_host_policy)

    _require_no_claims_outside_boundary(records_by_id.values(), boundary_supported_platform_ids=boundary_supported_ids)

    supported_row_ids: list[str] = []
    for row in payload.get("support_rows", []):
        expect(isinstance(row, dict), "platform support row must be an object")
        platform_id = str(row["platform_id"])
        support_state = str(row["support_state"])
        if support_state == "supported":
            supported_row_ids.append(platform_id)
            expect(platform_id in boundary_supported_ids, f"{platform_id} support row is outside the boundary inventory")
            expect(platform_id in supported_platform_ids, f"{platform_id} support row is absent from supported_platforms.json")
            expect(platform_id in tier_supported_ids, f"{platform_id} support row is absent from supported support tiers")
            expect(row.get("claim_class") == "supported", f"{platform_id} support row must use supported claim_class")
            required_classes = tuple(str(item) for item in row.get("required_evidence_classes", []))
            expect(set(required_classes) == set(REQUIRED_SUPPORTED_EVIDENCE_CLASSES), f"{platform_id} missing required evidence classes")
            evidence = row.get("evidence", {})
            expect(isinstance(evidence, dict), f"{platform_id} support row missing evidence map")
            for evidence_class in REQUIRED_SUPPORTED_EVIDENCE_CLASSES:
                evidence_id = str(evidence.get(evidence_class, ""))
                expect(evidence_id in records_by_id, f"{platform_id} missing {evidence_class} evidence record {evidence_id}")
                _supporting_record_is_claimable(records_by_id[evidence_id], platform_id, evidence_class)
            for evidence_id in (
                *row.get("toolchain_evidence_ids", []),
                *row.get("hosted_ci_evidence_ids", []),
            ):
                record = records_by_id[str(evidence_id)]
                _supporting_record_is_claimable(record, platform_id, str(record["evidence_class"]))
            for evidence_id in row.get("local_clean_room_evidence_ids", []):
                record = records_by_id[str(evidence_id)]
                _supporting_record_is_claimable(record, platform_id, str(record["evidence_class"]))
                _clean_room_record_proves_from_nothing_install(record)
            continue

        expect(support_state == "unsupported", f"{platform_id} used unknown support_state {support_state}")
        expect(platform_id not in boundary_supported_ids, f"{platform_id} unsupported row is listed as supported")
        expect(row.get("claim_class") == "fail-closed", f"{platform_id} unsupported row must fail closed")
        expect(row.get("tier_id") == "unsupported", f"{platform_id} unsupported row must use unsupported tier")
        failure_id = str(row.get("failure_id", ""))
        expect(failure_id in unsupported_failure_ids, f"{platform_id} failure_id is not in unsupported host policy")
        fail_closed_evidence_id = str(row.get("evidence", {}).get("fail_closed", ""))
        expect(fail_closed_evidence_id in records_by_id, f"{platform_id} missing fail-closed evidence record")
        _policy_record_is_fail_closed(records_by_id[fail_closed_evidence_id])

    expect(sorted(supported_row_ids) == sorted(boundary_supported_ids), "supported support rows drifted from boundary inventory")

    for toolchain_range in payload.get("toolchain_ranges", []):
        evidence_ids = [str(evidence_id) for evidence_id in toolchain_range.get("evidence_ids", [])]
        expect(toolchain_range.get("unsupported_version_behavior") == "fail-closed-no-range-claim", "toolchain range must fail closed outside evidence")
        expect("all" not in str(toolchain_range.get("range_claim", "")).lower(), "toolchain range attempted an all-version support claim")
        for platform_id in toolchain_range.get("platform_ids", []):
            expect(str(platform_id) in boundary_supported_ids, f"toolchain range widened support to {platform_id}")
        for evidence_id in evidence_ids:
            record = records_by_id[evidence_id]
            expect(record.get("evidence_class") == "toolchain", f"{evidence_id} is not toolchain evidence")
            expect(record.get("requires_network") is False, f"{evidence_id} requires network")
            _required_source_paths_exist(record)

    for sanitizer in payload.get("sanitizer_variants", []):
        if sanitizer.get("claim_state") == "reserved":
            expect(not sanitizer.get("platform_ids"), f"{sanitizer['variant_id']} reserved sanitizer variant cannot list platforms")
        for evidence_id in sanitizer.get("evidence_ids", []):
            expect(str(evidence_id) in records_by_id, f"{sanitizer['variant_id']} missing sanitizer evidence {evidence_id}")
            _policy_record_is_fail_closed(records_by_id[str(evidence_id)])


def build_support_evidence_matrix_sections(payload: dict[str, Any]) -> dict[str, Any]:
    records = payload["evidence_records"]
    return {
        "support_evidence_contract": {
            "contract_id": payload["contract_id"],
            "schema_version": payload["schema_version"],
            "issue": payload["issue"],
            "schema_path": payload["schema_path"],
            "source_path": payload["source_path"],
            "network_policy": payload["network_policy"],
        },
        "platform_support_rows": payload["support_rows"],
        "toolchain_support": {
            "toolchain_ranges": payload["toolchain_ranges"],
            "sanitizer_variants": payload["sanitizer_variants"],
        },
        "evidence_records": records,
        "support_evidence_ids": [str(record["evidence_id"]) for record in records],
    }


def build_support_evidence_summary(payload: dict[str, Any]) -> dict[str, Any]:
    supported_rows = [row for row in payload["support_rows"] if row["support_state"] == "supported"]
    unsupported_rows = [row for row in payload["support_rows"] if row["support_state"] == "unsupported"]
    records = payload["evidence_records"]
    return {
        "contract_id": "objc3c.platform.toolchain.support.evidence.summary.v1",
        "status": "PASS",
        "source_contract_id": payload["contract_id"],
        "schema_path": payload["schema_path"],
        "source_path": payload["source_path"],
        "support_row_count": len(payload["support_rows"]),
        "supported_platform_ids": [str(row["platform_id"]) for row in supported_rows],
        "unsupported_platform_ids": [str(row["platform_id"]) for row in unsupported_rows],
        "toolchain_range_ids": [str(row["toolchain_id"]) for row in payload["toolchain_ranges"]],
        "sanitizer_variant_ids": [str(row["variant_id"]) for row in payload["sanitizer_variants"]],
        "supporting_evidence_ids": [
            str(record["evidence_id"])
            for record in records
            if record["claim_weight"] == "supporting"
        ],
        "policy_evidence_ids": [
            str(record["evidence_id"])
            for record in records
            if record["claim_weight"] == "policy"
        ],
        "network_policy": payload["network_policy"],
    }


__all__ = [
    "REQUIRED_SUPPORTED_EVIDENCE_CLASSES",
    "build_support_evidence_matrix_sections",
    "build_support_evidence_summary",
    "evidence_records_by_id",
    "load_platform_toolchain_support_evidence",
    "validate_platform_toolchain_support_evidence",
]
