from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SUMMARY = ROOT / "tmp" / "reports" / "m266" / "M266-A002" / "frontend_pattern_guard_surface_summary.json"


def run_checker(*extra: str) -> dict[str, object]:
    completed = subprocess.run(
        [
            sys.executable,
            "scripts/check_m266_a002_frontend_pattern_grammar_and_guard_surface_completion_core_feature_implementation.py",
            "--summary-out",
            str(SUMMARY),
            *extra,
        ],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    return json.loads(SUMMARY.read_text(encoding="utf-8"))


def test_static_contract_surface() -> None:
    payload = run_checker("--skip-dynamic-probes")
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_failed"] == 0


def test_dynamic_contract_surface() -> None:
    payload = run_checker()
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    dynamic = payload["dynamic"]
    surface = dynamic["part5_control_flow_source_closure"]
    assert surface["match_statement_sites"] == 2
    assert surface["match_result_case_pattern_sites"] == 2
    assert len(dynamic["negative_cases"]) == 3
