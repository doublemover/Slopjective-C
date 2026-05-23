"""Conformance, stress, and external-validation handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import (
    application_surfaces,
    external_validation,
    stress,
)

REPORTING_CONFORMANCE_STRESS_HANDLERS: dict[str, ActionHandler] = {
    "validate-conformance-corpus": application_surfaces.action_validate_conformance_corpus,
    "validate-public-conformance-suite": application_surfaces.action_validate_public_conformance_suite,
    "check-conformance-minima": application_surfaces.action_check_conformance_minima,
    "validate-runnable-conformance-corpus": application_surfaces.action_validate_runnable_conformance_corpus,
    "validate-cross-lane-e2e": application_surfaces.action_validate_cross_lane_e2e,
    "check-stress-surface": stress.action_check_stress_surface,
    "test-fuzz-safety": stress.action_test_fuzz_safety,
    "test-lowering-runtime-stress": stress.action_test_lowering_runtime_stress,
    "test-mixed-module-differential": stress.action_test_mixed_module_differential,
    "test-stress-minimization": stress.action_test_stress_minimization,
    "test-stress-crash-triage": stress.action_test_stress_crash_triage,
    "validate-stress": stress.action_validate_stress,
    "validate-stress-integration": stress.action_validate_stress_integration,
    "validate-stress-end-to-end": stress.action_validate_stress_end_to_end,
    "check-external-validation-surface": external_validation.action_check_external_validation_surface,
    "test-external-validation-replay": external_validation.action_test_external_validation_replay,
    "publish-external-repro-corpus": external_validation.action_publish_external_repro_corpus,
    "check-external-support-claim-gate": external_validation.action_check_external_support_claim_gate,
    "validate-external-validation": external_validation.action_validate_external_validation,
    "validate-external-validation-integration": external_validation.action_validate_external_validation_integration,
}
