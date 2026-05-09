"""Release-claims source and policy runtime acceptance surfaces."""

from __future__ import annotations

from typing import Any

from ..case_result import CaseResult
from .release_claims_owner_contracts import release_claims_surface_owner_payload
from ..runtime_contract_release import (
    RELEASE_CLAIMABLE_SURFACE_FIXTURE,
    RUNTIME_CLAIMABILITY_SEMANTICS_RELEASE_POLICY_SURFACE_CONTRACT_ID,
    RUNTIME_CLAIMABLE_SURFACE_RESIDUAL_NON_CLAIMABLE_GAPS_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_STRICT_PROFILE_CLAIM_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_STRICT_PROFILE_FEATURE_CLAIM_SOURCE_SURFACE_CONTRACT_ID,
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
        "owner_contract": release_claims_surface_owner_payload(),
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
        "owner_contract": release_claims_surface_owner_payload(),
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
        "owner_contract": release_claims_surface_owner_payload(),
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
        "owner_contract": release_claims_surface_owner_payload(),
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


__all__ = [
    "build_runtime_claimable_surface_residual_non_claimable_gaps_source_surface",
    "build_runtime_strict_profile_feature_claim_source_surface",
    "build_runtime_claimability_semantics_release_policy_surface",
    "build_runtime_strict_profile_claim_implementation_surface",
]
