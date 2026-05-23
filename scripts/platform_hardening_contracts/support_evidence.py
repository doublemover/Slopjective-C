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
REQUIRED_SUPPORTED_TOOLCHAIN_COMPONENTS: tuple[str, ...] = (
    "llvm",
    "clang",
    "cmake",
    "ninja",
    "python",
    "node",
    "pwsh",
)
REQUIRED_LLVM_MATRIX_TOOLS: tuple[str, ...] = (
    "clang",
    "clang++",
    "llc",
    "llvm-ar",
    "headers-libs",
)
REQUIRED_ROADMAP_ISSUE_REFS: tuple[int, ...] = (8206, 8228, 8229, 8230, 8231, 8232)
REQUIRED_UNSUPPORTED_POLICY_HARD_FAIL_CLASSES: tuple[str, ...] = (
    "unsupported-host-os-or-arch",
    "missing-required-tier-tool",
    "unsupported-channel-claim",
    "support-tier-overclaim",
    "missing-runtime-library",
    "missing-sanitizer-runtime",
    "stale-package-metadata",
    "native-object-emission-unavailable",
)
EXPECTED_UNSUPPORTED_PLATFORM_ISSUES: dict[str, int] = {
    "linux-x64": 8228,
    "darwin-arm64": 8229,
}
EXPECTED_SANITIZER_ISSUES: dict[str, int] = {
    "address": 8230,
    "undefined": 8231,
}
FORBIDDEN_TOOLCHAIN_RANGE_CLAIM_TERMS: tuple[str, ...] = (
    "all",
    "best-effort",
    "best effort",
    "compat",
    "fallback",
)
CLEAN_INSTALL_SUMMARY_PATH = (
    "tmp/reports/package-ecosystem/install-distribution-credibility-summary.json"
)
CLEAN_INSTALL_VERIFICATION_PATH = (
    "tmp/artifacts/package-ecosystem/install-validation/objc3c-install-distribution-verification.json"
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


def _negative_contracts_are_fail_closed(owner_id: str, contracts: Any) -> None:
    expect(isinstance(contracts, list), f"{owner_id} missing negative fail-closed contracts")
    for contract in contracts:
        expect(isinstance(contract, dict), f"{owner_id} negative contract must be an object")
        contract_id = str(contract.get("contract_id", ""))
        expect(contract_id, f"{owner_id} negative contract missing contract_id")
        expect(str(contract.get("failure_class", "")), f"{contract_id} missing failure_class")
        required_behavior = str(contract.get("required_behavior", ""))
        expect(required_behavior.startswith("fail-closed"), f"{contract_id} does not fail closed")
        expect(str(contract.get("source_owner", "")), f"{contract_id} missing source_owner")


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
            "npm run objc3c -- validate-package-install-distribution --from-nothing"
            in command
            for command in replay_commands
        ),
        f"{evidence_id} did not replay the from-nothing package install path",
    )
    expect(
        CLEAN_INSTALL_SUMMARY_PATH in generated_report_paths,
        f"{evidence_id} did not publish clean install summary evidence",
    )
    expect(
        CLEAN_INSTALL_VERIFICATION_PATH in generated_report_paths,
        f"{evidence_id} did not publish clean install verification evidence",
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


def _validate_unsupported_host_policy_contract(unsupported_host_policy: dict[str, Any]) -> None:
    hard_fail_classes = {
        str(failure_class.get("failure_id")): failure_class
        for failure_class in unsupported_host_policy.get("hard_fail_classes", [])
        if isinstance(failure_class, dict) and failure_class.get("failure_id")
    }
    expect(
        set(REQUIRED_UNSUPPORTED_POLICY_HARD_FAIL_CLASSES) <= set(hard_fail_classes),
        "unsupported host hard-fail classes drifted",
    )
    for failure_id in REQUIRED_UNSUPPORTED_POLICY_HARD_FAIL_CLASSES:
        failure_class = hard_fail_classes[failure_id]
        expect(
            failure_class.get("required_behavior") == "fail-closed",
            f"{failure_id} hard-fail class does not fail closed",
        )
    native_object_failure = hard_fail_classes["native-object-emission-unavailable"]
    expect(
        {"toolchain", "package", "execution", "publication"}
        <= {str(surface) for surface in native_object_failure.get("applies_to", [])},
        "native object emission hard-fail class does not block toolchain package execution and publication",
    )
    required_claims = {str(claim) for claim in unsupported_host_policy.get("required_claims", [])}
    expect(
        "native object emission requires llc --filetype=obj and has no clang substitute success path" in required_claims,
        "unsupported host policy missing native object emission no-fallback claim",
    )
    forbidden_phrases = {str(phrase) for phrase in unsupported_host_policy.get("forbidden_phrases", [])}
    expect(
        "object emission supported via clang substitute" in forbidden_phrases,
        "unsupported host policy missing clang substitute forbidden phrase",
    )


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


def _required_toolchain_components(payload: dict[str, Any]) -> set[str]:
    requirements = payload.get("toolchain_evidence_requirements")
    expect(isinstance(requirements, dict), "platform support evidence missing toolchain_evidence_requirements")
    components = [str(component) for component in requirements.get("required_components", [])]
    expect(components, "platform support evidence missing required toolchain components")
    expect(
        set(components) == set(REQUIRED_SUPPORTED_TOOLCHAIN_COMPONENTS),
        "platform support evidence required toolchain components drifted",
    )
    expect(
        requirements.get("unsupported_component_behavior") == "fail-closed-no-range-claim",
        "unsupported toolchain components must fail closed",
    )
    expect(
        requirements.get("support_claim_policy") == "evidence-bound-current-probes-only",
        "toolchain support claim policy drifted",
    )
    return set(components)


def _toolchain_ranges_by_component(
    payload: dict[str, Any],
    *,
    records_by_id: dict[str, dict[str, Any]],
    boundary_supported_platform_ids: set[str],
) -> dict[str, dict[str, Any]]:
    by_component: dict[str, dict[str, Any]] = {}
    for toolchain_range in payload.get("toolchain_ranges", []):
        component = str(toolchain_range.get("component", ""))
        expect(component, "toolchain range missing component")
        expect(component not in by_component, f"duplicate toolchain range component: {component}")
        by_component[component] = toolchain_range

        claim_state = str(toolchain_range.get("claim_state", ""))
        range_claim = str(toolchain_range.get("range_claim", "")).lower()
        expect(
            toolchain_range.get("unsupported_version_behavior") == "fail-closed-no-range-claim",
            f"{component} toolchain range must fail closed outside evidence",
        )
        expect(str(toolchain_range.get("source_owner", "")), f"{component} toolchain range missing source_owner")
        expect(
            str(toolchain_range.get("unsupported_contract_id", "")),
            f"{component} toolchain range missing unsupported_contract_id",
        )
        for forbidden_term in FORBIDDEN_TOOLCHAIN_RANGE_CLAIM_TERMS:
            expect(
                forbidden_term not in range_claim,
                f"{component} toolchain range used unsupported compatibility language: {forbidden_term}",
            )

        platform_ids = [str(platform_id) for platform_id in toolchain_range.get("platform_ids", [])]
        evidence_ids = [str(evidence_id) for evidence_id in toolchain_range.get("evidence_ids", [])]
        required_classes = [str(item) for item in toolchain_range.get("required_evidence_classes", [])]
        expect(required_classes == ["toolchain"], f"{component} toolchain range must require toolchain evidence")
        expect(evidence_ids, f"{component} toolchain range missing evidence_ids")

        if claim_state == "evidence-bound":
            expect(platform_ids, f"{component} evidence-bound toolchain range missing platform ids")
            for platform_id in platform_ids:
                expect(platform_id in boundary_supported_platform_ids, f"{component} toolchain range widened support to {platform_id}")
            for evidence_id in evidence_ids:
                expect(evidence_id in records_by_id, f"{component} toolchain range missing evidence record {evidence_id}")
                record = records_by_id[evidence_id]
                for platform_id in platform_ids:
                    _supporting_record_is_claimable(record, platform_id, "toolchain")
            continue

        expect(claim_state == "reserved", f"{component} toolchain range used unknown claim_state {claim_state}")
        expect(not platform_ids, f"{component} reserved toolchain range cannot list platforms")
        for evidence_id in evidence_ids:
            expect(evidence_id in records_by_id, f"{component} reserved toolchain range missing evidence record {evidence_id}")
            _policy_record_is_fail_closed(records_by_id[evidence_id])

    return by_component


def _validate_package_variant_rows(
    payload: dict[str, Any],
    *,
    records_by_id: dict[str, dict[str, Any]],
    boundary_supported_platform_ids: set[str],
) -> dict[str, dict[str, Any]]:
    rows = payload.get("package_variant_rows")
    expect(isinstance(rows, list) and rows, "platform support evidence missing package_variant_rows")
    by_id: dict[str, dict[str, Any]] = {}
    for row in rows:
        expect(isinstance(row, dict), "package variant row must be an object")
        row_id = str(row.get("row_id", ""))
        expect(row_id, "package variant row missing row_id")
        expect(row_id not in by_id, f"duplicate package variant row: {row_id}")
        by_id[row_id] = row
        claim_state = str(row.get("claim_state", ""))
        platform_ids = {str(platform_id) for platform_id in row.get("platform_ids", [])}
        evidence_ids = [str(evidence_id) for evidence_id in row.get("evidence_ids", [])]
        required_classes = {str(item) for item in row.get("required_evidence_classes", [])}
        expect(evidence_ids, f"{row_id} missing evidence_ids")
        expect(row.get("unsupported_behavior") == "fail-closed", f"{row_id} does not fail closed")

        if claim_state == "evidence-bound":
            expect(platform_ids, f"{row_id} evidence-bound package row missing platform_ids")
            expect(
                not row.get("required_missing_evidence_classes"),
                f"{row_id} evidence-bound package row cannot list missing evidence classes",
            )
            expect(
                platform_ids <= boundary_supported_platform_ids,
                f"{row_id} widened package support to unsupported platforms: {sorted(platform_ids - boundary_supported_platform_ids)}",
            )
            for evidence_id in evidence_ids:
                expect(evidence_id in records_by_id, f"{row_id} missing evidence record {evidence_id}")
                record = records_by_id[evidence_id]
                expect(record.get("claim_weight") == "supporting", f"{row_id} evidence {evidence_id} is not supporting")
                evidence_class = str(record.get("evidence_class", ""))
                expect(evidence_class in required_classes, f"{row_id} evidence {evidence_id} class {evidence_class} is not required")
                for platform_id in platform_ids:
                    _supporting_record_is_claimable(record, platform_id, evidence_class)
            _negative_contracts_are_fail_closed(row_id, row.get("negative_contracts", []))
            continue

        expect(
            claim_state in {"fail-closed", "reserved"},
            f"{row_id} package row used unknown claim_state {claim_state}",
        )
        expect(not platform_ids, f"{row_id} {claim_state} package row cannot list supported platform_ids")
        expect(
            row.get("required_missing_evidence_classes"),
            f"{row_id} missing required_missing_evidence_classes",
        )
        missing_classes = {str(item) for item in row.get("required_missing_evidence_classes", [])}
        expect(
            missing_classes <= required_classes,
            f"{row_id} lists missing evidence outside required evidence classes: {sorted(missing_classes - required_classes)}",
        )
        for evidence_id in evidence_ids:
            expect(evidence_id in records_by_id, f"{row_id} missing policy evidence record {evidence_id}")
            _policy_record_is_fail_closed(records_by_id[evidence_id])
        _negative_contracts_are_fail_closed(row_id, row.get("negative_contracts", []))

    return by_id


def _validate_llvm_version_support_matrix(
    payload: dict[str, Any],
    *,
    records_by_id: dict[str, dict[str, Any]],
    boundary_supported_platform_ids: set[str],
) -> None:
    matrix = payload.get("llvm_version_support_matrix")
    expect(isinstance(matrix, dict), "platform support evidence missing LLVM version support matrix")
    expect(
        matrix.get("contract_id") == "objc3c.llvm.version-support-matrix.source.v1",
        "LLVM version support matrix contract_id drifted",
    )
    expect(matrix.get("issue_ref") == 8232, "LLVM version support matrix issue_ref drifted")
    expect(
        matrix.get("support_claim_policy") == "capability-probed-fail-closed",
        "LLVM version support matrix policy drifted",
    )
    native_object_contract = matrix.get("native_object_emission_contract")
    expect(
        isinstance(native_object_contract, dict),
        "LLVM version support matrix missing native object emission contract",
    )
    expect(
        native_object_contract.get("contract_id")
        == "objc3c.llvm.native-object-emission.fail-closed.v1",
        "native object emission contract_id drifted",
    )
    expect(native_object_contract.get("issue_ref") == 8232, "native object emission issue_ref drifted")
    expect(native_object_contract.get("required_tool") == "llc", "native object emission required tool drifted")
    expect(
        native_object_contract.get("required_probe") == "llc --filetype=obj",
        "native object emission required probe drifted",
    )
    expect(
        native_object_contract.get("missing_llc_status") == "native_object_emission_missing_llc",
        "native object emission missing-llc status drifted",
    )
    expect(
        native_object_contract.get("missing_filetype_status")
        == "native_object_emission_filetype_obj_unavailable",
        "native object emission filetype status drifted",
    )
    expect(
        native_object_contract.get("fallback_policy") == "no-clang-fallback-success-claim",
        "native object emission fallback policy drifted",
    )
    for claim_field in ("minimum_supported_version", "known_good_versions"):
        value = matrix.get(claim_field)
        expect(bool(value), f"LLVM version support matrix missing {claim_field}")

    tools_by_name: dict[str, dict[str, Any]] = {}
    for tool in matrix.get("required_tools", []):
        expect(isinstance(tool, dict), "LLVM matrix required tool must be an object")
        tool_name = str(tool.get("tool_name", ""))
        expect(tool_name, "LLVM matrix required tool missing tool_name")
        expect(tool_name not in tools_by_name, f"duplicate LLVM matrix tool: {tool_name}")
        tools_by_name[tool_name] = tool
        expect(
            tool.get("failure_behavior") == "fail-closed-before-support-claim",
            f"{tool_name} LLVM matrix tool does not fail closed",
        )
        if tool_name in {"clang", "clang++", "llc", "llvm-ar", "headers-libs"}:
            expect(tool.get("claim_state") == "required", f"{tool_name} must be required")

    expect(
        set(tools_by_name) == set(REQUIRED_LLVM_MATRIX_TOOLS),
        "LLVM version support matrix required tools drifted",
    )

    for entry in matrix.get("matrix_entries", []):
        expect(isinstance(entry, dict), "LLVM matrix entry must be an object")
        platform_id = str(entry.get("platform_id", ""))
        expect(platform_id in boundary_supported_platform_ids, f"LLVM matrix widened support to {platform_id}")
        expect(
            entry.get("unsupported_version_behavior") == "fail-closed-no-range-claim",
            f"{entry.get('entry_id', '')} LLVM matrix entry does not fail closed",
        )
        version_claim = str(entry.get("llvm_version_claim", "")).lower()
        for forbidden_term in FORBIDDEN_TOOLCHAIN_RANGE_CLAIM_TERMS:
            expect(
                forbidden_term not in version_claim,
                f"LLVM matrix entry used unsupported compatibility language: {forbidden_term}",
            )
        for evidence_id in entry.get("evidence_ids", []):
            evidence_text = str(evidence_id)
            expect(evidence_text in records_by_id, f"LLVM matrix entry missing evidence record {evidence_text}")
            record = records_by_id[evidence_text]
            expect(record.get("claim_weight") == "supporting", f"{evidence_text} is not supporting evidence")
            expect(platform_id in record.get("supports_platform_ids", []), f"{evidence_text} does not support {platform_id}")

    rejection_rules = matrix.get("rejection_rules", [])
    expect(isinstance(rejection_rules, list) and rejection_rules, "LLVM matrix missing rejection rules")
    missing_llc_rule: dict[str, Any] | None = None
    for rule in rejection_rules:
        expect(isinstance(rule, dict), "LLVM matrix rejection rule must be an object")
        expect(str(rule.get("rule_id", "")), "LLVM matrix rejection rule missing rule_id")
        expect(str(rule.get("condition", "")), f"{rule.get('rule_id', '')} missing rejection condition")
        expect(str(rule.get("diagnostic", "")), f"{rule.get('rule_id', '')} missing rejection diagnostic")
        if rule.get("rule_id") == "objc3c.llvm.reject.missing-llc":
            missing_llc_rule = rule
    expect(missing_llc_rule is not None, "LLVM matrix missing missing-llc rejection rule")
    expect(
        missing_llc_rule.get("failure_status") == "native_object_emission_missing_llc",
        "missing-llc rejection status drifted",
    )
    expect(
        missing_llc_rule.get("hosted_runner_behavior")
        == "fail-closed-no-native-object-success-claim",
        "missing-llc hosted runner behavior drifted",
    )
    expect(
        missing_llc_rule.get("conformance_minima_behavior")
        == "fail-closed-before-cross-lane-runtime-proof",
        "missing-llc conformance minima behavior drifted",
    )
    expect(
        missing_llc_rule.get("fallback_policy") == "no-clang-fallback-success-claim",
        "missing-llc fallback policy drifted",
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
    issue_refs = {int(issue_ref) for issue_ref in payload.get("roadmap_issue_refs", [])}
    expect(
        set(REQUIRED_ROADMAP_ISSUE_REFS) <= issue_refs,
        "platform support evidence roadmap issue refs drifted",
    )
    required_toolchain_components = _required_toolchain_components(payload)
    toolchain_ranges = _toolchain_ranges_by_component(
        payload,
        records_by_id=records_by_id,
        boundary_supported_platform_ids=boundary_supported_ids,
    )
    package_variant_rows = _validate_package_variant_rows(
        payload,
        records_by_id=records_by_id,
        boundary_supported_platform_ids=boundary_supported_ids,
    )
    _validate_llvm_version_support_matrix(
        payload,
        records_by_id=records_by_id,
        boundary_supported_platform_ids=boundary_supported_ids,
    )
    supported_platform_ids = {
        str(platform.get("platform_id"))
        for platform in supported_platforms.get("supported_platforms", [])
        if platform.get("platform_id")
    }
    _validate_unsupported_host_policy_contract(unsupported_host_policy)
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
            expect(
                not row.get("required_missing_evidence_classes"),
                f"{platform_id} supported row cannot list missing evidence classes",
            )
            row_toolchain_components = {str(item) for item in row.get("required_toolchain_components", [])}
            expect(
                row_toolchain_components == required_toolchain_components,
                f"{platform_id} missing required toolchain components",
            )
            for package_row_id in row.get("package_variant_row_ids", []):
                package_row = package_variant_rows.get(str(package_row_id))
                expect(package_row is not None, f"{platform_id} missing package variant row {package_row_id}")
                expect(package_row.get("claim_state") == "evidence-bound", f"{platform_id} package row is not evidence-bound")
                expect(platform_id in package_row.get("platform_ids", []), f"{platform_id} package row does not include the supported platform")
            evidence = row.get("evidence", {})
            expect(isinstance(evidence, dict), f"{platform_id} support row missing evidence map")
            for evidence_class in REQUIRED_SUPPORTED_EVIDENCE_CLASSES:
                evidence_id = str(evidence.get(evidence_class, ""))
                expect(evidence_id in records_by_id, f"{platform_id} missing {evidence_class} evidence record {evidence_id}")
                _supporting_record_is_claimable(records_by_id[evidence_id], platform_id, evidence_class)
            row_toolchain_evidence_ids = {str(evidence_id) for evidence_id in row.get("toolchain_evidence_ids", [])}
            for component in required_toolchain_components:
                expect(component in toolchain_ranges, f"{platform_id} missing required {component} toolchain range")
                toolchain_range = toolchain_ranges[component]
                expect(toolchain_range.get("claim_state") == "evidence-bound", f"{platform_id} {component} toolchain range is not evidence-bound")
                expect(platform_id in toolchain_range.get("platform_ids", []), f"{platform_id} {component} toolchain range does not include the supported platform")
                missing_range_evidence = [
                    str(evidence_id)
                    for evidence_id in toolchain_range.get("evidence_ids", [])
                    if str(evidence_id) not in row_toolchain_evidence_ids
                ]
                expect(
                    not missing_range_evidence,
                    f"{platform_id} {component} toolchain evidence missing from support row: {', '.join(missing_range_evidence)}",
                )
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
        expect(
            set(str(item) for item in row.get("required_missing_evidence_classes", []))
            == set(REQUIRED_SUPPORTED_EVIDENCE_CLASSES),
            f"{platform_id} unsupported row must list all package/install/native execution promotion prerequisites as missing",
        )
        expected_issue = EXPECTED_UNSUPPORTED_PLATFORM_ISSUES.get(platform_id)
        expect(expected_issue is not None and row.get("issue_ref") == expected_issue, f"{platform_id} unsupported issue_ref drifted")
        _negative_contracts_are_fail_closed(platform_id, row.get("negative_contracts"))
        for package_row_id in row.get("package_variant_row_ids", []):
            package_row = package_variant_rows.get(str(package_row_id))
            expect(package_row is not None, f"{platform_id} missing package variant row {package_row_id}")
            expect(package_row.get("claim_state") == "fail-closed", f"{platform_id} package row must fail closed")
            expect(not package_row.get("platform_ids"), f"{platform_id} package row widened support")
        failure_id = str(row.get("failure_id", ""))
        expect(failure_id in unsupported_failure_ids, f"{platform_id} failure_id is not in unsupported host policy")
        fail_closed_evidence_id = str(row.get("evidence", {}).get("fail_closed", ""))
        expect(fail_closed_evidence_id in records_by_id, f"{platform_id} missing fail-closed evidence record")
        _policy_record_is_fail_closed(records_by_id[fail_closed_evidence_id])

    expect(sorted(supported_row_ids) == sorted(boundary_supported_ids), "supported support rows drifted from boundary inventory")

    for sanitizer in payload.get("sanitizer_variants", []):
        sanitizer_name = str(sanitizer.get("sanitizer", ""))
        expected_issue = EXPECTED_SANITIZER_ISSUES.get(sanitizer_name)
        expect(expected_issue is not None and sanitizer.get("issue_ref") == expected_issue, f"{sanitizer.get('variant_id', '')} issue_ref drifted")
        package_row_id = str(sanitizer.get("package_variant_row_id", ""))
        package_row = package_variant_rows.get(package_row_id)
        expect(package_row is not None, f"{sanitizer.get('variant_id', '')} missing package variant row {package_row_id}")
        expect(package_row.get("claim_state") == sanitizer.get("claim_state"), f"{package_row_id} claim_state drifted from sanitizer variant")
        expect(package_row.get("package_id") == sanitizer.get("package_id"), f"{package_row_id} package_id drifted from sanitizer variant")
        expect(str(sanitizer.get("llvm_requirement", "")), f"{sanitizer.get('variant_id', '')} missing llvm_requirement")
        expect(str(sanitizer.get("runtime_requirement", "")), f"{sanitizer.get('variant_id', '')} missing runtime_requirement")
        expect(
            set(str(item) for item in sanitizer.get("required_promotion_evidence", []))
            == {"package", "install", "execution"},
            f"{sanitizer.get('variant_id', '')} sanitizer promotion prerequisites must be package/install/execution",
        )
        expect(
            set(str(item) for item in sanitizer.get("required_missing_evidence_classes", []))
            == {"package", "install", "execution"},
            f"{sanitizer.get('variant_id', '')} sanitizer missing evidence must remain package/install/execution",
        )
        _negative_contracts_are_fail_closed(str(sanitizer.get("variant_id", "")), sanitizer.get("negative_contracts"))
        if sanitizer.get("claim_state") == "reserved":
            expect(not sanitizer.get("platform_ids"), f"{sanitizer['variant_id']} reserved sanitizer variant cannot list platforms")
            expect(package_row.get("claim_state") == "reserved", f"{package_row_id} package row must remain reserved")
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
        "package_variant_rows": payload["package_variant_rows"],
        "toolchain_support": {
            "toolchain_evidence_requirements": payload["toolchain_evidence_requirements"],
            "toolchain_ranges": payload["toolchain_ranges"],
            "llvm_version_support_matrix": payload["llvm_version_support_matrix"],
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
        "required_toolchain_components": list(REQUIRED_SUPPORTED_TOOLCHAIN_COMPONENTS),
        "toolchain_range_ids": [str(row["toolchain_id"]) for row in payload["toolchain_ranges"]],
        "package_variant_row_ids": [str(row["row_id"]) for row in payload["package_variant_rows"]],
        "llvm_matrix_entry_ids": [
            str(row["entry_id"])
            for row in payload["llvm_version_support_matrix"]["matrix_entries"]
        ],
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
    "REQUIRED_SUPPORTED_TOOLCHAIN_COMPONENTS",
    "build_support_evidence_matrix_sections",
    "build_support_evidence_summary",
    "evidence_records_by_id",
    "load_platform_toolchain_support_evidence",
    "validate_platform_toolchain_support_evidence",
]
