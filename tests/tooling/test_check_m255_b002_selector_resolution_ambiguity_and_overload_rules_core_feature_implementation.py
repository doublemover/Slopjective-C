from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = (
    ROOT
    / "scripts"
    / "check_m255_b002_selector_resolution_ambiguity_and_overload_rules_core_feature_implementation.py"
)
SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "m255"
    / "M255-B002"
    / "selector_resolution_ambiguity_summary.json"
)


def test_checker_passes() -> None:
    result = subprocess.run(
        [sys.executable, str(SCRIPT)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
    )
    assert result.returncode == 0, result.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["mode"] == "m255-b002-selector-resolution-ambiguity-core-feature-implementation-v1"
    assert payload["contract_id"] == "objc3c-selector-resolution-ambiguity/m255-b002-v1"
    assert payload["ok"] is True
    assert payload["concrete_receiver_policy"] == "self-super-known-class-receivers-resolve-concretely"
    assert payload["dynamic_fallback_policy"] == "non-concrete-receivers-remain-runtime-dynamic"
    assert payload["diagnostics"]["missing_selector"] == "O3S216"
    assert payload["diagnostics"]["ambiguous_selector"] == "O3S217"
    assert payload["dynamic_probes"]["positive"]["returncode"] == 0
    assert payload["dynamic_probes"]["missing_selector"]["returncode"] != 0
    assert payload["dynamic_probes"]["ambiguous_signature"]["returncode"] != 0
