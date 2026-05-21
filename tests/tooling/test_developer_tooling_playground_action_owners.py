from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow.actions.developer_tooling_playground_inputs import (
    PlaygroundInvocation,
)
from scripts.objc3c_workflow.actions.developer_tooling_playground_manifest import (
    build_playground_workspace_payload,
)
from scripts.objc3c_workflow.actions.developer_tooling_playground_paths import (
    PlaygroundWorkspacePaths,
)

ROOT = Path(__file__).resolve().parents[2]
ACTION_ROOT = ROOT / "scripts" / "objc3c_workflow" / "actions"

PLAYGROUND_OWNER_MODULES = (
    "developer_tooling_playground_inputs",
    "developer_tooling_playground_paths",
    "developer_tooling_playground_runner",
    "developer_tooling_playground_editor",
    "developer_tooling_playground_manifest",
    "developer_tooling_playground_workspace",
)


def test_playground_action_owner_modules_are_explicit() -> None:
    for module_name in PLAYGROUND_OWNER_MODULES:
        assert importlib.import_module(
            f"scripts.objc3c_workflow.actions.{module_name}"
        )


def test_playground_workspace_orchestrator_delegates_owned_details() -> None:
    source = (ACTION_ROOT / "developer_tooling_playground_workspace.py").read_text(
        encoding="utf-8"
    )

    assert "from .developer_tooling_playground_paths import" in source
    assert "from .developer_tooling_playground_editor import" in source
    assert "from .developer_tooling_playground_manifest import" in source
    assert "PLAYGROUND_ARTIFACT_ROOT" not in source
    assert "PLAYGROUND_REPORT_ROOT" not in source
    assert "WORKFLOW_COMMAND_TEXT" not in source
    assert "load_json_object" not in source


def test_playground_manifest_preserves_public_workspace_shape() -> None:
    invocation = PlaygroundInvocation(
        source_path=ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3",
        source_display="tests/tooling/fixtures/native/hello.objc3",
        passthrough=[],
        workspace_id="hello-3bb3df22f2ea",
    )
    paths = PlaygroundWorkspacePaths(
        workspace_root=ROOT / "tmp" / "artifacts" / "playground" / "hello-3bb3df22f2ea",
        artifact_root=ROOT
        / "tmp"
        / "artifacts"
        / "playground"
        / "hello-3bb3df22f2ea"
        / "build",
        report_root=ROOT / "tmp" / "reports" / "playground" / "hello-3bb3df22f2ea",
        workspace_manifest_path=ROOT
        / "tmp"
        / "artifacts"
        / "playground"
        / "hello-3bb3df22f2ea"
        / "workspace.json",
        summary_path=ROOT
        / "tmp"
        / "reports"
        / "playground"
        / "hello-3bb3df22f2ea"
        / "compile-summary.json",
        dump_path=ROOT
        / "tmp"
        / "reports"
        / "playground"
        / "hello-3bb3df22f2ea"
        / "playground-repro.json",
    )

    payload = build_playground_workspace_payload(
        invocation,
        paths=paths,
        playground_payload={
            "contract_id": "objc3c.playground.repro.surface.v1",
            "compile_profile": {"backend": "native"},
            "artifact_paths": {"manifest": "tmp/artifacts/playground/build/module.json"},
            "showcase_examples": ["showcase/auroraBoard/main.objc3"],
            "dump_commands": {
                "repro_runner": "npm run objc3c -- inspect-playground-repro"
            },
        },
        editor_surface_payload={
            "navigation": {
                "workspace_index": {
                    "workspace_index_digest": "abc123",
                    "package_count": 9,
                    "cross_package_edge_count": 7,
                    "guardrails": {"ok": True},
                }
            },
            "formatter": {
                "supported": True,
                "formatted_output_path": "tmp/formatted.objc3",
            },
            "debug": {
                "debugger_model": "declaration-breakpoint-and-object-symbol-inspection",
                "declaration_breakpoint_anchor_count": 2,
                "statement_level_stepping": False,
                "runtime_debug_trace_command": "npm run objc3c -- trace-runtime-debug",
                "runtime_debug_trace_path": "tmp/reports/objc3c-public-workflow/runtime-debug-trace.json",
                "runtime_debug_trace_schema": "schemas/objc3c-runtime-debug-trace-v1.schema.json",
                "runtime_debug_trace_model": "deterministic-runtime-inspector-and-editor-debug-artifact-trace",
                "object_symbol_inventory_command": "npm run objc3c -- inspect-runtime",
            },
        },
        published_paths={
            "editor_surface_path": "tmp/reports/editor.json",
            "workspace_index_path": "tmp/reports/workspace-index.json",
        },
    )

    assert payload["contract_id"] == "objc3c.playground.workspace.v1"
    assert payload["public_actions"] == [
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
    ]
    assert payload["workspace_root"] == (
        "tmp/artifacts/playground/hello-3bb3df22f2ea"
    )
    assert payload["editor_tooling"]["format_preview_supported"] is True
    assert payload["editor_tooling"]["workspace_index_digest"] == "abc123"
    assert payload["editor_tooling"]["workspace_package_count"] == 9
    assert payload["workspace_drill_commands"]["workspace_navigation_index"] == (
        "Get-Content -Raw 'tmp/reports/workspace-index.json'"
    )
    assert payload["workspace_drill_commands"]["runtime_debug_trace"] == (
        "npm run objc3c -- trace-runtime-debug"
    )
    assert payload["editor_tooling"]["runtime_debug_trace_schema"] == (
        "schemas/objc3c-runtime-debug-trace-v1.schema.json"
    )
    assert payload["editor_tooling"]["runtime_debug_trace_model"] == (
        "deterministic-runtime-inspector-and-editor-debug-artifact-trace"
    )
    assert payload["workspace_drill_commands"]["object_symbol_inventory"] == (
        "npm run objc3c -- inspect-runtime"
    )
