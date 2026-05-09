"""Developer inspection and formatter action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

DEVELOPER_INSPECTION_ACTION_SPECS: dict[str, ActionSpec] = {
    "inspect-capability-explorer": ActionSpec("inspect-capability-explorer", "probe LLVM and backend-routing capability state through the live capability explorer surface", "python:scripts/probe_objc3c_llvm_capabilities.py", validation_tier="repo", guarantee_owner="capability explorer payloads stay tied to the live LLVM probe and backend-routing contracts", pass_through_args=True),
    "inspect-playground-repro": ActionSpec("inspect-playground-repro", "compile one source through the frontend C API runner and dump the playground and repro object", "runner-internal + artifacts/bin/objc3c-frontend-c-api-runner.exe", validation_tier="repo", guarantee_owner="playground and repro payloads stay tied to the real frontend runner summary, emitted artifacts, and executable replay command", pass_through_args=True),
    "inspect-compile-observability": ActionSpec("inspect-compile-observability", "compile one source through the frontend C API runner and dump the structured observability object", "runner-internal + artifacts/bin/objc3c-frontend-c-api-runner.exe", validation_tier="repo", guarantee_owner="developer-facing compile observability stays tied to the real frontend runner summary and emitted artifacts", pass_through_args=True),
    "inspect-runtime-inspector": ActionSpec("inspect-runtime-inspector", "compile one source through the frontend C API runner and dump the runtime inspector object", "runner-internal + artifacts/bin/objc3c-frontend-c-api-runner.exe", validation_tier="repo", guarantee_owner="developer-facing runtime inspection stays tied to the real emitted object artifact and runtime ABI boundary models", pass_through_args=True),
    "inspect-editor-tooling": ActionSpec("inspect-editor-tooling", "compile one source through the real frontend runner and dump the combined editor tooling surface", "python:scripts/build_objc3c_editor_tooling_surface.py", validation_tier="repo", guarantee_owner="editor-facing diagnostics, language-server capabilities, navigation, formatter output, and preview debug anchors stay tied to the real compile summary, diagnostics JSON, manifest declaration coordinates, and emitted object artifacts", pass_through_args=True),
    "format-objc3c": ActionSpec("format-objc3c", "format one objc3c source through the supported preview formatter subset", "python:scripts/format_objc3c_source.py", validation_tier="repo", guarantee_owner="preview formatter output stays fail-closed outside the supported subset and deterministic within it", pass_through_args=True),
}
