from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
POLICY = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b004_legacy_validation_quarantine_and_namespace_policy_core_feature_implementation_policy.json"


def test_policy_freezes_active_cleanup_window() -> None:
    payload = json.loads(POLICY.read_text(encoding="utf-8"))
    assert payload["active_milestone_codes"] == [313, 314, 315, 316, 317, 318]


def test_policy_imports_namespace_quarantine_stage_maximums() -> None:
    payload = json.loads(POLICY.read_text(encoding="utf-8"))
    assert payload["ratchet_stage"]["owner_issue"] == "M313-B004"
    assert payload["ratchet_stage"]["stage"] == "namespace_quarantine"
    assert payload["ratchet_stage"]["maximums"] == {
        "check_scripts": 1674,
        "readiness_runners": 479,
        "pytest_check_files": 1666,
    }
