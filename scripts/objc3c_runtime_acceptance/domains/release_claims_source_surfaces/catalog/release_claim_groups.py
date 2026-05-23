"""Release-claim group definitions for source-surface catalog assembly."""

from __future__ import annotations

from .constants import (
    CLAIMABILITY_POLICY_CODE_PATHS,
    CLAIMABILITY_POLICY_COMPILE_ARTIFACTS,
    CLAIMABILITY_POLICY_SOURCE_CONTRACT_IDS,
    CLAIMABILITY_SEMANTICS_RELEASE_POLICY_CASE_ID,
    CLAIMABLE_RESIDUAL_CODE_PATHS,
    CLAIMABLE_RESIDUAL_COMPILE_ARTIFACTS,
    CLAIMABLE_RESIDUAL_SOURCE_CONTRACT_IDS,
    CLAIMABLE_RESIDUAL_SOURCE_FIELDS,
    CLAIMABLE_SURFACE_RESIDUAL_CASE_ID,
    RELEASE_CLAIMABLE_SURFACE_FIXTURE,
    RUNTIME_CLAIMABILITY_SEMANTICS_RELEASE_POLICY_SURFACE_CONTRACT_ID,
    RUNTIME_CLAIMABLE_SURFACE_RESIDUAL_NON_CLAIMABLE_GAPS_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_STRICT_PROFILE_CLAIM_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_STRICT_PROFILE_FEATURE_CLAIM_SOURCE_SURFACE_CONTRACT_ID,
    STRICT_PROFILE_CLAIM_IMPLEMENTATION_CASE_ID,
    STRICT_PROFILE_FEATURE_CLAIM_CASE_ID,
    STRICT_PROFILE_FEATURE_CLAIM_CODE_PATHS,
    STRICT_PROFILE_FEATURE_CLAIM_COMPILE_ARTIFACTS,
    STRICT_PROFILE_FEATURE_CLAIM_SOURCE_CONTRACT_IDS,
    STRICT_PROFILE_FEATURE_CLAIM_SOURCE_FIELDS,
    STRICT_PROFILE_IMPLEMENTATION_CODE_PATHS,
    STRICT_PROFILE_IMPLEMENTATION_COMPILE_ARTIFACTS,
    STRICT_PROFILE_IMPLEMENTATION_SOURCE_CONTRACT_IDS,
)
from .evidence import (
    authoritative_code_paths,
    authoritative_source_fields,
    compile_artifact_set,
    explicit_non_goals,
    fixture_paths,
    payload_field,
    payload_fields,
    release_model,
    required_truth,
    source_contract_ids,
)
from .models import ReleaseClaimGroup


CLAIMABLE_RESIDUAL_RELEASE_CLAIM_GROUP = ReleaseClaimGroup(
    contract_id=(
        RUNTIME_CLAIMABLE_SURFACE_RESIDUAL_NON_CLAIMABLE_GAPS_SOURCE_SURFACE_CONTRACT_ID
    ),
    case_ids=frozenset({CLAIMABLE_SURFACE_RESIDUAL_CASE_ID}),
    pre_case_fields=payload_fields(
        compile_artifact_set(*CLAIMABLE_RESIDUAL_COMPILE_ARTIFACTS),
        source_contract_ids(*CLAIMABLE_RESIDUAL_SOURCE_CONTRACT_IDS),
        authoritative_code_paths(*CLAIMABLE_RESIDUAL_CODE_PATHS),
        authoritative_source_fields(*CLAIMABLE_RESIDUAL_SOURCE_FIELDS),
        release_model(
            "claim_surface_model",
            (
                "one-compile-coupled-conformance-report-publication-and-release-"
                "sidecar-set-defines-the-current-claimable-core-surface-and-the-"
                "residual-non-claimable-gaps"
            ),
        ),
    ),
    fixture_paths=fixture_paths(RELEASE_CLAIMABLE_SURFACE_FIXTURE),
    post_fixture_fields=payload_fields(
        payload_field(
            "residual_non_claimable_gap_ids",
            (
                "throws",
                "async-await",
                "actors",
                "blocks",
                "arc",
            ),
        ),
        explicit_non_goals(
            "no-sidecar-only-claim-boundary",
            "no-strict-profile-overclaim-without-live-selection-support",
            "no-release-claim-widening-without-coupled-native-cli-publication",
        ),
        required_truth("requires_conformance_report_artifact"),
        required_truth("requires_conformance_publication_artifact"),
        required_truth("requires_release_gate_artifacts"),
        required_truth("requires_real_compile_output"),
    ),
)


STRICT_PROFILE_FEATURE_CLAIM_RELEASE_CLAIM_GROUP = ReleaseClaimGroup(
    contract_id=RUNTIME_STRICT_PROFILE_FEATURE_CLAIM_SOURCE_SURFACE_CONTRACT_ID,
    case_ids=frozenset({STRICT_PROFILE_FEATURE_CLAIM_CASE_ID}),
    pre_case_fields=payload_fields(
        source_contract_ids(*STRICT_PROFILE_FEATURE_CLAIM_SOURCE_CONTRACT_IDS),
        compile_artifact_set(*STRICT_PROFILE_FEATURE_CLAIM_COMPILE_ARTIFACTS),
        authoritative_code_paths(*STRICT_PROFILE_FEATURE_CLAIM_CODE_PATHS),
        authoritative_source_fields(*STRICT_PROFILE_FEATURE_CLAIM_SOURCE_FIELDS),
        release_model(
            "strict_profile_feature_claim_model",
            (
                "strict-profile-targeting-stays-bounded-to-fail-closed-feature-"
                "claim-truth-semantics-and-release-gate-artifacts-until-real-"
                "selection-support-lands"
            ),
        ),
    ),
    fixture_paths=fixture_paths(RELEASE_CLAIMABLE_SURFACE_FIXTURE),
    post_fixture_fields=payload_fields(
        payload_field(
            "targeted_profile_ids",
            ("strict", "strict-concurrency", "strict-system"),
        ),
        explicit_non_goals(
            "no-strict-profile-selection-support-claim-yet",
            "no-feature-macro-publication-claim-yet",
            "no-publication-path-bypass-around-native-cli-sidecars",
        ),
        required_truth("requires_conformance_report_artifact"),
        required_truth("requires_conformance_publication_artifact"),
        required_truth("requires_release_gate_artifacts"),
        required_truth("requires_real_compile_output"),
    ),
)


CLAIMABILITY_SEMANTICS_RELEASE_POLICY_RELEASE_CLAIM_GROUP = ReleaseClaimGroup(
    contract_id=RUNTIME_CLAIMABILITY_SEMANTICS_RELEASE_POLICY_SURFACE_CONTRACT_ID,
    case_ids=frozenset({CLAIMABILITY_SEMANTICS_RELEASE_POLICY_CASE_ID}),
    pre_case_fields=payload_fields(
        source_contract_ids(*CLAIMABILITY_POLICY_SOURCE_CONTRACT_IDS),
        compile_artifact_set(*CLAIMABILITY_POLICY_COMPILE_ARTIFACTS),
        authoritative_code_paths(*CLAIMABILITY_POLICY_CODE_PATHS),
        release_model(
            "policy_model",
            (
                "claimed-profile-format-validation-and-release-targeting-policy-"
                "come-from-one-live-driver-process-frontend-source-of-truth-and-"
                "fail-closed-on-drift"
            ),
        ),
    ),
    fixture_paths=fixture_paths(RELEASE_CLAIMABLE_SURFACE_FIXTURE),
    post_fixture_fields=payload_fields(
        payload_field(
            "negative_operator_commands",
            ("--emit-objc3-conformance-format yaml",),
        ),
        explicit_non_goals(
            "no-parallel-claim-policy-table",
            "no-strict-profile-publication-bypass",
            "no-non-json-conformance-publication-claim-yet",
        ),
        required_truth("requires_conformance_validation_artifact"),
        required_truth("requires_release_gate_artifacts"),
        required_truth("requires_real_compile_output"),
    ),
)


STRICT_PROFILE_CLAIM_IMPLEMENTATION_RELEASE_CLAIM_GROUP = ReleaseClaimGroup(
    contract_id=RUNTIME_STRICT_PROFILE_CLAIM_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    case_ids=frozenset({STRICT_PROFILE_CLAIM_IMPLEMENTATION_CASE_ID}),
    pre_case_fields=payload_fields(
        source_contract_ids(*STRICT_PROFILE_IMPLEMENTATION_SOURCE_CONTRACT_IDS),
        compile_artifact_set(*STRICT_PROFILE_IMPLEMENTATION_COMPILE_ARTIFACTS),
        authoritative_code_paths(*STRICT_PROFILE_IMPLEMENTATION_CODE_PATHS),
        release_model(
            "claim_implementation_model",
            (
                "strict-strict-concurrency-and-strict-system-selection-fails-closed-"
                "through-the-same-native-cli-claim-policy-until-runtime-backed-"
                "claim-implementation-lands"
            ),
        ),
    ),
    fixture_paths=fixture_paths(RELEASE_CLAIMABLE_SURFACE_FIXTURE),
    post_fixture_fields=payload_fields(
        payload_field(
            "claimed_profile_ids",
            ("core",),
        ),
        payload_field(
            "rejected_profile_ids",
            ("strict", "strict-concurrency", "strict-system"),
        ),
        explicit_non_goals(
            "no-strict-profile-claim-before-runtime-backed-support",
            "no-non-json-publication-claim-yet",
            "no-optional-feature-overclaim-beyond-runtime-capability-report",
        ),
        required_truth("requires_conformance_validation_artifact"),
        required_truth("requires_real_compile_output"),
    ),
)


RELEASE_CLAIM_GROUPS = (
    CLAIMABLE_RESIDUAL_RELEASE_CLAIM_GROUP,
    STRICT_PROFILE_FEATURE_CLAIM_RELEASE_CLAIM_GROUP,
    CLAIMABILITY_SEMANTICS_RELEASE_POLICY_RELEASE_CLAIM_GROUP,
    STRICT_PROFILE_CLAIM_IMPLEMENTATION_RELEASE_CLAIM_GROUP,
)


__all__ = [
    "CLAIMABILITY_SEMANTICS_RELEASE_POLICY_RELEASE_CLAIM_GROUP",
    "CLAIMABLE_RESIDUAL_RELEASE_CLAIM_GROUP",
    "RELEASE_CLAIM_GROUPS",
    "STRICT_PROFILE_CLAIM_IMPLEMENTATION_RELEASE_CLAIM_GROUP",
    "STRICT_PROFILE_FEATURE_CLAIM_RELEASE_CLAIM_GROUP",
]
