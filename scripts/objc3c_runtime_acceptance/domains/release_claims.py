"""Release Claims runtime acceptance domain."""

from __future__ import annotations

import json
import re
from pathlib import Path
from typing import Any

from ..case_result import CaseResult
from ..commands import run
from ..probes import compile_probe, parse_key_value_output, run_probe
from ..core import (
    DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES,
    NATIVE_EXE,
    PRIVATE_RELEASE_CANDIDATE_CLAIM_RUNTIME_ABI_BOUNDARY,
    PRIVATE_RELEASE_CANDIDATE_EVIDENCE_RUNTIME_BOUNDARY,
    PUBLIC_RUNTIME_ABI_BOUNDARY,
    RELEASE_CANDIDATE_CLAIM_RUNTIME_ABI_PROBE,
    RELEASE_CANDIDATE_EVIDENCE_RUNTIME_PROBE,
    RELEASE_CLAIMABLE_SURFACE_FIXTURE,
    ROOT,
    RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
    RUNTIME_CLAIM_PUBLICATION_DASHBOARD_SCHEMA_SURFACE_CONTRACT_ID,
    RUNTIME_CLAIMABILITY_SEMANTICS_RELEASE_POLICY_SURFACE_CONTRACT_ID,
    RUNTIME_CLAIMABLE_SURFACE_RESIDUAL_NON_CLAIMABLE_GAPS_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_FINAL_CLAIM_PUBLICATION_DEPRECATED_PATH_SHUTDOWN_SURFACE_CONTRACT_ID,
    RUNTIME_FINAL_RELEASE_EVIDENCE_DESCAFFOLDING_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_PUBLIC_HEADER_PATH,
    RUNTIME_RELEASE_CANDIDATE_CLAIM_ABI_SURFACE_CONTRACT_ID,
    RUNTIME_SCAFFOLD_RETIREMENT_DEPRECATED_SIDECAR_COMPATIBILITY_DIAGNOSTICS_SURFACE_CONTRACT_ID,
    RUNTIME_STRICT_PROFILE_CLAIM_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_STRICT_PROFILE_FEATURE_CLAIM_SOURCE_SURFACE_CONTRACT_ID,
    compile_fixture_with_args,
    expect,
)


def build_runtime_claimable_surface_residual_non_claimable_gaps_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"claimable-surface-residual-non-claimable-gaps-source-surface"}
    ]
    return {
        "contract_id": (
            RUNTIME_CLAIMABLE_SURFACE_RESIDUAL_NON_CLAIMABLE_GAPS_SOURCE_SURFACE_CONTRACT_ID
        ),
        "compile_artifact_set": [
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.objc3-conformance-report.json",
            "<emit-prefix>.objc3-conformance-publication.json",
            "<emit-prefix>.objc3-advanced-feature-gate.json",
            "<emit-prefix>.objc3-release-candidate-matrix.json",
        ],
        "source_contract_ids": [
            "objc3c.versioned.conformance.report.lowering.v1",
            "objc3c.compatibility.strictness.claim.semantics.v1",
            "objc3c.feature.claim.strictness.truth.surface.v1",
            "objc3c.runtime.capability.reporting.v1",
            "objc3c.driver.conformance.report.publication.v1",
            "objc3c.tooling.integrated.advanced.feature.gate.v1",
            "objc3c.tooling.release.candidate.execution.matrix.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/driver/objc3_objc3_path.cpp",
            "native/objc3c/src/io/objc3_manifest_artifacts.cpp",
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "scripts/publish_release_runtime_claim_matrix.py",
        ],
        "authoritative_source_fields": [
            "module.objc3-conformance-report.json.runnable_feature_claim_ids",
            "module.objc3-conformance-report.json.source_only_feature_claim_ids",
            "module.objc3-conformance-report.json.unsupported_feature_claim_ids",
            "module.objc3-conformance-report.json.runtime_capability_report.claimed_profile_ids",
            "module.objc3-conformance-report.json.runtime_capability_report.not_claimed_profile_ids",
            "module.objc3-conformance-publication.json.selected_profile",
            "module.objc3-conformance-publication.json.rejected_profile_ids",
            "module.objc3-advanced-feature-gate.json.targeted_profile_ids",
            "module.objc3-release-candidate-matrix.json.targeted_profile_ids",
        ],
        "claim_surface_model": (
            "one-compile-coupled-conformance-report-publication-and-release-sidecar-set-defines-the-current-claimable-core-surface-and-the-residual-non-claimable-gaps"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [RELEASE_CLAIMABLE_SURFACE_FIXTURE],
        "residual_non_claimable_gap_ids": [
            "throws",
            "async-await",
            "actors",
            "blocks",
            "arc",
        ],
        "explicit_non_goals": [
            "no-sidecar-only-claim-boundary",
            "no-strict-profile-overclaim-without-live-selection-support",
            "no-release-claim-widening-without-coupled-native-cli-publication",
        ],
        "requires_conformance_report_artifact": True,
        "requires_conformance_publication_artifact": True,
        "requires_release_gate_artifacts": True,
        "requires_real_compile_output": True,
    }


def build_runtime_strict_profile_feature_claim_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"strict-profile-feature-claim-source-surface"}
    ]
    return {
        "contract_id": RUNTIME_STRICT_PROFILE_FEATURE_CLAIM_SOURCE_SURFACE_CONTRACT_ID,
        "source_contract_ids": [
            RUNTIME_CLAIMABLE_SURFACE_RESIDUAL_NON_CLAIMABLE_GAPS_SOURCE_SURFACE_CONTRACT_ID,
            "objc3c.versioned.conformance.report.lowering.v1",
            "objc3c.compatibility.strictness.claim.semantics.v1",
            "objc3c.feature.claim.strictness.truth.surface.v1",
            "objc3c.driver.conformance.report.publication.v1",
            "objc3c.tooling.integrated.advanced.feature.gate.v1",
            "objc3c.tooling.release.candidate.execution.matrix.v1",
        ],
        "compile_artifact_set": [
            "<emit-prefix>.objc3-conformance-report.json",
            "<emit-prefix>.objc3-conformance-publication.json",
            "<emit-prefix>.objc3-advanced-feature-gate.json",
            "<emit-prefix>.objc3-release-candidate-matrix.json",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/driver/objc3_objc3_path.cpp",
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "scripts/publish_release_runtime_claim_matrix.py",
        ],
        "authoritative_source_fields": [
            "module.objc3-conformance-report.json.feature_claim_truth_surface.supported_selection_surface_ids",
            "module.objc3-conformance-report.json.feature_claim_truth_surface.unsupported_selection_surface_ids",
            "module.objc3-conformance-report.json.compatibility_strictness_claim_semantics.rejection_model",
            "module.objc3-conformance-report.json.compatibility_strictness_claim_semantics.fail_closed",
            "module.objc3-conformance-publication.json.supported_profile_ids",
            "module.objc3-conformance-publication.json.rejected_profile_ids",
            "module.objc3-conformance-publication.json.advanced_feature_targeted_profile_ids",
            "module.objc3-advanced-feature-gate.json.targeted_profile_ids",
            "module.objc3-release-candidate-matrix.json.targeted_profile_ids",
        ],
        "strict_profile_feature_claim_model": (
            "strict-profile-targeting-stays-bounded-to-fail-closed-feature-claim-truth-semantics-and-release-gate-artifacts-until-real-selection-support-lands"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [RELEASE_CLAIMABLE_SURFACE_FIXTURE],
        "targeted_profile_ids": ["strict", "strict-concurrency", "strict-system"],
        "explicit_non_goals": [
            "no-strict-profile-selection-support-claim-yet",
            "no-feature-macro-publication-claim-yet",
            "no-publication-path-bypass-around-native-cli-sidecars",
        ],
        "requires_conformance_report_artifact": True,
        "requires_conformance_publication_artifact": True,
        "requires_release_gate_artifacts": True,
        "requires_real_compile_output": True,
    }


def build_runtime_claimability_semantics_release_policy_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"claimability-semantics-release-policy"}
    ]
    return {
        "contract_id": RUNTIME_CLAIMABILITY_SEMANTICS_RELEASE_POLICY_SURFACE_CONTRACT_ID,
        "source_contract_ids": [
            RUNTIME_CLAIMABLE_SURFACE_RESIDUAL_NON_CLAIMABLE_GAPS_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_STRICT_PROFILE_FEATURE_CLAIM_SOURCE_SURFACE_CONTRACT_ID,
            "objc3c.driver.conformance.report.publication.v1",
            "objc3c.toolchain.conformance.claim.operations.v1",
            "objc3c.tooling.integrated.advanced.feature.gate.v1",
            "objc3c.tooling.release.candidate.execution.matrix.v1",
        ],
        "compile_artifact_set": [
            "<emit-prefix>.objc3-conformance-report.json",
            "<emit-prefix>.objc3-conformance-publication.json",
            "<emit-prefix>.objc3-conformance-validation.json",
            "<emit-prefix>.objc3-advanced-feature-gate.json",
            "<emit-prefix>.objc3-release-candidate-matrix.json",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/driver/objc3_objc3_path.cpp",
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp",
        ],
        "policy_model": (
            "claimed-profile-format-validation-and-release-targeting-policy-come-from-one-live-driver-process-frontend-source-of-truth-and-fail-closed-on-drift"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [RELEASE_CLAIMABLE_SURFACE_FIXTURE],
        "negative_operator_commands": [
            "--emit-objc3-conformance-format yaml",
        ],
        "explicit_non_goals": [
            "no-parallel-claim-policy-table",
            "no-strict-profile-publication-bypass",
            "no-non-json-conformance-publication-claim-yet",
        ],
        "requires_conformance_validation_artifact": True,
        "requires_release_gate_artifacts": True,
        "requires_real_compile_output": True,
    }


def build_runtime_strict_profile_claim_implementation_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"strict-profile-claim-implementation"}
    ]
    return {
        "contract_id": RUNTIME_STRICT_PROFILE_CLAIM_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        "source_contract_ids": [
            RUNTIME_CLAIMABILITY_SEMANTICS_RELEASE_POLICY_SURFACE_CONTRACT_ID,
            "objc3c.driver.conformance.report.publication.v1",
            "objc3c.toolchain.conformance.claim.operations.v1",
            "objc3c.runtime.capability.reporting.v1",
        ],
        "compile_artifact_set": [
            "<emit-prefix>.objc3-conformance-report.json",
            "<emit-prefix>.objc3-conformance-publication.json",
            "<emit-prefix>.objc3-conformance-validation.json",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/driver/objc3_objc3_path.cpp",
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
        ],
        "claim_implementation_model": (
            "strict-strict-concurrency-and-strict-system-compile-publish-and-validate-through-the-same-native-cli-claim-surface-as-core"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [RELEASE_CLAIMABLE_SURFACE_FIXTURE],
        "claimed_profile_ids": ["core", "strict", "strict-concurrency", "strict-system"],
        "explicit_non_goals": [
            "no-non-json-publication-claim-yet",
            "no-optional-feature-overclaim-beyond-runtime-capability-report",
        ],
        "requires_conformance_validation_artifact": True,
        "requires_real_compile_output": True,
    }


def build_runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {"scaffold-retirement-deprecated-sidecar-compatibility-diagnostics"}
    ]
    return {
        "contract_id": (
            RUNTIME_SCAFFOLD_RETIREMENT_DEPRECATED_SIDECAR_COMPATIBILITY_DIAGNOSTICS_SURFACE_CONTRACT_ID
        ),
        "source_contract_ids": [
            RUNTIME_STRICT_PROFILE_CLAIM_IMPLEMENTATION_SURFACE_CONTRACT_ID,
            "objc3c.driver.conformance.report.publication.v1",
            "objc3c.toolchain.conformance.claim.operations.v1",
        ],
        "compile_artifact_set": [
            "<emit-prefix>.objc3-conformance-report.json",
            "<emit-prefix>.objc3-conformance-publication.json",
            "<emit-prefix>.objc3-conformance-validation.json",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/driver/objc3_objc3_path.cpp",
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp",
        ],
        "compatibility_diagnostic_model": (
            "live-compile-and-validation-fail-closed-when-deprecated-claim-or-scaffold-sidecars-appear-next-to-current-release-artifacts"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [RELEASE_CLAIMABLE_SURFACE_FIXTURE],
        "deprecated_sidecar_filenames": DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES,
        "explicit_non_goals": [
            "no-silent-compatibility-with-retired-sidecars",
            "no-separate-migration-path-for-deprecated-claim-artifacts",
        ],
        "requires_conformance_validation_artifact": True,
        "requires_real_compile_output": True,
    }


def build_runtime_claim_publication_dashboard_schema_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"claim-publication-dashboard-schema-surface"}
    ]
    return {
        "contract_id": RUNTIME_CLAIM_PUBLICATION_DASHBOARD_SCHEMA_SURFACE_CONTRACT_ID,
        "source_contract_ids": [
            RUNTIME_SCAFFOLD_RETIREMENT_DEPRECATED_SIDECAR_COMPATIBILITY_DIAGNOSTICS_SURFACE_CONTRACT_ID,
            "objc3c.toolchain.dashboard.status.publication.v1",
            "objc3c.toolchain.release.evidence.toolchain.operations.v1",
        ],
        "compile_artifact_set": [
            "<emit-prefix>.objc3-conformance-report.json",
            "<emit-prefix>.objc3-conformance-publication.json",
            "<emit-prefix>.objc3-conformance-validation.json",
            "<emit-prefix>.objc3-release-evidence-operation.json",
            "<emit-prefix>.objc3-dashboard-status.json",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/io/objc3_manifest_artifacts.cpp",
            "schemas/objc3-conformance-dashboard-status-v1.schema.json",
        ],
        "dashboard_schema_model": (
            "validation-publishes-a-schema-shaped-claim-dashboard-over-the-live-report-publication-validation-and-release-evidence-artifacts"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [RELEASE_CLAIMABLE_SURFACE_FIXTURE],
        "dashboard_artifact_path": "<emit-prefix>.objc3-dashboard-status.json",
        "dashboard_schema_path": "schemas/objc3-conformance-dashboard-status-v1.schema.json",
        "explicit_non_goals": [
            "no-dashboard-built-from-release-scope-notes",
            "no-schema-drift-hidden-behind-ad-hoc-dashboard-fields",
        ],
        "requires_conformance_validation_artifact": True,
        "requires_real_compile_output": True,
    }


def build_runtime_final_claim_publication_deprecated_path_shutdown_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"final-claim-publication-deprecated-path-shutdown"}
    ]
    return {
        "contract_id": (
            RUNTIME_FINAL_CLAIM_PUBLICATION_DEPRECATED_PATH_SHUTDOWN_SURFACE_CONTRACT_ID
        ),
        "source_contract_ids": [
            RUNTIME_SCAFFOLD_RETIREMENT_DEPRECATED_SIDECAR_COMPATIBILITY_DIAGNOSTICS_SURFACE_CONTRACT_ID,
            RUNTIME_CLAIM_PUBLICATION_DASHBOARD_SCHEMA_SURFACE_CONTRACT_ID,
            "objc3c.tooling.integrated.advanced.feature.gate.v1",
            "objc3c.tooling.release.candidate.execution.matrix.v1",
        ],
        "compile_artifact_set": [
            "<emit-prefix>.objc3-conformance-report.json",
            "<emit-prefix>.objc3-conformance-publication.json",
            "<emit-prefix>.objc3-conformance-validation.json",
            "<emit-prefix>.objc3-release-evidence-operation.json",
            "<emit-prefix>.objc3-dashboard-status.json",
            "<emit-prefix>.objc3-advanced-feature-gate.json",
            "<emit-prefix>.objc3-release-candidate-matrix.json",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/driver/objc3_objc3_path.cpp",
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/io/objc3_manifest_artifacts.cpp",
        ],
        "final_publication_model": (
            "compile-publishes-the-live-claim-report-publication-and-initial-release-artifacts-validation-completes-the-final-claim-publication-bundle-and-no-deprecated-sidecars-remain"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [RELEASE_CLAIMABLE_SURFACE_FIXTURE],
        "deprecated_sidecar_filenames": DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES,
        "explicit_non_goals": [
            "no-dashboard-ready-summary-revival",
            "no-toolchain-runtime-ga-scaffold-output-revival",
        ],
        "requires_conformance_validation_artifact": True,
        "requires_real_compile_output": True,
    }


def build_runtime_release_candidate_claim_abi_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"release-candidate-runtime-claim-abi"}
    ]
    return {
        "contract_id": RUNTIME_RELEASE_CANDIDATE_CLAIM_ABI_SURFACE_CONTRACT_ID,
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "claimability_semantics_release_policy_surface_contract_id": (
            RUNTIME_CLAIMABILITY_SEMANTICS_RELEASE_POLICY_SURFACE_CONTRACT_ID
        ),
        "final_claim_publication_deprecated_path_shutdown_surface_contract_id": (
            RUNTIME_FINAL_CLAIM_PUBLICATION_DEPRECATED_PATH_SHUTDOWN_SURFACE_CONTRACT_ID
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_release_candidate_claim_testing_boundary": (
            PRIVATE_RELEASE_CANDIDATE_CLAIM_RUNTIME_ABI_BOUNDARY
        ),
        "release_candidate_claim_snapshot_symbol": (
            "objc3_runtime_copy_release_candidate_claim_snapshot_for_testing"
        ),
        "release_candidate_claim_snapshot_type": (
            "objc3_runtime_release_candidate_claim_snapshot"
        ),
        "conformance_publication_contract_id": (
            "objc3c.driver.conformance.report.publication.v1"
        ),
        "conformance_claim_operations_contract_id": (
            "objc3c.toolchain.conformance.claim.operations.v1"
        ),
        "release_evidence_operation_contract_id": (
            "objc3c.tooling.release.evidence.toolchain.operations.v1"
        ),
        "dashboard_status_publication_contract_id": (
            "objc3c.tooling.dashboard.status.publication.v1"
        ),
        "release_candidate_matrix_contract_id": (
            "objc3c.tooling.release.candidate.execution.matrix.v1"
        ),
        "claimed_profile_ids": [
            "core",
            "strict",
            "strict-concurrency",
            "strict-system",
        ],
        "targeted_profile_ids": ["strict", "strict-concurrency", "strict-system"],
        "runtime_claim_boundary_model": (
            "private-release-candidate-claim-snapshot-freezes-the-final-claim-publication-contract-set-and-deprecated-path-shutdown-without-widening-the-public-runtime-header"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_probe_paths": [RELEASE_CANDIDATE_CLAIM_RUNTIME_ABI_PROBE],
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_final_release_evidence_descaffolding_implementation_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"final-release-evidence-descaffolding-implementation"}
    ]
    return {
        "contract_id": (
            RUNTIME_FINAL_RELEASE_EVIDENCE_DESCAFFOLDING_IMPLEMENTATION_SURFACE_CONTRACT_ID
        ),
        "runtime_release_candidate_claim_abi_surface_contract_id": (
            RUNTIME_RELEASE_CANDIDATE_CLAIM_ABI_SURFACE_CONTRACT_ID
        ),
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "private_release_candidate_evidence_testing_boundary": (
            PRIVATE_RELEASE_CANDIDATE_EVIDENCE_RUNTIME_BOUNDARY
        ),
        "release_candidate_evidence_snapshot_symbol": (
            "objc3_runtime_copy_release_candidate_evidence_state_for_testing"
        ),
        "release_candidate_evidence_snapshot_type": (
            "objc3_runtime_release_candidate_evidence_state_snapshot"
        ),
        "validation_artifact_name": "module.objc3-conformance-validation.json",
        "release_evidence_operation_artifact_name": (
            "module.objc3-release-evidence-operation.json"
        ),
        "dashboard_status_artifact_name": "module.objc3-dashboard-status.json",
        "advanced_feature_gate_artifact_name": (
            "module.objc3-advanced-feature-gate.json"
        ),
        "release_candidate_matrix_artifact_name": (
            "module.objc3-release-candidate-matrix.json"
        ),
        "implementation_model": (
            "private-release-candidate-evidence-snapshot-freezes-the-live-validation-release-evidence-dashboard-gate-matrix-and-deprecated-path-shutdown-implementation-boundary"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_probe_paths": [RELEASE_CANDIDATE_EVIDENCE_RUNTIME_PROBE],
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def check_claimable_surface_residual_non_claimable_gaps_source_surface_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "claimable-surface-residual-non-claimable-gaps-source-surface"
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)
    compile_dir = case_dir / "compile"
    compile_fixture_with_args(fixture, compile_dir)

    report = json.loads(
        (compile_dir / "module.objc3-conformance-report.json").read_text(
            encoding="utf-8"
        )
    )
    publication = json.loads(
        (compile_dir / "module.objc3-conformance-publication.json").read_text(
            encoding="utf-8"
        )
    )
    advanced_feature_gate = json.loads(
        (compile_dir / "module.objc3-advanced-feature-gate.json").read_text(
            encoding="utf-8"
        )
    )
    release_candidate_matrix = json.loads(
        (compile_dir / "module.objc3-release-candidate-matrix.json").read_text(
            encoding="utf-8"
        )
    )
    runtime_capability_report = report.get("runtime_capability_report", {})

    expected_optional_gap_ids = ["throws", "async-await", "actors", "blocks", "arc"]
    expected_claimed_profiles = [
        "core",
        "strict",
        "strict-concurrency",
        "strict-system",
    ]

    expect(
        report.get("contract_id") == "objc3c.versioned.conformance.report.lowering.v1",
        "expected native compile to publish the versioned conformance report sidecar",
    )
    expect(
        publication.get("contract_id")
        == "objc3c.driver.conformance.report.publication.v1",
        "expected native compile to publish the conformance publication sidecar",
    )
    expect(
        runtime_capability_report.get("contract_id")
        == "objc3c.runtime.capability.reporting.v1",
        "expected conformance report to embed the runtime capability report",
    )
    expect(
        advanced_feature_gate.get("contract_id")
        == "objc3c.tooling.integrated.advanced.feature.gate.v1",
        "expected native compile to publish the advanced feature gate sidecar",
    )
    expect(
        release_candidate_matrix.get("contract_id")
        == "objc3c.tooling.release.candidate.execution.matrix.v1",
        "expected native compile to publish the release-candidate matrix sidecar",
    )
    expect(
        publication.get("selected_profile") == "core"
        and publication.get("selected_profile_supported") is True
        and publication.get("supported_profile_ids") == expected_claimed_profiles,
        "expected native publication to publish all currently claimable profiles while defaulting to core",
    )
    expect(
        publication.get("rejected_profile_ids") == [],
        "expected native publication to stop reporting any rejected built-in profiles",
    )
    expect(
        runtime_capability_report.get("claimed_profile_ids") == expected_claimed_profiles
        and runtime_capability_report.get("not_claimed_profile_ids") == [],
        "expected runtime capability report to publish all currently claimable profiles and no residual profile gaps",
    )
    expect(
        report.get("unsupported_feature_claim_ids")
        == [
            "unsupported:strictness-selection",
            "unsupported:strict-concurrency-selection",
            "unsupported:throws",
            "unsupported:async-await",
            "unsupported:actors",
            "unsupported:blocks",
            "unsupported:arc",
        ],
        "expected versioned conformance report to publish the residual unsupported claim inventory",
    )
    optional_feature_statuses = {
        entry.get("id"): entry.get("status")
        for entry in runtime_capability_report.get("optional_features", [])
        if isinstance(entry, dict)
    }
    expect(
        optional_feature_statuses
        == {
            "throws": "not-claimed",
            "async-await": "not-claimed",
            "actors": "not-claimed",
            "blocks": "not-claimed",
            "arc": "not-claimed",
        },
        "expected runtime capability report to publish the residual optional non-claimable gaps",
    )
    expect(
        advanced_feature_gate.get("targeted_profile_ids")
        == expected_claimed_profiles[1:]
        and release_candidate_matrix.get("targeted_profile_ids")
        == expected_claimed_profiles[1:],
        "expected release gate sidecars to keep targeting the advanced strict profiles after they become claimable",
    )
    expect(
        release_candidate_matrix.get("advanced_feature_gate_artifact")
        == "module.objc3-advanced-feature-gate.json",
        "expected release-candidate matrix to stay coupled to the advanced feature gate artifact",
    )

    return CaseResult(
        case_id="claimable-surface-residual-non-claimable-gaps-source-surface",
        probe="compile-conformance-report-publication-runtime-capability-and-release-gate-sidecars",
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "claimed_profiles": runtime_capability_report.get("claimed_profile_ids"),
            "selected_profile": publication.get("selected_profile"),
            "unsupported_feature_claim_ids": report.get(
                "unsupported_feature_claim_ids"
            ),
            "optional_non_claimable_features": expected_optional_gap_ids,
        },
    )


def check_strict_profile_feature_claim_source_surface_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "strict-profile-feature-claim-source-surface"
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)
    compile_dir = case_dir / "compile"
    compile_fixture_with_args(fixture, compile_dir)

    report = json.loads(
        (compile_dir / "module.objc3-conformance-report.json").read_text(
            encoding="utf-8"
        )
    )
    publication = json.loads(
        (compile_dir / "module.objc3-conformance-publication.json").read_text(
            encoding="utf-8"
        )
    )
    advanced_feature_gate = json.loads(
        (compile_dir / "module.objc3-advanced-feature-gate.json").read_text(
            encoding="utf-8"
        )
    )
    release_candidate_matrix = json.loads(
        (compile_dir / "module.objc3-release-candidate-matrix.json").read_text(
            encoding="utf-8"
        )
    )
    feature_claim_truth_surface = report.get("feature_claim_truth_surface", {})
    compatibility_semantics = report.get(
        "compatibility_strictness_claim_semantics", {}
    )
    expected_targeted_profiles = ["strict", "strict-concurrency", "strict-system"]

    expect(
        feature_claim_truth_surface.get("contract_id")
        == "objc3c.feature.claim.strictness.truth.surface.v1",
        "expected conformance report to embed the strictness and feature-claim truth surface",
    )
    expect(
        feature_claim_truth_surface.get("supported_selection_surface_ids")
        == [
            "selection:language-version",
            "selection:language-profile",
        ],
        "expected feature-claim truth surface to preserve the live supported selection set",
    )
    expect(
        feature_claim_truth_surface.get("unsupported_selection_surface_ids")
        == [
            "selection:strictness",
            "selection:strict-concurrency",
            "selection:canonical-rejection-diagnostics",
        ],
        "expected feature-claim truth surface to preserve the fail-closed selection set",
    )
    expect(
        feature_claim_truth_surface.get("strictness_selection_supported") is False
        and feature_claim_truth_surface.get("strict_concurrency_selection_supported")
        is False
        and feature_claim_truth_surface.get("feature_macro_surface_supported")
        is False
        and feature_claim_truth_surface.get("claim_truth_fail_closed") is True,
        "expected feature-claim truth surface to publish fail-closed strictness, strict-concurrency, and macro behavior",
    )
    expect(
        compatibility_semantics.get("contract_id")
        == "objc3c.compatibility.strictness.claim.semantics.v1",
        "expected conformance report to embed the compatibility/strictness claim semantics surface",
    )
    expect(
        compatibility_semantics.get("rejection_model")
        == "strictness-strict-concurrency-and-feature-macro-claims-remain-fail-closed",
        "expected compatibility semantics to preserve the strict-profile rejection model",
    )
    expect(
        compatibility_semantics.get("fail_closed") is True
        and compatibility_semantics.get("strictness_selection_rejection_semantics_landed")
        is True
        and compatibility_semantics.get("feature_macro_claim_suppression_semantics_landed")
        is True
        and compatibility_semantics.get("ready_for_lowering_and_runtime") is True,
        "expected compatibility semantics to preserve a ready fail-closed strict-profile boundary",
    )
    expect(
        publication.get("supported_profile_ids")
        == ["core", "strict", "strict-concurrency", "strict-system"]
        and publication.get("rejected_profile_ids") == [],
        "expected publication to preserve the full strict-profile claim set once claim implementation lands",
    )
    expect(
        publication.get("advanced_feature_targeted_profile_ids")
        == expected_targeted_profiles
        and advanced_feature_gate.get("targeted_profile_ids")
        == expected_targeted_profiles
        and release_candidate_matrix.get("targeted_profile_ids")
        == expected_targeted_profiles,
        "expected publication, advanced feature gate, and release-candidate matrix to preserve one strict-profile targeting set",
    )

    return CaseResult(
        case_id="strict-profile-feature-claim-source-surface",
        probe="compile-conformance-report-publication-and-release-gate-targeting-sidecars",
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "supported_selection_surface_ids": feature_claim_truth_surface.get(
                "supported_selection_surface_ids"
            ),
            "unsupported_selection_surface_ids": feature_claim_truth_surface.get(
                "unsupported_selection_surface_ids"
            ),
            "targeted_profile_ids": expected_targeted_profiles,
            "rejection_model": compatibility_semantics.get("rejection_model"),
        },
    )


def check_claimability_semantics_release_policy_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "claimability-semantics-release-policy"
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)
    compile_dir = case_dir / "compile"
    compile_fixture_with_args(fixture, compile_dir)

    report_path = compile_dir / "module.objc3-conformance-report.json"
    publication_path = compile_dir / "module.objc3-conformance-publication.json"
    advanced_feature_gate_path = compile_dir / "module.objc3-advanced-feature-gate.json"
    release_candidate_matrix_path = (
        compile_dir / "module.objc3-release-candidate-matrix.json"
    )
    publication = json.loads(publication_path.read_text(encoding="utf-8"))

    validation_dir = case_dir / "validate"
    validation_dir.mkdir(parents=True, exist_ok=True)
    validation = run(
        [
            str(NATIVE_EXE),
            "--validate-objc3-conformance",
            str(report_path),
            "--out-dir",
            str(validation_dir),
            "--emit-prefix",
            "module",
            "--emit-objc3-conformance-format",
            "json",
        ]
    )
    expect(
        validation.returncode == 0,
        "expected conformance validation to succeed for the current claimed core profile",
    )
    validation_payload = json.loads(
        (validation_dir / "module.objc3-conformance-validation.json").read_text(
            encoding="utf-8"
        )
    )
    advanced_feature_gate = json.loads(
        advanced_feature_gate_path.read_text(encoding="utf-8")
    )
    release_candidate_matrix = json.loads(
        release_candidate_matrix_path.read_text(encoding="utf-8")
    )

    strict_compile_dir = case_dir / "strict"
    compile_fixture_with_args(
        fixture,
        strict_compile_dir,
        ["--objc3-conformance-profile", "strict"],
    )
    strict_publication = json.loads(
        (strict_compile_dir / "module.objc3-conformance-publication.json").read_text(
            encoding="utf-8"
        )
    )

    yaml_reject = run(
        [
            str(NATIVE_EXE),
            str(fixture),
            "--out-dir",
            str(case_dir / "yaml-reject"),
            "--emit-prefix",
            "module",
            "--emit-objc3-conformance-format",
            "yaml",
        ]
    )
    yaml_text = (yaml_reject.stderr or yaml_reject.stdout).strip()
    expect(
        yaml_reject.returncode != 0,
        "expected yaml conformance emission to fail closed",
    )
    expect(
        "claimed publication format: json; targeted release-evidence profiles: strict, strict-concurrency, strict-system"
        in yaml_text,
        "expected yaml emission rejection to publish the centralized format policy diagnostic",
    )

    expect(
        strict_publication.get("selected_profile") == "strict"
        and strict_publication.get("selected_profile_supported") is True,
        "expected strict profile selection to publish through the centralized claim policy",
    )
    expect(
        publication.get("supported_profile_ids") == ["core"]
        or publication.get("supported_profile_ids")
        == ["core", "strict", "strict-concurrency", "strict-system"],
        "expected conformance publication to preserve a recognized live claim policy profile set",
    )
    expect(
        publication.get("supported_profile_ids")
        == ["core", "strict", "strict-concurrency", "strict-system"]
        and publication.get("rejected_profile_ids") == [],
        "expected conformance publication to preserve the centralized live claim policy profile sets",
    )
    expect(
        validation_payload.get("supported_profile_ids")
        == ["core", "strict", "strict-concurrency", "strict-system"]
        and validation_payload.get("rejected_profile_ids") == [],
        "expected conformance validation to preserve the centralized live claim policy profile sets",
    )
    expect(
        publication.get("advanced_feature_targeted_profile_ids")
        == ["strict", "strict-concurrency", "strict-system"]
        and validation_payload.get("advanced_feature_targeted_profile_ids")
        == ["strict", "strict-concurrency", "strict-system"]
        and advanced_feature_gate.get("targeted_profile_ids")
        == ["strict", "strict-concurrency", "strict-system"]
        and release_candidate_matrix.get("targeted_profile_ids")
        == ["strict", "strict-concurrency", "strict-system"],
        "expected publication, validation, gate, and matrix artifacts to preserve one centralized release-targeting policy",
    )

    return CaseResult(
        case_id="claimability-semantics-release-policy",
        probe="compile-publication-validation-and-fail-closed-operator-diagnostics",
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "selected_profile": publication.get("selected_profile"),
            "supported_profile_ids": publication.get("supported_profile_ids"),
            "targeted_profile_ids": publication.get(
                "advanced_feature_targeted_profile_ids"
            ),
            "strict_selected_profile": strict_publication.get("selected_profile"),
            "yaml_reject_returncode": yaml_reject.returncode,
        },
    )


def check_strict_profile_claim_implementation_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "strict-profile-claim-implementation"
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)
    claimed_profiles = ["strict", "strict-concurrency", "strict-system"]
    supported_profiles = ["core", "strict", "strict-concurrency", "strict-system"]
    summaries: list[dict[str, Any]] = []

    for profile in claimed_profiles:
        compile_dir = case_dir / profile
        compile_fixture_with_args(
            fixture,
            compile_dir,
            ["--objc3-conformance-profile", profile],
        )
        report_path = compile_dir / "module.objc3-conformance-report.json"
        publication_path = compile_dir / "module.objc3-conformance-publication.json"
        report = json.loads(report_path.read_text(encoding="utf-8"))
        publication = json.loads(publication_path.read_text(encoding="utf-8"))

        validation_dir = compile_dir / "validate"
        validation_dir.mkdir(parents=True, exist_ok=True)
        validation = run(
            [
                str(NATIVE_EXE),
                "--validate-objc3-conformance",
                str(report_path),
                "--out-dir",
                str(validation_dir),
                "--emit-prefix",
                "module",
                "--emit-objc3-conformance-format",
                "json",
            ]
        )
        expect(
            validation.returncode == 0,
            f"expected {profile} conformance validation to succeed",
        )
        validation_payload = json.loads(
            (validation_dir / "module.objc3-conformance-validation.json").read_text(
                encoding="utf-8"
            )
        )

        expect(
            publication.get("selected_profile") == profile
            and publication.get("selected_profile_supported") is True,
            f"expected publication to claim the selected {profile} profile",
        )
        expect(
            publication.get("supported_profile_ids") == supported_profiles
            and publication.get("rejected_profile_ids") == [],
            f"expected publication to preserve the fully claimed profile inventory for {profile}",
        )
        expect(
            validation_payload.get("selected_profile") == profile
            and validation_payload.get("selected_profile_supported") is True,
            f"expected validation to preserve the selected {profile} profile",
        )
        expect(
            validation_payload.get("supported_profile_ids") == supported_profiles
            and validation_payload.get("rejected_profile_ids") == [],
            f"expected validation to preserve the fully claimed profile inventory for {profile}",
        )
        expect(
            report.get("runtime_capability_report", {}).get("claimed_profile_ids")
            == supported_profiles
            and report.get("runtime_capability_report", {}).get("not_claimed_profile_ids")
            == [],
            f"expected runtime capability report to preserve the fully claimed profile inventory for {profile}",
        )
        summaries.append(
            {
                "profile": profile,
                "selected_profile": publication.get("selected_profile"),
                "supported_profile_ids": publication.get("supported_profile_ids"),
            }
        )

    return CaseResult(
        case_id="strict-profile-claim-implementation",
        probe="compile-publication-validation-and-runtime-capability-report-for-claimed-strict-profiles",
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={"profiles": summaries},
    )


def check_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "scaffold-retirement-deprecated-sidecar-compatibility-diagnostics"
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)

    compile_deprecated_dir = case_dir / "compile-deprecated"
    compile_deprecated_dir.mkdir(parents=True, exist_ok=True)
    for filename in DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES:
        (compile_deprecated_dir / filename).write_text("{}", encoding="utf-8")
    compile_reject = run(
        [
            str(NATIVE_EXE),
            str(fixture),
            "--out-dir",
            str(compile_deprecated_dir),
            "--emit-prefix",
            "module",
        ]
    )
    compile_reject_text = (compile_reject.stderr or compile_reject.stdout).strip()
    expect(
        compile_reject.returncode != 0,
        "expected native compile to fail closed when deprecated claim/scaffold sidecars are present",
    )
    expect(
        "deprecated claim/scaffold compatibility sidecar(s) detected"
        in compile_reject_text,
        "expected native compile rejection to publish the deprecated sidecar compatibility diagnostic",
    )

    validate_compile_dir = case_dir / "validate-source"
    compile_fixture_with_args(fixture, validate_compile_dir)
    report_path = validate_compile_dir / "module.objc3-conformance-report.json"
    for filename in DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES:
        (validate_compile_dir / filename).write_text("{}", encoding="utf-8")
    validate_dir = case_dir / "validate-output"
    validate_dir.mkdir(parents=True, exist_ok=True)
    validate_reject = run(
        [
            str(NATIVE_EXE),
            "--validate-objc3-conformance",
            str(report_path),
            "--out-dir",
            str(validate_dir),
            "--emit-prefix",
            "module",
            "--emit-objc3-conformance-format",
            "json",
        ]
    )
    validate_reject_text = (validate_reject.stderr or validate_reject.stdout).strip()
    expect(
        validate_reject.returncode != 0,
        "expected conformance validation to fail closed when deprecated claim/scaffold sidecars are present next to the validated report",
    )
    expect(
        "deprecated claim/scaffold compatibility sidecar(s) detected"
        in validate_reject_text,
        "expected conformance validation rejection to publish the deprecated sidecar compatibility diagnostic",
    )

    return CaseResult(
        case_id="scaffold-retirement-deprecated-sidecar-compatibility-diagnostics",
        probe="compile-and-validate-with-deprecated-claim-sidecars-present",
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "deprecated_sidecar_filenames": DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES,
            "compile_reject_returncode": compile_reject.returncode,
            "validate_reject_returncode": validate_reject.returncode,
        },
    )


def check_claim_publication_dashboard_schema_surface_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "claim-publication-dashboard-schema-surface"
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)
    compile_dir = case_dir / "compile"
    compile_fixture_with_args(fixture, compile_dir)
    report_path = compile_dir / "module.objc3-conformance-report.json"

    validate_dir = case_dir / "validate"
    validate_dir.mkdir(parents=True, exist_ok=True)
    validation = run(
        [
            str(NATIVE_EXE),
            "--validate-objc3-conformance",
            str(report_path),
            "--out-dir",
            str(validate_dir),
            "--emit-prefix",
            "module",
            "--emit-objc3-conformance-format",
            "json",
        ]
    )
    expect(
        validation.returncode == 0,
        "expected conformance validation to publish the dashboard artifact successfully",
    )

    dashboard_path = validate_dir / "module.objc3-dashboard-status.json"
    dashboard = json.loads(dashboard_path.read_text(encoding="utf-8"))
    schema = json.loads(
        (ROOT / "schemas" / "objc3-conformance-dashboard-status-v1.schema.json").read_text(
            encoding="utf-8"
        )
    )
    required_keys = schema.get("required", [])
    expect(
        all(key in dashboard for key in required_keys),
        "expected dashboard artifact to publish every required top-level schema field",
    )
    expect(
        dashboard.get("schema_id") == "objc3-conformance-dashboard-status/v1"
        and dashboard.get("schema_version") == 1
        and dashboard.get("dashboard_version") == "0.11.0"
        and dashboard.get("release_label") == "v0.11"
        and dashboard.get("status") == "pass",
        "expected dashboard artifact to preserve the live schema identity and release status surface",
    )
    expect(
        [entry.get("profile_id") for entry in dashboard.get("profiles", [])]
        == ["core", "strict", "strict-concurrency", "strict-system"],
        "expected dashboard artifact to publish one schema-shaped profile row for each claimed profile",
    )
    expect(
        [entry.get("dependency_id") for entry in dashboard.get("dependencies", [])]
        == ["B-04", "B-10", "B-11", "B-12"],
        "expected dashboard artifact to publish the schema-shaped dependency inventory",
    )
    artifact_paths = [entry.get("artifact_path") for entry in dashboard.get("artifacts", [])]
    expect(
        artifact_paths
        == [
            "module.objc3-conformance-report.json",
            "module.objc3-conformance-publication.json",
            "module.objc3-conformance-validation.json",
            "module.objc3-release-evidence-operation.json",
        ],
        "expected dashboard artifact to preserve the live emitted artifact refs behind the schema surface",
    )
    expect(
        all(
            isinstance(entry.get("file_sha256"), str)
            and re.fullmatch(r"[a-f0-9]{64}", entry["file_sha256"])
            for entry in dashboard.get("artifacts", [])
        ),
        "expected dashboard artifact to publish schema-shaped lowercase 64-hex file digests for each emitted artifact ref",
    )
    expect(
        dashboard.get("summary", {}).get("profile_counts")
        == {"pass": 4, "fail": 0, "blocked": 0, "incomplete": 0}
        and dashboard.get("summary", {}).get("dependency_counts")
        == {"pass": 4, "fail": 0, "blocked": 0, "stale": 0, "missing": 0},
        "expected dashboard artifact to publish deterministic schema-shaped summary counts",
    )
    expect(
        dashboard.get("refresh", {}).get("trigger") == "manual-replay"
        and dashboard.get("refresh", {}).get("stale_dependency_ids") == []
        and dashboard.get("refresh", {}).get("escalation_state") == "none",
        "expected dashboard artifact to publish deterministic refresh telemetry",
    )
    expect(
        len(dashboard.get("change_history", [])) == 1
        and dashboard["change_history"][0].get("change_kind") == "refresh-only",
        "expected dashboard artifact to publish deterministic change history over the schema surface",
    )

    return CaseResult(
        case_id="claim-publication-dashboard-schema-surface",
        probe="compile-validate-and-inspect-schema-shaped-dashboard-artifact",
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "dashboard_schema_id": dashboard.get("schema_id"),
            "artifact_paths": artifact_paths,
            "profile_ids": [entry.get("profile_id") for entry in dashboard.get("profiles", [])],
        },
    )


def check_final_claim_publication_deprecated_path_shutdown_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "final-claim-publication-deprecated-path-shutdown"
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)

    compile_dir = case_dir / "compile"
    compile_fixture_with_args(fixture, compile_dir)
    compile_artifacts = sorted(path.name for path in compile_dir.glob("module.objc3-*.json"))
    expect(
        compile_artifacts
        == [
            "module.objc3-advanced-feature-gate.json",
            "module.objc3-conformance-publication.json",
            "module.objc3-conformance-report.json",
            "module.objc3-release-candidate-matrix.json",
        ],
        "expected native compile to publish only the live claim publication and release-candidate sidecars",
    )
    expect(
        all(not (compile_dir / filename).exists() for filename in DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES),
        "expected native compile to stop emitting every deprecated claim/scaffold sidecar filename",
    )

    report_path = compile_dir / "module.objc3-conformance-report.json"
    validate_dir = case_dir / "validate"
    validate_dir.mkdir(parents=True, exist_ok=True)
    validation = run(
        [
            str(NATIVE_EXE),
            "--validate-objc3-conformance",
            str(report_path),
            "--out-dir",
            str(validate_dir),
            "--emit-prefix",
            "module",
            "--emit-objc3-conformance-format",
            "json",
        ]
    )
    expect(
        validation.returncode == 0,
        "expected conformance validation to publish the full final claim publication bundle",
    )

    validate_artifacts = sorted(path.name for path in validate_dir.glob("module.objc3-*.json"))
    expect(
        validate_artifacts
        == [
            "module.objc3-advanced-feature-gate.json",
            "module.objc3-conformance-validation.json",
            "module.objc3-dashboard-status.json",
            "module.objc3-release-candidate-matrix.json",
            "module.objc3-release-evidence-operation.json",
        ],
        "expected conformance validation to publish the final post-publication release artifacts and no deprecated sidecars",
    )
    expect(
        all(not (validate_dir / filename).exists() for filename in DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES),
        "expected conformance validation to keep deprecated claim/scaffold sidecar paths shut down",
    )

    release_evidence_operation = json.loads(
        (validate_dir / "module.objc3-release-evidence-operation.json").read_text(
            encoding="utf-8"
        )
    )
    advanced_feature_gate = json.loads(
        (validate_dir / "module.objc3-advanced-feature-gate.json").read_text(
            encoding="utf-8"
        )
    )
    release_candidate_matrix = json.loads(
        (validate_dir / "module.objc3-release-candidate-matrix.json").read_text(
            encoding="utf-8"
        )
    )
    expect(
        release_evidence_operation.get("operation_model")
        == "validation-publishes-release-evidence-command-surface-and-dashboard-status-over-the-final-claim-publication-artifact-set",
        "expected release evidence operation artifact to describe the final claim publication bundle instead of the retired dashboard-ready summary path",
    )
    expect(
        advanced_feature_gate.get("surface_kind") == "native-cli-validation"
        and release_candidate_matrix.get("surface_kind") == "native-cli-validation",
        "expected validation-emitted gate and matrix artifacts to identify the live validation publication path",
    )
    expect(
        advanced_feature_gate.get("dashboard_artifact_expected")
        == "module.objc3-dashboard-status.json"
        and release_candidate_matrix.get("advanced_feature_gate_artifact")
        == "module.objc3-advanced-feature-gate.json",
        "expected validation-emitted release artifacts to preserve the final live artifact wiring",
    )
    expect(
        release_candidate_matrix.get("matrix_model")
        == "release-candidate-matrix-freezes-cross-lane-advanced-feature-evidence-over-the-final-claim-publication-artifact-set",
        "expected release candidate matrix to describe the final claim publication artifact set instead of emitted sidecars generically",
    )

    return CaseResult(
        case_id="final-claim-publication-deprecated-path-shutdown",
        probe="compile-validate-and-inspect-the-final-claim-publication-bundle",
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "compile_artifacts": compile_artifacts,
            "validate_artifacts": validate_artifacts,
            "validation_surface_kind": advanced_feature_gate.get("surface_kind"),
        },
    )


def check_release_candidate_runtime_claim_abi_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "release-candidate-runtime-claim-abi"
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)
    compile_dir = case_dir / "compile"
    compile_fixture_with_args(fixture, compile_dir)

    manifest = json.loads((compile_dir / "module.manifest.json").read_text(encoding="utf-8"))
    runtime_claim_abi_surface = manifest.get("runtime_release_candidate_claim_abi_surface")
    expect(
        isinstance(runtime_claim_abi_surface, dict),
        "expected compiled fixture manifest to publish runtime_release_candidate_claim_abi_surface",
    )
    expect(
        runtime_claim_abi_surface.get("contract_id")
        == RUNTIME_RELEASE_CANDIDATE_CLAIM_ABI_SURFACE_CONTRACT_ID,
        "expected compiled fixture manifest to publish the release-candidate claim ABI surface contract",
    )
    expect(
        runtime_claim_abi_surface.get("public_header_path") == RUNTIME_PUBLIC_HEADER_PATH
        and runtime_claim_abi_surface.get("internal_header_path")
        == RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "expected release-candidate claim ABI surface to preserve the runtime header paths",
    )
    expect(
        runtime_claim_abi_surface.get("public_runtime_abi_boundary")
        == PUBLIC_RUNTIME_ABI_BOUNDARY,
        "expected release-candidate claim ABI surface to preserve the public runtime ABI boundary",
    )
    expect(
        runtime_claim_abi_surface.get("private_release_candidate_claim_testing_boundary")
        == PRIVATE_RELEASE_CANDIDATE_CLAIM_RUNTIME_ABI_BOUNDARY,
        "expected release-candidate claim ABI surface to preserve the private claim snapshot boundary",
    )
    expect(
        runtime_claim_abi_surface.get("release_candidate_claim_snapshot_symbol")
        == "objc3_runtime_copy_release_candidate_claim_snapshot_for_testing"
        and runtime_claim_abi_surface.get("release_candidate_claim_snapshot_type")
        == "objc3_runtime_release_candidate_claim_snapshot",
        "expected release-candidate claim ABI surface to publish the runtime snapshot symbol and type",
    )
    expect(
        runtime_claim_abi_surface.get("claimed_profile_ids")
        == ["core", "strict", "strict-concurrency", "strict-system"]
        and runtime_claim_abi_surface.get("targeted_profile_ids")
        == ["strict", "strict-concurrency", "strict-system"],
        "expected release-candidate claim ABI surface to publish the claimed and targeted profile sets",
    )
    expect(
        runtime_claim_abi_surface.get("authoritative_probe_path")
        == RELEASE_CANDIDATE_CLAIM_RUNTIME_ABI_PROBE,
        "expected release-candidate claim ABI surface to publish the authoritative runtime probe path",
    )

    probe = ROOT / Path(RELEASE_CANDIDATE_CLAIM_RUNTIME_ABI_PROBE)
    exe_path = case_dir / "release_candidate_claim_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [])
    payload = parse_key_value_output(
        run_probe(exe_path), "release-candidate runtime claim ABI probe"
    )
    expect(
        payload.get("copy_status") == 0
        and payload.get("claim_bundle_ready") == 1
        and payload.get("deterministic") == 1,
        "expected release-candidate runtime claim ABI probe to publish a ready deterministic snapshot",
    )
    expect(
        payload.get("selected_profile") == "core"
        and payload.get("claimed_profile_ids_csv")
        == "core,strict,strict-concurrency,strict-system"
        and payload.get("targeted_profile_ids_csv")
        == "strict,strict-concurrency,strict-system",
        "expected release-candidate runtime claim ABI probe to publish the live selected, claimed, and targeted profile sets",
    )
    expect(
        payload.get("conformance_publication_contract_id")
        == "objc3c.driver.conformance.report.publication.v1"
        and payload.get("conformance_claim_operations_contract_id")
        == "objc3c.toolchain.conformance.claim.operations.v1"
        and payload.get("release_evidence_operation_contract_id")
        == "objc3c.tooling.release.evidence.toolchain.operations.v1"
        and payload.get("dashboard_status_publication_contract_id")
        == "objc3c.tooling.dashboard.status.publication.v1"
        and payload.get("release_candidate_matrix_contract_id")
        == "objc3c.tooling.release.candidate.execution.matrix.v1",
        "expected release-candidate runtime claim ABI probe to preserve the live publication contract set",
    )
    expect(
        payload.get("dashboard_schema_path")
        == "schemas/objc3-conformance-dashboard-status-v1.schema.json"
        and payload.get("gate_script_path") == "scripts/check_release_evidence.py"
        and payload.get("runbook_reference_path")
        == "spec/conformance/release_evidence_gate_maintenance.md",
        "expected release-candidate runtime claim ABI probe to preserve the live dashboard schema and release evidence operator paths",
    )

    return CaseResult(
        case_id="release-candidate-runtime-claim-abi",
        probe=RELEASE_CANDIDATE_CLAIM_RUNTIME_ABI_PROBE,
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "selected_profile": payload.get("selected_profile"),
            "claimed_profile_ids_csv": payload.get("claimed_profile_ids_csv"),
            "targeted_profile_ids_csv": payload.get("targeted_profile_ids_csv"),
        },
    )


def check_final_release_evidence_descaffolding_implementation_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "final-release-evidence-descaffolding-implementation"
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)
    compile_dir = case_dir / "compile"
    compile_fixture_with_args(fixture, compile_dir)

    report_path = compile_dir / "module.objc3-conformance-report.json"
    validate_dir = case_dir / "validate"
    validate_dir.mkdir(parents=True, exist_ok=True)
    validation = run(
        [
            str(NATIVE_EXE),
            "--validate-objc3-conformance",
            str(report_path),
            "--out-dir",
            str(validate_dir),
            "--emit-prefix",
            "module",
            "--emit-objc3-conformance-format",
            "json",
        ]
    )
    expect(
        validation.returncode == 0,
        "expected conformance validation to succeed for the final release evidence implementation case",
    )

    manifest = json.loads((compile_dir / "module.manifest.json").read_text(encoding="utf-8"))
    implementation_surface = manifest.get(
        "runtime_final_release_evidence_descaffolding_implementation_surface"
    )
    expect(
        isinstance(implementation_surface, dict),
        "expected compiled fixture manifest to publish runtime_final_release_evidence_descaffolding_implementation_surface",
    )
    expect(
        implementation_surface.get("contract_id")
        == RUNTIME_FINAL_RELEASE_EVIDENCE_DESCAFFOLDING_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        "expected compiled fixture manifest to publish the final release evidence descaffolding implementation surface contract",
    )
    expect(
        implementation_surface.get("runtime_release_candidate_claim_abi_surface_contract_id")
        == RUNTIME_RELEASE_CANDIDATE_CLAIM_ABI_SURFACE_CONTRACT_ID,
        "expected final release evidence implementation surface to depend on the release-candidate claim ABI surface",
    )
    expect(
        implementation_surface.get("private_release_candidate_evidence_testing_boundary")
        == PRIVATE_RELEASE_CANDIDATE_EVIDENCE_RUNTIME_BOUNDARY,
        "expected final release evidence implementation surface to preserve the private evidence snapshot boundary",
    )
    expect(
        implementation_surface.get("validation_artifact_name")
        == "module.objc3-conformance-validation.json"
        and implementation_surface.get("release_evidence_operation_artifact_name")
        == "module.objc3-release-evidence-operation.json"
        and implementation_surface.get("dashboard_status_artifact_name")
        == "module.objc3-dashboard-status.json"
        and implementation_surface.get("advanced_feature_gate_artifact_name")
        == "module.objc3-advanced-feature-gate.json"
        and implementation_surface.get("release_candidate_matrix_artifact_name")
        == "module.objc3-release-candidate-matrix.json",
        "expected final release evidence implementation surface to publish the final artifact inventory",
    )

    validate_artifacts = sorted(path.name for path in validate_dir.glob("module.objc3-*.json"))
    expect(
        validate_artifacts
        == [
            "module.objc3-advanced-feature-gate.json",
            "module.objc3-conformance-validation.json",
            "module.objc3-dashboard-status.json",
            "module.objc3-release-candidate-matrix.json",
            "module.objc3-release-evidence-operation.json",
        ],
        "expected validation output to preserve the final release evidence artifact inventory",
    )

    probe = ROOT / Path(RELEASE_CANDIDATE_EVIDENCE_RUNTIME_PROBE)
    exe_path = case_dir / "release_candidate_evidence_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [])
    payload = parse_key_value_output(
        run_probe(exe_path), "release-candidate evidence runtime probe"
    )
    expect(
        payload.get("copy_status") == 0
        and payload.get("validation_artifact_ready") == 1
        and payload.get("release_evidence_operation_ready") == 1
        and payload.get("dashboard_status_ready") == 1
        and payload.get("advanced_feature_gate_ready") == 1
        and payload.get("release_candidate_matrix_ready") == 1
        and payload.get("deprecated_paths_shutdown") == 1
        and payload.get("deterministic") == 1,
        "expected final release evidence runtime probe to publish a ready deterministic implementation snapshot",
    )
    expect(
        payload.get("validation_artifact_name")
        == "module.objc3-conformance-validation.json"
        and payload.get("release_evidence_operation_artifact_name")
        == "module.objc3-release-evidence-operation.json"
        and payload.get("dashboard_status_artifact_name")
        == "module.objc3-dashboard-status.json"
        and payload.get("advanced_feature_gate_artifact_name")
        == "module.objc3-advanced-feature-gate.json"
        and payload.get("release_candidate_matrix_artifact_name")
        == "module.objc3-release-candidate-matrix.json",
        "expected final release evidence runtime probe to preserve the final artifact inventory",
    )

    return CaseResult(
        case_id="final-release-evidence-descaffolding-implementation",
        probe=RELEASE_CANDIDATE_EVIDENCE_RUNTIME_PROBE,
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "validate_artifacts": validate_artifacts,
            "validation_model": payload.get("validation_model"),
        },
    )

def exported_case_names() -> list[str]:
    return sorted(name for name in __all__ if name != "exported_case_names")


__all__ = [
    "exported_case_names",
    "build_runtime_claimable_surface_residual_non_claimable_gaps_source_surface",
    "build_runtime_strict_profile_feature_claim_source_surface",
    "build_runtime_claimability_semantics_release_policy_surface",
    "build_runtime_strict_profile_claim_implementation_surface",
    "build_runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface",
    "build_runtime_claim_publication_dashboard_schema_surface",
    "build_runtime_final_claim_publication_deprecated_path_shutdown_surface",
    "build_runtime_release_candidate_claim_abi_surface",
    "build_runtime_final_release_evidence_descaffolding_implementation_surface",
    "check_claimable_surface_residual_non_claimable_gaps_source_surface_case",
    "check_strict_profile_feature_claim_source_surface_case",
    "check_claimability_semantics_release_policy_case",
    "check_strict_profile_claim_implementation_case",
    "check_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_case",
    "check_claim_publication_dashboard_schema_surface_case",
    "check_final_claim_publication_deprecated_path_shutdown_case",
    "check_release_candidate_runtime_claim_abi_case",
    "check_final_release_evidence_descaffolding_implementation_case",
]
