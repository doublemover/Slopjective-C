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
RUNNER = ROOT / "scripts/check_objc3c_runnable_storage_reflection_end_to_end.py"
RUNNER_REPORT = ROOT / "tmp/reports/runtime/runnable-storage-reflection-e2e/summary.json"
OUT_DIR = ROOT / "tmp/reports/object-model-closure/live-property-reflection"
JSON_OUT = OUT_DIR / "live_property_ivar_reflection_summary.json"
SUMMARY_CONTRACT_ID = "objc3c.object_model.closure.live.property.ivar.reflection.summary.v1"


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
    if os.environ.get("OBJC3C_SKIP_STORAGE_REFLECTION_E2E_RERUN") != "1":
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
            raise RuntimeError("runnable storage/reflection e2e runner failed")

    runner_report = load_json(RUNNER_REPORT)
    expect(runner_report.get("status") == "PASS", "runnable storage/reflection e2e report did not publish PASS")
    probe_payload = runner_report.get("probe_payload", {})
    box_entry = probe_payload.get("box_entry", {})
    implementation_surface = probe_payload.get("implementation_surface", {})
    current_value = probe_payload.get("current_value_property", {})
    weak_value = probe_payload.get("weak_value_property", {})
    guarded_value = probe_payload.get("guarded_value_property", {})

    expect(box_entry.get("found") == 1, "expected Box class lookup to succeed")
    expect(box_entry.get("runtime_property_accessor_count", 0) >= 5, "expected Box to expose at least five runtime-backed accessors")
    expect(implementation_surface.get("property_registry_ready") == 1, "expected property registry readiness")
    expect(implementation_surface.get("runtime_accessor_dispatch_ready") == 1, "expected accessor dispatch readiness")
    expect(implementation_surface.get("reflection_query_ready") == 1, "expected reflection readiness")
    expect(current_value.get("found") == 1, "expected currentValue property lookup to succeed")
    expect(weak_value.get("ownership_runtime_hook_profile") == "objc-weak-side-table", "expected weakValue hook profile to preserve objc weak side-table handling")
    expect(guarded_value.get("ownership_runtime_hook_profile") == "objc-unowned-safe-guard", "expected guardedValue hook profile to preserve safe-unowned guarding")

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_object_model_closure_live_property_reflection.py",
        "used_existing_storage_reflection_e2e_report": os.environ.get("OBJC3C_SKIP_STORAGE_REFLECTION_E2E_RERUN") == "1",
        "child_report_paths": [repo_rel(RUNNER_REPORT)],
        "probe_payload": probe_payload,
    }
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(JSON_OUT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
