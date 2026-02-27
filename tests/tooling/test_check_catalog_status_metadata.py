from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2]
    / "scripts"
    / "check_catalog_status_metadata.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_catalog_status_metadata",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_catalog_status_metadata.py")

module = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = module
SPEC.loader.exec_module(module)


def write_catalog(path: Path, tasks: list[dict[str, object]]) -> None:
    payload = {"generated_on": "2026-02-23", "task_count": len(tasks), "tasks": tasks}
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def test_main_returns_0_when_metadata_is_complete(tmp_path: Path, capsys: pytest.CaptureFixture[str]) -> None:
    catalog_path = tmp_path / "catalog.json"
    write_catalog(
        catalog_path,
        [
            {
                "task_id": "SPT-0001",
                "lane": "A",
                "execution_status_rationale": "Complete and validated.",
                "execution_status_evidence_refs": ["spec/planning/example.md:10"],
            }
        ],
    )

    exit_code = module.main(["--catalog", str(catalog_path)])

    assert exit_code == 0
    out = capsys.readouterr().out
    assert "missing_rationale=0" in out
    assert "missing_evidence_refs=0" in out


def test_main_returns_2_when_metadata_is_missing(tmp_path: Path, capsys: pytest.CaptureFixture[str]) -> None:
    catalog_path = tmp_path / "catalog.json"
    write_catalog(
        catalog_path,
        [
            {
                "task_id": "SPT-0001",
                "lane": "B",
                "execution_status_rationale": "",
                "execution_status_evidence_refs": [],
            }
        ],
    )

    exit_code = module.main(["--catalog", str(catalog_path)])

    assert exit_code == 2
    out = capsys.readouterr().out
    assert "missing_rationale=1" in out
    assert "missing_evidence_refs=1" in out


def test_lane_filter_limits_scope(tmp_path: Path, capsys: pytest.CaptureFixture[str]) -> None:
    catalog_path = tmp_path / "catalog.json"
    write_catalog(
        catalog_path,
        [
            {
                "task_id": "SPT-0001",
                "lane": "A",
                "execution_status_rationale": "",
                "execution_status_evidence_refs": [],
            },
            {
                "task_id": "SPT-0002",
                "lane": "B",
                "execution_status_rationale": "ok",
                "execution_status_evidence_refs": ["spec/planning/example.md:20"],
            },
        ],
    )

    exit_code = module.main(["--catalog", str(catalog_path), "--lane", "B"])

    assert exit_code == 0
    out = capsys.readouterr().out
    assert "missing_rationale=0" in out
    assert "missing_evidence_refs=0" in out


def test_main_returns_1_for_invalid_lane_filter(capsys: pytest.CaptureFixture[str]) -> None:
    exit_code = module.main(["--lane", "   "])

    assert exit_code == 1
    err = capsys.readouterr().err
    assert "lane filters must be non-empty" in err
