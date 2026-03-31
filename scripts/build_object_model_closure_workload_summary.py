#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/object_model_closure/loader_category_protocol_workload_map.json"
RUNNER_PATH = ROOT / "scripts/check_objc3c_runnable_object_model_conformance.py"
OUT_DIR = ROOT / "tmp/reports/object-model-closure/loader-category-protocol-workload"
JSON_OUT = OUT_DIR / "loader_category_protocol_workload_summary.json"
MD_OUT = OUT_DIR / "loader_category_protocol_workload_summary.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runner_text = RUNNER_PATH.read_text(encoding="utf-8")
    workload_paths = []
    for workload in contract["workloads"]:
        workload_paths.extend(ROOT / path for path in workload["fixtures"])
        workload_paths.extend(ROOT / path for path in workload["probes"])

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_object_model_closure_workload_summary.py",
        "all_workload_paths_exist": all(path.is_file() for path in workload_paths),
        "all_authoritative_runners_exist": all((ROOT / path).is_file() for path in contract["authoritative_runners"]),
        "required_case_ids_are_wired_into_conformance_runner": all(
            case_id in runner_text
            for workload in contract["workloads"]
            for case_id in workload["required_case_ids"]
        ),
    }
    summary = {
        "issue": "object-model-closure-loader-category-protocol-workload",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "workload_count": len(contract["workloads"]),
        "required_case_id_count": sum(len(workload["required_case_ids"]) for workload in contract["workloads"]),
        "fixture_count": sum(len(workload["fixtures"]) for workload in contract["workloads"]),
        "probe_count": sum(len(workload["probes"]) for workload in contract["workloads"]),
        "checks": checks,
        "ok": all(checks.values())
    }
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Object-Model Closure Workload Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Workloads: `{summary['workload_count']}`\n"
        f"- Required case ids: `{summary['required_case_id_count']}`\n"
        f"- Fixtures: `{summary['fixture_count']}`\n"
        f"- Probes: `{summary['probe_count']}`\n"
        f"- Status: `{'PASS' if summary['ok'] else 'FAIL'}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
