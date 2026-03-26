from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SURFACE_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_b002_public_package_script_collapse_core_feature_implementation_surface.json"


def test_surface_keeps_public_scripts_under_budget() -> None:
    payload = json.loads(SURFACE_JSON.read_text(encoding="utf-8"))
    assert len(payload["public_scripts"]) <= payload["public_script_budget_maximum"] == 25


def test_surface_marks_readme_check_migration_transitional() -> None:
    payload = json.loads(SURFACE_JSON.read_text(encoding="utf-8"))
    transitional = [entry for entry in payload["readme_migrations"] if entry["status"] == "transitional"]
    assert transitional == [{"from": "python scripts/build_site_index.py --check", "to": None, "status": "transitional"}]
