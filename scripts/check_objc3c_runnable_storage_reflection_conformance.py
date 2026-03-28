#!/usr/bin/env python3
"""Validate runnable storage/reflection conformance against the integrated live workflow."""

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
REPORT_PATH = ROOT / "tmp" / "reports" / "runtime" / "runnable-storage-reflection-conformance" / "summary.json"
SUMMARY_CONTRACT_ID = "objc3c.runtime.runnable.storage.reflection.conformance.summary.v1"

REQUIRED_CASES = {
    "cross-module-storage-reflection-artifact-preservation",
    "accessor-storage-lowering-metadata-surface",
    "property-accessor-layout-lowering",
    "synthesized-accessor-runtime",
    "property-reflection",
    "property-execution",
    "arc-property-helper-abi",
    "storage-ownership-reflection",
}

REQUIRED_SURFACE_CONTRACTS = {
    "storage_accessor_runtime_abi_surface": "objc3c.runtime.storage.accessor.abi.surface.v1",
    "runtime_property_ivar_accessor_reflection_implementation_surface": (
        "objc3c.runtime.property.ivar.accessor.reflection.implementation.surface.v1"
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
        expect(case.get("passed") is True, f"required storage/reflection case {case_id} did not pass")

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

    abi_surface = acceptance_report["storage_accessor_runtime_abi_surface"]
    implementation_surface = acceptance_report[
        "runtime_property_ivar_accessor_reflection_implementation_surface"
    ]
    expect(
        "storage-ownership-reflection" in implementation_surface.get("proof_cases", []),
        "storage/reflection implementation surface did not carry the storage-ownership-reflection case",
    )
    expect(
        implementation_surface.get("implementation_snapshot_symbol")
        == "objc3_runtime_copy_storage_accessor_implementation_snapshot_for_testing",
        "storage/reflection implementation surface drifted from the implementation snapshot symbol",
    )
    expect(
        abi_surface.get("property_registry_state_snapshot_symbol")
        == "objc3_runtime_copy_property_registry_state_for_testing",
        "storage/accessor ABI surface drifted from the property registry snapshot symbol",
    )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_storage_reflection_conformance.py",
        "required_case_ids": sorted(REQUIRED_CASES),
        "required_surface_keys": list(REQUIRED_SURFACE_CONTRACTS.keys()),
        "child_report_paths": [
            repo_rel(INTEGRATION_REPORT),
            repo_rel(ACCEPTANCE_REPORT),
        ],
        "storage_accessor_runtime_abi_surface": abi_surface,
        "runtime_property_ivar_accessor_reflection_implementation_surface": implementation_surface,
    }

    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
