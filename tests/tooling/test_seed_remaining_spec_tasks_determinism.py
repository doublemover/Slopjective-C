import importlib.util
import json
import re
import sys
from pathlib import Path
from typing import Any

MODULE_PATH = Path(__file__).resolve().parents[2] / "scripts" / "seed_remaining_spec_tasks.py"
SPEC = importlib.util.spec_from_file_location("seed_remaining_spec_tasks", MODULE_PATH)
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
    generated_on: str | None,
    source_date_epoch: str | None = None,
) -> tuple[bytes, bytes, dict[str, Any]]:
    output_dir.mkdir(parents=True, exist_ok=True)
    catalog_md = output_dir / "catalog.md"
    catalog_json = output_dir / "catalog.json"

    monkeypatch.setattr(seed_script, "ROOT", FIXTURE_ROOT)
    monkeypatch.setattr(seed_script, "SPEC_ROOT", FIXTURE_ROOT / "spec")

    if source_date_epoch is None:
        monkeypatch.delenv("SOURCE_DATE_EPOCH", raising=False)
    else:
        monkeypatch.setenv("SOURCE_DATE_EPOCH", source_date_epoch)

    argv = [
        "--config",
        str(FIXTURE_CONFIG),
        "--catalog-md",
        str(catalog_md),
        "--catalog-json",
        str(catalog_json),
    ]
    if generated_on is not None:
        argv.extend(["--generated-on", generated_on])

    exit_code = seed_script.main(argv)
    assert exit_code == 0

    md_bytes = catalog_md.read_bytes()
    json_bytes = catalog_json.read_bytes()
    payload = json.loads(json_bytes)
    return md_bytes, json_bytes, payload


def test_catalog_json_contains_stable_identity_metadata(tmp_path: Path, monkeypatch: Any) -> None:
    _, _, first_payload = run_seed_catalog(
        tmp_path / "first",
        monkeypatch,
        generated_on="2026-02-23",
    )
    _, _, second_payload = run_seed_catalog(
        tmp_path / "second",
        monkeypatch,
        generated_on="2026-02-23",
    )

    first_tasks = first_payload["tasks"]
    second_tasks = second_payload["tasks"]

    assert len(first_tasks) == 5
    assert len(second_tasks) == 5

    hash_pattern = re.compile(r"^[0-9a-f]{64}$")
    for task in first_tasks:
        assert task["task_key"] == f"{task['path']}:{task['line']}"
        assert hash_pattern.fullmatch(task["source_line_hash"]) is not None

    assert [task["task_key"] for task in first_tasks] == [
        task["task_key"] for task in second_tasks
    ]
    assert [task["source_line_hash"] for task in first_tasks] == [
        task["source_line_hash"] for task in second_tasks
    ]


def test_repeat_runs_are_byte_identical_for_fixed_generated_on(
    tmp_path: Path, monkeypatch: Any
) -> None:
    first_md, first_json, _ = run_seed_catalog(
        tmp_path / "first",
        monkeypatch,
        generated_on="2026-02-23",
    )
    second_md, second_json, _ = run_seed_catalog(
        tmp_path / "second",
        monkeypatch,
        generated_on="2026-02-23",
    )

    assert first_md == second_md
    assert first_json == second_json


def test_generated_on_uses_source_date_epoch_when_flag_missing(
    tmp_path: Path, monkeypatch: Any
) -> None:
    md_bytes, _, payload = run_seed_catalog(
        tmp_path / "source-date-epoch",
        monkeypatch,
        generated_on=None,
        source_date_epoch="1704067200",
    )

    assert payload["generated_on"] == "2024-01-01"
    assert "_Generated on 2024-01-01 by scripts/seed_remaining_spec_tasks.py._" in md_bytes.decode(
        "utf-8"
    )


def test_generated_on_flag_overrides_source_date_epoch(
    tmp_path: Path, monkeypatch: Any
) -> None:
    md_bytes, _, payload = run_seed_catalog(
        tmp_path / "explicit-generated-on",
        monkeypatch,
        generated_on="2026-02-23",
        source_date_epoch="1704067200",
    )

    assert payload["generated_on"] == "2026-02-23"
    assert "_Generated on 2026-02-23 by scripts/seed_remaining_spec_tasks.py._" in md_bytes.decode(
        "utf-8"
    )
