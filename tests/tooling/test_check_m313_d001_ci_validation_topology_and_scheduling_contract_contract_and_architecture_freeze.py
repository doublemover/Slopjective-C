from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CONTRACT = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_d001_ci_validation_topology_and_scheduling_contract_contract_and_architecture_freeze_contract.json"


def test_contract_tracks_expected_future_workflow_jobs() -> None:
    payload = json.loads(CONTRACT.read_text(encoding="utf-8"))
    assert [job["job_id"] for job in payload["future_workflow"]["jobs"]] == [
        "static-policy-guards",
        "acceptance-suites",
        "compatibility-bridges",
        "validation-topology",
    ]


def test_contract_tracks_expected_report_roots() -> None:
    payload = json.loads(CONTRACT.read_text(encoding="utf-8"))
    assert payload["report_roots"]["topology"] == "tmp/reports/m313/ci/acceptance_first/summary.json"
