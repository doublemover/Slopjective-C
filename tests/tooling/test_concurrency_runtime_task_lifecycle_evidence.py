from __future__ import annotations

import json
import sys
from pathlib import Path

from scripts.objc3c_runnable_concurrency_e2e.catalog import CONCURRENCY_SCENARIOS


ROOT = Path(__file__).resolve().parents[2]
CONTRACT_PATH = (
    ROOT
    / "tests/tooling/fixtures/concurrency_runtime_closure/task_continuation_lifecycle_contract.json"
)
SCRIPT_ROOT = ROOT / "scripts"
TASK_HARDENING_ASSERTIONS = (
    ROOT / "tests/tooling/runtime/task_runtime_hardening_probe/runtime_assertion_helpers.h"
)


def _scenario_payloads() -> dict[str, dict[str, object]]:
    return {
        scenario.scenario_id: scenario.expected_payload
        for scenario in CONCURRENCY_SCENARIOS
    }


def test_task_lifecycle_summary_uses_contract_implementation_anchor() -> None:
    summary_builder = (
        ROOT / "scripts/build_concurrency_runtime_closure_task_lifecycle_summary.py"
    ).read_text(encoding="utf-8")

    assert 'contract["summary_implementation_anchor"]' in summary_builder
    assert 'contract["summary_script"]' not in summary_builder


def test_packaged_concurrency_e2e_asserts_lifecycle_contract_payloads() -> None:
    contract = json.loads(CONTRACT_PATH.read_text(encoding="utf-8"))
    payloads = _scenario_payloads()

    for key, expected_value in contract["required_continuation_probe_payload"].items():
        assert payloads["continuation"].get(key) == expected_value

    for key, expected_value in contract["required_task_probe_payload"].items():
        assert payloads["task"].get(key) == expected_value


def test_task_hardening_parser_recovers_lifecycle_contract_values() -> None:
    if str(SCRIPT_ROOT) not in sys.path:
        sys.path.insert(0, str(SCRIPT_ROOT))

    from scripts.build_concurrency_runtime_closure_task_lifecycle_summary import (
        parse_hardening_probe,
    )

    contract = json.loads(CONTRACT_PATH.read_text(encoding="utf-8"))
    parsed_values = parse_hardening_probe(TASK_HARDENING_ASSERTIONS)

    for key, expected_value in contract["required_task_hardening_probe_values"].items():
        assert parsed_values.get(key) == expected_value
