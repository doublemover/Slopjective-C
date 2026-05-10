#!/usr/bin/env python3
"""Validate runnable concurrency conformance against the integrated live workflow."""

from __future__ import annotations

import os
import tempfile
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence

import check_objc3c_runtime_acceptance as runtime_acceptance
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import require_json_object as load_json, write_json_file
from objc3c_tooling.subprocesses import python_script_command, run_capture


ROOT = Path(__file__).resolve().parents[1]
INTEGRATION_SCRIPT = ROOT / "scripts" / "check_objc3c_runtime_architecture_integration.py"
INTEGRATION_REPORT = ROOT / "tmp" / "reports" / "runtime" / "architecture-integration" / "summary.json"
ACCEPTANCE_REPORT = ROOT / "tmp" / "reports" / "runtime" / "acceptance" / "summary.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "runtime" / "runnable-concurrency-conformance" / "summary.json"
SUMMARY_CONTRACT_ID = "objc3c.runtime.runnable.concurrency.conformance.summary.v1"

REQUIRED_CASES = {
    "unified-concurrency-runtime-architecture",
    "async-task-actor-normalization-completion",
    "unified-concurrency-lowering-metadata-surface",
    "unified-concurrency-runtime-abi",
    "live-unified-concurrency-runtime-implementation",
    "cross-module-concurrency-actor-artifact-preservation",
}

REQUIRED_SURFACE_CONTRACTS = {
    "runtime_unified_concurrency_source_surface": (
        "objc3c.runtime.unified.concurrency.source.surface.v1"
    ),
    "runtime_async_task_actor_normalization_completion_surface": (
        "objc3c.runtime.async.task.actor.normalization.completion.surface.v1"
    ),
    "runtime_unified_concurrency_lowering_metadata_surface": (
        "objc3c.runtime.unified.concurrency.lowering.metadata.surface.v1"
    ),
    "runtime_unified_concurrency_runtime_abi_surface": (
        "objc3c.runtime.unified.concurrency.runtime.abi.surface.v1"
    ),
}


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)

def ensure_case_passed(case_map: dict[str, dict[str, Any]], case_id: str) -> None:
    case = case_map.get(case_id)
    if case is not None:
        expect(case.get("passed") is True, f"required concurrency case {case_id} did not pass")
        return

    retired_route_root = ROOT / "tmp" / "reports" / "runtime" / "runnable-concurrency-conformance" / "live-case"
    retired_route_root.mkdir(parents=True, exist_ok=True)
    clangxx = runtime_acceptance.find_clangxx()
    with tempfile.TemporaryDirectory(dir=retired_route_root) as tmp_dir:
        run_dir = Path(tmp_dir)
        if case_id == "unified-concurrency-runtime-architecture":
            result = runtime_acceptance.check_unified_concurrency_runtime_architecture_case(run_dir)
        elif case_id == "async-task-actor-normalization-completion":
            result = runtime_acceptance.check_async_task_actor_normalization_completion_case(run_dir)
        elif case_id == "unified-concurrency-lowering-metadata-surface":
            result = runtime_acceptance.check_unified_concurrency_lowering_metadata_surface_case(run_dir)
        elif case_id == "unified-concurrency-runtime-abi":
            result = runtime_acceptance.check_unified_concurrency_runtime_abi_case(clangxx, run_dir)
        elif case_id == "live-unified-concurrency-runtime-implementation":
            result = runtime_acceptance.check_live_unified_concurrency_runtime_implementation_case(clangxx, run_dir)
        elif case_id == "cross-module-concurrency-actor-artifact-preservation":
            result = runtime_acceptance.check_cross_module_concurrency_actor_artifact_preservation_case(run_dir)
        else:
            raise RuntimeError(f"runtime acceptance report did not publish required case {case_id}")
    expect(result.passed is True, f"required concurrency case {case_id} did not pass")


def main() -> int:
    skip_integration_rerun = os.environ.get("OBJC3C_SKIP_INTEGRATION_RERUN") == "1"
    if not skip_integration_rerun:
        integration_result = run_capture(python_script_command(INTEGRATION_SCRIPT))
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
        ensure_case_passed(case_map, case_id)

    for surface_key, contract_id in REQUIRED_SURFACE_CONTRACTS.items():
        acceptance_surface = acceptance_report.get(surface_key)
        integration_surface = integration_report.get(surface_key)
        expect(
            isinstance(acceptance_surface, dict),
            f"runtime acceptance report did not publish {surface_key}",
        )
        expect(
            acceptance_surface.get("contract_id") == contract_id,
            f"runtime acceptance report published the wrong contract id for {surface_key}",
        )
        if isinstance(integration_surface, dict):
            expect(
                integration_surface == acceptance_surface,
                f"runtime integration report drifted from acceptance for {surface_key}",
            )
        elif not skip_integration_rerun:
            raise RuntimeError(f"runtime integration report did not publish {surface_key}")

    source_surface = acceptance_report["runtime_unified_concurrency_source_surface"]
    normalization_surface = acceptance_report[
        "runtime_async_task_actor_normalization_completion_surface"
    ]
    lowering_surface = acceptance_report[
        "runtime_unified_concurrency_lowering_metadata_surface"
    ]
    abi_surface = acceptance_report["runtime_unified_concurrency_runtime_abi_surface"]

    authoritative_case_ids = abi_surface.get("authoritative_case_ids", [])
    if authoritative_case_ids:
        if "live-unified-concurrency-runtime-implementation" not in authoritative_case_ids and not skip_integration_rerun:
            raise RuntimeError(
                "unified concurrency runtime ABI surface did not carry the live implementation case"
            )
    elif not skip_integration_rerun:
        raise RuntimeError(
            "unified concurrency runtime ABI surface did not publish authoritative case ids"
        )
    expect(
        "objc3_runtime_copy_actor_runtime_state_for_testing"
        in source_surface.get("private_concurrency_runtime_boundary", []),
        "unified concurrency source surface drifted from the private runtime boundary",
    )
    expect(
        "objc3c.concurrency.task.runtime.lowering.contract.v1"
        in normalization_surface.get("lowering_contract_ids", []),
        "async/task/actor normalization surface drifted from the task runtime lowering contract",
    )
    expect(
        "objc3c.concurrency.actor.lowering.and.metadata.contract.v1"
        in lowering_surface.get("lowering_contract_ids", []),
        "unified concurrency lowering surface drifted from the actor lowering metadata contract",
    )
    expect(
        abi_surface.get("task_runtime_state_snapshot_symbol")
        == "objc3_runtime_copy_task_runtime_state_for_testing",
        "unified concurrency runtime ABI surface drifted from the task runtime snapshot symbol",
    )
    expect(
        abi_surface.get("actor_runtime_state_snapshot_symbol")
        == "objc3_runtime_copy_actor_runtime_state_for_testing",
        "unified concurrency runtime ABI surface drifted from the actor runtime snapshot symbol",
    )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_concurrency_conformance.py",
        "required_case_ids": sorted(REQUIRED_CASES),
        "required_surface_keys": list(REQUIRED_SURFACE_CONTRACTS.keys()),
        "child_report_paths": [
            repo_rel(INTEGRATION_REPORT),
            repo_rel(ACCEPTANCE_REPORT),
        ],
        "runtime_unified_concurrency_source_surface": source_surface,
        "runtime_async_task_actor_normalization_completion_surface": normalization_surface,
        "runtime_unified_concurrency_lowering_metadata_surface": lowering_surface,
        "runtime_unified_concurrency_runtime_abi_surface": abi_surface,
    }

    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
