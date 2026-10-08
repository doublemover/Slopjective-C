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
from .actions.developer_tooling_llvm_contract_constants import FRONTEND_RUNNER_BACKEND

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
    "inspect-playground-repro": ActionSpec("inspect-playground-repro", "compile one source through the frontend C API runner and dump the playground and repro object", FRONTEND_RUNNER_BACKEND, validation_tier="repo", guarantee_owner="playground and repro payloads stay tied to the real frontend runner summary, emitted artifacts, and executable replay command", pass_through_args=True),
    "inspect-compile-observability": _dump_action_spec(COMPILE_OBSERVABILITY_DUMP),
    "inspect-runtime-inspector": _dump_action_spec(RUNTIME_INSPECTOR_DUMP),
    "inspect-artifact": ActionSpec("inspect-artifact", "compile one source through the real frontend runner and dump the fail-closed object and runtime artifact inventory", "python:scripts/build_objc3c_editor_tooling_surface.py --artifact-inspector-only", validation_tier="repo", guarantee_owner="object file digests, symbol tables, runtime metadata, package identities, receipts, trust rows, and source/debug artifact links stay tied to real emitted artifacts", pass_through_args=True),
    "inspect-editor-tooling": ActionSpec("inspect-editor-tooling", "compile one source through the real frontend runner and dump the combined editor tooling surface", "python:scripts/build_objc3c_editor_tooling_surface.py", validation_tier="repo", guarantee_owner="editor-facing diagnostics, language-server capabilities, navigation, formatter output, and preview debug anchors stay tied to the real compile summary, diagnostics JSON, manifest declaration coordinates, and emitted object artifacts", pass_through_args=True),
    "inspect-source-graph": ActionSpec("inspect-source-graph", "compile one source through the real frontend runner and dump the compiler/tooling source graph artifact", "python:scripts/build_objc3c_editor_tooling_surface.py --source-graph-only", validation_tier="repo", guarantee_owner="source graph nodes, spans, package provenance, fail-closed reference candidates, rename diagnostics, and semantic-token consumers stay tied to real compile artifacts and checked workspace guardrails", pass_through_args=True),
    "inspect-language-service": ActionSpec("inspect-language-service", "compile one source through the real frontend runner and replay Objective-C 3 language-service requests over the source graph", "python:scripts/build_objc3c_language_service_surface.py", validation_tier="repo", guarantee_owner="language-service diagnostics, hover, declaration-coordinate definitions, document symbols, workspace symbols, lifecycle invalidation, and unsupported request failures stay tied to source graph evidence", pass_through_args=True),
    "format-objc3c": ActionSpec("format-objc3c", "format one objc3c source through the supported canonical Objective-C 3 source subset", "python:scripts/format_objc3c_source.py", validation_tier="repo", guarantee_owner="formatter output stays fail-closed on malformed source and deterministic across canonical Objective-C 3 declarations, actor, await, message-send, block/comment, and string surfaces", pass_through_args=True),
    "rewrite-objc3c-source": ActionSpec("rewrite-objc3c-source", "apply deterministic safe Objective-C 3 source rewrites without touching strings or comments", "python:scripts/rewrite_objc3c_source.py", validation_tier="repo", guarantee_owner="source rewrite edits stay token-boundary safe, deterministic, non-overlapping, and fail-closed on malformed source", pass_through_args=True),
    "analyze-migration-source": ActionSpec("analyze-migration-source", "analyze checked-in Objective-C 2, Swift, and C++ migration input contracts and emit fail-closed diagnostics plus rewrite planning JSON", "python:scripts/analyze_objc3c_migration.py", validation_tier="repo", guarantee_owner="migration analyzer claims stay rooted in checked-in source contracts, required interop surfaces, packaged execution evidence, and fail-closed diagnostics", pass_through_args=True),
    "rewrite-migration-source": ActionSpec("rewrite-migration-source", "apply the safe automatic portion of a checked migration rewrite plan and emit a rewrite report", "python:scripts/rewrite_objc3c_migration.py", validation_tier="repo", guarantee_owner="migration rewrite output stays derived from the analyzer report, applies only token-boundary or import-line edits, and reports manual migration surfaces without support overclaiming", pass_through_args=True),
    "validate-migration-workflow": ActionSpec("validate-migration-workflow", "validate migration analyzer contracts, positive and negative fixtures, rewrite reports, and public workflow action wiring", "python:scripts/check_objc3c_migration_workflow.py", validation_tier="repo", guarantee_owner="ObjC2 Swift C++ migration analyzer and rewrite workflow remain backed by checked-in contracts and runnable validation"),
    "check-developer-diagnostic-quality": ActionSpec("check-developer-diagnostic-quality", "validate developer-facing diagnostic taxonomy and machine-applicable fix-it quality", "python:scripts/check_developer_tooling_diagnostic_quality.py", validation_tier="repo", guarantee_owner="diagnostic taxonomy and fix-it claims stay backed by structured checked-in diagnostic fixtures"),
    "check-developer-tooling-editor-source-truth": ActionSpec("check-developer-tooling-editor-source-truth", "validate #8170 formatter, LSP, workspace-index, artifact-inspector source truth and public command wiring", "python:scripts/check_developer_tooling_editor_source_truth.py", validation_tier="repo", guarantee_owner="editor tooling claims stay rooted in checked-in source contracts, capability rows, evidence-map rows, and registered npm bridge commands"),
}
