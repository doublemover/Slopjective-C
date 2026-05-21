"""Workspace manifest assembly for public playground materialization."""

from __future__ import annotations

from ..environment import ROOT
from .developer_tooling_paths import PLAYGROUND_WORKSPACE_CONTRACT_ID
from .developer_tooling_playground_editor import workspace_drill_commands
from .developer_tooling_playground_inputs import PlaygroundInvocation
from .developer_tooling_playground_paths import PlaygroundWorkspacePaths


def _object_payload(value: object) -> dict[str, object]:
    return value if isinstance(value, dict) else {}


def build_playground_workspace_payload(
    invocation: PlaygroundInvocation,
    *,
    paths: PlaygroundWorkspacePaths,
    playground_payload: dict[str, object],
    editor_surface_payload: dict[str, object],
    published_paths: dict[str, str],
) -> dict[str, object]:
    formatter_payload = _object_payload(editor_surface_payload.get("formatter"))
    debug_payload = _object_payload(editor_surface_payload.get("debug"))
    navigation_payload = _object_payload(editor_surface_payload.get("navigation"))
    artifact_inspector_payload = _object_payload(
        editor_surface_payload.get("artifact_inspector")
    )
    workspace_index_payload = _object_payload(navigation_payload.get("workspace_index"))

    return {
        "contract_id": PLAYGROUND_WORKSPACE_CONTRACT_ID,
        "schema_version": 1,
        "workspace_id": invocation.workspace_id,
        "source_path": invocation.source_display,
        "workspace_root": paths.workspace_root.relative_to(ROOT).as_posix(),
        "artifact_root": paths.artifact_root.relative_to(ROOT).as_posix(),
        "report_root": paths.report_root.relative_to(ROOT).as_posix(),
        "emit_prefix": "module",
        "summary_path": paths.summary_path.relative_to(ROOT).as_posix(),
        "playground_payload_path": paths.dump_path.relative_to(ROOT).as_posix(),
        "playground_payload_contract_id": playground_payload.get("contract_id"),
        "public_actions": [
            "materialize-playground-workspace",
            "compile-objc3c",
            "inspect-playground-repro",
            "inspect-compile-observability",
            "inspect-editor-tooling",
            "format-objc3c",
            "rewrite-objc3c-source",
            "check-developer-diagnostic-quality",
            "trace-runtime-debug",
            "trace-compile-stages",
            "validate-developer-tooling",
        ],
        "compile_profile": playground_payload.get("compile_profile", {}),
        "artifact_paths": playground_payload.get("artifact_paths", {}),
        "showcase_examples": playground_payload.get("showcase_examples", []),
        "repro_command": playground_payload.get("dump_commands", {}).get(
            "repro_runner",
            "",
        ),
        "editor_tooling": {
            **published_paths,
            "formatted_output_path": formatter_payload.get("formatted_output_path"),
            "format_preview_supported": formatter_payload.get("supported"),
            "workspace_index_digest": workspace_index_payload.get(
                "workspace_index_digest"
            ),
            "workspace_package_count": workspace_index_payload.get("package_count", 0),
            "cross_package_edge_count": workspace_index_payload.get(
                "cross_package_edge_count",
                0,
            ),
            "workspace_index_guardrails_ok": _object_payload(
                workspace_index_payload.get("guardrails")
            ).get("ok"),
            "debugger_model": debug_payload.get("debugger_model", ""),
            "declaration_breakpoint_anchor_count": debug_payload.get(
                "declaration_breakpoint_anchor_count",
                0,
            ),
            "statement_level_stepping": debug_payload.get("statement_level_stepping"),
            "runtime_debug_trace_path": debug_payload.get(
                "runtime_debug_trace_path",
                "",
            ),
            "runtime_debug_trace_command": debug_payload.get(
                "runtime_debug_trace_command",
                "",
            ),
            "runtime_debug_trace_schema": debug_payload.get(
                "runtime_debug_trace_schema",
                "",
            ),
            "runtime_debug_trace_model": debug_payload.get(
                "runtime_debug_trace_model",
                "",
            ),
            "artifact_inspector_supported": artifact_inspector_payload.get(
                "supported"
            ),
            "artifact_inspector_contract_id": artifact_inspector_payload.get(
                "contract_id",
                "",
            ),
            "artifact_inspector_kinds": artifact_inspector_payload.get(
                "inspected_artifact_kinds",
                [],
            ),
        },
        "workspace_drill_commands": workspace_drill_commands(
            invocation.source_display,
            debug_payload,
            workspace_index_path=published_paths.get("workspace_index_path", ""),
            artifact_inspector_path=published_paths.get(
                "artifact_inspector_path",
                "",
            ),
        ),
    }
