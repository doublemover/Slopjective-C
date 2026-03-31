#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/concurrency_runtime_closure/actor_isolation_sendability_hop_contract.json"
OUT_DIR = ROOT / "tmp/reports/concurrency-runtime-closure/actor-isolation-sendability-hop"
JSON_OUT = OUT_DIR / "actor_isolation_sendability_hop_summary.json"
MD_OUT = OUT_DIR / "actor_isolation_sendability_hop_summary.md"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_concurrency_runtime_closure.md"
CONFORMANCE_REPORT = ROOT / "tmp/reports/runtime/runnable-concurrency-conformance/summary.json"
E2E_REPORT = ROOT / "tmp/reports/runtime/runnable-concurrency-e2e/summary.json"
ACTOR_LOWERING_PROBE = ROOT / "tests/tooling/runtime/actor_lowering_runtime_probe.cpp"
ACTOR_EXECUTOR_PROBE = ROOT / "tests/tooling/runtime/actor_runtime_executor_contract_probe.cpp"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def contains_required_case_ids(report: dict[str, Any], required_case_ids: list[str]) -> bool:
    declared = report.get("required_case_ids")
    return isinstance(declared, list) and all(case_id in declared for case_id in required_case_ids)


def parse_probe_contract(path: Path, markers: dict[str, str], truthy_marker: str | None = None) -> dict[str, int]:
    text = path.read_text(encoding="utf-8")
    summary: dict[str, int] = {}
    for key, marker in markers.items():
        idx = text.find(marker)
        if idx == -1:
            continue
        start = idx + len(marker)
        end = start
        while end < len(text) and text[end].isdigit():
            end += 1
        if end > start:
            summary[key] = int(text[start:end])
    if truthy_marker is not None:
        summary[truthy_marker] = 1 if truthy_marker in text else 0
    return summary


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    conformance_report = read_json(CONFORMANCE_REPORT)
    e2e_report = read_json(E2E_REPORT)

    fixture_paths = [ROOT / path for path in contract["authoritative_fixture_paths"]]
    probe_paths = [ROOT / path for path in contract["authoritative_probe_paths"]]
    code_paths = [ROOT / path for path in contract["authoritative_code_paths"]]

    actor_payload = e2e_report.get("probe_payloads", {}).get("actor", {})
    lowering_values = parse_probe_contract(
        ACTOR_LOWERING_PROBE,
        {
            "isolation": "isolation == ",
            "nonisolated": "nonisolated == ",
            "hopped": "hopped == ",
            "isolation_thunk_call_count": "snapshot.isolation_thunk_call_count == ",
            "nonisolated_entry_call_count": "snapshot.nonisolated_entry_call_count == ",
            "hop_to_executor_call_count": "snapshot.hop_to_executor_call_count == ",
            "last_isolation_executor_tag": "snapshot.last_isolation_executor_tag == ",
            "last_nonisolated_value": "snapshot.last_nonisolated_value == ",
            "last_nonisolated_executor_tag": "snapshot.last_nonisolated_executor_tag == ",
            "last_hop_value": "snapshot.last_hop_value == ",
            "last_hop_executor_tag": "snapshot.last_hop_executor_tag == ",
            "last_hop_result": "snapshot.last_hop_result == ",
        },
    )
    executor_values = parse_probe_contract(
        ACTOR_EXECUTOR_PROBE,
        {
            "replay": "replay == ",
            "guard": "guard == ",
            "isolation": "isolation == ",
            "nonisolated": "nonisolated == ",
            "hopped": "hopped == ",
            "replay_proof_call_count": "snapshot.replay_proof_call_count == ",
            "race_guard_call_count": "snapshot.race_guard_call_count == ",
            "isolation_thunk_call_count": "snapshot.isolation_thunk_call_count == ",
            "nonisolated_entry_call_count": "snapshot.nonisolated_entry_call_count == ",
            "hop_to_executor_call_count": "snapshot.hop_to_executor_call_count == ",
            "last_replay_proof_executor_tag": "snapshot.last_replay_proof_executor_tag == ",
            "last_race_guard_executor_tag": "snapshot.last_race_guard_executor_tag == ",
            "last_isolation_executor_tag": "snapshot.last_isolation_executor_tag == ",
            "last_nonisolated_value": "snapshot.last_nonisolated_value == ",
            "last_nonisolated_executor_tag": "snapshot.last_nonisolated_executor_tag == ",
            "last_hop_value": "snapshot.last_hop_value == ",
            "last_hop_executor_tag": "snapshot.last_hop_executor_tag == ",
            "last_hop_result": "snapshot.last_hop_result == ",
        },
    )

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_concurrency_runtime_closure_actor_semantics_summary.py",
        "all_authoritative_code_paths_exist": all(path.is_file() for path in code_paths),
        "all_authoritative_fixture_paths_exist": all(path.is_file() for path in fixture_paths),
        "all_authoritative_probe_paths_exist": all(path.is_file() for path in probe_paths),
        "runbook_mentions_actor_semantics_boundary": "actor isolation entry, nonisolated entry, executor hops, replay proof, race guard, executor binding, and mailbox ownership remain supported only through the private actor helper cluster and runtime snapshots" in runbook_text,
        "runbook_mentions_actor_interop_fail_closed": "broader interop and public actor runtime ABI claims remain out of scope for this milestone" in runbook_text,
        "conformance_report_passes": conformance_report.get("status") == "PASS",
        "conformance_report_preserves_required_case_ids": contains_required_case_ids(conformance_report, contract["authoritative_case_ids"]),
        "actor_payload_matches_contract": all(
            actor_payload.get(key) == value for key, value in contract["required_actor_probe_payload"].items()
        ),
        "actor_lowering_probe_matches_contract": all(
            lowering_values.get(key) == value for key, value in contract["required_actor_lowering_probe_values"].items()
        ),
        "actor_executor_probe_matches_contract": all(
            executor_values.get(key) == value for key, value in contract["required_actor_executor_contract_probe_values"].items()
        ),
    }

    summary = {
        "issue": "concurrency-runtime-closure-actor-isolation-sendability-hop",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "actor_model_count": len(contract["actor_models"]),
        "authoritative_case_id_count": len(contract["authoritative_case_ids"]),
        "required_actor_probe_value_count": len(contract["required_actor_probe_payload"]),
        "required_actor_lowering_value_count": len(contract["required_actor_lowering_probe_values"]),
        "required_actor_executor_value_count": len(contract["required_actor_executor_contract_probe_values"]),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Concurrency Runtime Closure Actor Semantics Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Actor models: `{summary['actor_model_count']}`\n"
        f"- Authoritative case ids: `{summary['authoritative_case_id_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
