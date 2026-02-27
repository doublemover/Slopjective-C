import importlib.util
import os
import subprocess
import sys
from pathlib import Path
from typing import Any

REPO_ROOT = Path(__file__).resolve().parents[2]
FIXTURES_ROOT = Path(__file__).resolve().parent / "fixtures"
IDEMPOTENCE_FIXTURES = FIXTURES_ROOT / "idempotence"
SEED_INPUT_FIXTURES = FIXTURES_ROOT / "seed_catalog"
MICROTASK_INPUT_FIXTURES = FIXTURES_ROOT / "microtasks"
GENERATED_ON = "2026-02-23"

SEED_SCRIPT_PATH = REPO_ROOT / "scripts" / "seed_remaining_spec_tasks.py"
MICROTASK_SCRIPT_PATH = REPO_ROOT / "scripts" / "generate_execution_microtasks.py"
SEED_SPEC = importlib.util.spec_from_file_location(
    "seed_remaining_spec_tasks_ci_idempotence",
    SEED_SCRIPT_PATH,
)
if SEED_SPEC is None or SEED_SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/seed_remaining_spec_tasks.py for tests.")
seed_script = importlib.util.module_from_spec(SEED_SPEC)
sys.modules[SEED_SPEC.name] = seed_script
SEED_SPEC.loader.exec_module(seed_script)


def normalize_lf_bytes(value: bytes) -> bytes:
    return value.replace(b"\r\n", b"\n").replace(b"\r", b"\n")


def run_seed_catalog_once(output_dir: Path, monkeypatch: Any) -> tuple[bytes, bytes]:
    output_dir.mkdir(parents=True, exist_ok=True)
    catalog_md = output_dir / "catalog.md"
    catalog_json = output_dir / "catalog.json"

    monkeypatch.setattr(seed_script, "ROOT", SEED_INPUT_FIXTURES)
    monkeypatch.setattr(seed_script, "SPEC_ROOT", SEED_INPUT_FIXTURES / "spec")
    monkeypatch.delenv("SOURCE_DATE_EPOCH", raising=False)

    code = seed_script.main(
        [
            "--config",
            str(SEED_INPUT_FIXTURES / "seed_config.json"),
            "--catalog-md",
            str(catalog_md),
            "--catalog-json",
            str(catalog_json),
            "--generated-on",
            GENERATED_ON,
        ]
    )
    assert code == 0

    return catalog_md.read_bytes(), catalog_json.read_bytes()


def run_microtasks_once(*, hash_seed: str) -> bytes:
    issues_json = MICROTASK_INPUT_FIXTURES / "issues_deterministic.json"
    command = [
        sys.executable,
        str(MICROTASK_SCRIPT_PATH),
        "--issues-json",
        str(issues_json),
        "--closed-count",
        "10",
        "--generated-on",
        GENERATED_ON,
    ]
    env = os.environ.copy()
    env["PYTHONHASHSEED"] = hash_seed
    env.pop("SOURCE_DATE_EPOCH", None)

    result = subprocess.run(
        command,
        cwd=REPO_ROOT,
        capture_output=True,
        check=False,
        env=env,
    )
    assert result.returncode == 0, result.stderr.decode("utf-8", errors="replace")
    assert result.stderr == b""
    return result.stdout


def test_seed_remaining_spec_tasks_outputs_are_idempotent_in_ci_gate(
    tmp_path: Path, monkeypatch: Any
) -> None:
    expected_md = (IDEMPOTENCE_FIXTURES / "expected_seed_catalog_2026-02-23.md").read_bytes()
    expected_json = (IDEMPOTENCE_FIXTURES / "expected_seed_catalog_2026-02-23.json").read_bytes()
    assert b"\r" not in expected_md
    assert b"\r" not in expected_json

    first_md, first_json = run_seed_catalog_once(tmp_path / "first", monkeypatch)
    second_md, second_json = run_seed_catalog_once(tmp_path / "second", monkeypatch)

    assert first_md == second_md
    assert first_json == second_json
    assert normalize_lf_bytes(first_md) == expected_md
    assert normalize_lf_bytes(first_json) == expected_json


def test_generate_execution_microtasks_output_is_idempotent_in_ci_gate() -> None:
    expected_md = (IDEMPOTENCE_FIXTURES / "expected_microtasks_2026-02-23.md").read_bytes()
    assert b"\r" not in expected_md

    first_md = run_microtasks_once(hash_seed="101")
    second_md = run_microtasks_once(hash_seed="202")

    assert first_md == second_md
    assert normalize_lf_bytes(first_md) == expected_md
