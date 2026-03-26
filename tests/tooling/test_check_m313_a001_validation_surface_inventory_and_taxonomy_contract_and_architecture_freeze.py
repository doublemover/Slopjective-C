from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
INVENTORY = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_a001_validation_surface_inventory_and_taxonomy_contract_and_architecture_freeze_inventory.json"


def test_inventory_freezes_required_count_keys() -> None:
    payload = json.loads(INVENTORY.read_text(encoding="utf-8"))
    assert set(payload["measured_counts"].keys()) == {"check_scripts", "readiness_runners", "pytest_check_files", "runtime_probe_cpp", "native_fixture_objc3"}


def test_inventory_freezes_namespace_policy_and_next_issue() -> None:
    payload = json.loads(INVENTORY.read_text(encoding="utf-8"))
    assert payload["namespace_policy"]["active"] == ["tests/tooling/runtime", "tests/tooling/fixtures/native", "tmp/reports"]
    assert payload["namespace_policy"]["migration_only"] == ["scripts/check_*.py", "scripts/run_*_readiness.py", "tests/tooling/test_check_*.py"]
    assert payload["next_issue"] == "M313-A002"
