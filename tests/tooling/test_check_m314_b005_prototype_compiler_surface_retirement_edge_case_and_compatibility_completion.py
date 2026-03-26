from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
REGISTRY_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_b005_prototype_compiler_surface_retirement_edge_case_and_compatibility_completion_registry.json"


def test_registry_marks_prototype_surface_as_retired_archived_text() -> None:
    payload = json.loads(REGISTRY_JSON.read_text(encoding="utf-8"))
    assert payload["retired_surface"]["state"] == "retired-archived-text"


def test_registry_keeps_native_objc3c_as_supported_compiler_root() -> None:
    payload = json.loads(REGISTRY_JSON.read_text(encoding="utf-8"))
    assert payload["retired_surface"]["supported_compiler_root"] == "native/objc3c"
