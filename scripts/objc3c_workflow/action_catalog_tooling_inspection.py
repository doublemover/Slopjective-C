"""Developer tooling inspection action specs."""

from __future__ import annotations

from .actions.developer_tooling_dump_contracts import (
    COMPILE_STAGE_TRACE_DUMP,
    DeveloperToolingDumpContract,
)

from .action_spec import ActionSpec


def _dump_action_spec(contract: DeveloperToolingDumpContract) -> ActionSpec:
    return ActionSpec(
        contract.action,
        contract.summary,
        contract.backend,
        validation_tier=contract.validation_tier,
        guarantee_owner=contract.guarantee_owner,
        pass_through_args=contract.pass_through_args,
    )


TOOLING_INSPECTION_ACTION_SPECS: dict[str, ActionSpec] = {
    "inspect-bonus-tool-integration": ActionSpec(
        "inspect-bonus-tool-integration",
        "emit the live bonus-tool integration surface from the build-owned source-of-truth artifact and checked-in showcase/tutorial contracts",
        "runner-internal + tmp/build-objc3c-native/repo_superclean_source_of_truth.json",
        validation_tier="repo",
        guarantee_owner=(
            "bonus-tool integration stays rooted in the build-owned source-of-truth "
            "artifact and checked-in showcase/tutorial contracts"
        ),
    ),
    "inspect-validation-timing": ActionSpec(
        "inspect-validation-timing",
        "build the local validation-speed dashboard from latest generated timing reports",
        "runner-internal + tmp timing reports",
        validation_tier="repo",
        guarantee_owner=(
            "validation timing, warning budgets, child reports, and issue-specific "
            "profiles stay explainable from generated suite reports"
        ),
    ),
    "trace-runtime-debug": ActionSpec(
        "trace-runtime-debug",
        "compose runtime-inspector, compile-stage, and editor debug artifacts into a deterministic runtime debug trace report",
        "python:scripts/build_objc3c_runtime_debug_trace.py",
        validation_tier="repo",
        guarantee_owner=(
            "runtime debug traces stay structured, deterministic, and rooted in "
            "the real runtime inspector, compile-stage trace, and editor debug "
            "artifacts without publishing LLDB or statement-stepping claims"
        ),
        pass_through_args=True,
    ),
    "trace-compile-stages": _dump_action_spec(COMPILE_STAGE_TRACE_DUMP),
}
