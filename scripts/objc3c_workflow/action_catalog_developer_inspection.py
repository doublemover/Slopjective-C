"""Developer inspection and formatter action specs."""

from __future__ import annotations

from .actions.developer_tooling_dump_contracts import (
    COMPILE_OBSERVABILITY_DUMP,
    DeveloperToolingDumpContract,
    RUNTIME_INSPECTOR_DUMP,
)
from .actions.developer_tooling_llvm_contracts import (
    CAPABILITY_EXPLORER_CONTRACT,
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


def _capability_explorer_spec() -> ActionSpec:
    contract = CAPABILITY_EXPLORER_CONTRACT
    return ActionSpec(
        contract.action,
        contract.summary,
        contract.backend,
        validation_tier=contract.validation_tier,
        guarantee_owner=contract.guarantee_owner,
        pass_through_args=contract.pass_through_args,
    )


DEVELOPER_INSPECTION_ACTION_SPECS: dict[str, ActionSpec] = {
    "inspect-capability-explorer": _capability_explorer_spec(),
    "inspect-playground-repro": ActionSpec("inspect-playground-repro", "compile one source through the frontend C API runner and dump the playground and repro object", "runner-internal + artifacts/bin/objc3c-frontend-c-api-runner.exe", validation_tier="repo", guarantee_owner="playground and repro payloads stay tied to the real frontend runner summary, emitted artifacts, and executable replay command", pass_through_args=True),
    "inspect-compile-observability": _dump_action_spec(COMPILE_OBSERVABILITY_DUMP),
    "inspect-runtime-inspector": _dump_action_spec(RUNTIME_INSPECTOR_DUMP),
    "inspect-editor-tooling": ActionSpec("inspect-editor-tooling", "compile one source through the real frontend runner and dump the combined editor tooling surface", "python:scripts/build_objc3c_editor_tooling_surface.py", validation_tier="repo", guarantee_owner="editor-facing diagnostics, language-server capabilities, navigation, formatter output, and preview debug anchors stay tied to the real compile summary, diagnostics JSON, manifest declaration coordinates, and emitted object artifacts", pass_through_args=True),
    "format-objc3c": ActionSpec("format-objc3c", "format one objc3c source through the supported preview formatter subset", "python:scripts/format_objc3c_source.py", validation_tier="repo", guarantee_owner="preview formatter output stays fail-closed outside the supported subset and deterministic within it", pass_through_args=True),
}
