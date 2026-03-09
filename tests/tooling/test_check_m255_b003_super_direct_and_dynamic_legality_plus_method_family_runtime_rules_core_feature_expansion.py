from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = (
    ROOT
    / "scripts"
    / "check_m255_b003_super_direct_and_dynamic_legality_plus_method_family_runtime_rules_core_feature_expansion.py"
)
SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "m255"
    / "M255-B003"
    / "super_direct_dynamic_method_family_summary.json"
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
    assert payload["mode"] == "m255-b003-super-direct-dynamic-method-family-core-feature-expansion-v1"
    assert payload["contract_id"] == "objc3c-super-dynamic-method-family/m255-b003-v1"
    assert payload["ok"] is True
    assert payload["policies"]["super_legality"] == "super-requires-enclosing-method-and-real-superclass"
    assert payload["policies"]["direct_dispatch"] == "direct-dispatch-remains-reserved-non-goal"
    assert payload["policies"]["dynamic_dispatch"] == "dynamic-dispatch-preserves-runtime-resolution-and-method-family-accounting"
    assert payload["dynamic_probes"]["positive"]["returncode"] == 0
    assert payload["dynamic_probes"]["super_outside_method"]["returncode"] != 0
    assert payload["dynamic_probes"]["super_root_dispatch"]["returncode"] != 0
    assert payload["dynamic_probes"]["positive"]["dispatch_surface"]["super_dispatch_sites"] == 4
    assert payload["dynamic_probes"]["positive"]["dispatch_surface"]["dynamic_dispatch_sites"] == 3
    assert payload["dynamic_probes"]["positive"]["method_family_surface"]["method_family_copy_sites"] == 2
