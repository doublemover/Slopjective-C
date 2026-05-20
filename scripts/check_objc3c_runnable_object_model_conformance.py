#!/usr/bin/env python3
"""Validate runnable object-model conformance against the integrated live workflow."""

from __future__ import annotations

import os
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import require_json_object as load_json, write_json_file
from objc3c_tooling.subprocesses import python_script_command, run_capture


ROOT = Path(__file__).resolve().parents[1]
INTEGRATION_SCRIPT = ROOT / "scripts" / "check_objc3c_runtime_architecture_integration.py"
INTEGRATION_REPORT = ROOT / "tmp" / "reports" / "runtime" / "architecture-integration" / "summary.json"
ACCEPTANCE_REPORT = ROOT / "tmp" / "reports" / "runtime" / "acceptance" / "summary.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "runtime" / "runnable-object-model-conformance" / "summary.json"
SUMMARY_CONTRACT_ID = "objc3c.runtime.runnable.object.model.conformance.summary.v1"

REQUIRED_CASES = {
    "canonical-dispatch",
    "metaclass-graph-root-class",
    "dispatch-fast-path",
    "imported-runtime-packaging-replay",
    "multi-image-registration-reset-replay",
    "property-layout",
    "instance-allocation-layout-runtime",
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
    "runtime_cross_module_realized_metadata_replay_preservation_surface": (
        "objc3c.runtime.cross.module.realized.metadata.replay.preservation.surface.v1"
    ),
    "runtime_category_attachment_merged_dispatch_surface": (
        "objc3c.runtime.category.attachment.merged.dispatch.surface.v1"
    ),
    "runtime_reflection_visibility_coherence_diagnostics_surface": (
        "objc3c.runtime.reflection.visibility.coherence.diagnostics.surface.v1"
    ),
}

REQUIRED_SURFACE_CASES = {
    "runtime_object_model_abi_query_surface": {
        "canonical-dispatch",
        "metaclass-graph-root-class",
        "dispatch-fast-path",
        "imported-runtime-packaging-replay",
        "multi-image-registration-reset-replay",
        "instance-allocation-layout-runtime",
        "storage-ownership-reflection",
    },
    "runtime_realization_lookup_reflection_implementation_surface": {
        "dispatch-fast-path",
    },
    "runtime_reflection_query_surface": {
        "storage-ownership-reflection",
    },
    "runtime_realization_lookup_semantics_surface": {
        "canonical-dispatch",
        "dispatch-fast-path",
    },
    "runtime_class_metaclass_protocol_realization_surface": {
        "canonical-dispatch",
        "metaclass-graph-root-class",
    },
    "runtime_cross_module_realized_metadata_replay_preservation_surface": {
        "imported-runtime-packaging-replay",
        "multi-image-registration-reset-replay",
    },
    "runtime_category_attachment_merged_dispatch_surface": {
        "canonical-dispatch",
    },
    "runtime_reflection_visibility_coherence_diagnostics_surface": {
        "storage-ownership-reflection",
    },
}


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)

def main() -> int:
    if os.environ.get("OBJC3C_SKIP_INTEGRATION_RERUN") != "1":
        integration_result = run_capture(
            python_script_command(INTEGRATION_SCRIPT)
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
        authoritative_case_ids = set(acceptance_surface.get("authoritative_case_ids", []))
        for case_id in sorted(REQUIRED_SURFACE_CASES.get(surface_key, set())):
            expect(
                case_id in authoritative_case_ids,
                f"{surface_key} did not carry required case {case_id}",
            )

    object_model_surface = acceptance_report["runtime_object_model_abi_query_surface"]
    implementation_surface = acceptance_report[
        "runtime_realization_lookup_reflection_implementation_surface"
    ]
    expect(
        "objc3_runtime_copy_instance_entry_for_testing"
        in object_model_surface.get("private_object_model_query_boundary", []),
        "object-model ABI query surface must publish the runtime instance identity snapshot boundary",
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
        "required_surface_case_ids": {
            surface_key: sorted(case_ids)
            for surface_key, case_ids in REQUIRED_SURFACE_CASES.items()
        },
        "child_report_paths": [
            repo_rel(INTEGRATION_REPORT),
            repo_rel(ACCEPTANCE_REPORT),
        ],
        "object_model_abi_query_surface": object_model_surface,
        "realization_lookup_reflection_implementation_surface": implementation_surface,
    }

    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
