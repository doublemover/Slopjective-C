from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
REGISTRY_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_b004_alias_deprecation_and_compatibility_window_core_feature_implementation_registry.json"


def test_registry_freezes_four_compatibility_families() -> None:
    payload = json.loads(REGISTRY_JSON.read_text(encoding="utf-8"))
    assert len(payload["compatibility_families"]) == 4


def test_registry_requires_no_new_legacy_alias_families() -> None:
    payload = json.loads(REGISTRY_JSON.read_text(encoding="utf-8"))
    assert payload["no_new_legacy_alias_families"] is True
