#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/metaprogramming_interop_closure/packaged_interop_proof_contract.json"
REPORT_PATH = ROOT / "tmp/reports/runtime/runnable-interop-e2e/summary.json"
OUT_DIR = ROOT / "tmp/reports/metaprogramming-interop-closure/packaged-interop-proof"
JSON_OUT = OUT_DIR / "packaged_interop_proof_summary.json"
MD_OUT = OUT_DIR / "packaged_interop_proof_summary.md"
SUMMARY_CONTRACT_ID = "objc3c.metaprogramming.interop.closure.packaged.cross.module.interop.proof.summary.v1"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def resolve_report_path(raw_path: str) -> Path:
    path = Path(raw_path)
    return path if path.is_absolute() else ROOT / path


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def read_nested(payload: dict[str, Any], dotted_key: str) -> Any:
    current: Any = payload
    for part in dotted_key.split("."):
        if not isinstance(current, dict):
            return None
        current = current.get(part)
    return current


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    report_path = resolve_report_path(contract["authoritative_report"])
    expect(report_path.is_file(), f"missing authoritative report: {repo_rel(report_path)}")
    report = read_json(report_path)
    steps = [step for step in report.get("steps", []) if isinstance(step, dict)]
    step_actions = [step.get("action") for step in steps]
    step_map = {
        str(step.get("action")): step
        for step in steps
        if step.get("action") is not None
    }

    required_artifact_paths = {
        field: resolve_report_path(str(report.get(field)))
        for field in contract["required_artifact_fields"]
        if report.get(field)
    }
    required_probe_paths = {
        field: resolve_report_path(str(read_nested(report, field)))
        for field in contract.get("required_probe_fields", [])
        if read_nested(report, field)
    }
    child_report_paths = [resolve_report_path(path) for path in report.get("child_report_paths", []) if isinstance(path, str)]

    step_report_paths = {
        action: [resolve_report_path(path) for path in step_map.get(action, {}).get("report_paths", []) if isinstance(path, str)]
        for action in contract.get("required_step_report_actions", [])
    }

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/check_metaprogramming_interop_closure_packaged_interop_proof.py",
        "report_passes": report.get("status") == "PASS",
        "all_required_step_actions_present": all(action in step_actions for action in contract["required_step_actions"]),
        "all_required_steps_exit_zero": all(step_map.get(action, {}).get("exit_code") == 0 for action in contract["required_step_actions"]),
        "all_required_artifact_fields_present": all(bool(report.get(field)) for field in contract["required_artifact_fields"]),
        "all_required_artifact_paths_exist": all(path.exists() for path in required_artifact_paths.values()),
        "all_required_probe_fields_present": all(bool(read_nested(report, field)) for field in contract.get("required_probe_fields", [])),
        "all_required_probe_paths_exist": all(path.exists() for path in required_probe_paths.values()),
        "required_packaging_probe_payload_matches": all(
            report.get("packaging_probe_payload", {}).get(field) == value
            for field, value in contract.get("required_packaging_probe_payload", {}).items()
        ),
        "required_bridge_probe_payload_matches": all(
            report.get("bridge_probe_payload", {}).get(field) == value
            for field, value in contract.get("required_bridge_probe_payload", {}).items()
        ),
        "child_report_count_matches": len(child_report_paths) >= contract.get("required_child_report_count", 0),
        "required_step_report_actions_present": all(
            step_map.get(action, {}).get("report_paths")
            for action in contract.get("required_step_report_actions", [])
        ),
        "step_report_paths_match_child_report_paths": all(
            path in child_report_paths
            for paths in step_report_paths.values()
            for path in paths
        ),
    }

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "source_contract_id": contract["contract_id"],
        "runner_path": "scripts/check_metaprogramming_interop_closure_packaged_interop_proof.py",
        "authoritative_report": repo_rel(report_path),
        "issue": "packaged-interop-proof",
        "surface_kind": contract["surface_kind"],
        "required_step_action_count": len(contract["required_step_actions"]),
        "required_artifact_field_count": len(contract["required_artifact_fields"]),
        "required_probe_field_count": len(contract.get("required_probe_fields", [])),
        "required_artifact_paths": {field: repo_rel(path) for field, path in required_artifact_paths.items()},
        "required_probe_paths": {field: repo_rel(path) for field, path in required_probe_paths.items()},
        "child_report_paths": [repo_rel(path) for path in child_report_paths],
        "step_report_paths": {
            action: [repo_rel(path) for path in paths]
            for action, paths in step_report_paths.items()
        },
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Packaged Interop Proof Summary\n\n"
        f"- Summary contract: `{summary['contract_id']}`\n"
        f"- Source contract: `{summary['source_contract_id']}`\n"
        f"- Authoritative report: `{summary['authoritative_report']}`\n"
        f"- Required step actions: `{summary['required_step_action_count']}`\n"
        f"- Required artifact fields: `{summary['required_artifact_field_count']}`\n"
        f"- Required probe fields: `{summary['required_probe_field_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
