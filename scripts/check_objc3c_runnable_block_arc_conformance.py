#!/usr/bin/env python3
"""Validate runnable block/ARC conformance against the integrated live workflow."""

from __future__ import annotations

import json
import os
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
INTEGRATION_SCRIPT = ROOT / "scripts" / "check_objc3c_runtime_architecture_integration.py"
INTEGRATION_REPORT = ROOT / "tmp" / "reports" / "runtime" / "architecture-integration" / "summary.json"
ACCEPTANCE_REPORT = ROOT / "tmp" / "reports" / "runtime" / "acceptance" / "summary.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "runtime" / "runnable-block-arc-conformance" / "summary.json"
SUMMARY_CONTRACT_ID = "objc3c.runtime.runnable.block.arc.conformance.summary.v1"

REQUIRED_CASES = {
    "escaping-block-capture-legality",
    "block-storage-arc-automation-semantics",
    "block-arc-runtime-abi",
    "block-helper-runtime-execution",
}

REQUIRED_SURFACE_CONTRACTS = {
    "runtime_block_arc_unified_source_surface": (
        "objc3c.runtime.block.arc.unified.source.surface.v1"
    ),
    "runtime_ownership_transfer_capture_family_source_surface": (
        "objc3c.runtime.ownership.transfer.capture.family.source.surface.v1"
    ),
    "runtime_block_arc_lowering_helper_surface": (
        "objc3c.runtime.block.arc.lowering.helper.surface.v1"
    ),
    "runtime_block_arc_runtime_abi_surface": (
        "objc3c.runtime.block.arc.runtime.abi.surface.v1"
    ),
}


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def run_capture(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        list(command),
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result


def load_json(path: Path) -> dict[str, Any]:
    expect(path.is_file(), f"expected JSON artifact was not published: {repo_rel(path)}")
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON artifact at {repo_rel(path)} did not contain an object")
    return payload


def main() -> int:
    if os.environ.get("OBJC3C_SKIP_INTEGRATION_RERUN") != "1":
        integration_result = run_capture(
            [sys.executable, str(INTEGRATION_SCRIPT)]
        )
        if integration_result.returncode != 0:
            raise RuntimeError("runtime architecture integration workflow failed")

    integration_report = load_json(INTEGRATION_REPORT)
    acceptance_report = load_json(ACCEPTANCE_REPORT)
    expect(
        integration_report.get("status") == "PASS",
        "runtime architecture integration report did not publish PASS",
    )
    expect(
        acceptance_report.get("status") == "PASS",
        "runtime acceptance report did not publish PASS",
    )

    cases = acceptance_report.get("cases", [])
    expect(isinstance(cases, list), "runtime acceptance report did not publish cases")
    case_map = {
        str(case.get("case_id")): case
        for case in cases
        if isinstance(case, dict) and case.get("case_id") is not None
    }
    for case_id in sorted(REQUIRED_CASES):
        case = case_map.get(case_id)
        expect(case is not None, f"runtime acceptance report did not publish required case {case_id}")
        expect(case.get("passed") is True, f"required block/ARC case {case_id} did not pass")

    for surface_key, contract_id in REQUIRED_SURFACE_CONTRACTS.items():
        acceptance_surface = acceptance_report.get(surface_key)
        integration_surface = integration_report.get(surface_key)
        expect(
            isinstance(acceptance_surface, dict),
            f"runtime acceptance report did not publish {surface_key}",
        )
        expect(
            isinstance(integration_surface, dict),
            f"runtime integration report did not publish {surface_key}",
        )
        expect(
            acceptance_surface.get("contract_id") == contract_id,
            f"runtime acceptance report published the wrong contract id for {surface_key}",
        )
        expect(
            integration_surface == acceptance_surface,
            f"runtime integration report drifted from acceptance for {surface_key}",
        )

    unified_surface = acceptance_report["runtime_block_arc_unified_source_surface"]
    ownership_surface = acceptance_report[
        "runtime_ownership_transfer_capture_family_source_surface"
    ]
    lowering_surface = acceptance_report["runtime_block_arc_lowering_helper_surface"]
    abi_surface = acceptance_report["runtime_block_arc_runtime_abi_surface"]

    expect(
        "block-helper-runtime-execution"
        in unified_surface.get("authoritative_case_ids", []),
        "block/ARC unified source surface did not carry the block-helper-runtime-execution case",
    )
    expect(
        ownership_surface.get("block_capture_ownership_contract_id")
        == "objc3c.executable.block.byref.copy.dispose.and.object.capture.ownership.v1",
        "block/ARC ownership-transfer surface drifted from the block capture ownership contract",
    )
    expect(
        lowering_surface.get("block_escape_runtime_hook_lowering_contract_id")
        == "objc3c.executable.block.escape.runtime.hook.lowering.v1",
        "block/ARC lowering/helper surface drifted from the escape-runtime-hook lowering contract",
    )
    expect(
        abi_surface.get("block_arc_runtime_abi_snapshot_symbol")
        == "objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing",
        "block/ARC runtime ABI surface drifted from the block ARC runtime ABI snapshot symbol",
    )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_block_arc_conformance.py",
        "required_case_ids": sorted(REQUIRED_CASES),
        "required_surface_keys": list(REQUIRED_SURFACE_CONTRACTS.keys()),
        "child_report_paths": [
            repo_rel(INTEGRATION_REPORT),
            repo_rel(ACCEPTANCE_REPORT),
        ],
        "runtime_block_arc_unified_source_surface": unified_surface,
        "runtime_ownership_transfer_capture_family_source_surface": ownership_surface,
        "runtime_block_arc_lowering_helper_surface": lowering_surface,
        "runtime_block_arc_runtime_abi_surface": abi_surface,
    }

    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
