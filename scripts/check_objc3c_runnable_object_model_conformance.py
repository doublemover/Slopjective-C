#!/usr/bin/env python3
"""Validate runnable object-model conformance against the integrated live workflow."""

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
REPORT_PATH = ROOT / "tmp" / "reports" / "runtime" / "runnable-object-model-conformance" / "summary.json"
SUMMARY_CONTRACT_ID = "objc3c.runtime.runnable.object.model.conformance.summary.v1"

REQUIRED_CASES = {
    "imported-runtime-packaging-replay",
    "canonical-dispatch",
    "canonical-sample-set",
    "realization-lookup-reflection-runtime",
    "dispatch-fast-path",
    "property-reflection",
    "property-execution",
    "storage-ownership-reflection",
}

REQUIRED_SURFACE_CONTRACTS = {
    "runtime_object_model_abi_query_surface": "objc3c.runtime.object.model.abi.query.surface.v1",
    "runtime_realization_lookup_reflection_implementation_surface": (
        "objc3c.runtime.realization.lookup.reflection.implementation.surface.v1"
    ),
    "runtime_reflection_query_surface": "objc3c.runtime.reflection.query.surface.v1",
    "runtime_realization_lookup_semantics_surface": (
        "objc3c.runtime.realization.lookup.semantics.v1"
    ),
    "runtime_class_metaclass_protocol_realization_surface": (
        "objc3c.runtime.class.metaclass.protocol.realization.v1"
    ),
    "runtime_category_attachment_merged_dispatch_surface": (
        "objc3c.runtime.category.attachment.merged.dispatch.surface.v1"
    ),
    "runtime_reflection_visibility_coherence_diagnostics_surface": (
        "objc3c.runtime.reflection.visibility.coherence.diagnostics.surface.v1"
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
        expect(case.get("passed") is True, f"required object-model case {case_id} did not pass")

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

    object_model_surface = acceptance_report["runtime_object_model_abi_query_surface"]
    implementation_surface = acceptance_report[
        "runtime_realization_lookup_reflection_implementation_surface"
    ]
    expect(
        "realization-lookup-reflection-runtime"
        in object_model_surface.get("authoritative_case_ids", []),
        "object-model ABI/query surface did not carry the realization-lookup-reflection-runtime case",
    )
    expect(
        implementation_surface.get("object_model_query_state_snapshot_symbol")
        == "objc3_runtime_copy_object_model_query_state_for_testing",
        "object-model implementation surface drifted from the aggregate runtime query symbol",
    )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_object_model_conformance.py",
        "required_case_ids": sorted(REQUIRED_CASES),
        "required_surface_keys": list(REQUIRED_SURFACE_CONTRACTS.keys()),
        "child_report_paths": [
            repo_rel(INTEGRATION_REPORT),
            repo_rel(ACCEPTANCE_REPORT),
        ],
        "object_model_abi_query_surface": object_model_surface,
        "realization_lookup_reflection_implementation_surface": implementation_surface,
    }

    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
