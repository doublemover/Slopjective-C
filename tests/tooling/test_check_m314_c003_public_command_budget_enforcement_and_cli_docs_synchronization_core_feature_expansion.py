from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
REGISTRY_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_c003_public_command_budget_enforcement_and_cli_docs_synchronization_core_feature_expansion_registry.json"


def test_registry_keeps_public_command_budget_maximum() -> None:
    payload = json.loads(REGISTRY_JSON.read_text(encoding="utf-8"))
    assert payload["public_script_budget_maximum"] == 25


def test_registry_targets_the_synchronized_runbook_path() -> None:
    payload = json.loads(REGISTRY_JSON.read_text(encoding="utf-8"))
    assert payload["runbook_path"] == "docs/runbooks/objc3c_public_command_surface.md"
