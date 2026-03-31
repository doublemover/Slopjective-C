#!/usr/bin/env python3
from __future__ import annotations

import json
import os
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
RUNNER = ROOT / "scripts/check_objc3c_runnable_object_model_end_to_end.py"
RUNNER_REPORT = ROOT / "tmp/reports/runtime/runnable-object-model-e2e/summary.json"
OUT_DIR = ROOT / "tmp/reports/m319/M319-D002"
JSON_OUT = OUT_DIR / "live_object_model_runtime_summary.json"
SUMMARY_CONTRACT_ID = "objc3c.object_model.closure.live.runtime.summary.v1"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def load_json(path: Path) -> dict[str, Any]:
    expect(path.is_file(), f"expected JSON artifact was not published: {repo_rel(path)}")
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON artifact at {repo_rel(path)} did not contain an object")
    return payload


def main() -> int:
    if os.environ.get("OBJC3C_SKIP_OBJECT_MODEL_E2E_RERUN") != "1":
        result = subprocess.run(
            [sys.executable, str(RUNNER)],
            cwd=ROOT,
            check=False,
            text=True,
            capture_output=True,
        )
        if result.stdout:
            sys.stdout.write(result.stdout)
        if result.stderr:
            sys.stderr.write(result.stderr)
        if result.returncode != 0:
            raise RuntimeError("runnable object-model e2e runner failed")

    runner_report = load_json(RUNNER_REPORT)
    expect(runner_report.get("status") == "PASS", "runnable object-model e2e report did not publish PASS")
    probe_payload = runner_report.get("probe_payload", {})
    aggregate = probe_payload.get("aggregate", {})
    expect(probe_payload.get("widget_found") == 1, "expected Widget class lookup to succeed")
    expect(probe_payload.get("traced_value") == 13, "expected tracedValue to return 13")
    expect(probe_payload.get("count_value") == 41, "expected count to reload 41")
    expect(probe_payload.get("count_property_found") == 1, "expected count property lookup to succeed")
    expect(probe_payload.get("tracer_conforms") == 1, "expected Widget to conform to Tracer")
    expect(aggregate.get("realized_class_count") == 2, "expected aggregate realized class count to report two live classes")
    expect(aggregate.get("reflectable_property_count") == 4, "expected aggregate property count to report four live accessors")
    expect(aggregate.get("attached_category_count") == 1, "expected aggregate category count to report one attached category")

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_object_model_closure_live_runtime.py",
        "used_existing_object_model_e2e_report": os.environ.get("OBJC3C_SKIP_OBJECT_MODEL_E2E_RERUN") == "1",
        "child_report_paths": [repo_rel(RUNNER_REPORT)],
        "probe_payload": probe_payload,
    }
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(JSON_OUT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
