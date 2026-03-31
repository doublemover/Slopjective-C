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
RUNNER = ROOT / "scripts/check_objc3c_runnable_block_arc_conformance.py"
RUNNER_REPORT = ROOT / "tmp/reports/runtime/runnable-block-arc-conformance/summary.json"
ACCEPTANCE_REPORT = ROOT / "tmp/reports/runtime/acceptance/summary.json"
OUT_DIR = ROOT / "tmp/reports/block-arc-closure/escaping-block-lowering"
JSON_OUT = OUT_DIR / "escaping_block_promotion_byref_lowering_summary.json"
SUMMARY_CONTRACT_ID = "objc3c.block_arc.closure.escaping.block.promotion.byref.lowering.summary.v1"
REQUIRED_SURFACES = {
    "runtime_block_arc_unified_source_surface": "objc3c.runtime.block.arc.unified.source.surface.v1",
    "runtime_ownership_transfer_capture_family_source_surface": "objc3c.runtime.ownership.transfer.capture.family.source.surface.v1",
    "runtime_block_arc_lowering_helper_surface": "objc3c.runtime.block.arc.lowering.helper.surface.v1",
    "runtime_block_arc_runtime_abi_surface": "objc3c.runtime.block.arc.runtime.abi.surface.v1",
}


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
    if os.environ.get("OBJC3C_SKIP_BLOCK_ARC_CONFORMANCE_RERUN") != "1":
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
            raise RuntimeError("runnable block/ARC conformance runner failed")

    runner_report = load_json(RUNNER_REPORT)
    acceptance_report = load_json(ACCEPTANCE_REPORT)
    expect(runner_report.get("status") == "PASS", "block/ARC conformance report did not publish PASS")
    expect(acceptance_report.get("status") == "PASS", "runtime acceptance report did not publish PASS")

    surface_contracts: dict[str, str] = {}
    for surface_key, contract_id in REQUIRED_SURFACES.items():
        surface = acceptance_report.get(surface_key)
        expect(isinstance(surface, dict), f"runtime acceptance report did not publish {surface_key}")
        expect(surface.get("contract_id") == contract_id, f"runtime acceptance report published the wrong contract for {surface_key}")
        surface_contracts[surface_key] = str(surface.get("contract_id"))

    unified_surface = acceptance_report["runtime_block_arc_unified_source_surface"]
    ownership_surface = acceptance_report["runtime_ownership_transfer_capture_family_source_surface"]
    lowering_surface = acceptance_report["runtime_block_arc_lowering_helper_surface"]
    abi_surface = acceptance_report["runtime_block_arc_runtime_abi_surface"]

    expect(
        "block-helper-runtime-execution" in unified_surface.get("authoritative_case_ids", []),
        "block/ARC unified source surface did not carry block-helper-runtime-execution",
    )
    expect(
        ownership_surface.get("block_capture_ownership_contract_id")
        == "objc3c.executable.block.byref.copy.dispose.and.object.capture.ownership.v1",
        "ownership-transfer capture surface drifted from the block capture ownership contract",
    )
    expect(
        lowering_surface.get("block_byref_helper_lowering_contract_id")
        == "objc3c.executable.block.byref.helper.lowering.v1",
        "block/ARC lowering/helper surface drifted from the byref-helper lowering contract",
    )
    expect(
        lowering_surface.get("block_escape_runtime_hook_lowering_contract_id")
        == "objc3c.executable.block.escape.runtime.hook.lowering.v1",
        "block/ARC lowering/helper surface drifted from the escape-runtime-hook lowering contract",
    )
    expect(
        abi_surface.get("block_arc_runtime_abi_snapshot_symbol")
        == "objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing",
        "block/ARC runtime ABI surface drifted from the ABI snapshot symbol",
    )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_block_arc_closure_escaping_block_lowering.py",
        "used_existing_block_arc_conformance_report": os.environ.get(
            "OBJC3C_SKIP_BLOCK_ARC_CONFORMANCE_RERUN"
        ) == "1",
        "child_report_paths": [repo_rel(RUNNER_REPORT), repo_rel(ACCEPTANCE_REPORT)],
        "required_surface_contracts": surface_contracts,
        "runtime_block_arc_unified_source_surface": unified_surface,
        "runtime_ownership_transfer_capture_family_source_surface": ownership_surface,
        "runtime_block_arc_lowering_helper_surface": lowering_surface,
        "runtime_block_arc_runtime_abi_surface": abi_surface,
    }
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(JSON_OUT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
