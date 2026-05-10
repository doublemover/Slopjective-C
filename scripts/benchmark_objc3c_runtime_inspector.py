#!/usr/bin/env python3
"""Benchmark the live runtime-inspector and capability-explorer workflow."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Sequence
from objc3c_tooling.paths import display_path
from scripts.objc3c_workflow.public_command_api import public_workflow_command
from objc3c_tooling.subprocesses import run_timed
from objc3c_tooling.public_workflow_output import extract_line_value


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_SOURCE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "objc3c-public-workflow" / "runtime-inspector-benchmark.json"
WORKSPACE_CONTRACT_ID = "objc3c.playground.workspace.v1"
BENCHMARK_CONTRACT_ID = "objc3c.runtime.inspector.benchmark.v1"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", nargs="?", default=DEFAULT_SOURCE.relative_to(ROOT).as_posix())
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)



def read_json(path: Path) -> object:
    return json.loads(path.read_text(encoding="utf-8"))




def run_step(name: str, command: list[str]) -> dict[str, object]:
    completed = run_timed(command, cwd=ROOT, echo=True)
    return {
        "name": name,
        "command": command,
        "exit_code": completed.returncode,
        "duration_ms": completed.duration_ms,
        "stdout": completed.stdout,
    }


def maybe_size(path_text: str) -> int:
    if not path_text:
        return 0
    candidate = Path(path_text)
    if not candidate.is_absolute():
        candidate = ROOT / candidate
    if not candidate.is_file():
        return 0
    return candidate.stat().st_size


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    args = parse_args(sys.argv[1:])
    steps = [
        run_step(
            "materialize-playground-workspace",
            public_workflow_command("materialize-playground-workspace", args.source),
        ),
        run_step(
            "inspect-runtime-inspector",
            public_workflow_command("inspect-runtime-inspector", args.source),
        ),
        run_step(
            "inspect-capability-explorer",
            public_workflow_command("inspect-capability-explorer"),
        ),
    ]

    failures: list[str] = []
    for step in steps:
        expect(step["exit_code"] == 0, f"{step['name']} failed", failures)

    workspace_path_text = extract_line_value(str(steps[0]["stdout"]), "workspace_path:")
    runtime_inspector_path = ROOT / "tmp" / "reports" / "objc3c-public-workflow" / "runtime-inspector.json"
    capability_explorer_path = ROOT / "tmp" / "reports" / "objc3c-public-workflow" / "capability-explorer.json"
    workspace_path = ROOT / workspace_path_text if workspace_path_text else Path()

    expect(workspace_path_text != "", "materialize-playground-workspace did not publish workspace_path", failures)
    expect(workspace_path.is_file(), f"missing playground workspace report: {workspace_path_text}", failures)
    expect(runtime_inspector_path.is_file(), "missing runtime inspector report", failures)
    expect(capability_explorer_path.is_file(), "missing capability explorer report", failures)

    workspace = read_json(workspace_path) if workspace_path.is_file() else {}
    runtime_inspector = read_json(runtime_inspector_path) if runtime_inspector_path.is_file() else {}
    capability_explorer = read_json(capability_explorer_path) if capability_explorer_path.is_file() else {}

    expect(workspace.get("contract_id") == WORKSPACE_CONTRACT_ID, "unexpected playground workspace contract id", failures)
    expect(runtime_inspector.get("contract_id") == "objc3c.runtime.metadata.object.inspection.harness.v1", "unexpected runtime inspector contract id", failures)
    expect(capability_explorer.get("mode") == "objc3c-llvm-capabilities-v2", "unexpected capability explorer mode", failures)
    expect(capability_explorer.get("ok") is True, "capability explorer did not report ok=true", failures)
    expect(capability_explorer.get("sema_type_system_parity", {}).get("parity_ready") is True, "capability explorer parity_ready drifted", failures)
    expect(capability_explorer.get("capability_demo_compatibility", {}).get("ok") is True, "capability explorer capability_demo_compatibility drifted", failures)
    expect(capability_explorer.get("capability_demo_compatibility", {}).get("drift_checks", {}).get("story_capabilities_match") is True, "capability explorer story capability drift check failed", failures)
    expect(float(capability_explorer.get("clang", {}).get("version_duration_ms", 0.0)) > 0.0, "clang probe timing was not recorded", failures)
    expect(float(capability_explorer.get("llc", {}).get("version_duration_ms", 0.0)) > 0.0, "llc probe timing was not recorded", failures)

    object_path = str(runtime_inspector.get("object_path", ""))
    object_size_bytes = maybe_size(object_path)
    expect(object_size_bytes > 0, "runtime inspector object artifact missing or empty", failures)

    payload = {
        "contract_id": BENCHMARK_CONTRACT_ID,
        "schema_version": 1,
        "ok": not failures,
        "failures": failures,
        "source_path": workspace.get("source_path", args.source),
        "workspace_path": workspace_path_text,
        "reports": {
            "workspace": workspace_path_text,
            "runtime_inspector": display_path(runtime_inspector_path),
            "capability_explorer": display_path(capability_explorer_path),
        },
        "measurements": {
            "materialize_playground_workspace_ms": steps[0]["duration_ms"],
            "inspect_runtime_inspector_ms": steps[1]["duration_ms"],
            "inspect_capability_explorer_ms": steps[2]["duration_ms"],
            "runtime_inspector_object_size_bytes": object_size_bytes,
            "capability_probe_durations_ms": {
                "clang_version": capability_explorer.get("clang", {}).get("version_duration_ms", 0.0),
                "llc_version": capability_explorer.get("llc", {}).get("version_duration_ms", 0.0),
                "llc_help": capability_explorer.get("llc_features", {}).get("help_duration_ms", 0.0),
                "llc_filetype_obj": capability_explorer.get("llc_features", {}).get("version_with_filetype_duration_ms", 0.0),
            },
        },
        "reproducibility_model": {
            "timings_capture_model": "single-run wall-clock timings over public workflow actions",
            "comparison_fields": [
                "source_path",
                "runtime_inspector_object_size_bytes",
                "capability_probe_durations_ms.clang_version",
                "capability_probe_durations_ms.llc_version",
            ],
        },
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {display_path(args.summary_out)}")
    if failures:
        print("runtime-inspector-benchmark: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("runtime-inspector-benchmark: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
