"""Semantic checks for developer-tooling integration reports."""

from __future__ import annotations

from typing import Any

from .assertions import expect


def assert_observability_report(observability: dict[str, Any], failures: list[str]) -> None:
    expect(observability.get("status_name") == "ok", "expected compile observability status_name=ok", failures)
    expect("summary" in observability.get("dump_commands", {}), "expected compile observability dump_commands.summary", failures)


def assert_runtime_inspector_report(runtime_inspector: dict[str, Any], failures: list[str]) -> None:
    expect(runtime_inspector.get("contract_id") == "objc3c.runtime.metadata.object.inspection.harness.v1", "expected runtime inspector contract id", failures)
    expect(runtime_inspector.get("arc_debug_state_snapshot_symbol") == "objc3_runtime_copy_arc_debug_state_for_testing", "expected runtime inspector ARC debug snapshot symbol", failures)
    expect("object_sections" in runtime_inspector.get("dump_commands", {}), "expected runtime inspector object_sections dump command", failures)


def assert_editor_tooling_reports(
    editor_surface: dict[str, Any],
    formatter_debug_summary: dict[str, Any],
    formatter_rewrite_summary: dict[str, Any],
    diagnostic_quality_summary: dict[str, Any],
    workspace_integration_summary: dict[str, Any],
    failures: list[str],
) -> None:
    expect(editor_surface.get("formatter", {}).get("supported") is True, "expected editor tooling formatter surface to report supported=true", failures)
    expect(editor_surface.get("debug", {}).get("supported") is True, "expected editor tooling debug surface to report supported=true", failures)
    expect(editor_surface.get("artifact_inspector", {}).get("supported") is True, "expected editor tooling artifact inspector surface to report supported=true", failures)
    expect(editor_surface.get("debug", {}).get("statement_level_stepping") is False, "expected editor tooling debug surface to keep statement stepping fail-closed", failures)
    workspace_index = editor_surface.get("navigation", {}).get("workspace_index", {})
    expect(workspace_index.get("available") is True, "expected editor tooling workspace index available=true", failures)
    expect(int(workspace_index.get("package_count", 0)) >= 9, "expected editor tooling workspace index to include stdlib and showcase packages", failures)
    expect(workspace_index.get("guardrails", {}).get("ok") is True, "expected editor tooling workspace package guardrails ok=true", failures)
    expect(formatter_debug_summary.get("ok") is True, "expected formatter/debug surface validation ok=true", failures)
    expect(formatter_rewrite_summary.get("ok") is True, "expected formatter/rewrite surface validation ok=true", failures)
    expect(diagnostic_quality_summary.get("ok") is True, "expected diagnostic quality validation ok=true", failures)
    expect(workspace_integration_summary.get("ok") is True, "expected workspace editor/debug integration ok=true", failures)


def assert_capability_explorer_report(capability_explorer: dict[str, Any], failures: list[str]) -> None:
    expect(capability_explorer.get("mode") == "objc3c-llvm-capabilities-v2", "expected capability explorer mode", failures)
    expect(capability_explorer.get("ok") is True, "expected capability explorer ok=true", failures)
    expect(capability_explorer.get("clang", {}).get("found") is True, "expected capability explorer clang probe to succeed", failures)
    expect(capability_explorer.get("llc", {}).get("found") is True, "expected capability explorer llc probe to succeed", failures)
    expect(capability_explorer.get("sema_type_system_parity", {}).get("parity_ready") is True, "expected capability explorer parity_ready=true", failures)
    expect(capability_explorer.get("capability_demo_compatibility", {}).get("ok") is True, "expected capability explorer capability_demo_compatibility ok=true", failures)
    expect(capability_explorer.get("capability_demo_compatibility", {}).get("drift_checks", {}).get("actor_claims_are_qualified") is True, "expected capability explorer to keep actor claims qualified", failures)
    expect(
        [entry.get("id") for entry in capability_explorer.get("capability_demo_compatibility", {}).get("examples", [])]
        == ["auroraBoard", "signalMesh", "patchKit"],
        "expected capability explorer example ids to match the capability demo portfolio",
        failures,
    )
    expect(float(capability_explorer.get("clang", {}).get("version_duration_ms", 0.0)) > 0.0, "expected capability explorer clang timing to be recorded", failures)
    expect(float(capability_explorer.get("llc", {}).get("version_duration_ms", 0.0)) > 0.0, "expected capability explorer llc timing to be recorded", failures)


def assert_runtime_inspector_benchmark_report(runtime_inspector_benchmark: dict[str, Any], failures: list[str]) -> None:
    expect(runtime_inspector_benchmark.get("contract_id") == "objc3c.runtime.inspector.benchmark.v1", "expected runtime inspector benchmark contract id", failures)
    expect(runtime_inspector_benchmark.get("ok") is True, "expected runtime inspector benchmark ok=true", failures)
    expect(float(runtime_inspector_benchmark.get("measurements", {}).get("inspect_runtime_inspector_ms", 0.0)) > 0.0, "expected runtime inspector benchmark timing to be recorded", failures)
    expect(float(runtime_inspector_benchmark.get("measurements", {}).get("inspect_capability_explorer_ms", 0.0)) > 0.0, "expected capability explorer benchmark timing to be recorded", failures)


def assert_stage_trace_report(stage_trace: dict[str, Any], failures: list[str]) -> None:
    expect(stage_trace.get("mode") == "objc3c-frontend-stage-trace-v1", "expected stage trace mode", failures)
    expect(stage_trace.get("stages", {}).get("emit", {}).get("attempted") is True, "expected emit stage trace attempted=true", failures)
    expect(stage_trace.get("stages", {}).get("lex", {}).get("stage") == 0, "expected lex stage ordinal 0", failures)


def assert_loaded_reports(reports: dict[str, Any], failures: list[str]) -> None:
    assert_observability_report(reports["compile_observability"], failures)
    assert_runtime_inspector_report(reports["runtime_inspector"], failures)
    assert_editor_tooling_reports(
        reports["editor_surface"],
        reports["formatter_debug_summary"],
        reports["formatter_rewrite_summary"],
        reports["diagnostic_quality_summary"],
        reports["workspace_integration_summary"],
        failures,
    )
    assert_capability_explorer_report(reports["capability_explorer"], failures)
    assert_runtime_inspector_benchmark_report(reports["runtime_inspector_benchmark"], failures)
    assert_stage_trace_report(reports["compile_stage_trace"], failures)
