#!/usr/bin/env python3
from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/runtime_corrective/acceptance_workload_map.json"
OUT_DIR = ROOT / "tmp/reports/runtime-corrective/acceptance-workload-map"
JSON_OUT = OUT_DIR / "acceptance_workload_map_summary.json"
MD_OUT = OUT_DIR / "acceptance_workload_map_summary.md"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_runtime_corrective.md"
HARNESS_PATH = ROOT / "scripts/shared_compiler_runtime_acceptance_harness.py"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def run_command(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        command,
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")

    suite_payloads: dict[str, dict[str, Any]] = {}
    suite_ok = True
    for suite_id in contract["suite_ids"]:
        result = run_command([sys.executable, str(HARNESS_PATH), "--show-suite", suite_id])
        if result.returncode != 0:
            suite_ok = False
            continue
        payload = json.loads(result.stdout)
        suite_payloads[suite_id] = payload

    catalog_result = run_command([sys.executable, str(HARNESS_PATH), "--check-catalog"])
    catalog_payload = json.loads(catalog_result.stdout) if catalog_result.returncode == 0 else {}

    dispatch_probe_paths = [ROOT / path for path in contract["dispatch_focus"]["authoritative_probe_paths"]]
    synthesized_probe_paths = [ROOT / path for path in contract["synthesized_accessor_focus"]["authoritative_probe_paths"]]
    synthesized_fixture_paths = [ROOT / path for path in contract["synthesized_accessor_focus"]["authoritative_fixture_paths"]]

    checks = {
        "runbook_link_matches": contract["runbook"] == "docs/runbooks/objc3c_runtime_corrective.md",
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_runtime_corrective_acceptance_workload_summary.py",
        "suite_catalog_check_passes": catalog_result.returncode == 0 and catalog_payload.get("ok") is True,
        "all_suite_payloads_loaded": suite_ok and len(suite_payloads) == len(contract["suite_ids"]),
        "runtime_acceptance_suite_present": "runtime-acceptance" in suite_payloads,
        "public_test_fast_suite_present": "public-test-fast" in suite_payloads,
        "public_test_full_suite_present": "public-test-full" in suite_payloads,
        "all_dispatch_probe_paths_exist": all(path.is_file() for path in dispatch_probe_paths),
        "all_synthesized_probe_paths_exist": all(path.is_file() for path in synthesized_probe_paths),
        "all_synthesized_fixture_paths_exist": all(path.is_file() for path in synthesized_fixture_paths),
        "required_reports_align_with_suite_ids": contract["required_reports"] == [
            "tmp/reports/runtime/acceptance/summary.json",
            "tmp/reports/objc3c-public-workflow/test-fast.json",
            "tmp/reports/objc3c-public-workflow/test-full.json",
        ],
        "runbook_mentions_acceptance_workload_map": "acceptance_workload_map.json" in runbook_text,
        "non_goals_keep_closeout_claims_out_of_b004": "no-final-closeout-claim-from-catalog-check-alone" in contract["explicit_non_goals"],
    }

    measured_inventory = {
        "suite_count": len(contract["suite_ids"]),
        "dispatch_case_count": len(contract["dispatch_focus"]["runtime_acceptance_case_ids"]),
        "dispatch_probe_count": len(contract["dispatch_focus"]["authoritative_probe_paths"]),
        "synthesized_fixture_count": len(contract["synthesized_accessor_focus"]["authoritative_fixture_paths"]),
        "synthesized_probe_count": len(contract["synthesized_accessor_focus"]["authoritative_probe_paths"]),
        "required_report_count": len(contract["required_reports"]),
        "explicit_non_goal_count": len(contract["explicit_non_goals"]),
        "catalog_suite_result_count": len(catalog_payload.get("suite_results", [])),
    }

    summary = {
        "issue": "runtime-corrective-acceptance-workload-map",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "measured_inventory": measured_inventory,
        "checks": checks,
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Runtime Corrective Acceptance Workload Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Suites: `{measured_inventory['suite_count']}`\n"
        f"- Dispatch cases: `{measured_inventory['dispatch_case_count']}`\n"
        f"- Dispatch probes: `{measured_inventory['dispatch_probe_count']}`\n"
        f"- Synthesized-accessor fixtures: `{measured_inventory['synthesized_fixture_count']}`\n"
        f"- Synthesized-accessor probes: `{measured_inventory['synthesized_probe_count']}`\n"
        f"- Status: `{'PASS' if summary['ok'] else 'FAIL'}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
