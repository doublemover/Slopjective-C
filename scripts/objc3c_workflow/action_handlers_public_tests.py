"""Public test orchestration handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_handlers_defaults import action_test_default
from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import test_orchestration

PUBLIC_TEST_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "test-default": action_test_default,
    "test-behavior-matrix": test_orchestration.action_test_behavior_matrix,
    "test-smoke": test_orchestration.action_test_smoke,
    "test-ci": test_orchestration.action_test_ci,
    "test-recovery": test_orchestration.action_test_recovery,
    "test-compile-wrapper-self-audit": test_orchestration.action_test_compile_wrapper_self_audit,
    "test-llvm-capability-routing": test_orchestration.action_test_llvm_capability_routing,
    "test-execution-smoke": test_orchestration.action_test_execution_smoke,
    "test-hosted-execution-smoke": test_orchestration.action_test_hosted_execution_smoke,
    "test-execution-replay": test_orchestration.action_test_execution_replay,
    "test-execution-replay-focused": test_orchestration.action_test_execution_replay_focused,
    "test-fixture-matrix": test_orchestration.action_test_fixture_matrix,
    "test-negative-expectations": test_orchestration.action_test_negative_expectations,
    "test-full": test_orchestration.action_test_full,
    "test-nightly": test_orchestration.action_test_nightly,
}
