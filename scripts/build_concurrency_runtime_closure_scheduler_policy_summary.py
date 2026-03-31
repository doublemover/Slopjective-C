#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/concurrency_runtime_closure/scheduler_executor_cancellation_policy.json"
OUT_DIR = ROOT / "tmp/reports/concurrency-runtime-closure/scheduler-executor-cancellation-policy"
JSON_OUT = OUT_DIR / "scheduler_executor_cancellation_policy_summary.json"
MD_OUT = OUT_DIR / "scheduler_executor_cancellation_policy_summary.md"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_concurrency_runtime_closure.md"
CONFORMANCE_REPORT = ROOT / "tmp/reports/runtime/runnable-concurrency-conformance/summary.json"
E2E_REPORT = ROOT / "tmp/reports/runtime/runnable-concurrency-e2e/summary.json"
ACTOR_EXECUTOR_PROBE = ROOT / "tests/tooling/runtime/actor_runtime_executor_contract_probe.cpp"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def run_probe_source_summary(path: Path) -> dict[str, int]:
    text = path.read_text(encoding="utf-8")
    keys = [
        "replay",
        "guard",
        "isolation",
        "nonisolated",
        "hopped",
        "replay_proof_call_count",
        "race_guard_call_count",
        "isolation_thunk_call_count",
        "nonisolated_entry_call_count",
        "hop_to_executor_call_count",
        "last_replay_proof_executor_tag",
        "last_race_guard_executor_tag",
        "last_isolation_executor_tag",
        "last_nonisolated_value",
        "last_nonisolated_executor_tag",
        "last_hop_value",
        "last_hop_executor_tag",
        "last_hop_result",
    ]
    summary: dict[str, int] = {}
    for key in keys:
        marker = f"{key} == "
        idx = text.find(marker)
        if idx == -1:
            continue
        start = idx + len(marker)
        end = start
        while end < len(text) and text[end].isdigit():
            end += 1
        if end > start:
            summary[key] = int(text[start:end])
    return summary


def contains_required_case_ids(report: dict[str, Any], required_case_ids: list[str]) -> bool:
    declared = report.get("required_case_ids")
    if isinstance(declared, list):
        return all(case_id in declared for case_id in required_case_ids)
    case_ids = {case.get("case_id") for case in report.get("cases", []) if isinstance(case, dict)}
    return all(case_id in case_ids for case_id in required_case_ids)


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    conformance_report = read_json(CONFORMANCE_REPORT)
    e2e_report = read_json(E2E_REPORT)

    fixture_paths = [ROOT / path for path in contract["authoritative_fixture_paths"]]
    probe_paths = [ROOT / path for path in contract["authoritative_probe_paths"]]
    code_paths = [ROOT / path for path in contract["authoritative_code_paths"]]

    task_payload = e2e_report.get("probe_payloads", {}).get("task", {})
    actor_payload = e2e_report.get("probe_payloads", {}).get("actor", {})
    abi_surface = conformance_report.get("runtime_unified_concurrency_runtime_abi_surface", {})
    actor_executor_contract = run_probe_source_summary(ACTOR_EXECUTOR_PROBE)
    required_task_payload = contract["required_task_probe_payload"]
    required_actor_payload = contract["required_actor_probe_payload"]
    required_actor_executor_contract = contract["required_actor_executor_contract_probe_values"]

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_concurrency_runtime_closure_scheduler_policy_summary.py",
        "all_authoritative_code_paths_exist": all(path.is_file() for path in code_paths),
        "all_authoritative_fixture_paths_exist": all(path.is_file() for path in fixture_paths),
        "all_authoritative_probe_paths_exist": all(path.is_file() for path in probe_paths),
        "runbook_mentions_scheduler_policy_boundary": "executor selection, cancellation polling, cancellation callbacks, and executor hops are supported only through the current private runtime helper cluster" in runbook_text,
        "runbook_mentions_fail_closed_fairness": (
            "scheduler fairness, distributed actor transport, or external executor integration are complete beyond the current manifest/runtime-registration/replay proof"
            in runbook_text
        ),
        "conformance_report_passes": conformance_report.get("status") == "PASS",
        "conformance_report_preserves_required_case_ids": contains_required_case_ids(
            conformance_report, contract["authoritative_case_ids"]
        ),
        "conformance_runtime_abi_surface_preserves_runtime_models": (
            isinstance(abi_surface, dict)
            and abi_surface.get("continuation_runtime_model") == "continuation-allocation-handoff-resume-and-testing-snapshots-stay-on-bootstrap-internal-runtime-entrypoints"
            and abi_surface.get("task_runtime_model") == "task-spawn-group-cancellation-executor-hop-and-testing-snapshots-stay-on-bootstrap-internal-runtime-entrypoints"
            and abi_surface.get("actor_runtime_model") == "actor-isolation-nonisolated-hop-replay-race-guard-mailbox-and-testing-snapshots-stay-on-bootstrap-internal-runtime-entrypoints"
        ),
        "task_probe_payload_preserves_scheduler_counts": all(
            task_payload.get(key) == value for key, value in required_task_payload.items()
        ),
        "actor_probe_payload_preserves_executor_and_mailbox_counts": all(
            actor_payload.get(key) == value for key, value in required_actor_payload.items()
        ),
        "actor_executor_contract_probe_preserves_expected_values": all(
            actor_executor_contract.get(key) == value
            for key, value in required_actor_executor_contract.items()
        ),
    }

    summary = {
        "issue": "concurrency-runtime-closure-scheduler-executor-cancellation-policy",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "workload_rule_count": len(contract["workload_rules"]),
        "authoritative_case_id_count": len(contract["authoritative_case_ids"]),
        "required_task_probe_value_count": len(required_task_payload),
        "required_actor_probe_value_count": len(required_actor_payload),
        "required_actor_executor_contract_value_count": len(required_actor_executor_contract),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Concurrency Runtime Closure Scheduler Policy Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Workload rules: `{summary['workload_rule_count']}`\n"
        f"- Authoritative case ids: `{summary['authoritative_case_id_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
