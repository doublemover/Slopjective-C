from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m313_b002_shared_compiler_runtime_integration_harness_core_feature_implementation.py"
HARNESS = ROOT / "scripts" / "shared_compiler_runtime_acceptance_harness.py"
SUMMARY = ROOT / "tmp" / "reports" / "m313" / "M313-B002" / "shared_compiler_runtime_harness_summary.json"


def test_b002_checker_writes_summary() -> None:
    completed = subprocess.run([sys.executable, str(CHECKER)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stderr or completed.stdout
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["contract_id"] == "objc3c-cleanup-shared-compiler-runtime-harness/m313-b002-v1"
    assert payload["next_issue"] == "M313-B003"
    assert payload["ok"] is True


def test_shared_harness_lists_expected_suites() -> None:
    completed = subprocess.run([sys.executable, str(HARNESS), "--list-suites"], cwd=ROOT, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stderr or completed.stdout
    payload = json.loads(completed.stdout)
    assert [entry["suite_id"] for entry in payload["suite_registry"]] == [
        "runtime_bootstrap_dispatch",
        "frontend_split_recovery",
        "module_parity_packaging",
        "native_fixture_corpus_and_runtime_probes",
    ]
