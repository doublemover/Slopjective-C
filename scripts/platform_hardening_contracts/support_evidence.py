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
    HOSTED_RUNNER_CAPABILITY_SUMMARIES_PATH,
    HOSTED_RUNNER_CAPABILITY_SUMMARIES_SCHEMA_PATH,
    PLATFORM_EXPANSION_CLAIM_CONTRACT_PATH,
    PLATFORM_EXPANSION_CLAIM_CONTRACT_SCHEMA_PATH,
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
    "mixed-toolchain-root",
    "unsupported-toolchain-version",
)
EXPECTED_UNSUPPORTED_PLATFORM_ISSUES: dict[str, int] = {
    "linux-x64": 8228,
    "darwin-arm64": 8229,
}
EXPECTED_SANITIZER_ISSUES: dict[str, int] = {
    "address": 8230,
    "undefined": 8231,
}
RELEASE_RUNTIME_PACKAGE_IDS: tuple[str, ...] = (
    "org.objc3c.runtime:objc3c-runtime-release",
    "org.objc3c.runtime:objc3c-runtime-linux-x64-release",
    "org.objc3c.runtime:objc3c-runtime-darwin-arm64-release",
)
EXPECTED_SANITIZER_DETECTION_RECORDS: dict[str, set[str]] = {
    "address": {
        "objc3c.sanitizer.address.heap-use-after-free",
        "objc3c.sanitizer.address.container-overflow",
    },
    "undefined": {
        "objc3c.sanitizer.undefined.signed-integer-overflow",
        "objc3c.sanitizer.undefined.invalid-shift",
    },
}
REQUIRED_SANITIZER_METADATA_FIELDS: dict[str, set[str]] = {
    "address": {
        "target_platform_id",
        "sanitizer",
        "runtime_library_ids",
        "compiler_flags",
        "linker_flags",
        "environment",
        "release_runtime_package_ids",
        "release_runtime_mixing_allowed",
        "expected_detection_records",
        "unsupported_host_diagnostics",
    },
    "undefined": {
        "target_platform_id",
        "sanitizer",
        "runtime_library_ids",
        "compiler_flags",
        "linker_flags",
        "trap_or_recover_mode",
        "release_runtime_package_ids",
        "release_runtime_mixing_allowed",
        "expected_detection_records",
        "unsupported_host_diagnostics",
    },
}
REQUIRED_UNSUPPORTED_SANITIZER_DIAGNOSTIC_BLOCKS: set[str] = {
    "package",
    "install",
    "execution",
    "publication",
}
FORBIDDEN_TOOLCHAIN_RANGE_CLAIM_TERMS: tuple[str, ...] = (
    "all",
    "best-effort",
    "best effort",
    "compat",
    "fallback",
)
HOST_EVIDENCE_CONTRACT_ID = "objc3c.platform.host-evidence.promotion.v1"
REQUIRED_HOST_EVIDENCE_SECTIONS: tuple[str, ...] = (
    "host_identity_records",
    "toolchain_probe_records",
    "package_root_evidence_records",
    "native_execution_evidence_records",
    "negative_host_toolchain_cases",
)
REQUIRED_NEGATIVE_HOST_TOOLCHAIN_CASES: tuple[str, ...] = (
    "objc3c.negative.host.linux-x64.no-native-execution",
    "objc3c.negative.host.darwin-arm64.no-native-execution",
    "objc3c.negative.toolchain.missing-llc",
    "objc3c.negative.toolchain.mixed-root",
    "objc3c.negative.toolchain.mismatched-version",
    "objc3c.negative.toolchain.unsupported-version",
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


def load_hosted_runner_capability_summaries() -> dict[str, Any]:
    schema = load_json_object(HOSTED_RUNNER_CAPABILITY_SUMMARIES_SCHEMA_PATH)
    payload = load_json_object(HOSTED_RUNNER_CAPABILITY_SUMMARIES_PATH)
    validate_json_schema(
        payload,
        schema,
        label=repo_rel(HOSTED_RUNNER_CAPABILITY_SUMMARIES_PATH),
    )
    return payload


def load_platform_expansion_claim_contract() -> dict[str, Any]:
    schema = load_json_object(PLATFORM_EXPANSION_CLAIM_CONTRACT_SCHEMA_PATH)
    payload = load_json_object(PLATFORM_EXPANSION_CLAIM_CONTRACT_PATH)
    validate_json_schema(
        payload,
        schema,
        label=repo_rel(PLATFORM_EXPANSION_CLAIM_CONTRACT_PATH),
    )
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


def _records_by_field(owner_id: str, rows: Any, field_name: str) -> dict[str, dict[str, Any]]:
    expect(isinstance(rows, list), f"{owner_id} must be a list")
    by_id: dict[str, dict[str, Any]] = {}
    for row in rows:
        expect(isinstance(row, dict), f"{owner_id} entries must be objects")
        row_id = str(row.get(field_name, ""))
        expect(row_id, f"{owner_id} entry missing {field_name}")
        expect(row_id not in by_id, f"duplicate {owner_id} {field_name}: {row_id}")
        by_id[row_id] = row
    return by_id


def _evidence_ids_are_supporting(
    evidence_ids: Iterable[Any],
    *,
    records_by_id: dict[str, dict[str, Any]],
    platform_id: str,
    allowed_classes: set[str],
    owner_id: str,
) -> None:
    for raw_evidence_id in evidence_ids:
        evidence_id = str(raw_evidence_id)
        expect(evidence_id in records_by_id, f"{owner_id} missing evidence record {evidence_id}")
        record = records_by_id[evidence_id]
        expect(record.get("claim_weight") == "supporting", f"{owner_id} evidence {evidence_id} is not supporting")
        evidence_class = str(record.get("evidence_class", ""))
        expect(evidence_class in allowed_classes, f"{owner_id} evidence {evidence_id} has unsupported class {evidence_class}")
        expect(platform_id in record.get("supports_platform_ids", []), f"{owner_id} evidence {evidence_id} does not support {platform_id}")


def _validate_host_identity_records(
    payload: dict[str, Any],
    *,
    records_by_id: dict[str, dict[str, Any]],
    boundary_supported_platform_ids: set[str],
) -> dict[str, dict[str, Any]]:
    support_rows = {
        str(row.get("platform_id")): row
        for row in payload.get("support_rows", [])
        if isinstance(row, dict) and row.get("platform_id")
    }
    identities = _records_by_field(
        "host_identity_records",
        payload.get("host_identity_records"),
        "record_id",
    )
    identity_platforms = {str(row.get("platform_id", "")) for row in identities.values()}
    expect(
        set(support_rows) <= identity_platforms,
        "host identity records did not cover every platform support row",
    )
    for record_id, identity in identities.items():
        platform_id = str(identity.get("platform_id", ""))
        row = support_rows.get(platform_id)
        expect(row is not None, f"{record_id} host identity used unknown platform {platform_id}")
        expect(identity.get("host_os") == row.get("host_os"), f"{record_id} host_os drifted from support row")
        expect(identity.get("host_arch") == row.get("host_arch"), f"{record_id} host_arch drifted from support row")
        expect(identity.get("host_triples") == row.get("host_triples"), f"{record_id} host triples drifted from support row")
        expect(str(identity.get("host_system", "")), f"{record_id} missing host_system")
        expect(str(identity.get("host_machine", "")), f"{record_id} missing host_machine")
        expect(identity.get("support_row_id") == row.get("row_id"), f"{record_id} support_row_id drifted")
        expect(identity.get("unsupported_behavior") == "fail-closed", f"{record_id} host identity does not fail closed")
        evidence_ids = [str(evidence_id) for evidence_id in identity.get("evidence_ids", [])]
        expect(evidence_ids, f"{record_id} missing evidence_ids")
        if platform_id in boundary_supported_platform_ids:
            expect(identity.get("claim_state") == "evidence-bound", f"{record_id} supported host identity is not evidence-bound")
            expect(identity.get("promotion_allowed") is True, f"{record_id} did not allow supported promotion")
            expect(identity.get("platform_ids") == [platform_id], f"{record_id} supported host identity platform_ids drifted")
            _evidence_ids_are_supporting(
                evidence_ids,
                records_by_id=records_by_id,
                platform_id=platform_id,
                allowed_classes={"build", "package", "install", "execution", "toolchain", "hosted_ci", "clean_room"},
                owner_id=record_id,
            )
            continue
        expect(identity.get("claim_state") == "fail-closed", f"{record_id} unsupported host identity must fail closed")
        expect(identity.get("promotion_allowed") is False, f"{record_id} unsupported host identity allowed promotion")
        expect(not identity.get("platform_ids"), f"{record_id} unsupported host identity widened support")
        for evidence_id in evidence_ids:
            expect(evidence_id in records_by_id, f"{record_id} missing policy evidence record {evidence_id}")
            _policy_record_is_fail_closed(records_by_id[evidence_id])
    return identities


def _validate_toolchain_probe_records(
    payload: dict[str, Any],
    *,
    records_by_id: dict[str, dict[str, Any]],
    boundary_supported_platform_ids: set[str],
    required_toolchain_components: set[str],
) -> dict[str, dict[str, Any]]:
    probes = _records_by_field(
        "toolchain_probe_records",
        payload.get("toolchain_probe_records"),
        "record_id",
    )
    support_platform_ids = {
        str(row.get("platform_id", ""))
        for row in payload.get("support_rows", [])
        if isinstance(row, dict) and row.get("platform_id")
    }
    seen_platform_ids = {str(record.get("platform_id", "")) for record in probes.values()}
    expect(
        support_platform_ids <= seen_platform_ids,
        "toolchain probe records did not cover every platform support row",
    )
    for record_id, probe in probes.items():
        platform_id = str(probe.get("platform_id", ""))
        expect(platform_id in support_platform_ids, f"{record_id} toolchain probe used unknown platform")
        expect(
            set(str(component) for component in probe.get("required_components", []))
            == required_toolchain_components,
            f"{record_id} required toolchain components drifted",
        )
        component_probes = probe.get("component_probes", [])
        expect(isinstance(component_probes, list) and component_probes, f"{record_id} missing component probes")
        seen_components = {str(component.get("component", "")) for component in component_probes if isinstance(component, dict)}
        expect(seen_components == required_toolchain_components, f"{record_id} component probes drifted")
        for component in component_probes:
            expect(isinstance(component, dict), f"{record_id} component probe must be an object")
            component_name = str(component.get("component", ""))
            expect(str(component.get("unsupported_behavior", "")) == "fail-closed-no-range-claim", f"{record_id} {component_name} does not fail closed")
            expect(component.get("required_probe_fields"), f"{record_id} {component_name} missing required probe fields")
        evidence_ids = [str(evidence_id) for evidence_id in probe.get("evidence_ids", [])]
        expect(evidence_ids, f"{record_id} missing evidence_ids")
        if platform_id in boundary_supported_platform_ids:
            expect(probe.get("claim_state") == "evidence-bound", f"{record_id} supported toolchain probe is not evidence-bound")
            expect(probe.get("promotion_allowed") is True, f"{record_id} did not allow supported promotion")
            expect(probe.get("platform_ids") == [platform_id], f"{record_id} supported toolchain probe platform_ids drifted")
            expect(not probe.get("required_missing_probe_classes"), f"{record_id} listed missing probe classes")
            _evidence_ids_are_supporting(
                evidence_ids,
                records_by_id=records_by_id,
                platform_id=platform_id,
                allowed_classes={"toolchain", "build", "package", "install", "execution", "clean_room"},
                owner_id=record_id,
            )
            continue
        expect(probe.get("claim_state") == "fail-closed", f"{record_id} unsupported toolchain probe must fail closed")
        expect(probe.get("promotion_allowed") is False, f"{record_id} unsupported toolchain probe allowed promotion")
        expect(not probe.get("platform_ids"), f"{record_id} unsupported toolchain probe widened support")
        expect(probe.get("required_missing_probe_classes"), f"{record_id} missing fail-closed probe blockers")
        for evidence_id in evidence_ids:
            expect(evidence_id in records_by_id, f"{record_id} missing policy evidence record {evidence_id}")
            _policy_record_is_fail_closed(records_by_id[evidence_id])
    return probes


def _validate_package_root_evidence_records(
    payload: dict[str, Any],
    *,
    records_by_id: dict[str, dict[str, Any]],
    package_variant_rows: dict[str, dict[str, Any]],
    boundary_supported_platform_ids: set[str],
) -> dict[str, dict[str, Any]]:
    roots = _records_by_field(
        "package_root_evidence_records",
        payload.get("package_root_evidence_records"),
        "record_id",
    )
    package_rows_with_roots = {
        row_id
        for row_id, row in package_variant_rows.items()
        if str(row.get("variant_kind", "")) == "release-runtime"
    }
    seen_package_rows = {str(root.get("package_variant_row_id", "")) for root in roots.values()}
    expect(
        package_rows_with_roots <= seen_package_rows,
        "package root evidence records did not cover release runtime package rows",
    )
    for record_id, root in roots.items():
        package_row_id = str(root.get("package_variant_row_id", ""))
        package_row = package_variant_rows.get(package_row_id)
        expect(package_row is not None, f"{record_id} referenced missing package row {package_row_id}")
        artifact = package_row.get("artifact_identity_contract", {})
        expect(root.get("target_platform_id") == package_row.get("target_platform_id"), f"{record_id} target platform drifted")
        expect(root.get("claim_state") == package_row.get("claim_state"), f"{record_id} claim_state drifted from package row")
        expect(root.get("platform_ids") == package_row.get("platform_ids"), f"{record_id} platform_ids drifted from package row")
        expect(root.get("object_format") == artifact.get("object_format"), f"{record_id} object format drifted from package row")
        expect(root.get("debug_format") == artifact.get("debug_format"), f"{record_id} debug format drifted from package row")
        expect(root.get("runtime_library_names") == artifact.get("runtime_library_names"), f"{record_id} runtime libraries drifted")
        expect(root.get("package_root_layout") == artifact.get("package_root_layout"), f"{record_id} package root layout drifted")
        expect(root.get("unsupported_behavior") == "fail-closed", f"{record_id} package root does not fail closed")
        evidence_ids = [str(evidence_id) for evidence_id in root.get("evidence_ids", [])]
        expect(evidence_ids, f"{record_id} missing evidence_ids")
        platform_id = str(root.get("target_platform_id", ""))
        if platform_id in boundary_supported_platform_ids:
            expect(root.get("promotion_allowed") is True, f"{record_id} supported package root did not allow promotion")
            _evidence_ids_are_supporting(
                evidence_ids,
                records_by_id=records_by_id,
                platform_id=platform_id,
                allowed_classes={"package", "install", "execution"},
                owner_id=record_id,
            )
            continue
        expect(root.get("promotion_allowed") is False, f"{record_id} unsupported package root allowed promotion")
        for evidence_id in evidence_ids:
            expect(evidence_id in records_by_id, f"{record_id} missing policy evidence record {evidence_id}")
            _policy_record_is_fail_closed(records_by_id[evidence_id])
    return roots


def _validate_native_execution_evidence_records(
    payload: dict[str, Any],
    *,
    records_by_id: dict[str, dict[str, Any]],
    host_identities: dict[str, dict[str, Any]],
    package_roots: dict[str, dict[str, Any]],
    package_variant_rows: dict[str, dict[str, Any]],
    boundary_supported_platform_ids: set[str],
) -> dict[str, dict[str, Any]]:
    execution_records = _records_by_field(
        "native_execution_evidence_records",
        payload.get("native_execution_evidence_records"),
        "record_id",
    )
    support_platform_ids = {
        str(row.get("platform_id", ""))
        for row in payload.get("support_rows", [])
        if isinstance(row, dict) and row.get("platform_id")
    }
    seen_platform_ids = {str(record.get("platform_id", "")) for record in execution_records.values()}
    expect(
        support_platform_ids <= seen_platform_ids,
        "native execution records did not cover every platform support row",
    )
    for record_id, execution in execution_records.items():
        platform_id = str(execution.get("platform_id", ""))
        package_row_id = str(execution.get("package_variant_row_id", ""))
        package_row = package_variant_rows.get(package_row_id)
        expect(package_row is not None, f"{record_id} referenced missing package row {package_row_id}")
        host_record_id = str(execution.get("host_identity_record_id", ""))
        package_root_record_id = str(execution.get("package_root_record_id", ""))
        expect(host_record_id in host_identities, f"{record_id} missing host identity {host_record_id}")
        expect(package_root_record_id in package_roots, f"{record_id} missing package root {package_root_record_id}")
        artifact = package_row.get("artifact_identity_contract", {})
        required_formats = execution.get("required_artifact_formats", {})
        expect(required_formats.get("object_format") == artifact.get("object_format"), f"{record_id} object format drifted")
        expect(required_formats.get("debug_format") == artifact.get("debug_format"), f"{record_id} debug format drifted")
        expect(execution.get("runtime_library_names") == artifact.get("runtime_library_names"), f"{record_id} runtime library names drifted")
        expect(execution.get("package_root_layout") == artifact.get("package_root_layout"), f"{record_id} package root layout drifted")
        expect(execution.get("native_execution_required") is True, f"{record_id} did not require native execution")
        expect(execution.get("unsupported_behavior") == "fail-closed", f"{record_id} native execution does not fail closed")
        evidence_ids = [str(evidence_id) for evidence_id in execution.get("execution_evidence_ids", [])]
        if platform_id in boundary_supported_platform_ids:
            expect(execution.get("claim_state") == "evidence-bound", f"{record_id} supported native execution is not evidence-bound")
            expect(execution.get("promotion_allowed") is True, f"{record_id} supported native execution did not allow promotion")
            expect(execution.get("platform_ids") == [platform_id], f"{record_id} supported native execution platform_ids drifted")
            expect(evidence_ids, f"{record_id} supported native execution missing evidence ids")
            _evidence_ids_are_supporting(
                evidence_ids,
                records_by_id=records_by_id,
                platform_id=platform_id,
                allowed_classes={"execution"},
                owner_id=record_id,
            )
            continue
        expect(execution.get("claim_state") == "missing-host-execution", f"{record_id} unsupported native execution must be missing-host-execution")
        expect(execution.get("promotion_allowed") is False, f"{record_id} unsupported native execution allowed promotion")
        expect(not execution.get("platform_ids"), f"{record_id} unsupported native execution widened support")
        expect(not evidence_ids, f"{record_id} unsupported native execution carried execution evidence")
        expect(
            execution.get("source_only_result") == "fail-closed-before-support-promotion",
            f"{record_id} source-only native execution did not fail closed",
        )
    return execution_records


def _validate_negative_host_toolchain_cases(
    payload: dict[str, Any],
    *,
    unsupported_host_policy: dict[str, Any],
) -> dict[str, dict[str, Any]]:
    cases = _records_by_field(
        "negative_host_toolchain_cases",
        payload.get("negative_host_toolchain_cases"),
        "case_id",
    )
    expect(
        set(REQUIRED_NEGATIVE_HOST_TOOLCHAIN_CASES) <= set(cases),
        "negative host/toolchain cases did not cover required fail-closed cases",
    )
    failure_ids = _unsupported_failure_ids(unsupported_host_policy) | {
        str(failure_class.get("failure_id"))
        for failure_class in unsupported_host_policy.get("hard_fail_classes", [])
        if isinstance(failure_class, dict) and failure_class.get("failure_id")
    }
    for case_id, case in cases.items():
        failure_class = str(case.get("failure_class", ""))
        expect(failure_class in failure_ids, f"{case_id} failure_class is not in unsupported host policy")
        expect(str(case.get("required_behavior", "")).startswith("fail-closed"), f"{case_id} does not fail closed")
        expect(case.get("promotion_allowed") is False, f"{case_id} allowed promotion")
        blocked_surfaces = {str(surface) for surface in case.get("blocks_surfaces", [])}
        expect(
            {"package", "execution", "publication"} <= blocked_surfaces,
            f"{case_id} did not block package execution and publication",
        )
        expect(str(case.get("source_owner", "")), f"{case_id} missing source_owner")
    return cases


def _validate_host_evidence_architecture(
    payload: dict[str, Any],
    *,
    records_by_id: dict[str, dict[str, Any]],
    package_variant_rows: dict[str, dict[str, Any]],
    boundary_supported_platform_ids: set[str],
    unsupported_host_policy: dict[str, Any],
    required_toolchain_components: set[str],
) -> dict[str, dict[str, dict[str, Any]]]:
    contract = payload.get("host_evidence_contract")
    expect(isinstance(contract, dict), "platform support evidence missing host_evidence_contract")
    expect(contract.get("contract_id") == HOST_EVIDENCE_CONTRACT_ID, "host evidence contract_id drifted")
    expect(contract.get("promotion_policy") == "real-host-execution-required", "host evidence promotion policy drifted")
    expect(contract.get("source_only_or_hosted_summary_result") == "fail-closed-no-support-promotion", "host evidence source-only behavior drifted")
    expect(contract.get("native_execution_required_for_support") is True, "host evidence did not require native execution")
    expect(
        set(str(platform_id) for platform_id in contract.get("supported_platform_ids", []))
        == boundary_supported_platform_ids,
        "host evidence supported platform boundary drifted",
    )
    expect(
        set(REQUIRED_HOST_EVIDENCE_SECTIONS) <= {str(section) for section in contract.get("source_sections", [])},
        "host evidence contract missing required source sections",
    )

    host_identities = _validate_host_identity_records(
        payload,
        records_by_id=records_by_id,
        boundary_supported_platform_ids=boundary_supported_platform_ids,
    )
    toolchain_probes = _validate_toolchain_probe_records(
        payload,
        records_by_id=records_by_id,
        boundary_supported_platform_ids=boundary_supported_platform_ids,
        required_toolchain_components=required_toolchain_components,
    )
    package_roots = _validate_package_root_evidence_records(
        payload,
        records_by_id=records_by_id,
        package_variant_rows=package_variant_rows,
        boundary_supported_platform_ids=boundary_supported_platform_ids,
    )
    native_execution_records = _validate_native_execution_evidence_records(
        payload,
        records_by_id=records_by_id,
        host_identities=host_identities,
        package_roots=package_roots,
        package_variant_rows=package_variant_rows,
        boundary_supported_platform_ids=boundary_supported_platform_ids,
    )
    negative_cases = _validate_negative_host_toolchain_cases(
        payload,
        unsupported_host_policy=unsupported_host_policy,
    )
    return {
        "host_identity_records": host_identities,
        "toolchain_probe_records": toolchain_probes,
        "package_root_evidence_records": package_roots,
        "native_execution_evidence_records": native_execution_records,
        "negative_host_toolchain_cases": negative_cases,
    }


def _package_artifact_identity_is_source_owned(row_id: str, row: dict[str, Any]) -> None:
    artifact = row.get("artifact_identity_contract")
    expect(isinstance(artifact, dict), f"{row_id} missing artifact_identity_contract")
    for field_name in (
        "archive_name_suffix",
        "object_format",
        "debug_format",
        "loader_path_policy",
        "symbol_export_policy",
    ):
        expect(str(artifact.get(field_name, "")), f"{row_id} artifact identity missing {field_name}")

    runtime_names = [str(item) for item in artifact.get("runtime_library_names", [])]
    package_root_layout = [str(item) for item in artifact.get("package_root_layout", [])]
    expect(runtime_names, f"{row_id} artifact identity missing runtime_library_names")
    expect(package_root_layout, f"{row_id} artifact identity missing package_root_layout")
    expect(
        not any(path.startswith(("tmp/", "artifacts/")) for path in package_root_layout),
        f"{row_id} artifact identity used generated report roots as package layout",
    )

    target_platform_id = str(row.get("target_platform_id", ""))
    object_format = str(artifact.get("object_format", ""))
    if target_platform_id == "linux-x64":
        expect(object_format == "ELF", f"{row_id} Linux package identity must be ELF")
        expect(
            "libobjc3-runtime.so" in runtime_names,
            f"{row_id} Linux package identity missing libobjc3-runtime.so",
        )
    elif target_platform_id == "darwin-arm64":
        expect(object_format == "Mach-O", f"{row_id} macOS package identity must be Mach-O")
        expect(
            "libobjc3-runtime.dylib" in runtime_names,
            f"{row_id} macOS package identity missing libobjc3-runtime.dylib",
        )
    elif target_platform_id == "windows-x64":
        expect(object_format == "COFF", f"{row_id} Windows package identity must be COFF")
        expect(
            {"objc3-runtime.lib", "objc3-runtime.dll"} <= set(runtime_names),
            f"{row_id} Windows package identity missing import library or DLL",
        )


def _package_promotion_gate_is_fail_closed(row_id: str, row: dict[str, Any]) -> None:
    gate = row.get("promotion_gate_contract")
    expect(isinstance(gate, dict), f"{row_id} missing promotion_gate_contract")
    expect(
        gate.get("promotion_source") == "checked-in-package-variant-row",
        f"{row_id} promotion gate source drifted",
    )
    expect(
        gate.get("source_only_metadata_behavior") == "fail-closed-before-publication",
        f"{row_id} source-only package metadata did not fail closed",
    )
    expect(
        gate.get("hosted_runner_summary_behavior") == "summary-only-no-support-promotion",
        f"{row_id} hosted runner summary could promote support",
    )
    required_classes = {str(item) for item in row.get("required_evidence_classes", [])}
    required_positive = {str(item) for item in gate.get("required_positive_evidence_classes", [])}
    expect(required_positive, f"{row_id} promotion gate missing positive evidence classes")
    expect(
        required_positive <= required_classes,
        f"{row_id} promotion gate required evidence is outside package row requirements",
    )
    blocked_surfaces = {str(item) for item in gate.get("blocked_publication_surfaces", [])}
    expect("publication" in blocked_surfaces, f"{row_id} promotion gate does not block publication")

    missing_classes = {str(item) for item in row.get("required_missing_evidence_classes", [])}
    blocked_until = {str(item) for item in gate.get("blocked_until_evidence_classes", [])}
    if row.get("claim_state") == "evidence-bound":
        expect(not missing_classes, f"{row_id} evidence-bound package row listed missing evidence")
        expect(not blocked_until, f"{row_id} evidence-bound package row still listed blocked evidence")
        return
    expect(missing_classes, f"{row_id} unclaimable package row missing blocked evidence classes")
    expect(
        missing_classes <= blocked_until,
        f"{row_id} promotion gate did not block all missing evidence classes",
    )
    expect(
        {"package", "install", "execution", "publication"} <= blocked_surfaces,
        f"{row_id} unclaimable package row did not block package install execution and publication",
    )


def _sanitizer_package_runtime_contract_is_fail_closed(
    variant_id: str,
    row: dict[str, Any],
) -> None:
    package_runtime = row.get("package_runtime_contract")
    expect(isinstance(package_runtime, dict), f"{variant_id} missing package_runtime_contract")
    sanitizer_name = str(row.get("sanitizer", ""))
    expected_detection_records = EXPECTED_SANITIZER_DETECTION_RECORDS.get(sanitizer_name, set())
    expect(expected_detection_records, f"{variant_id} sanitizer identity has no expected detection records")
    expect(
        package_runtime.get("runtime_probe_required") is True,
        f"{variant_id} sanitizer package runtime did not require runtime probe",
    )
    expect(
        package_runtime.get("default_release_channel_allowed") is False,
        f"{variant_id} sanitizer package allowed default release channel",
    )
    expect(
        package_runtime.get("report_artifact_support_truth") is False,
        f"{variant_id} sanitizer reports were treated as support truth",
    )
    expect(
        package_runtime.get("mixed_release_sanitizer_runtime_behavior") == "fail-closed",
        f"{variant_id} mixed release/sanitizer runtime did not fail closed",
    )
    release_runtime_package_ids = [
        str(package_id)
        for package_id in package_runtime.get("release_runtime_package_ids", [])
    ]
    expect(
        release_runtime_package_ids == list(RELEASE_RUNTIME_PACKAGE_IDS),
        f"{variant_id} release runtime package isolation ids drifted",
    )
    expect(
        str(row.get("package_id", "")) not in release_runtime_package_ids,
        f"{variant_id} sanitizer package id matched a release runtime package id",
    )
    expect(
        package_runtime.get("release_runtime_mixing_allowed") is False,
        f"{variant_id} allowed sanitizer/release runtime package mixing",
    )
    detection_records = package_runtime.get("expected_detection_records", [])
    expect(isinstance(detection_records, list) and detection_records, f"{variant_id} missing expected detection records")
    detection_record_ids: set[str] = set()
    for record in detection_records:
        expect(isinstance(record, dict), f"{variant_id} detection record must be an object")
        record_id = str(record.get("record_id", ""))
        expect(record_id, f"{variant_id} detection record missing record_id")
        detection_record_ids.add(record_id)
        expect(
            record.get("required_behavior") == "record-only-no-support-promotion",
            f"{variant_id} detection record behavior drifted",
        )
        expect(record.get("support_truth") is False, f"{variant_id} detection record was treated as support truth")
    expect(
        detection_record_ids == expected_detection_records,
        f"{variant_id} expected detection records drifted",
    )
    unsupported_host_diagnostics = package_runtime.get("unsupported_host_diagnostics", [])
    expect(
        isinstance(unsupported_host_diagnostics, list) and unsupported_host_diagnostics,
        f"{variant_id} missing unsupported-host diagnostics",
    )
    for diagnostic in unsupported_host_diagnostics:
        expect(isinstance(diagnostic, dict), f"{variant_id} unsupported-host diagnostic must be an object")
        expect(str(diagnostic.get("diagnostic_id", "")), f"{variant_id} unsupported-host diagnostic missing diagnostic_id")
        expect(
            diagnostic.get("failure_class") == "unsupported-sanitizer-platform",
            f"{variant_id} unsupported-host diagnostic failure class drifted",
        )
        expect(
            diagnostic.get("required_behavior") == "fail-closed-before-capability-promotion",
            f"{variant_id} unsupported-host diagnostic behavior drifted",
        )
        blocks = {str(block) for block in diagnostic.get("blocks", [])}
        expect(
            REQUIRED_UNSUPPORTED_SANITIZER_DIAGNOSTIC_BLOCKS <= blocks,
            f"{variant_id} unsupported-host diagnostic did not block package install execution and publication",
        )
    metadata_fields = {str(field) for field in package_runtime.get("required_metadata_fields", [])}
    expect(
        REQUIRED_SANITIZER_METADATA_FIELDS[sanitizer_name] <= metadata_fields,
        f"{variant_id} sanitizer package runtime metadata is incomplete",
    )


def _sanitizer_install_guard_is_fail_closed(variant_id: str, row: dict[str, Any]) -> None:
    install_guard = row.get("install_guard")
    expect(isinstance(install_guard, dict), f"{variant_id} missing sanitizer install_guard")
    expect(
        "default release runtime" in str(install_guard.get("release_channel_policy", "")).lower(),
        f"{variant_id} install guard does not name default release runtime isolation",
    )
    expect(
        install_guard.get("unsupported_platform_behavior") == "fail-closed",
        f"{variant_id} unsupported platform install guard drifted",
    )
    expect(
        install_guard.get("missing_runtime_behavior") == "fail-closed-before-package-install",
        f"{variant_id} missing sanitizer runtime install guard drifted",
    )
    expect(
        install_guard.get("mixed_runtime_behavior") == "fail-closed",
        f"{variant_id} mixed runtime install guard drifted",
    )
    expect(
        install_guard.get("stale_package_metadata_behavior") == "fail-closed-before-publication",
        f"{variant_id} stale metadata install guard drifted",
    )


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
    expect(
        "mixed LLVM roots and mismatched LLVM tool versions fail closed before object package execution or platform support" in required_claims,
        "unsupported host policy missing coherent LLVM toolchain claim",
    )
    forbidden_phrases = {str(phrase) for phrase in unsupported_host_policy.get("forbidden_phrases", [])}
    expect(
        "object emission supported via clang substitute" in forbidden_phrases,
        "unsupported host policy missing clang substitute forbidden phrase",
    )
    expect(
        "mixed LLVM roots are supported" in forbidden_phrases,
        "unsupported host policy missing mixed-root forbidden phrase",
    )
    expect(
        "LLVM tool version mismatch is supported" in forbidden_phrases,
        "unsupported host policy missing mismatched-version forbidden phrase",
    )


def _validate_hosted_runner_capability_summaries(
    *,
    boundary_supported_platform_ids: set[str],
) -> None:
    payload = load_hosted_runner_capability_summaries()
    expect(
        payload.get("support_claim_policy") == "summary-only-no-support-promotion",
        "hosted runner capability summaries can only summarize source truth",
    )
    summary_ids: set[str] = set()
    required_summary_ids = {
        "objc3c.hosted.windows-x64.supported.current",
        "objc3c.hosted.linux-x64.unsupported",
        "objc3c.hosted.darwin-arm64.unsupported",
        "objc3c.hosted.sanitizer.address.reserved",
        "objc3c.hosted.sanitizer.undefined.reserved",
        "objc3c.hosted.toolchain.missing-llc.fail-closed",
        "objc3c.hosted.toolchain.mixed-root.fail-closed",
        "objc3c.hosted.toolchain.mismatched-version.fail-closed",
    }
    for summary in payload.get("summaries", []):
        summary_id = str(summary.get("summary_id", ""))
        expect(summary_id, "hosted runner capability summary missing summary_id")
        expect(summary_id not in summary_ids, f"duplicate hosted runner summary: {summary_id}")
        summary_ids.add(summary_id)
        platform_ids = {str(platform_id) for platform_id in summary.get("platform_ids", [])}
        if bool(summary.get("publication_allowed", False)):
            expect(
                summary.get("claim_state") == "evidence-bound",
                f"{summary_id} allowed publication without evidence-bound state",
            )
            expect(
                platform_ids <= boundary_supported_platform_ids,
                f"{summary_id} widened hosted-runner support outside the platform boundary",
            )
            continue
        expect(not platform_ids, f"{summary_id} fail-closed/reserved summary listed platform ids")
        expect(
            summary.get("claim_state") in {"unsupported", "reserved", "fail-closed"},
            f"{summary_id} non-publication summary used unsupported claim state",
        )
    expect(
        required_summary_ids <= summary_ids,
        "hosted runner capability summaries are missing required source-truth cases",
    )


def _contracts_by_id(rows: Iterable[dict[str, Any]]) -> dict[str, dict[str, Any]]:
    contracts: dict[str, dict[str, Any]] = {}
    for row in rows:
        for contract in row.get("negative_contracts", []):
            if not isinstance(contract, dict):
                continue
            contract_id = str(contract.get("contract_id", ""))
            if contract_id:
                contracts[contract_id] = contract
    return contracts


def _native_object_statuses(native_contract: dict[str, Any]) -> set[str]:
    status_fields = (
        "success_status",
        "missing_llc_status",
        "missing_filetype_status",
        "mixed_toolchain_status",
        "mismatched_version_status",
        "unsupported_version_status",
        "unresolved_version_status",
    )
    return {str(native_contract[field]) for field in status_fields if native_contract.get(field)}


def _validate_platform_expansion_object_emission_cases(
    contract: dict[str, Any],
    native_contract: dict[str, Any],
) -> None:
    known_statuses = _native_object_statuses(native_contract)
    positive_status = str(native_contract.get("success_status", ""))
    seen_statuses: set[str] = set()
    for case in contract.get("object_emission_truth_cases", []):
        case_id = str(case.get("case_id", ""))
        status = str(case.get("status", ""))
        seen_statuses.add(status)
        expect(status in known_statuses, f"{case_id} used unknown native object emission status: {status}")
        expect(case.get("required_tool") == "llc", f"{case_id} did not require llc")
        expect(case.get("required_probe") == "llc --filetype=obj", f"{case_id} did not require llc --filetype=obj")
        expect(case.get("clang_substitute_allowed") is False, f"{case_id} allowed clang substitute object emission")
        expect(case.get("toolchain_identity_required") is True, f"{case_id} did not require coherent toolchain identity")
        if bool(case.get("positive_case")):
            expect(status == positive_status, f"{case_id} positive object-emission case did not use the success status")
            expect(not case.get("blocks_surfaces"), f"{case_id} positive object-emission case blocked support surfaces")
            continue
        expect(status != positive_status, f"{case_id} negative object-emission case used the success status")
        expect(
            {"package", "execution", "publication"} <= {str(item) for item in case.get("blocks_surfaces", [])},
            f"{case_id} negative object-emission case did not block package execution and publication",
        )
    expect(
        known_statuses <= seen_statuses,
        "platform expansion object-emission cases did not cover every native object status",
    )


def _validate_platform_expansion_hosted_cases(
    contract: dict[str, Any],
    *,
    boundary_supported_platform_ids: set[str],
) -> None:
    hosted_payload = load_hosted_runner_capability_summaries()
    hosted_by_id = {
        str(summary.get("summary_id")): summary
        for summary in hosted_payload.get("summaries", [])
        if isinstance(summary, dict) and summary.get("summary_id")
    }
    for case in contract.get("hosted_runner_projection_cases", []):
        summary_id = str(case.get("summary_id", ""))
        hosted = hosted_by_id.get(summary_id)
        expect(hosted is not None, f"platform expansion hosted case missing upstream summary {summary_id}")
        expect(hosted.get("issue_ref") == case.get("issue_ref"), f"{summary_id} issue_ref drifted from hosted summary")
        expect(hosted.get("summary_kind") == case.get("summary_kind"), f"{summary_id} summary_kind drifted from hosted summary")
        expect(hosted.get("hosted_runner") == case.get("hosted_runner"), f"{summary_id} hosted_runner drifted from hosted summary")
        expect(hosted.get("claim_state") == case.get("claim_state"), f"{summary_id} claim_state drifted from hosted summary")
        expect(hosted.get("platform_ids") == case.get("platform_ids"), f"{summary_id} platform_ids drifted from hosted summary")
        expect(hosted.get("publication_allowed") == case.get("publication_allowed"), f"{summary_id} publication_allowed drifted from hosted summary")
        expect(
            hosted.get("native_object_emission_status") == case.get("native_object_emission_status"),
            f"{summary_id} native object status drifted from hosted summary",
        )
        platform_ids = {str(platform_id) for platform_id in case.get("platform_ids", [])}
        if bool(case.get("publication_allowed")):
            expect(platform_ids <= boundary_supported_platform_ids, f"{summary_id} projected unsupported hosted platforms")
            continue
        expect(not platform_ids, f"{summary_id} non-publication hosted case listed platforms")
        expect(str(case.get("projection_rule", "")), f"{summary_id} missing projection rule")


def _validate_platform_expansion_platform_cases(
    contract: dict[str, Any],
    *,
    payload: dict[str, Any],
    package_variant_rows: dict[str, dict[str, Any]],
    unsupported_host_policy: dict[str, Any],
) -> None:
    support_rows = {
        str(row.get("row_id")): row
        for row in payload.get("support_rows", [])
        if isinstance(row, dict) and row.get("row_id")
    }
    support_negative_contracts = _contracts_by_id(payload.get("support_rows", []))
    package_negative_contracts = _contracts_by_id(package_variant_rows.values())
    host_identities = _records_by_field(
        "host_identity_records",
        payload.get("host_identity_records"),
        "record_id",
    )
    toolchain_probes = _records_by_field(
        "toolchain_probe_records",
        payload.get("toolchain_probe_records"),
        "record_id",
    )
    package_roots = _records_by_field(
        "package_root_evidence_records",
        payload.get("package_root_evidence_records"),
        "record_id",
    )
    native_execution_records = _records_by_field(
        "native_execution_evidence_records",
        payload.get("native_execution_evidence_records"),
        "record_id",
    )
    failure_ids = _unsupported_failure_ids(unsupported_host_policy) | {
        str(failure_class.get("failure_id"))
        for failure_class in unsupported_host_policy.get("hard_fail_classes", [])
        if isinstance(failure_class, dict) and failure_class.get("failure_id")
    }
    for case in contract.get("platform_claim_cases", []):
        case_id = str(case.get("case_id", ""))
        row_id = str(case.get("support_row_id", ""))
        row = support_rows.get(row_id)
        expect(row is not None, f"{case_id} missing support row {row_id}")
        platform_id = str(case.get("platform_id", ""))
        expect(row.get("platform_id") == platform_id, f"{case_id} platform_id drifted from support row")
        expect(row.get("issue_ref") == case.get("issue_ref"), f"{case_id} issue_ref drifted from support row")
        expect(row.get("support_state") == case.get("claim_state"), f"{case_id} claim_state drifted from support row")
        expect(case.get("publication_allowed") is False, f"{case_id} unexpectedly allowed publication")
        identity = case.get("platform_identity", {})
        expect(identity.get("host_os") == row.get("host_os"), f"{case_id} host_os drifted from support row")
        expect(identity.get("host_arch") == row.get("host_arch"), f"{case_id} host_arch drifted from support row")
        expect(identity.get("host_triples") == row.get("host_triples"), f"{case_id} host triples drifted from support row")
        expect(
            set(str(item) for item in case.get("required_promotion_evidence", []))
            == set(REQUIRED_SUPPORTED_EVIDENCE_CLASSES),
            f"{case_id} promotion evidence does not require build package install execution",
        )
        expect(
            set(str(item) for item in case.get("required_missing_evidence_classes", []))
            == set(str(item) for item in row.get("required_missing_evidence_classes", [])),
            f"{case_id} missing evidence drifted from support row",
        )
        expect(
            set(str(item) for item in case.get("required_toolchain_components", []))
            == set(str(item) for item in row.get("required_toolchain_components", [])),
            f"{case_id} required toolchain components drifted from support row",
        )
        package_row_id = str(case.get("package_variant_row_id", ""))
        package_row = package_variant_rows.get(package_row_id)
        expect(package_row is not None, f"{case_id} missing package variant row {package_row_id}")
        expect(package_row.get("claim_state") == "fail-closed", f"{case_id} package row is not fail-closed")
        expect(not package_row.get("platform_ids"), f"{case_id} package row widened support")
        expect(package_row_id in row.get("package_variant_row_ids", []), f"{case_id} package row not referenced by support row")
        expect(
            {str(item) for item in case.get("fail_closed_failure_ids", [])} <= failure_ids,
            f"{case_id} listed fail-closed failure ids outside unsupported host policy",
        )
        available_negative_contracts = set(support_negative_contracts) | set(package_negative_contracts)
        expect(
            {str(item) for item in case.get("negative_contract_ids", [])} <= available_negative_contracts,
            f"{case_id} referenced a missing negative contract",
        )
        artifact = case.get("artifact_contract", {})
        expect(
            artifact.get("object_emission_alone_supports_platform") is False,
            f"{case_id} allowed object emission alone to support the platform",
        )
        expect(str(artifact.get("native_object_emission_status", "")), f"{case_id} missing native object status")
        host_requirements = case.get("host_evidence_requirements")
        expect(isinstance(host_requirements, dict), f"{case_id} missing host evidence requirements")
        expect(
            str(host_requirements.get("host_identity_record_id", "")) in host_identities,
            f"{case_id} referenced missing host identity record",
        )
        expect(
            str(host_requirements.get("toolchain_probe_record_id", "")) in toolchain_probes,
            f"{case_id} referenced missing toolchain probe record",
        )
        expect(
            str(host_requirements.get("package_root_record_id", "")) in package_roots,
            f"{case_id} referenced missing package root record",
        )
        expect(
            str(host_requirements.get("native_execution_record_id", "")) in native_execution_records,
            f"{case_id} referenced missing native execution record",
        )
        expect(
            host_requirements.get("source_only_probe_promotes_support") is False,
            f"{case_id} allowed source-only probe promotion",
        )
        expect(
            host_requirements.get("promotion_allowed") is False,
            f"{case_id} host evidence requirements allowed promotion",
        )


def _validate_platform_expansion_package_identity_cases(
    contract: dict[str, Any],
    *,
    payload: dict[str, Any],
    package_variant_rows: dict[str, dict[str, Any]],
) -> None:
    package_roots = _records_by_field(
        "package_root_evidence_records",
        payload.get("package_root_evidence_records"),
        "record_id",
    )
    for case in contract.get("package_variant_identity_cases", []):
        identity_id = str(case.get("identity_id", ""))
        row_id = str(case.get("row_id", ""))
        row = package_variant_rows.get(row_id)
        expect(row is not None, f"{identity_id} missing package variant row {row_id}")
        package_root_record_id = str(case.get("package_root_record_id", ""))
        if row.get("variant_kind") == "release-runtime":
            expect(package_root_record_id, f"{identity_id} missing package_root_record_id")
        if package_root_record_id:
            expect(
                package_root_record_id in package_roots,
                f"{identity_id} referenced missing package root record {package_root_record_id}",
            )
            expect(
                package_roots[package_root_record_id].get("package_variant_row_id") == row_id,
                f"{identity_id} package root record drifted from package row",
            )
        for field_name in (
            "issue_ref",
            "variant_kind",
            "target_platform_id",
            "platform_ids",
            "claim_state",
            "package_id",
            "channel_scope",
        ):
            expect(row.get(field_name) == case.get(field_name), f"{identity_id} {field_name} drifted from package row")
        expect(
            row.get("runtime_library_contract", {}).get("runtime_library_ids") == case.get("runtime_library_ids"),
            f"{identity_id} runtime library identity drifted from package row",
        )
        expect(
            row.get("metadata_freshness_guard", {}).get("metadata_source") == case.get("metadata_source"),
            f"{identity_id} metadata source drifted from package row",
        )
        expect(
            row.get("artifact_identity_contract") == case.get("artifact_identity_contract"),
            f"{identity_id} artifact identity drifted from package row",
        )
        expect(
            row.get("promotion_gate_contract") == case.get("promotion_gate_contract"),
            f"{identity_id} promotion gate drifted from package row",
        )
        required_classes = {str(item) for item in row.get("required_evidence_classes", [])}
        expect(
            {str(item) for item in case.get("required_promotion_evidence", [])} <= required_classes,
            f"{identity_id} promotion evidence is not a subset of package row requirements",
        )
        if row.get("claim_state") != "evidence-bound":
            expect(
                case.get("default_release_runtime_mutation_allowed") is False,
                f"{identity_id} allowed default release runtime mutation for an unsupported or reserved row",
            )
            expect(case.get("blocked_publication_conditions"), f"{identity_id} missing blocked publication conditions")


def _validate_platform_expansion_sanitizer_cases(
    contract: dict[str, Any],
    *,
    payload: dict[str, Any],
    package_variant_rows: dict[str, dict[str, Any]],
) -> None:
    sanitizer_rows = {
        str(row.get("variant_id")): row
        for row in payload.get("sanitizer_variants", [])
        if isinstance(row, dict) and row.get("variant_id")
    }
    sanitizer_negative_contracts = _contracts_by_id(payload.get("sanitizer_variants", []))
    package_negative_contracts = _contracts_by_id(package_variant_rows.values())
    for case in contract.get("sanitizer_runtime_package_cases", []):
        variant_id = str(case.get("variant_id", ""))
        row = sanitizer_rows.get(variant_id)
        expect(row is not None, f"{variant_id} missing sanitizer variant row")
        for field_name in (
            "issue_ref",
            "sanitizer",
            "claim_state",
            "platform_ids",
            "package_variant_row_id",
            "package_id",
            "install_guard",
            "package_runtime_contract",
            "required_promotion_evidence",
            "required_missing_evidence_classes",
        ):
            expect(row.get(field_name) == case.get(field_name), f"{variant_id} {field_name} drifted from sanitizer row")
        _sanitizer_install_guard_is_fail_closed(variant_id, row)
        _sanitizer_package_runtime_contract_is_fail_closed(variant_id, row)
        package_row_id = str(case.get("package_variant_row_id", ""))
        package_row = package_variant_rows.get(package_row_id)
        expect(package_row is not None, f"{variant_id} missing package variant row {package_row_id}")
        expect(package_row.get("claim_state") == "reserved", f"{variant_id} package row must remain reserved")
        expect(package_row.get("package_id") == case.get("package_id"), f"{variant_id} package_id drifted from package row")
        expect(
            package_row.get("runtime_library_contract", {}).get("runtime_library_ids") == case.get("runtime_library_ids"),
            f"{variant_id} runtime libraries drifted from package row",
        )
        build_contract = row.get("build_contract", {})
        expect(
            set(str(item) for item in case.get("compiler_flags", []))
            <= set(str(item) for item in build_contract.get("compiler_flags", [])),
            f"{variant_id} compiler flags drifted from sanitizer build contract",
        )
        expect(
            set(str(item) for item in case.get("linker_flags", []))
            <= set(str(item) for item in build_contract.get("linker_flags", [])),
            f"{variant_id} linker flags drifted from sanitizer build contract",
        )
        expect(
            set(str(item) for item in case.get("environment_requirements", []))
            <= set(str(item) for item in build_contract.get("environment_requirements", [])),
            f"{variant_id} environment requirements drifted from sanitizer build contract",
        )
        if str(case.get("sanitizer")) == "undefined":
            expect(
                build_contract.get("mode") == case.get("mode_policy"),
                f"{variant_id} UBSan mode policy drifted from build contract",
            )
        expect(
            "not support truth" in str(case.get("report_truth_policy", "")),
            f"{variant_id} report truth policy must keep reports out of support truth",
        )
        available_negative_contracts = set(sanitizer_negative_contracts) | set(package_negative_contracts)
        expect(
            {str(item) for item in case.get("negative_contract_ids", [])} <= available_negative_contracts,
            f"{variant_id} referenced a missing sanitizer negative contract",
        )


def _validate_platform_expansion_claim_contract(
    payload: dict[str, Any],
    *,
    package_variant_rows: dict[str, dict[str, Any]],
    boundary_supported_platform_ids: set[str],
    unsupported_host_policy: dict[str, Any],
) -> dict[str, Any]:
    contract = load_platform_expansion_claim_contract()
    expect(
        set(contract.get("issue_refs", [])) == {8228, 8229, 8230, 8231, 8232},
        "platform expansion claim contract issue refs drifted",
    )
    authority = contract.get("source_authority", {})
    expect(authority.get("publication_boundary") == "windows-x64-only", "platform expansion widened publication boundary")
    expect(authority.get("generated_reports_are_source_truth") is False, "platform expansion allowed generated source truth")
    expect(authority.get("sanitizer_reports_are_support_truth") is False, "platform expansion treated sanitizer reports as support truth")
    upstream = contract.get("upstream_sources", {})
    expect(
        upstream.get("platform_toolchain_support_evidence") == repo_rel(PLATFORM_TOOLCHAIN_SUPPORT_EVIDENCE_PATH),
        "platform expansion contract upstream support evidence path drifted",
    )
    expect(
        upstream.get("hosted_runner_capability_summaries") == repo_rel(HOSTED_RUNNER_CAPABILITY_SUMMARIES_PATH),
        "platform expansion contract hosted runner source path drifted",
    )
    expect(
        "native object emission success from clang substitute output"
        in {str(item) for item in contract.get("forbidden_overclaims", [])},
        "platform expansion contract missing clang substitute overclaim guard",
    )

    native_contract = payload["llvm_version_support_matrix"]["native_object_emission_contract"]
    _validate_platform_expansion_object_emission_cases(contract, native_contract)
    _validate_platform_expansion_hosted_cases(
        contract,
        boundary_supported_platform_ids=boundary_supported_platform_ids,
    )
    _validate_platform_expansion_platform_cases(
        contract,
        payload=payload,
        package_variant_rows=package_variant_rows,
        unsupported_host_policy=unsupported_host_policy,
    )
    _validate_platform_expansion_package_identity_cases(
        contract,
        payload=payload,
        package_variant_rows=package_variant_rows,
    )
    _validate_platform_expansion_sanitizer_cases(
        contract,
        payload=payload,
        package_variant_rows=package_variant_rows,
    )
    return contract


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
        _package_artifact_identity_is_source_owned(row_id, row)
        _package_promotion_gate_is_fail_closed(row_id, row)

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
        native_object_contract.get("mixed_toolchain_status")
        == "native_object_emission_mixed_toolchain_root",
        "native object emission mixed-toolchain status drifted",
    )
    expect(
        native_object_contract.get("mismatched_version_status")
        == "native_object_emission_mismatched_tool_versions",
        "native object emission mismatched-version status drifted",
    )
    expect(
        native_object_contract.get("unsupported_version_status")
        == "native_object_emission_unsupported_tool_version",
        "native object emission unsupported-version status drifted",
    )
    expect(
        native_object_contract.get("unresolved_version_status")
        == "native_object_emission_unresolved_tool_version",
        "native object emission unresolved-version status drifted",
    )
    expect(
        native_object_contract.get("fallback_policy") == "no-clang-fallback-success-claim",
        "native object emission fallback policy drifted",
    )
    expect(
        native_object_contract.get("coherent_toolchain_policy")
        == "no-mixed-root-or-mismatched-version-success-claim",
        "native object emission coherent-toolchain policy drifted",
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
    mixed_toolchain_rule: dict[str, Any] | None = None
    mismatched_version_rule: dict[str, Any] | None = None
    for rule in rejection_rules:
        expect(isinstance(rule, dict), "LLVM matrix rejection rule must be an object")
        expect(str(rule.get("rule_id", "")), "LLVM matrix rejection rule missing rule_id")
        expect(str(rule.get("condition", "")), f"{rule.get('rule_id', '')} missing rejection condition")
        expect(str(rule.get("diagnostic", "")), f"{rule.get('rule_id', '')} missing rejection diagnostic")
        if rule.get("rule_id") == "objc3c.llvm.reject.missing-llc":
            missing_llc_rule = rule
        if rule.get("rule_id") == "objc3c.llvm.reject.mixed-toolchain":
            mixed_toolchain_rule = rule
        if rule.get("rule_id") == "objc3c.llvm.reject.mismatched-tool-version":
            mismatched_version_rule = rule
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
    expect(mixed_toolchain_rule is not None, "LLVM matrix missing mixed-toolchain rejection rule")
    expect(
        mixed_toolchain_rule.get("failure_status")
        == "native_object_emission_mixed_toolchain_root",
        "mixed-toolchain rejection status drifted",
    )
    expect(
        mixed_toolchain_rule.get("fallback_policy") == "no-clang-fallback-success-claim",
        "mixed-toolchain fallback policy drifted",
    )
    expect(
        mismatched_version_rule is not None,
        "LLVM matrix missing mismatched-tool-version rejection rule",
    )
    expect(
        mismatched_version_rule.get("failure_status")
        == "native_object_emission_mismatched_tool_versions",
        "mismatched-version rejection status drifted",
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
    _validate_unsupported_host_policy_contract(unsupported_host_policy)
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
    _validate_host_evidence_architecture(
        payload,
        records_by_id=records_by_id,
        package_variant_rows=package_variant_rows,
        boundary_supported_platform_ids=boundary_supported_ids,
        unsupported_host_policy=unsupported_host_policy,
        required_toolchain_components=required_toolchain_components,
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
    _validate_hosted_runner_capability_summaries(
        boundary_supported_platform_ids=boundary_supported_ids,
    )
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
        _sanitizer_install_guard_is_fail_closed(str(sanitizer.get("variant_id", "")), sanitizer)
        _sanitizer_package_runtime_contract_is_fail_closed(str(sanitizer.get("variant_id", "")), sanitizer)
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

    _validate_platform_expansion_claim_contract(
        payload,
        package_variant_rows=package_variant_rows,
        boundary_supported_platform_ids=boundary_supported_ids,
        unsupported_host_policy=unsupported_host_policy,
    )


def build_support_evidence_matrix_sections(payload: dict[str, Any]) -> dict[str, Any]:
    records = payload["evidence_records"]
    expansion_contract = load_platform_expansion_claim_contract()
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
        "host_evidence_contract": payload["host_evidence_contract"],
        "host_identity_records": payload["host_identity_records"],
        "toolchain_probe_records": payload["toolchain_probe_records"],
        "package_root_evidence_records": payload["package_root_evidence_records"],
        "native_execution_evidence_records": payload["native_execution_evidence_records"],
        "negative_host_toolchain_cases": payload["negative_host_toolchain_cases"],
        "toolchain_support": {
            "toolchain_evidence_requirements": payload["toolchain_evidence_requirements"],
            "toolchain_ranges": payload["toolchain_ranges"],
            "llvm_version_support_matrix": payload["llvm_version_support_matrix"],
            "sanitizer_variants": payload["sanitizer_variants"],
        },
        "platform_expansion_claim_contract": {
            "contract_id": expansion_contract["contract_id"],
            "schema_path": expansion_contract["schema_path"],
            "source_path": expansion_contract["source_path"],
            "issue_refs": expansion_contract["issue_refs"],
            "platform_claim_case_ids": [
                str(case["case_id"])
                for case in expansion_contract["platform_claim_cases"]
            ],
            "package_variant_identity_ids": [
                str(case["identity_id"])
                for case in expansion_contract["package_variant_identity_cases"]
            ],
            "sanitizer_runtime_variant_ids": [
                str(case["variant_id"])
                for case in expansion_contract["sanitizer_runtime_package_cases"]
            ],
            "object_emission_truth_case_ids": [
                str(case["case_id"])
                for case in expansion_contract["object_emission_truth_cases"]
            ],
        },
        "evidence_records": records,
        "support_evidence_ids": [str(record["evidence_id"]) for record in records],
    }


def build_support_evidence_summary(payload: dict[str, Any]) -> dict[str, Any]:
    supported_rows = [row for row in payload["support_rows"] if row["support_state"] == "supported"]
    unsupported_rows = [row for row in payload["support_rows"] if row["support_state"] == "unsupported"]
    records = payload["evidence_records"]
    expansion_contract = load_platform_expansion_claim_contract()
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
        "host_identity_record_ids": [
            str(row["record_id"])
            for row in payload["host_identity_records"]
        ],
        "toolchain_probe_record_ids": [
            str(row["record_id"])
            for row in payload["toolchain_probe_records"]
        ],
        "package_root_record_ids": [
            str(row["record_id"])
            for row in payload["package_root_evidence_records"]
        ],
        "native_execution_record_ids": [
            str(row["record_id"])
            for row in payload["native_execution_evidence_records"]
        ],
        "negative_host_toolchain_case_ids": [
            str(row["case_id"])
            for row in payload["negative_host_toolchain_cases"]
        ],
        "llvm_matrix_entry_ids": [
            str(row["entry_id"])
            for row in payload["llvm_version_support_matrix"]["matrix_entries"]
        ],
        "sanitizer_variant_ids": [str(row["variant_id"]) for row in payload["sanitizer_variants"]],
        "platform_expansion_claim_contract": {
            "contract_id": expansion_contract["contract_id"],
            "platform_claim_case_count": len(expansion_contract["platform_claim_cases"]),
            "hosted_runner_projection_case_count": len(expansion_contract["hosted_runner_projection_cases"]),
            "package_variant_identity_case_count": len(expansion_contract["package_variant_identity_cases"]),
            "sanitizer_runtime_package_case_count": len(expansion_contract["sanitizer_runtime_package_cases"]),
            "object_emission_truth_case_count": len(expansion_contract["object_emission_truth_cases"]),
        },
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
    "load_hosted_runner_capability_summaries",
    "load_platform_expansion_claim_contract",
    "load_platform_toolchain_support_evidence",
    "validate_platform_toolchain_support_evidence",
]
