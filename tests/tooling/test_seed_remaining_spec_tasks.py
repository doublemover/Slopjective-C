from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path
from typing import Any

MODULE_PATH = Path(__file__).resolve().parents[2] / "scripts" / "seed_remaining_spec_tasks.py"
SPEC = importlib.util.spec_from_file_location("seed_remaining_spec_tasks_status", MODULE_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/seed_remaining_spec_tasks.py for tests.")
seed_script = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = seed_script
SPEC.loader.exec_module(seed_script)

FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "seed_catalog"
FIXTURE_CONFIG = FIXTURE_ROOT / "seed_config.json"


def run_seed_catalog(
    output_dir: Path,
    monkeypatch: Any,
    *,
    generated_on: str = "2026-02-23",
) -> dict[str, Any]:
    output_dir.mkdir(parents=True, exist_ok=True)
    catalog_path = output_dir / "catalog.json"

    monkeypatch.setattr(seed_script, "ROOT", FIXTURE_ROOT)
    monkeypatch.setattr(seed_script, "SPEC_ROOT", FIXTURE_ROOT / "spec")
    monkeypatch.delenv("SOURCE_DATE_EPOCH", raising=False)

    exit_code = seed_script.main(
        [
            "--config",
            str(FIXTURE_CONFIG),
            "--catalog-md",
            str(output_dir / "catalog.md"),
            "--catalog-json",
            str(catalog_path),
            "--generated-on",
            generated_on,
        ]
    )
    assert exit_code == 0
    return json.loads(catalog_path.read_text(encoding="utf-8"))


def test_seed_catalog_sets_required_execution_status_for_all_rows(
    tmp_path: Path, monkeypatch: Any
) -> None:
    payload = run_seed_catalog(tmp_path / "single-run", monkeypatch)
    tasks = payload["tasks"]

    assert len(tasks) == payload["task_count"]
    assert all("execution_status" in task for task in tasks)
    assert {task["execution_status"] for task in tasks} == {"open"}


def test_seed_catalog_execution_status_generation_is_deterministic(
    tmp_path: Path, monkeypatch: Any
) -> None:
    first = run_seed_catalog(tmp_path / "first", monkeypatch)
    second = run_seed_catalog(tmp_path / "second", monkeypatch)

    first_status_rows = [(task["task_id"], task["execution_status"]) for task in first["tasks"]]
    second_status_rows = [(task["task_id"], task["execution_status"]) for task in second["tasks"]]

    assert first_status_rows == second_status_rows
