#!/usr/bin/env python3
from __future__ import annotations

from typing import Any, Iterable

from objc3c_tooling.paths import repo_rel
from platform_hardening_contracts import (
    HOST_PROMOTION_EVIDENCE_CLASSES,
    HOST_PROMOTION_EVIDENCE_ACTION,
    HOST_PROMOTION_EVIDENCE_CONTRACT_ID,
    HOST_PROMOTION_EVIDENCE_CONTRACT_RELATIVE_PATH,
    HOST_PROMOTION_EVIDENCE_SUMMARY_CONTRACT_ID,
    HOST_PROMOTION_EVIDENCE_SUMMARY_RELATIVE_PATH,
    HOST_PROMOTION_FAIL_CLOSED_BLOCKER_CLASSES,
    HOST_PROMOTION_GENERATED_REPORT_ROOT,
    HOST_PROMOTION_PLATFORM_CONTRACTS,
    HOST_PROMOTION_POLICY,
    HOST_PROMOTION_REQUIRED_GATE_CLASSES,
    HOST_PROMOTION_REQUIRED_SOURCE_RECORD_TYPES,
    HOST_PROMOTION_VALIDATE_PLATFORM_HARDENING_ACTION,
    ROOT,
    build_host_promotion_contract_payload,
    expect,
    generated_report_paths_for_platform,
    generated_report_root_for_platform,
    host_promotion_platform_ids,
    load_json_object,
    write_json,
)

CONTRACT_PATH = ROOT / HOST_PROMOTION_EVIDENCE_CONTRACT_RELATIVE_PATH
SUMMARY_PATH = ROOT / HOST_PROMOTION_EVIDENCE_SUMMARY_RELATIVE_PATH

REQUIRED_PLATFORM_IDS: tuple[str, ...] = host_promotion_platform_ids()
REQUIRED_GATE_CLASSES: tuple[str, ...] = HOST_PROMOTION_REQUIRED_GATE_CLASSES
REQUIRED_SOURCE_RECORD_TYPES: tuple[str, ...] = HOST_PROMOTION_REQUIRED_SOURCE_RECORD_TYPES
REQUIRED_BLOCKER_FAILURE_CLASSES: tuple[str, ...] = HOST_PROMOTION_FAIL_CLOSED_BLOCKER_CLASSES
REQUIRED_REVIEWED_SOURCE_FIELDS: tuple[str, ...] = (
    "object_identity",
    "debug_identity",
    "package_install_identity",
    "runtime_load_link_proof",
)
EXPECTED_REVIEWED_SOURCE_RECORD_FIELDS: dict[str, str] = {
    "object_identity": "object_identity_record_id",
    "debug_identity": "debug_identity_record_id",
    "package_install_identity": "package_install_identity_record_id",
    "runtime_load_link_proof": "runtime_load_link_proof_record_id",
}
EXPECTED_REVIEWED_SOURCE_RECORD_TYPES: dict[str, str] = {
    "object_identity": "object_identity",
    "debug_identity": "debug_identity",
    "package_install_identity": "package_install_identity",
    "runtime_load_link_proof": "runtime_load_link_proof",
}
EXPECTED_PLATFORM_CONTRACTS = {
    contract.platform_id: contract
    for contract in HOST_PROMOTION_PLATFORM_CONTRACTS
}
EXPECTED_OBJECT_DEBUG_IDENTITY: dict[str, dict[str, str]] = {
    contract.platform_id: {
        "object_format": contract.artifact_identity.object_format,
        "debug_format": contract.artifact_identity.debug_format,
        "target_triple": contract.artifact_identity.target_triple,
        "arch": contract.artifact_identity.arch,
    }
    for contract in HOST_PROMOTION_PLATFORM_CONTRACTS
}
EXPECTED_RUNTIME_LIBRARY_NAMES: dict[str, tuple[str, ...]] = {
    contract.platform_id: contract.artifact_identity.runtime_library_names
    for contract in HOST_PROMOTION_PLATFORM_CONTRACTS
}


def _require_dict(payload: dict[str, Any], field_name: str, owner: str) -> dict[str, Any]:
    value = payload.get(field_name)
    expect(isinstance(value, dict), f"{owner} missing object field {field_name}")
    return value


def _require_list(payload: dict[str, Any], field_name: str, owner: str) -> list[Any]:
    value = payload.get(field_name)
    expect(isinstance(value, list), f"{owner} missing list field {field_name}")
    return value


def _require_nonempty_list(payload: dict[str, Any], field_name: str, owner: str) -> list[Any]:
    value = _require_list(payload, field_name, owner)
    expect(value, f"{owner} field {field_name} must not be empty")
    return value


def _require_set_contains(
    actual_values: Iterable[Any],
    required_values: Iterable[str],
    *,
    owner: str,
    description: str,
) -> set[str]:
    actual = {str(value) for value in actual_values}
    required = set(required_values)
    missing = sorted(required - actual)
    expect(not missing, f"{owner} missing {description}: {', '.join(missing)}")
    return actual


def _records_by_field(rows: list[Any], field_name: str, owner: str) -> dict[str, dict[str, Any]]:
    records: dict[str, dict[str, Any]] = {}
    for row in rows:
        expect(isinstance(row, dict), f"{owner} entries must be objects")
        row_id = str(row.get(field_name, ""))
        expect(row_id, f"{owner} entry missing {field_name}")
        expect(row_id not in records, f"{owner} duplicate {field_name}: {row_id}")
        records[row_id] = row
    return records


def _require_fields(payload: dict[str, Any], field_names: Iterable[str], owner: str) -> None:
    for field_name in field_names:
        expect(field_name in payload, f"{owner} missing required field {field_name}")


def _require_tmp_reports_path(path: Any, owner: str) -> str:
    value = str(path)
    expect(value.startswith("tmp/reports/"), f"{owner} must write under tmp/reports")
    return value


def _validate_policy(payload: dict[str, Any]) -> None:
    expect(
        payload.get("contract_id") == HOST_PROMOTION_EVIDENCE_CONTRACT_ID,
        "host promotion evidence contract_id drifted",
    )
    expect(
        payload.get("source_path") == HOST_PROMOTION_EVIDENCE_CONTRACT_RELATIVE_PATH,
        "host promotion evidence source_path drifted",
    )
    policy = _require_dict(payload, "promotion_policy", "host promotion evidence contract")
    expect(
        policy.get("support_promotion_allowed") is False,
        "host promotion policy allowed support promotion",
    )
    expect(
        policy.get("source_truth_required") is True,
        "host promotion policy must require reviewed source truth",
    )
    expect(
        policy.get("generated_evidence_required") is True,
        "host promotion policy must require hosted evidence",
    )
    expect(
        policy.get("generated_evidence_support_truth") is False,
        "generated evidence became support truth",
    )
    expect(
        policy.get("native_execution_required") is True,
        "native execution gate is not required",
    )
    expect(
        policy.get("package_install_required") is True,
        "package/install gate is not required",
    )
    expect(
        policy.get("prose_only_platform_support_claims_allowed") is False,
        "prose-only platform support claims were allowed",
    )
    expect(
        policy.get("sanitizer_variants_promote_platform_support") is False,
        "sanitizer variants promoted platform support",
    )
    for field_name, expected_value in HOST_PROMOTION_POLICY.items():
        expect(
            policy.get(field_name) == expected_value,
            f"host promotion policy field {field_name} drifted",
        )
    source_record_types = _require_set_contains(
        _require_list(payload, "required_source_record_types", "host promotion evidence contract"),
        REQUIRED_SOURCE_RECORD_TYPES,
        owner="host promotion evidence contract",
        description="required source record types",
    )
    _require_set_contains(
        source_record_types,
        EXPECTED_REVIEWED_SOURCE_RECORD_TYPES.values(),
        owner="host promotion evidence contract",
        description="reviewed source promotion record types",
    )
    _require_set_contains(
        _require_list(payload, "required_gate_classes", "host promotion evidence contract"),
        REQUIRED_GATE_CLASSES,
        owner="host promotion evidence contract",
        description="required gate classes",
    )


def _validate_evidence_classes(payload: dict[str, Any]) -> None:
    classes = _records_by_field(
        _require_list(payload, "evidence_classes", "host promotion evidence contract"),
        "class_id",
        "host promotion evidence classes",
    )
    for evidence_class in HOST_PROMOTION_EVIDENCE_CLASSES:
        class_id = evidence_class.class_id
        expect(class_id in classes, f"host promotion evidence class missing {class_id}")
        expected = evidence_class.as_json()
        actual = classes[class_id]
        for field_name in ("claim_weight", "support_truth", "promotion_result"):
            expect(
                actual.get(field_name) == expected[field_name],
                f"{class_id} field {field_name} drifted",
            )
        if evidence_class.required is not None:
            expect(
                actual.get("required") is evidence_class.required,
                f"{class_id} required flag drifted",
            )
        _require_set_contains(
            actual.get("required_fields", []),
            expected["required_fields"],
            owner=class_id,
            description="required fields",
        )


def _validate_reviewed_source_fields(payload: dict[str, Any]) -> None:
    fields = _records_by_field(
        _require_list(payload, "reviewed_source_fields", "host promotion evidence contract"),
        "field_id",
        "host promotion reviewed source fields",
    )
    _require_set_contains(
        fields,
        REQUIRED_REVIEWED_SOURCE_FIELDS,
        owner="host promotion reviewed source fields",
        description="required reviewed source fields",
    )
    for field_id in REQUIRED_REVIEWED_SOURCE_FIELDS:
        field = fields[field_id]
        expect(
            field.get("required_record_id_field")
            == EXPECTED_REVIEWED_SOURCE_RECORD_FIELDS[field_id],
            f"{field_id} reviewed source record field drifted",
        )
        expect(
            field.get("reviewed_source_required") is True,
            f"{field_id} did not require reviewed source",
        )
        expect(
            field.get("generated_report_support_truth") is False,
            f"{field_id} allowed generated reports as support truth",
        )
        expect(
            field.get("promotion_allowed_from_generated_evidence") is False,
            f"{field_id} allowed generated evidence promotion",
        )
        expect(
            str(field.get("generated_report_path_suffix", "")),
            f"{field_id} missing generated report path suffix",
        )
        expect(
            field.get("failure_class") in REQUIRED_BLOCKER_FAILURE_CLASSES,
            f"{field_id} failure class is not a fail-closed blocker",
        )
        expect(
            str(field.get("required_behavior", "")).startswith("fail-closed"),
            f"{field_id} required behavior does not fail closed",
        )


def _validate_common_blockers(payload: dict[str, Any]) -> set[str]:
    blockers = _records_by_field(
        _require_list(payload, "common_fail_closed_blockers", "host promotion evidence contract"),
        "failure_class",
        "common fail-closed blockers",
    )
    _require_set_contains(
        blockers,
        REQUIRED_BLOCKER_FAILURE_CLASSES,
        owner="common fail-closed blockers",
        description="required blocker failure classes",
    )
    for failure_class, blocker in blockers.items():
        required_behavior = str(blocker.get("required_behavior", ""))
        expect(required_behavior.startswith("fail-closed"), f"{failure_class} does not fail closed")
        _require_nonempty_list(blocker, "blocks_surfaces", failure_class)
        expect(str(blocker.get("source_owner", "")), f"{failure_class} missing source_owner")
    return set(blockers)


def _validate_generated_evidence(platform_id: str, platform: dict[str, Any]) -> None:
    expected_contract = EXPECTED_PLATFORM_CONTRACTS[platform_id]
    generated = _require_dict(platform, "generated_hosted_evidence", platform_id)
    _require_fields(
        generated,
        (
            "required",
            "runner_label",
            "workflow_path",
            "generated_report_root",
            "generated_report_contract_id",
            "generated_report_paths",
            "candidate_evidence_record_id",
            "ingestion_summary_path",
        ),
        f"{platform_id} generated evidence",
    )
    expect(generated.get("required") is True, f"{platform_id} hosted evidence is not required")
    expect(
        generated.get("runner_label") == expected_contract.runner_label,
        f"{platform_id} generated evidence runner label drifted",
    )
    expect(
        generated.get("support_truth") is False,
        f"{platform_id} generated evidence promoted support",
    )
    expect(
        generated.get("promotion_result") == "refuse-source-truth-promotion",
        f"{platform_id} generated evidence promotion result drifted",
    )
    expect(
        str(generated.get("candidate_evidence_record_id", "")).endswith(
            f"{platform_id}.generated-host-run"
        ),
        f"{platform_id} generated evidence record id drifted",
    )
    report_root = _require_tmp_reports_path(
        generated.get("generated_report_root"),
        f"{platform_id} generated report root",
    )
    expect(
        report_root.startswith(f"{HOST_PROMOTION_GENERATED_REPORT_ROOT}/"),
        f"{platform_id} generated report root left host evidence report root",
    )
    expect(
        report_root == generated_report_root_for_platform(platform_id),
        f"{platform_id} generated report root drifted",
    )
    report_paths = [
        _require_tmp_reports_path(path, f"{platform_id} generated report path")
        for path in _require_nonempty_list(
            generated,
            "generated_report_paths",
            f"{platform_id} generated evidence",
        )
    ]
    expect(
        report_paths == generated_report_paths_for_platform(platform_id),
        f"{platform_id} generated report paths drifted",
    )
    expect(
        str(generated.get("ingestion_summary_path")) == f"{report_root}/ingestion-summary.json",
        f"{platform_id} ingestion summary path drifted",
    )


def _validate_reviewed_source_truth(platform_id: str, platform: dict[str, Any]) -> None:
    expected_contract = EXPECTED_PLATFORM_CONTRACTS[platform_id]
    reviewed = _require_dict(platform, "reviewed_source_truth_evidence", platform_id)
    _require_fields(
        reviewed,
        (
            "required",
            "source_review_record_id",
            "reviewed_source_paths",
            "support_row_id",
            "package_variant_row_id",
            "required_record_ids",
            "missing_record_classes",
            "support_truth",
        ),
        f"{platform_id} reviewed source truth",
    )
    expect(reviewed.get("required") is True, f"{platform_id} reviewed source truth is not required")
    expect(
        reviewed.get("support_row_id") == expected_contract.support_row_id,
        f"{platform_id} reviewed source truth support row drifted",
    )
    expect(
        reviewed.get("package_variant_row_id") == expected_contract.package_variant_row_id,
        f"{platform_id} reviewed source truth package row drifted",
    )
    expect(
        reviewed.get("support_truth") is False,
        f"{platform_id} reviewed source truth prematurely supports",
    )
    required_record_ids = _require_dict(
        reviewed,
        "required_record_ids",
        f"{platform_id} reviewed source truth",
    )
    for field_name in (
        "host_identity_record_id",
        "toolchain_probe_record_id",
        "package_root_record_id",
        "install_receipt_record_id",
        "native_execution_record_id",
        *EXPECTED_REVIEWED_SOURCE_RECORD_FIELDS.values(),
    ):
        expect(str(required_record_ids.get(field_name, "")), f"{platform_id} missing {field_name}")
    _require_set_contains(
        reviewed.get("required_reviewed_source_fields", []),
        REQUIRED_REVIEWED_SOURCE_FIELDS,
        owner=platform_id,
        description="reviewed source fields",
    )
    field_status = _require_dict(
        reviewed,
        "reviewed_source_field_status",
        f"{platform_id} reviewed source truth",
    )
    for field_id in REQUIRED_REVIEWED_SOURCE_FIELDS:
        expect(
            field_status.get(field_id) == "missing-reviewed-source",
            f"{platform_id} reviewed source field {field_id} did not remain missing",
        )
    expect(
        reviewed.get("reviewed_source_support_ready") is False,
        f"{platform_id} reviewed source truth became support-ready",
    )
    expect(
        reviewed.get("generated_evidence_support_truth") is False,
        f"{platform_id} generated evidence became reviewed source truth",
    )
    _require_set_contains(
        reviewed.get("missing_record_classes", []),
        REQUIRED_GATE_CLASSES,
        owner=platform_id,
        description="build/package/install/execution gates",
    )


def _validate_package_install_execution(platform_id: str, platform: dict[str, Any]) -> None:
    expected_contract = EXPECTED_PLATFORM_CONTRACTS[platform_id]
    evidence = _require_dict(platform, "package_install_native_execution_evidence", platform_id)
    _require_fields(
        evidence,
        (
            "package_variant_row_id",
            "package_id",
            "package_root_layout",
            "package_manifest_path",
            "install_receipt_path",
            "execution_summary_path",
            "native_smoke_summary_path",
            "runtime_library_names",
            "loader_policy",
            "host_identity_record_id",
            "support_truth",
        ),
        f"{platform_id} package/install/native execution",
    )
    expect(
        evidence.get("package_variant_row_id") == expected_contract.package_variant_row_id,
        f"{platform_id} package/install package row drifted",
    )
    expect(
        evidence.get("package_id") == expected_contract.package_id,
        f"{platform_id} package/install package id drifted",
    )
    _require_nonempty_list(
        evidence,
        "package_root_layout",
        f"{platform_id} package/install/native execution",
    )
    for field_name in (
        "package_manifest_path",
        "install_receipt_path",
        "execution_summary_path",
        "native_smoke_summary_path",
    ):
        _require_tmp_reports_path(
            evidence.get(field_name),
            f"{platform_id} package/install/native execution {field_name}",
        )
    expect(
        evidence.get("install_receipt_required") is True,
        f"{platform_id} install receipt is not mandatory",
    )
    expect(
        evidence.get("native_execution_required") is True,
        f"{platform_id} native execution is not mandatory",
    )
    expect(
        evidence.get("support_truth") is False,
        f"{platform_id} package/install/native execution evidence prematurely supports",
    )
    expect(
        tuple(str(name) for name in evidence.get("runtime_library_names", []))
        == EXPECTED_RUNTIME_LIBRARY_NAMES[platform_id],
        f"{platform_id} package/install runtime library identity drifted",
    )


def _validate_object_debug_identity(platform_id: str, platform: dict[str, Any]) -> None:
    identity = _require_dict(platform, "object_debug_identity", platform_id)
    _require_fields(
        identity,
        (
            "object_format",
            "debug_format",
            "target_triple",
            "arch",
            "object_identity_report_path",
            "debug_identity_report_path",
            "llvm_toolchain_record_id",
        ),
        f"{platform_id} object/debug identity",
    )
    expected = EXPECTED_OBJECT_DEBUG_IDENTITY[platform_id]
    for field_name, expected_value in expected.items():
        expect(
            identity.get(field_name) == expected_value,
            f"{platform_id} object/debug identity {field_name} drifted from {expected_value}",
        )
    _require_tmp_reports_path(
        identity.get("object_identity_report_path"),
        f"{platform_id} object identity report path",
    )
    _require_tmp_reports_path(
        identity.get("debug_identity_report_path"),
        f"{platform_id} debug identity report path",
    )
    expect(
        identity.get("support_truth_without_package_install_execution") is False,
        f"{platform_id} object/debug identity promoted without package install execution",
    )


def _validate_runtime_link_load_identity(platform_id: str, platform: dict[str, Any]) -> None:
    identity = _require_dict(platform, "runtime_link_load_identity", platform_id)
    _require_fields(
        identity,
        (
            "runtime_library_names",
            "runtime_library_manifest_path",
            "linker_flags",
            "loader_policy",
            "load_probe_path",
            "load_probe_exit_code",
            "resolved_runtime_paths",
        ),
        f"{platform_id} runtime link/load identity",
    )
    expect(
        tuple(str(name) for name in identity.get("runtime_library_names", []))
        == EXPECTED_RUNTIME_LIBRARY_NAMES[platform_id],
        f"{platform_id} runtime library identity drifted",
    )
    _require_tmp_reports_path(
        identity.get("runtime_library_manifest_path"),
        f"{platform_id} runtime library manifest path",
    )
    _require_tmp_reports_path(
        identity.get("load_probe_path"),
        f"{platform_id} runtime load probe path",
    )
    expect(str(identity.get("loader_policy", "")), f"{platform_id} missing loader policy")
    expect(
        identity.get("runtime_load_probe_required") is True,
        f"{platform_id} runtime load probe is not mandatory",
    )
    expect(
        identity.get("runtime_load_failure_behavior")
        == "fail-closed-before-native-execution-claim",
        f"{platform_id} runtime load failure behavior drifted",
    )
    expect(
        identity.get("support_truth") is False,
        f"{platform_id} runtime link/load identity promoted support",
    )


def _validate_platform(
    platform_id: str,
    platform: dict[str, Any],
    common_blockers: set[str],
) -> None:
    expected_contract = EXPECTED_PLATFORM_CONTRACTS[platform_id]
    expect(
        platform.get("issue_ref") == expected_contract.issue_ref,
        f"{platform_id} issue ref drifted",
    )
    expect(
        platform.get("support_row_id") == expected_contract.support_row_id,
        f"{platform_id} support row drifted",
    )
    expect(
        platform.get("runner_label") == expected_contract.runner_label,
        f"{platform_id} runner label drifted",
    )
    expect(
        platform.get("package_variant_row_id") == expected_contract.package_variant_row_id,
        f"{platform_id} package variant row drifted",
    )
    expect(
        platform.get("generated_evidence_record_id")
        == expected_contract.generated_evidence_record_id,
        f"{platform_id} generated evidence record drifted",
    )
    expect(
        platform.get("support_state") == "unsupported",
        f"{platform_id} support state is not unsupported",
    )
    expect(platform.get("promotion_allowed") is False, f"{platform_id} promotion was allowed")
    expect(
        platform.get("claim_class") == "fail-closed",
        f"{platform_id} claim class is not fail-closed",
    )
    support_claim_policy = _require_dict(platform, "support_claim_policy", platform_id)
    expect(
        support_claim_policy.get("prose_only_platform_support_claims_allowed") is False,
        f"{platform_id} allowed prose-only support claims",
    )
    expect(
        support_claim_policy.get("source_truth_required") is True,
        f"{platform_id} support claim did not require source truth",
    )
    expect(
        support_claim_policy.get("generated_hosted_evidence_required") is True,
        f"{platform_id} support claim did not require hosted evidence",
    )
    expect(
        support_claim_policy.get("required_output_root")
        == generated_report_root_for_platform(platform_id),
        f"{platform_id} support claim output root drifted",
    )
    _validate_generated_evidence(platform_id, platform)
    _validate_reviewed_source_truth(platform_id, platform)
    _validate_package_install_execution(platform_id, platform)
    _validate_object_debug_identity(platform_id, platform)
    _validate_runtime_link_load_identity(platform_id, platform)

    llvm_policy = _require_dict(platform, "llvm_version_policy", platform_id)
    expect(
        llvm_policy.get("unsupported_version_behavior") == "fail-closed-no-range-claim",
        f"{platform_id} unsupported LLVM version behavior drifted",
    )
    sanitizer = _require_dict(platform, "sanitizer_variant_boundary", platform_id)
    expect(
        sanitizer.get("release_runtime_allows_sanitizer_artifacts") is False,
        f"{platform_id} release runtime allowed sanitizer artifacts",
    )
    expect(
        sanitizer.get("sanitizer_variant_leakage_behavior") == "fail-closed-before-package-install",
        f"{platform_id} sanitizer leakage behavior drifted",
    )
    platform_blockers = _require_set_contains(
        _require_list(platform, "promotion_blockers", platform_id),
        REQUIRED_BLOCKER_FAILURE_CLASSES,
        owner=platform_id,
        description="promotion blockers",
    )
    missing_common = sorted(set(REQUIRED_BLOCKER_FAILURE_CLASSES) - common_blockers)
    expect(
        not missing_common,
        f"{platform_id} blockers absent from common blocker catalog: {', '.join(missing_common)}",
    )
    expect(
        set(REQUIRED_BLOCKER_FAILURE_CLASSES) <= platform_blockers,
        f"{platform_id} did not include all required blocker cases",
    )


def _validate_future_checker_contract(payload: dict[str, Any]) -> None:
    expected_payload = build_host_promotion_contract_payload()
    expect(
        payload.get("upstream_source_paths") == expected_payload["upstream_source_paths"],
        "host promotion upstream source paths drifted from model",
    )
    expect(
        payload.get("unsupported_platform_ids") == expected_payload["unsupported_platform_ids"],
        "host promotion unsupported platform ids drifted from model",
    )
    future_contract = _require_dict(
        payload,
        "future_checker_contract",
        "host promotion future checker contract",
    )
    expect(
        future_contract.get("must_require_platform_rows") == list(REQUIRED_PLATFORM_IDS),
        "future checker platform row requirement drifted",
    )
    expect(
        future_contract.get("must_reject_prose_only_platform_support_claims") is True,
        "future checker stopped rejecting prose-only platform support claims",
    )
    expect(
        future_contract.get("must_write_summary_under")
        == HOST_PROMOTION_EVIDENCE_SUMMARY_RELATIVE_PATH,
        "future checker summary output path drifted",
    )
    expect(
        future_contract.get("must_register_public_command")
        == HOST_PROMOTION_EVIDENCE_ACTION,
        "future checker public command drifted",
    )
    expect(
        future_contract.get("must_be_child_of_public_action")
        == HOST_PROMOTION_VALIDATE_PLATFORM_HARDENING_ACTION,
        "future checker integration parent action drifted",
    )
    expect(
        future_contract.get("must_require_reviewed_source_fields")
        == list(REQUIRED_REVIEWED_SOURCE_FIELDS),
        "future checker reviewed source field requirement drifted",
    )
    _require_tmp_reports_path(
        future_contract.get("must_write_summary_under"),
        "host promotion future checker summary output path",
    )


def validate_host_promotion_evidence_contract(payload: dict[str, Any]) -> dict[str, Any]:
    _validate_policy(payload)
    _validate_evidence_classes(payload)
    _validate_reviewed_source_fields(payload)
    _validate_future_checker_contract(payload)
    common_blockers = _validate_common_blockers(payload)
    platforms = _records_by_field(
        _require_list(payload, "platforms", "host promotion evidence contract"),
        "platform_id",
        "host promotion platforms",
    )
    _require_set_contains(
        platforms,
        REQUIRED_PLATFORM_IDS,
        owner="host promotion platforms",
        description="required platform rows",
    )

    for platform_id in REQUIRED_PLATFORM_IDS:
        _validate_platform(platform_id, platforms[platform_id], common_blockers)

    summary = {
        "contract_id": HOST_PROMOTION_EVIDENCE_SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "source_contract_id": payload["contract_id"],
        "source_path": repo_rel(CONTRACT_PATH),
        "platform_ids": list(REQUIRED_PLATFORM_IDS),
        "required_gate_classes": list(REQUIRED_GATE_CLASSES),
        "required_source_record_types": list(REQUIRED_SOURCE_RECORD_TYPES),
        "required_blocker_failure_classes": list(REQUIRED_BLOCKER_FAILURE_CLASSES),
        "required_hosted_evidence": True,
        "prose_only_platform_support_claims_allowed": False,
        "summary_path": HOST_PROMOTION_EVIDENCE_SUMMARY_RELATIVE_PATH,
        "promotion_allowed": False,
    }
    return summary


def load_host_promotion_evidence_contract() -> dict[str, Any]:
    return load_json_object(CONTRACT_PATH)


def main() -> int:
    payload = load_host_promotion_evidence_contract()
    summary = validate_host_promotion_evidence_contract(payload)
    write_json(SUMMARY_PATH, summary)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-platform-host-promotion-evidence: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
