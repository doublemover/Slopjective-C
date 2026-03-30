#!/usr/bin/env python3
"""Validate the live developer-tooling inspect and trace workflow."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
PUBLIC_RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
PUBLIC_WORKFLOW_REPORT_ROOT = ROOT / "tmp" / "reports" / "objc3c-public-workflow"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "developer-tooling" / "integration-summary.json"


def run_step(name: str, command: list[str]) -> dict[str, object]:
    completed = subprocess.run(
        command,
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if completed.stdout:
        sys.stdout.write(completed.stdout)
    if completed.stderr:
        sys.stderr.write(completed.stderr)
    return {
        "name": name,
        "command": command,
        "exit_code": completed.returncode,
    }


def read_json(path: Path) -> object:
    return json.loads(path.read_text(encoding="utf-8"))


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    steps = [
        run_step(
            "inspect-compile-observability",
            [sys.executable, str(PUBLIC_RUNNER), "inspect-compile-observability"],
        ),
        run_step(
            "inspect-runtime-inspector",
            [sys.executable, str(PUBLIC_RUNNER), "inspect-runtime-inspector"],
        ),
        run_step(
            "inspect-capability-explorer",
            [sys.executable, str(PUBLIC_RUNNER), "inspect-capability-explorer"],
        ),
        run_step(
            "benchmark-runtime-inspector",
            [sys.executable, str(PUBLIC_RUNNER), "benchmark-runtime-inspector"],
        ),
        run_step(
            "trace-compile-stages",
            [sys.executable, str(PUBLIC_RUNNER), "trace-compile-stages"],
        ),
    ]
    failures: list[str] = []
    for step in steps:
        expect(step["exit_code"] == 0, f"{step['name']} failed", failures)
    observability_path = PUBLIC_WORKFLOW_REPORT_ROOT / "compile-observability.json"
    runtime_inspector_path = PUBLIC_WORKFLOW_REPORT_ROOT / "runtime-inspector.json"
    capability_explorer_path = PUBLIC_WORKFLOW_REPORT_ROOT / "capability-explorer.json"
    runtime_inspector_benchmark_path = PUBLIC_WORKFLOW_REPORT_ROOT / "runtime-inspector-benchmark.json"
    stage_trace_path = PUBLIC_WORKFLOW_REPORT_ROOT / "compile-stage-trace.json"
    for path in (observability_path, runtime_inspector_path, capability_explorer_path, runtime_inspector_benchmark_path, stage_trace_path):
        expect(path.is_file(), f"missing expected report: {path.relative_to(ROOT).as_posix()}", failures)
    observability = read_json(observability_path) if observability_path.is_file() else {}
    runtime_inspector = read_json(runtime_inspector_path) if runtime_inspector_path.is_file() else {}
    capability_explorer = read_json(capability_explorer_path) if capability_explorer_path.is_file() else {}
    runtime_inspector_benchmark = read_json(runtime_inspector_benchmark_path) if runtime_inspector_benchmark_path.is_file() else {}
    stage_trace = read_json(stage_trace_path) if stage_trace_path.is_file() else {}
    expect(observability.get("status_name") == "ok", "expected compile observability status_name=ok", failures)
    expect("summary" in observability.get("dump_commands", {}), "expected compile observability dump_commands.summary", failures)
    expect(runtime_inspector.get("contract_id") == "objc3c.runtime.metadata.object.inspection.harness.v1", "expected runtime inspector contract id", failures)
    expect(runtime_inspector.get("arc_debug_state_snapshot_symbol") == "objc3_runtime_copy_arc_debug_state_for_testing", "expected runtime inspector ARC debug snapshot symbol", failures)
    expect("object_sections" in runtime_inspector.get("dump_commands", {}), "expected runtime inspector object_sections dump command", failures)
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
    expect(runtime_inspector_benchmark.get("contract_id") == "objc3c.runtime.inspector.benchmark.v1", "expected runtime inspector benchmark contract id", failures)
    expect(runtime_inspector_benchmark.get("ok") is True, "expected runtime inspector benchmark ok=true", failures)
    expect(float(runtime_inspector_benchmark.get("measurements", {}).get("inspect_runtime_inspector_ms", 0.0)) > 0.0, "expected runtime inspector benchmark timing to be recorded", failures)
    expect(float(runtime_inspector_benchmark.get("measurements", {}).get("inspect_capability_explorer_ms", 0.0)) > 0.0, "expected capability explorer benchmark timing to be recorded", failures)
    expect(stage_trace.get("mode") == "objc3c-frontend-stage-trace-v1", "expected stage trace mode", failures)
    expect(stage_trace.get("stages", {}).get("emit", {}).get("attempted") is True, "expected emit stage trace attempted=true", failures)
    expect(stage_trace.get("stages", {}).get("lex", {}).get("stage") == 0, "expected lex stage ordinal 0", failures)
    payload = {
        "mode": "objc3c-developer-tooling-integration-v1",
        "ok": not failures,
        "failures": failures,
        "steps": steps,
        "reports": {
            "compile_observability": observability_path.relative_to(ROOT).as_posix(),
            "runtime_inspector": runtime_inspector_path.relative_to(ROOT).as_posix(),
            "capability_explorer": capability_explorer_path.relative_to(ROOT).as_posix(),
            "runtime_inspector_benchmark": runtime_inspector_benchmark_path.relative_to(ROOT).as_posix(),
            "compile_stage_trace": stage_trace_path.relative_to(ROOT).as_posix(),
        },
    }
    SUMMARY_OUT.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {SUMMARY_OUT.relative_to(ROOT).as_posix()}")
    if failures:
        print("developer-tooling-integration: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("developer-tooling-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
