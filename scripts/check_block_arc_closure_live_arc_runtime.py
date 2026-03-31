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
RUNNER = ROOT / "scripts/check_objc3c_runnable_block_arc_end_to_end.py"
RUNNER_REPORT = ROOT / "tmp/reports/runtime/runnable-block-arc-e2e/summary.json"
OUT_DIR = ROOT / "tmp/reports/m320/M320-D003"
JSON_OUT = OUT_DIR / "live_arc_automation_ownership_summary.json"
SUMMARY_CONTRACT_ID = "objc3c.block_arc.closure.live.arc.automation.ownership.summary.v1"


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
    if os.environ.get("OBJC3C_SKIP_BLOCK_ARC_E2E_RERUN") != "1":
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
            raise RuntimeError("runnable block/ARC e2e runner failed")

    runner_report = load_json(RUNNER_REPORT)
    expect(runner_report.get("status") == "PASS", "runnable block/ARC e2e report did not publish PASS")

    runtime_abi_probe_payload = runner_report.get("runtime_abi_probe_payload", {})
    child_report_paths = runner_report.get("child_report_paths", [])
    steps = runner_report.get("steps", [])

    expect(runtime_abi_probe_payload.get("retained") == 77, "expected packaged block ARC runtime ABI probe to preserve retained 77")
    expect(runtime_abi_probe_payload.get("autoreleased") == 77, "expected packaged block ARC runtime ABI probe to preserve autoreleased 77")
    expect(runtime_abi_probe_payload.get("released") == 77, "expected packaged block ARC runtime ABI probe to preserve released 77")
    expect(runtime_abi_probe_payload.get("retain_call_count") == 2, "expected packaged block ARC runtime ABI probe to preserve two retain calls")
    expect(runtime_abi_probe_payload.get("release_call_count") == 3, "expected packaged block ARC runtime ABI probe to preserve three release calls")
    expect(runtime_abi_probe_payload.get("autorelease_call_count") == 1, "expected packaged block ARC runtime ABI probe to preserve one autorelease call")
    expect(runtime_abi_probe_payload.get("autoreleasepool_push_count") == 1, "expected packaged block ARC runtime ABI probe to preserve one autoreleasepool push")
    expect(runtime_abi_probe_payload.get("autoreleasepool_pop_count") == 1, "expected packaged block ARC runtime ABI probe to preserve one autoreleasepool pop")
    expect(runtime_abi_probe_payload.get("arc_retain_call_count") == 2, "expected ARC debug state to preserve two retain calls")
    expect(runtime_abi_probe_payload.get("arc_release_call_count") == 3, "expected ARC debug state to preserve three release calls")
    expect(runtime_abi_probe_payload.get("arc_autorelease_call_count") == 1, "expected ARC debug state to preserve one autorelease call")
    expect(runtime_abi_probe_payload.get("arc_autoreleasepool_push_count") == 1, "expected ARC debug state to preserve one autoreleasepool push")
    expect(runtime_abi_probe_payload.get("arc_autoreleasepool_pop_count") == 1, "expected ARC debug state to preserve one autoreleasepool pop")
    expect(runtime_abi_probe_payload.get("retain_symbol") == "objc3_runtime_retain_i32", "expected packaged block ARC runtime ABI probe to preserve the retain symbol")
    expect(runtime_abi_probe_payload.get("release_symbol") == "objc3_runtime_release_i32", "expected packaged block ARC runtime ABI probe to preserve the release symbol")
    expect(runtime_abi_probe_payload.get("autorelease_symbol") == "objc3_runtime_autorelease_i32", "expected packaged block ARC runtime ABI probe to preserve the autorelease symbol")
    expect(runtime_abi_probe_payload.get("autoreleasepool_push_symbol") == "objc3_runtime_push_autoreleasepool_scope", "expected packaged block ARC runtime ABI probe to preserve the autoreleasepool push symbol")
    expect(runtime_abi_probe_payload.get("autoreleasepool_pop_symbol") == "objc3_runtime_pop_autoreleasepool_scope", "expected packaged block ARC runtime ABI probe to preserve the autoreleasepool pop symbol")
    expect(runtime_abi_probe_payload.get("weak_current_property_load_symbol") == "objc3_runtime_load_weak_current_property_i32", "expected packaged block ARC runtime ABI probe to preserve the weak current-property load symbol")
    expect(runtime_abi_probe_payload.get("weak_current_property_store_symbol") == "objc3_runtime_store_weak_current_property_i32", "expected packaged block ARC runtime ABI probe to preserve the weak current-property store symbol")
    expect(runtime_abi_probe_payload.get("arc_debug_state_snapshot_symbol") == "objc3_runtime_copy_arc_debug_state_for_testing", "expected packaged block ARC runtime ABI probe to preserve the ARC debug state snapshot symbol")
    expect(len(child_report_paths) >= 2, "expected packaged block/ARC e2e report to publish smoke and replay child reports")
    expect(any(step.get("action") == "packaged-block-arc-execution-smoke" and step.get("exit_code") == 0 for step in steps if isinstance(step, dict)), "expected packaged block/ARC e2e report to publish a passing execution smoke step")
    expect(any(step.get("action") == "packaged-execution-replay" and step.get("exit_code") == 0 for step in steps if isinstance(step, dict)), "expected packaged block/ARC e2e report to publish a passing execution replay step")

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_block_arc_closure_live_arc_runtime.py",
        "used_existing_block_arc_e2e_report": os.environ.get("OBJC3C_SKIP_BLOCK_ARC_E2E_RERUN") == "1",
        "child_report_paths": [repo_rel(RUNNER_REPORT), *child_report_paths],
        "runtime_abi_probe_payload": runtime_abi_probe_payload,
    }
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(JSON_OUT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
