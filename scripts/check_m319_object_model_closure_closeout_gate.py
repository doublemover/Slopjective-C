#!/usr/bin/env python3
from __future__ import annotations

import json
import os
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
OUT_DIR = ROOT / "tmp/reports/m319/M319-E001"
JSON_OUT = OUT_DIR / "object_model_closure_closeout_gate.json"
REQUIRED_REPORTS = [
    ROOT / "tmp/reports/m319/M319-A001/object_model_closure_boundary_inventory_summary.json",
    ROOT / "tmp/reports/m319/M319-B001/realized_object_graph_reflection_semantic_summary.json",
    ROOT / "tmp/reports/m319/M319-B002/loader_category_protocol_workload_summary.json",
    ROOT / "tmp/reports/m319/M319-B003/realized_object_graph_runtime_implementation_summary.json",
    ROOT / "tmp/reports/m319/M319-C001/object_model_reflection_artifact_runtime_registration_summary.json",
    ROOT / "tmp/reports/m319/M319-C002/class_category_protocol_realization_lowering_summary.json",
    ROOT / "tmp/reports/m319/M319-C003/property_ivar_aggregate_reflection_artifact_summary.json",
    ROOT / "tmp/reports/m319/M319-D001/object_model_closure_executable_proof_summary.json",
    ROOT / "tmp/reports/m319/M319-D002/live_object_model_runtime_summary.json",
    ROOT / "tmp/reports/m319/M319-D003/live_property_ivar_reflection_summary.json",
]


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def run_capture(command: Sequence[str], env: dict[str, str] | None = None) -> None:
    merged_env = os.environ.copy()
    if env:
        merged_env.update(env)
    result = subprocess.run(
        list(command),
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
        env=merged_env,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    if result.returncode != 0:
        raise RuntimeError(f"command failed: {' '.join(command)}")


def load_json(path: Path) -> dict[str, Any]:
    expect(path.is_file(), f"expected JSON artifact was not published: {repo_rel(path)}")
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON artifact at {repo_rel(path)} did not contain an object")
    return payload


def main() -> int:
    run_capture([sys.executable, str(ROOT / "scripts/build_object_model_closure_executable_proof_summary.py")])
    run_capture(
        [sys.executable, str(ROOT / "scripts/check_object_model_closure_live_runtime.py")],
        env={"OBJC3C_SKIP_OBJECT_MODEL_E2E_RERUN": "1"},
    )
    run_capture(
        [sys.executable, str(ROOT / "scripts/check_object_model_closure_live_property_reflection.py")],
        env={"OBJC3C_SKIP_STORAGE_REFLECTION_E2E_RERUN": "1"},
    )
    run_capture([sys.executable, str(ROOT / "scripts/check_documentation_surface.py")])
    run_capture([sys.executable, str(ROOT / "scripts/check_repo_superclean_surface.py")])

    report_statuses: dict[str, str] = {}
    for path in REQUIRED_REPORTS:
        payload = load_json(path)
        status = str(payload.get("status", "PASS"))
        report_statuses[repo_rel(path)] = status
        expect(status == "PASS", f"required report did not publish PASS: {repo_rel(path)}")

    payload = {
        "contract_id": "objc3c.object_model.closure.closeout.gate.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_m319_object_model_closure_closeout_gate.py",
        "required_report_count": len(REQUIRED_REPORTS),
        "report_statuses": report_statuses,
    }
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(JSON_OUT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
