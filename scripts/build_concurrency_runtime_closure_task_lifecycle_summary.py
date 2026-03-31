#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/concurrency_runtime_closure/task_continuation_lifecycle_contract.json"
OUT_DIR = ROOT / "tmp/reports/concurrency-runtime-closure/task-continuation-lifecycle"
JSON_OUT = OUT_DIR / "task_continuation_lifecycle_summary.json"
MD_OUT = OUT_DIR / "task_continuation_lifecycle_summary.md"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_concurrency_runtime_closure.md"
CONFORMANCE_REPORT = ROOT / "tmp/reports/runtime/runnable-concurrency-conformance/summary.json"
E2E_REPORT = ROOT / "tmp/reports/runtime/runnable-concurrency-e2e/summary.json"
TASK_HARDENING_PROBE = ROOT / "tests/tooling/runtime/task_runtime_hardening_probe.cpp"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def contains_required_case_ids(report: dict[str, Any], required_case_ids: list[str]) -> bool:
    declared = report.get("required_case_ids")
    return isinstance(declared, list) and all(case_id in declared for case_id in required_case_ids)


def parse_hardening_probe(path: Path) -> dict[str, int]:
    text = path.read_text(encoding="utf-8")
    mapping = {
        "spawn_group": "pass1.spawn_group == ",
        "scope": "pass1.scope == ",
        "add_task": "pass1.add_task == ",
        "cancelled": "pass1.cancelled == ",
        "wait_next": "pass1.wait_next == ",
        "hop": "pass1.hop == ",
        "cancel_all": "pass1.cancel_all == ",
        "on_cancel": "pass1.on_cancel == ",
        "spawn_detached": "pass1.spawn_detached == ",
        "spawn_call_count": "pass1.task.spawn_call_count == ",
        "scope_call_count": "pass1.task.scope_call_count == ",
        "add_task_call_count": "pass1.task.add_task_call_count == ",
        "wait_next_call_count": "pass1.task.wait_next_call_count == ",
        "cancel_all_call_count": "pass1.task.cancel_all_call_count == ",
        "cancellation_poll_call_count": "pass1.task.cancellation_poll_call_count == ",
        "on_cancel_call_count": "pass1.task.on_cancel_call_count == ",
        "executor_hop_call_count": "pass1.task.executor_hop_call_count == ",
        "last_spawn_kind": "pass1.task.last_spawn_kind == ",
        "last_spawn_executor_tag": "pass1.task.last_spawn_executor_tag == ",
        "last_wait_next_result": "pass1.task.last_wait_next_result == ",
        "last_executor_hop_executor_tag": "pass1.task.last_executor_hop_executor_tag == ",
        "last_executor_hop_value": "pass1.task.last_executor_hop_value == ",
        "autoreleasepool_depth": "pass1.memory.autoreleasepool_depth == ",
        "autoreleasepool_max_depth": "pass1.memory.autoreleasepool_max_depth == ",
        "autoreleasepool_push_count": "pass1.arc.autoreleasepool_push_count == ",
        "autoreleasepool_pop_count": "pass1.arc.autoreleasepool_pop_count == ",
    }
    summary: dict[str, int] = {}
    for key, marker in mapping.items():
        idx = text.find(marker)
        if idx == -1:
            continue
        start = idx + len(marker)
        end = start
        while end < len(text) and text[end].isdigit():
            end += 1
        if end > start:
            summary[key] = int(text[start:end])
    summary["replay_equal"] = 1 if "Equivalent(pass1, pass2)" in text else 0
    return summary


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    conformance_report = read_json(CONFORMANCE_REPORT)
    e2e_report = read_json(E2E_REPORT)

    fixture_paths = [ROOT / path for path in contract["authoritative_fixture_paths"]]
    probe_paths = [ROOT / path for path in contract["authoritative_probe_paths"]]
    code_paths = [ROOT / path for path in contract["authoritative_code_paths"]]

    continuation_payload = e2e_report.get("probe_payloads", {}).get("continuation", {})
    task_payload = e2e_report.get("probe_payloads", {}).get("task", {})
    hardening_values = parse_hardening_probe(TASK_HARDENING_PROBE)

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_concurrency_runtime_closure_task_lifecycle_summary.py",
        "all_authoritative_code_paths_exist": all(path.is_file() for path in code_paths),
        "all_authoritative_fixture_paths_exist": all(path.is_file() for path in fixture_paths),
        "all_authoritative_probe_paths_exist": all(path.is_file() for path in probe_paths),
        "runbook_mentions_task_lifecycle_boundary": "continuation allocation, handoff, resume, task spawn, task-group scope, task-group add/wait/cancel, and on-cancel behavior remain supported only through the emitted lowering packets and private runtime-owned helper ABI" in runbook_text,
        "runbook_mentions_successor_bounded_arc_cleanup": "interaction with ARC and thrown-error cleanup remains a successor concern until dedicated runtime evidence lands" in runbook_text,
        "conformance_report_passes": conformance_report.get("status") == "PASS",
        "conformance_report_preserves_required_case_ids": contains_required_case_ids(conformance_report, contract["authoritative_case_ids"]),
        "continuation_payload_matches_contract": all(
            continuation_payload.get(key) == value for key, value in contract["required_continuation_probe_payload"].items()
        ),
        "task_payload_matches_contract": all(
            task_payload.get(key) == value for key, value in contract["required_task_probe_payload"].items()
        ),
        "task_hardening_probe_matches_contract": all(
            hardening_values.get(key) == value for key, value in contract["required_task_hardening_probe_values"].items()
        ),
    }

    summary = {
        "issue": "concurrency-runtime-closure-task-continuation-lifecycle",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "lifecycle_model_count": len(contract["lifecycle_models"]),
        "authoritative_case_id_count": len(contract["authoritative_case_ids"]),
        "required_continuation_probe_value_count": len(contract["required_continuation_probe_payload"]),
        "required_task_probe_value_count": len(contract["required_task_probe_payload"]),
        "required_task_hardening_value_count": len(contract["required_task_hardening_probe_values"]),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Concurrency Runtime Closure Task/Continuation Lifecycle Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Lifecycle models: `{summary['lifecycle_model_count']}`\n"
        f"- Authoritative case ids: `{summary['authoritative_case_id_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
