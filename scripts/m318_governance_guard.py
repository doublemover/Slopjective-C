#!/usr/bin/env python3
"""Shared governance guard for M318 budget enforcement and regression alarms."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
PACKAGE_JSON = ROOT / "package.json"
AUTOMATION_CONTRACT = ROOT / "spec" / "governance" / "objc3c_governance_automation_contract.json"
EXCEPTION_PROCESS = ROOT / "spec" / "governance" / "objc3c_anti_noise_exception_process.json"
EXCEPTION_REGISTRY = ROOT / "spec" / "governance" / "objc3c_anti_noise_exception_registry.json"
VALIDATION_RATCHET = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_a002_validation_reduction_targets_and_ratchet_map_source_completion_ratchet_map.json"
VALIDATION_NAMESPACE_POLICY = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b004_legacy_validation_quarantine_and_namespace_policy_core_feature_implementation_policy.json"
M315_D002_CHECKER = ROOT / "scripts" / "check_m315_d002_anti_noise_enforcement_implementation_core_feature_implementation.py"
M315_C002_CHECKER = ROOT / "scripts" / "check_m315_c002_artifact_authenticity_schema_and_evidence_contract_and_architecture_freeze.py"
M315_D002_RESULT = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_d002_anti_noise_enforcement_implementation_core_feature_implementation_result.json"
CLASS_TO_GLOB = {
    "check_scripts": "scripts/check_*.py",
    "readiness_runners": "scripts/run_*_readiness.py",
    "pytest_check_files": "tests/tooling/test_check_*.py",
}


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def extract_milestone_code(name: str) -> int | None:
    match = re.search(r"(?:^|_)m(\d{2,3})(?:_|-)", name)
    if not match:
        return None
    return int(match.group(1))


def classify(glob: str, active_codes: set[int]) -> dict[str, Any]:
    active: list[str] = []
    quarantined: list[str] = []
    retained_non_milestone: list[str] = []
    for path in sorted(ROOT.glob(glob)):
        code = extract_milestone_code(path.name)
        if code is None:
            retained_non_milestone.append(rel(path))
        elif code in active_codes:
            active.append(rel(path))
        else:
            quarantined.append(rel(path))
    return {
        "active": active,
        "quarantined": quarantined,
        "retained_non_milestone": retained_non_milestone,
        "non_quarantined_count": len(active) + len(retained_non_milestone),
    }


def default_summary_path(stage: str) -> Path:
    contract = read_json(AUTOMATION_CONTRACT)
    return ROOT / contract["summary_paths"][stage]


def run_checker(path: Path) -> dict[str, Any]:
    completed = subprocess.run([sys.executable, str(path)], cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)
    return {
        "path": rel(path),
        "returncode": completed.returncode,
        "stderr": (completed.stderr or completed.stdout).strip(),
        "ok": completed.returncode == 0,
    }


def run_budget_snapshot() -> dict[str, Any]:
    package = read_json(PACKAGE_JSON)
    ratchet = read_json(VALIDATION_RATCHET)
    namespace_policy = read_json(VALIDATION_NAMESPACE_POLICY)
    command = package["objc3cCommandSurface"]
    active_codes = set(namespace_policy["active_milestone_codes"])
    classified = {key: classify(glob, active_codes) for key, glob in CLASS_TO_GLOB.items()}
    validation_counts = {key: value["non_quarantined_count"] for key, value in classified.items()}
    validation_maximums = ratchet["closeout_maximums"]
    public_command_ok = command["workflowApi"]["publicActionCount"] <= command["budgetMaximum"]
    validation_ok = all(validation_counts[key] <= validation_maximums[key] for key in validation_maximums)
    alarms: list[str] = []
    if not public_command_ok:
        alarms.append("public_command_budget_regression")
    if not validation_ok:
        alarms.append("validation_budget_regression")
    return {
        "mode": "m318-c002-governance-budget-enforcement-v1",
        "stage": "budget-snapshot",
        "ok": public_command_ok and validation_ok,
        "public_command": {
            "current": command["workflowApi"]["publicActionCount"],
            "maximum": command["budgetMaximum"],
            "ok": public_command_ok,
        },
        "validation_counts": validation_counts,
        "validation_maximums": validation_maximums,
        "classification_samples": {key: value["active"][:5] for key, value in classified.items()},
        "alarms": alarms,
    }


def run_exception_registry() -> dict[str, Any]:
    process = read_json(EXCEPTION_PROCESS)
    registry = read_json(EXCEPTION_REGISTRY)
    required_fields = set(process["required_record_fields"])
    active = [entry for entry in registry.get("exceptions", []) if entry.get("status") == "active"]
    expired = [entry for entry in registry.get("exceptions", []) if entry.get("status") == "expired"]
    malformed_active = [entry.get("id", "<missing>") for entry in active if not required_fields.issubset(entry.keys())]
    ok = not expired and not malformed_active
    alarms: list[str] = []
    if expired:
        alarms.append("expired_exception_record")
    return {
        "mode": "m318-c002-governance-budget-enforcement-v1",
        "stage": "exception-registry",
        "ok": ok,
        "active_exception_ids": [entry.get("id") for entry in active],
        "expired_exception_ids": [entry.get("id") for entry in expired],
        "malformed_active_exception_ids": malformed_active,
        "alarms": alarms,
    }


def run_residue_proof() -> dict[str, Any]:
    residue = run_checker(M315_D002_CHECKER)
    authenticity = run_checker(M315_C002_CHECKER)
    d002 = read_json(M315_D002_RESULT)
    alarms: list[str] = []
    if not residue["ok"]:
        alarms.append("source_hygiene_regression")
    if not authenticity["ok"]:
        alarms.append("artifact_authenticity_regression")
    return {
        "mode": "m318-c002-governance-budget-enforcement-v1",
        "stage": "residue-proof",
        "ok": residue["ok"] and authenticity["ok"],
        "zero_target_classes_removed": d002["zero_target_classes_removed"],
        "source_hygiene_checker": residue,
        "artifact_authenticity_checker": authenticity,
        "alarms": alarms,
    }


def run_topology() -> dict[str, Any]:
    contract = read_json(AUTOMATION_CONTRACT)
    budget = run_budget_snapshot()
    registry = run_exception_registry()
    residue = run_residue_proof()
    alarms = budget["alarms"] + registry["alarms"] + residue["alarms"]
    return {
        "mode": "m318-c002-governance-budget-enforcement-v1",
        "stage": "topology",
        "ok": budget["ok"] and registry["ok"] and residue["ok"],
        "stage_order": contract["stage_order"],
        "stage_reports": {stage: rel(default_summary_path(stage)) for stage in ["budget-snapshot", "exception-registry", "residue-proof"]},
        "budget_snapshot": budget,
        "exception_registry": registry,
        "residue_proof": residue,
        "alarms": alarms,
    }


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--stage", choices=["budget-snapshot", "exception-registry", "residue-proof", "topology"], required=True)
    parser.add_argument("--summary-out", type=Path)
    return parser.parse_args(argv)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    summary_out = args.summary_out or default_summary_path(args.stage)

    if args.stage == "budget-snapshot":
        payload = run_budget_snapshot()
    elif args.stage == "exception-registry":
        payload = run_exception_registry()
    elif args.stage == "residue-proof":
        payload = run_residue_proof()
    else:
        payload = run_topology()

    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(payload, indent=2))
    return 0 if payload["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
