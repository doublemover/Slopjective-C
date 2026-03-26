from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
REGISTRY_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_c002_parameterized_task_runner_implementation_core_feature_implementation_registry.json"


def test_registry_keeps_single_pass_through_action() -> None:
    payload = json.loads(REGISTRY_JSON.read_text(encoding="utf-8"))
    assert payload["pass_through_actions"] == ["compile-objc3c"]


def test_registry_freezes_seventeen_actions() -> None:
    payload = json.loads(REGISTRY_JSON.read_text(encoding="utf-8"))
    assert payload["action_count"] == 17
