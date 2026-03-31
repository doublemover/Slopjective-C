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
OUT_DIR = ROOT / "tmp/reports/block-arc-closure/live-block-runtime"
JSON_OUT = OUT_DIR / "live_escaping_block_byref_runtime_summary.json"
SUMMARY_CONTRACT_ID = "objc3c.block_arc.closure.live.escaping.block.byref.runtime.summary.v1"


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
    byref_probe_payload = runner_report.get("byref_forwarding_probe_payload", {})
    expect(runtime_abi_probe_payload.get("handle", 0) > 0, "expected packaged block ARC runtime ABI probe to publish a positive handle")
    expect(runtime_abi_probe_payload.get("invoke_result") == 17, "expected packaged block ARC runtime ABI probe to preserve invoke_result 17")
    expect(runtime_abi_probe_payload.get("block_promote_call_count") == 1, "expected packaged block ARC runtime ABI probe to preserve one promote call")
    expect(runtime_abi_probe_payload.get("block_invoke_call_count") == 1, "expected packaged block ARC runtime ABI probe to preserve one invoke call")
    expect(runtime_abi_probe_payload.get("block_promote_symbol") == "objc3_runtime_promote_block_i32", "expected packaged block ARC runtime ABI probe to preserve the block promote symbol")
    expect(runtime_abi_probe_payload.get("block_invoke_symbol") == "objc3_runtime_invoke_block_i32", "expected packaged block ARC runtime ABI probe to preserve the block invoke symbol")

    expect(byref_probe_payload.get("handle", 0) > 0, "expected packaged byref forwarding probe to publish a positive handle")
    expect(byref_probe_payload.get("copy_count_after_promotion") == 1, "expected packaged byref forwarding probe to preserve one copy helper call")
    expect(byref_probe_payload.get("first_invoke_result") == 23, "expected packaged byref forwarding probe to preserve first invoke result 23")
    expect(byref_probe_payload.get("second_invoke_result") == 25, "expected packaged byref forwarding probe to preserve second invoke result 25")
    expect(byref_probe_payload.get("dispose_count_before_final_release") == 0, "expected packaged byref forwarding probe to defer dispose before final release")
    expect(byref_probe_payload.get("dispose_count_after_final_release") == 1, "expected packaged byref forwarding probe to execute dispose on final release")
    expect(byref_probe_payload.get("last_disposed_value") == 11, "expected packaged byref forwarding probe to preserve the disposed payload")
    expect(byref_probe_payload.get("invoke_after_release_result") == 0, "expected packaged byref forwarding probe to reject invoke after final release")

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_block_arc_closure_live_block_runtime.py",
        "used_existing_block_arc_e2e_report": os.environ.get("OBJC3C_SKIP_BLOCK_ARC_E2E_RERUN") == "1",
        "child_report_paths": [repo_rel(RUNNER_REPORT)],
        "runtime_abi_probe_payload": runtime_abi_probe_payload,
        "byref_forwarding_probe_payload": byref_probe_payload,
    }
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(JSON_OUT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
