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
RUNNER = ROOT / "scripts/check_objc3c_runnable_object_model_conformance.py"
RUNNER_REPORT = ROOT / "tmp/reports/runtime/runnable-object-model-conformance/summary.json"
ACCEPTANCE_REPORT = ROOT / "tmp/reports/runtime/acceptance/summary.json"
OUT_DIR = ROOT / "tmp/reports/m319/M319-C002"
JSON_OUT = OUT_DIR / "class_category_protocol_realization_lowering_summary.json"
SUMMARY_CONTRACT_ID = "objc3c.object_model.closure.class.category.protocol.realization.lowering.summary.v1"
REQUIRED_SURFACES = {
    "runtime_realization_lowering_reflection_artifact_surface": "objc3c.runtime.realization.lowering.reflection.artifact.surface.v1",
    "runtime_dispatch_table_reflection_record_lowering_surface": "objc3c.runtime.dispatch.table.reflection.record.lowering.surface.v1",
    "runtime_cross_module_realized_metadata_replay_preservation_surface": "objc3c.runtime.cross.module.realized.metadata.replay.preservation.surface.v1",
    "runtime_object_model_abi_query_surface": "objc3c.runtime.object.model.abi.query.surface.v1",
    "runtime_realization_lookup_reflection_implementation_surface": "objc3c.runtime.realization.lookup.reflection.implementation.surface.v1",
    "runtime_class_metaclass_protocol_realization_surface": "objc3c.runtime.class.metaclass.protocol.realization.v1",
    "runtime_category_attachment_merged_dispatch_surface": "objc3c.runtime.category.attachment.merged.dispatch.surface.v1",
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
    if os.environ.get("OBJC3C_SKIP_OBJECT_MODEL_CONFORMANCE_RERUN") != "1":
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
            raise RuntimeError("runnable object-model conformance runner failed")

    runner_report = load_json(RUNNER_REPORT)
    acceptance_report = load_json(ACCEPTANCE_REPORT)
    expect(runner_report.get("status") == "PASS", "object-model conformance report did not publish PASS")
    expect(acceptance_report.get("status") == "PASS", "runtime acceptance report did not publish PASS")

    surface_contracts: dict[str, str] = {}
    for surface_key, contract_id in REQUIRED_SURFACES.items():
        surface = acceptance_report.get(surface_key)
        expect(isinstance(surface, dict), f"runtime acceptance report did not publish {surface_key}")
        expect(surface.get("contract_id") == contract_id, f"runtime acceptance report published the wrong contract for {surface_key}")
        surface_contracts[surface_key] = str(surface.get("contract_id"))

    object_model_surface = acceptance_report["runtime_object_model_abi_query_surface"]
    implementation_surface = acceptance_report["runtime_realization_lookup_reflection_implementation_surface"]
    expect(
        "realization-lookup-reflection-runtime" in object_model_surface.get("authoritative_case_ids", []),
        "runtime object-model ABI surface did not publish realization-lookup-reflection-runtime",
    )
    expect(
        implementation_surface.get("object_model_query_state_snapshot_symbol") == "objc3_runtime_copy_object_model_query_state_for_testing",
        "realization lookup implementation surface drifted from the aggregate object-model query snapshot symbol",
    )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_object_model_closure_realization_lowering.py",
        "used_existing_object_model_conformance_report": os.environ.get(
            "OBJC3C_SKIP_OBJECT_MODEL_CONFORMANCE_RERUN"
        ) == "1",
        "child_report_paths": [repo_rel(RUNNER_REPORT), repo_rel(ACCEPTANCE_REPORT)],
        "required_surface_contracts": surface_contracts,
        "runtime_object_model_abi_query_surface": object_model_surface,
        "runtime_realization_lookup_reflection_implementation_surface": implementation_surface,
    }
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(JSON_OUT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
