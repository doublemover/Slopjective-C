#!/usr/bin/env python3
from __future__ import annotations

import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
ACTOR_CONTRACT_PATH = ROOT / "tests/tooling/fixtures/concurrency_runtime_closure/actor_isolation_sendability_hop_contract.json"
E2E_REPORT = ROOT / "tmp/reports/runtime/runnable-concurrency-e2e/summary.json"
ACCEPTANCE_REPORT = ROOT / "tmp/reports/runtime/acceptance/summary.json"
OUT_DIR = ROOT / "tmp/reports/concurrency-runtime-closure/live-actor-runtime"
JSON_OUT = OUT_DIR / "live_actor_runtime_summary.json"
SUMMARY_CONTRACT_ID = "objc3c.concurrency.runtime.closure.live.actor.runtime.summary.v1"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace('\\', '/')


def load_json(path: Path) -> dict[str, Any]:
    expect(path.is_file(), f"expected JSON artifact was not published: {repo_rel(path)}")
    payload = json.loads(path.read_text(encoding='utf-8'))
    expect(isinstance(payload, dict), f"JSON artifact at {repo_rel(path)} did not contain an object")
    return payload


def find_case(report: dict[str, Any], case_id: str) -> dict[str, Any]:
    for case in report.get('cases', []):
        if isinstance(case, dict) and case.get('case_id') == case_id:
            return case
    raise RuntimeError(f"expected acceptance case was not published: {case_id}")


def main() -> int:
    actor_contract = load_json(ACTOR_CONTRACT_PATH)
    e2e_report = load_json(E2E_REPORT)
    acceptance_report = load_json(ACCEPTANCE_REPORT)

    expect(e2e_report.get('status') == 'PASS', 'runnable concurrency e2e report did not publish PASS')
    expect(acceptance_report.get('status') == 'PASS', 'runtime acceptance report did not publish PASS')

    actor_payload = e2e_report.get('probe_payloads', {}).get('actor', {})
    for field_name, expected_value in actor_contract['required_actor_probe_payload'].items():
        expect(
            actor_payload.get(field_name) == expected_value,
            f"expected runnable concurrency actor payload to preserve {field_name}",
        )

    live_case = find_case(acceptance_report, 'live-unified-concurrency-runtime-implementation')
    cross_module_case = find_case(acceptance_report, 'cross-module-concurrency-actor-artifact-preservation')
    expect(live_case.get('passed') is True, 'live unified concurrency runtime acceptance case did not pass')
    expect(cross_module_case.get('passed') is True, 'cross-module actor preservation acceptance case did not pass')

    live_summary = live_case.get('summary', {})
    cross_module_summary = cross_module_case.get('summary', {})
    expect(live_summary.get('actor_fixture') == 'tests/tooling/fixtures/native/actor_lowering_runtime_positive.objc3', 'expected live actor fixture to stay on the actor lowering runtime fixture')
    expect(live_summary.get('actor_probe') == 'tests/tooling/runtime/live_actor_mailbox_runtime_probe.cpp', 'expected live actor probe to stay on the mailbox runtime probe')
    expect(cross_module_summary.get('provider_fixture') == 'tests/tooling/fixtures/native/cross_module_actor_isolation_provider.objc3', 'expected cross-module provider fixture to stay on the actor provider fixture')
    expect(cross_module_summary.get('consumer_fixture') == 'tests/tooling/fixtures/native/cross_module_actor_isolation_consumer.objc3', 'expected cross-module consumer fixture to stay on the actor consumer fixture')

    payload = {
        'contract_id': SUMMARY_CONTRACT_ID,
        'generated_at_utc': datetime.now(timezone.utc).isoformat(),
        'status': 'PASS',
        'runner_path': 'scripts/check_concurrency_runtime_closure_live_actor_runtime.py',
        'used_existing_runnable_concurrency_e2e_report': True,
        'used_existing_runtime_acceptance_report': True,
        'child_report_paths': [repo_rel(E2E_REPORT), repo_rel(ACCEPTANCE_REPORT)],
        'actor_payload': actor_payload,
        'cross_module_summary': cross_module_summary,
    }
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + '\n', encoding='utf-8')
    print(f"summary_path: {repo_rel(JSON_OUT)}")
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
