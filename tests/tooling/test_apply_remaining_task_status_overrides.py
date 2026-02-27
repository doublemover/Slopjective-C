from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2]
    / "scripts"
    / "apply_remaining_task_status_overrides.py"
)
SPEC = importlib.util.spec_from_file_location(
    "apply_remaining_task_status_overrides",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/apply_remaining_task_status_overrides.py")
tool = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = tool
SPEC.loader.exec_module(tool)


def write_json(path: Path, payload: object) -> None:
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def test_apply_overrides_updates_catalog_rows_and_preserves_existing_invariants() -> None:
    catalog = {
        "generated_on": "2026-02-23",
        "task_count": 3,
        "tasks": [
            {"task_id": "SPT-0001", "execution_status": "open"},
            {"task_id": "SPT-0002", "execution_status": "blocked"},
            {"task_id": "SPT-0003", "execution_status": "open-blocked"},
        ],
    }
    overrides = {
        "SPT-0001": tool.OverrideEntry(
            task_id="SPT-0001",
            recommended_status="complete",
            evidence_refs=("spec/planning/foo.md:10",),
            rationale="Verified in package closeout section.",
            source_path=Path("lane_a.json"),
        ),
        "SPT-0002": tool.OverrideEntry(
            task_id="SPT-0002",
            recommended_status="open-blocked",
            evidence_refs=("spec/planning/bar.md:20",),
            rationale="Waiting on external reviewer signoff.",
            source_path=Path("lane_b.json"),
        ),
    }

    changes, missing = tool.apply_overrides(catalog, overrides)

    assert missing == []
    assert [change["task_id"] for change in changes] == ["SPT-0001", "SPT-0002"]
    assert catalog["tasks"][0]["execution_status"] == "complete"
    assert catalog["tasks"][1]["execution_status"] == "open-blocked"
    assert catalog["tasks"][2]["execution_status"] == "open-blocked"
    assert "execution_status_rationale" not in catalog["tasks"][2]
    assert catalog["tasks"][0]["execution_status_evidence_refs"] == [
        "spec/planning/foo.md:10"
    ]
    assert catalog["tasks"][1]["execution_status_rationale"] == (
        "Waiting on external reviewer signoff."
    )


def test_apply_overrides_fails_when_catalog_status_is_missing() -> None:
    catalog = {"tasks": [{"task_id": "SPT-0001"}]}

    with pytest.raises(ValueError, match="SPT-0001"):
        tool.apply_overrides(catalog, {})


def test_apply_overrides_allows_missing_status_when_row_is_covered_by_override() -> None:
    catalog = {"tasks": [{"task_id": "SPT-0001"}]}
    overrides = {
        "SPT-0001": tool.OverrideEntry(
            task_id="SPT-0001",
            recommended_status="complete",
            evidence_refs=("spec/planning/example.md:10",),
            rationale="Backfilled by lane override.",
            source_path=Path("lane_a.json"),
        )
    }

    changes, missing = tool.apply_overrides(catalog, overrides)

    assert missing == []
    assert changes == [
        {
            "task_id": "SPT-0001",
            "before_status": "(unset)",
            "after_status": "complete",
            "override_source": "lane_a.json",
        }
    ]
    assert catalog["tasks"][0]["execution_status"] == "complete"


def test_apply_overrides_fails_when_catalog_status_is_invalid() -> None:
    catalog = {"tasks": [{"task_id": "SPT-0001", "execution_status": "in-progress"}]}

    with pytest.raises(ValueError, match="invalid 'execution_status' value 'in-progress'"):
        tool.apply_overrides(catalog, {})


def test_duplicate_override_task_id_fails(tmp_path: Path) -> None:
    o1 = tmp_path / "o1.json"
    o2 = tmp_path / "o2.json"
    write_json(
        o1,
        [
            {
                "task_id": "SPT-0001",
                "recommended_status": "complete",
                "evidence_refs": [],
                "rationale": "r1",
            }
        ],
    )
    write_json(
        o2,
        [
            {
                "task_id": "SPT-0001",
                "recommended_status": "open",
                "evidence_refs": [],
                "rationale": "r2",
            }
        ],
    )

    with pytest.raises(ValueError, match="duplicate override for task_id 'SPT-0001'"):
        tool.load_overrides([o1, o2])


def test_main_returns_2_when_override_task_missing(tmp_path: Path, capsys: pytest.CaptureFixture[str]) -> None:
    catalog_path = tmp_path / "catalog.json"
    overrides_path = tmp_path / "lane.json"

    write_json(catalog_path, {"tasks": [{"task_id": "SPT-0001", "execution_status": "open"}]})
    write_json(
        overrides_path,
        [
            {
                "task_id": "SPT-9999",
                "recommended_status": "complete",
                "evidence_refs": [],
                "rationale": "no matching row",
            }
        ],
    )

    exit_code = tool.main(
        [
            "--catalog",
            str(catalog_path),
            "--overrides",
            str(overrides_path),
        ]
    )

    assert exit_code == 2
    captured = capsys.readouterr()
    assert "missing_task_ids=['SPT-9999']" in captured.out


def test_main_returns_1_with_actionable_error_for_missing_catalog_status(
    tmp_path: Path, capsys: pytest.CaptureFixture[str]
) -> None:
    catalog_path = tmp_path / "catalog.json"
    overrides_path = tmp_path / "lane.json"

    write_json(catalog_path, {"tasks": [{"task_id": "SPT-0001"}]})
    write_json(overrides_path, [])

    exit_code = tool.main(
        [
            "--catalog",
            str(catalog_path),
            "--overrides",
            str(overrides_path),
        ]
    )

    assert exit_code == 1
    captured = capsys.readouterr()
    assert "missing required 'execution_status'" in captured.err
    assert "SPT-0001" in captured.err


def test_load_overrides_accepts_object_with_overrides_array(tmp_path: Path) -> None:
    override_path = tmp_path / "lane.json"
    write_json(
        override_path,
        {
            "generated_on": "2026-02-23",
            "overrides": [
                {
                    "task_id": "SPT-0001",
                    "recommended_status": "complete",
                    "evidence_refs": ["spec/planning/example.md:10"],
                    "rationale": "Verified.",
                }
            ],
        },
    )

    loaded = tool.load_overrides([override_path])
    assert "SPT-0001" in loaded
    assert loaded["SPT-0001"].recommended_status == "complete"
