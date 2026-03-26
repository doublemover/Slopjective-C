#!/usr/bin/env python3
"""Acceptance-first CI runner for M313 lane-D/E work."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]

D001_CONTRACT = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_d001_ci_validation_topology_and_scheduling_contract_contract_and_architecture_freeze_contract.json"
D002_PLAN = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_d002_ci_migration_to_acceptance_first_validation_core_feature_implementation_plan.json"
A002_RATCHET = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_a002_validation_reduction_targets_and_ratchet_map_source_completion_ratchet_map.json"
B004_POLICY = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b004_legacy_validation_quarantine_and_namespace_policy_core_feature_implementation_policy.json"
B005_REGISTRY = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b005_new_checker_and_temporary_validation_exception_policy_edge_case_and_compatibility_completion_exception_registry.json"
C002_PLAN = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_c002_subsystem_executable_acceptance_suites_core_feature_implementation_plan.json"
C003_PLAN = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_c003_historical_checker_compatibility_bridge_and_deprecation_surface_core_feature_expansion_plan.json"
HARNESS = ROOT / "scripts" / "shared_compiler_runtime_acceptance_harness.py"
BRIDGE_TOOL = ROOT / "scripts" / "m313_historical_checker_compatibility_bridge.py"

CLASS_TO_GLOB = {
    "issue_local_checker": "scripts/check_*.py",
    "issue_local_readiness_runner": "scripts/run_*_readiness.py",
    "issue_local_pytest_wrapper": "tests/tooling/test_check_*.py",
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
    quarantine: list[str] = []
    retained_non_milestone: list[str] = []
    for path in sorted(ROOT.glob(glob)):
        rel_path = rel(path)
        code = extract_milestone_code(path.name)
        if code is None:
            retained_non_milestone.append(rel_path)
        elif code in active_codes:
            active.append(rel_path)
        else:
            quarantine.append(rel_path)
    return {
        "active": active,
        "quarantine": quarantine,
        "retained_non_milestone": retained_non_milestone,
        "non_quarantined_count": len(active) + len(retained_non_milestone),
        "total_count": len(active) + len(quarantine) + len(retained_non_milestone),
    }


def run_json(command: list[str]) -> tuple[int, dict[str, Any] | None, str]:
    completed = subprocess.run(command, cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)
    if completed.returncode != 0:
        return completed.returncode, None, completed.stderr.strip() or completed.stdout.strip()
    try:
        return completed.returncode, json.loads(completed.stdout), ""
    except json.JSONDecodeError as exc:
        return 1, None, str(exc)


def stage_summary_path(stage: str) -> Path:
    plan = read_json(D002_PLAN)
    return ROOT / plan["default_reports"][stage]


def run_static_guards() -> dict[str, Any]:
    contract = read_json(D001_CONTRACT)
    results: list[dict[str, Any]] = []
    ok = True
    for guard in contract["retained_static_guards"]:
        command = guard["command"].split()
        completed = subprocess.run(command, cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)
        enforcement_mode = guard.get("enforcement_mode", "strict")
        blocking = enforcement_mode != "observational"
        result = {
            "guard_id": guard["guard_id"],
            "guard_class": guard["guard_class"],
            "owner_issue": guard["owner_issue"],
            "command": guard["command"],
            "returncode": completed.returncode,
            "enforcement_mode": enforcement_mode,
            "blocking": blocking,
        }
        if completed.returncode != 0:
            result["stderr"] = (completed.stderr or completed.stdout).strip()
            if blocking:
                ok = False
            else:
                result["observed_drift"] = True
        results.append(result)
    return {
        "mode": "m313-d002-ci-acceptance-first-migration-v1",
        "contract_id": "objc3c-cleanup-ci-acceptance-first-migration/m313-d002-v1",
        "stage": "static-guards",
        "ok": ok,
        "results": results,
    }


def run_acceptance_suites() -> dict[str, Any]:
    plan = read_json(C002_PLAN)
    results: list[dict[str, Any]] = []
    ok = True
    for suite in plan["suite_execution_plan"]:
        suite_id = suite["suite_id"]
        summary_path = ROOT / f"tmp/reports/m313/acceptance/{suite_id}/summary.json"
        rc, payload, error = run_json([sys.executable, str(HARNESS), "--run-suite", suite_id, "--summary-out", str(summary_path)])
        ok = ok and rc == 0 and isinstance(payload, dict) and bool(payload.get("ok"))
        case_counts = payload.get("measurements", {}).get("case_counts", {}) if isinstance(payload, dict) else {}
        results.append({
            "suite_id": suite_id,
            "summary_path": rel(summary_path),
            "ok": bool(isinstance(payload, dict) and payload.get("ok")),
            "objc3_fixture_count": case_counts.get("objc3_fixture_count", 0),
            "runtime_probe_count": case_counts.get("runtime_probe_count", 0),
            "error": error or None,
        })
    return {
        "mode": "m313-d002-ci-acceptance-first-migration-v1",
        "contract_id": "objc3c-cleanup-ci-acceptance-first-migration/m313-d002-v1",
        "stage": "acceptance-suites",
        "ok": ok,
        "results": results,
    }


def run_compatibility_bridges() -> dict[str, Any]:
    plan = read_json(C003_PLAN)
    results: list[dict[str, Any]] = []
    ok = True
    for bridge in plan["bridge_families"]:
        bridge_id = bridge["bridge_id"]
        summary_path = ROOT / f"tmp/reports/m313/compatibility/{bridge_id}/summary.json"
        rc, payload, error = run_json([sys.executable, str(BRIDGE_TOOL), "--run-bridge", bridge_id, "--summary-out", str(summary_path)])
        ok = ok and rc == 0 and isinstance(payload, dict) and bool(payload.get("ok"))
        measures = payload.get("measurements", {}) if isinstance(payload, dict) else {}
        results.append({
            "bridge_id": bridge_id,
            "suite_id": bridge["suite_id"],
            "summary_path": rel(summary_path),
            "ok": bool(isinstance(payload, dict) and payload.get("ok")),
            "legacy_wrappers_observed": measures.get("legacy_wrappers_observed", 0),
            "legacy_wrappers_remaining": measures.get("legacy_wrappers_remaining", 0),
            "deprecation_owner_issue": measures.get("deprecation_owner_issue"),
            "error": error or None,
        })
    return {
        "mode": "m313-d002-ci-acceptance-first-migration-v1",
        "contract_id": "objc3c-cleanup-ci-acceptance-first-migration/m313-d002-v1",
        "stage": "compatibility-bridges",
        "ok": ok,
        "results": results,
    }


def build_budget_snapshot() -> dict[str, Any]:
    a002 = read_json(A002_RATCHET)
    b004 = read_json(B004_POLICY)
    active_codes = set(b004["active_milestone_codes"])
    registry = read_json(B005_REGISTRY)
    classified = {
        "check_scripts": classify(CLASS_TO_GLOB["issue_local_checker"], active_codes),
        "readiness_runners": classify(CLASS_TO_GLOB["issue_local_readiness_runner"], active_codes),
        "pytest_check_files": classify(CLASS_TO_GLOB["issue_local_pytest_wrapper"], active_codes),
    }
    non_quarantined_counts = {key: value["non_quarantined_count"] for key, value in classified.items()}
    gate_threshold = a002["closeout_maximums"]
    registry_entries = registry.get("exceptions", []) if isinstance(registry, dict) else registry
    active_exceptions = [entry for entry in registry_entries if isinstance(entry, dict) and entry.get("status") == "active"]
    gate_threshold_met = all(non_quarantined_counts[key] <= gate_threshold[key] for key in gate_threshold)
    return {
        "non_quarantined_counts": non_quarantined_counts,
        "gate_threshold_maximums": gate_threshold,
        "gate_threshold_met": gate_threshold_met,
        "active_exception_count": len(active_exceptions),
        "active_exception_ids": [entry["id"] for entry in active_exceptions],
        "classification_samples": {
            key: {
                "active_count": len(value["active"]),
                "retained_non_milestone_count": len(value["retained_non_milestone"]),
                "quarantined_count": len(value["quarantine"]),
                "active_samples": value["active"][:5],
                "retained_samples": value["retained_non_milestone"][:5],
                "quarantined_samples": value["quarantine"][:5]
            }
            for key, value in classified.items()
        }
    }


def run_topology() -> dict[str, Any]:
    static_summary = run_static_guards()
    acceptance_summary = run_acceptance_suites()
    bridge_summary = run_compatibility_bridges()
    budget_snapshot = build_budget_snapshot()
    ok = static_summary["ok"] and acceptance_summary["ok"] and bridge_summary["ok"] and budget_snapshot["gate_threshold_met"] and budget_snapshot["active_exception_count"] == 0
    return {
        "mode": "m313-d002-ci-acceptance-first-migration-v1",
        "contract_id": "objc3c-cleanup-ci-acceptance-first-migration/m313-d002-v1",
        "stage": "topology",
        "ok": ok,
        "stage_order": [
            "static-guards",
            "acceptance-suites",
            "compatibility-bridges",
            "topology"
        ],
        "stage_reports": {
            "static-guards": rel(stage_summary_path("static-guards")),
            "acceptance-suites": rel(stage_summary_path("acceptance-suites")),
            "compatibility-bridges": rel(stage_summary_path("compatibility-bridges"))
        },
        "suite_results": acceptance_summary["results"],
        "bridge_results": bridge_summary["results"],
        "budget_snapshot": budget_snapshot
    }


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--stage", choices=["static-guards", "acceptance-suites", "compatibility-bridges", "topology"], required=True)
    parser.add_argument("--summary-out", type=Path)
    return parser.parse_args(argv)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    summary_out = args.summary_out or stage_summary_path(args.stage)

    if args.stage == "static-guards":
        payload = run_static_guards()
    elif args.stage == "acceptance-suites":
        payload = run_acceptance_suites()
    elif args.stage == "compatibility-bridges":
        payload = run_compatibility_bridges()
    else:
        payload = run_topology()

    payload["summary_path"] = rel(summary_out)
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(payload, indent=2))
    return 0 if payload.get("ok") else 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
