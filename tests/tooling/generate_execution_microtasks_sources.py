from __future__ import annotations

import importlib.util
import io
import os
import subprocess
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path
from types import ModuleType

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "generate_execution_microtasks.py"
FIXTURES_DIR = Path(__file__).resolve().parent / "fixtures" / "microtasks"
STATUS_INTEGRITY_FIXTURES = (
    Path(__file__).resolve().parent / "fixtures" / "remaining_tasks_status_integrity"
)


def load_generator() -> ModuleType:
    spec = importlib.util.spec_from_file_location(
        "generate_execution_microtasks",
        SCRIPT_PATH,
    )
    if spec is None or spec.loader is None:
        raise RuntimeError("Unable to load scripts/generate_execution_microtasks.py")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


generate_execution_microtasks = load_generator()


def run_main(argv: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = generate_execution_microtasks.main(argv)
    return code, stdout.getvalue(), stderr.getvalue()


def deterministic_issues_path() -> Path:
    return FIXTURES_DIR / "issues_deterministic.json"


def deterministic_expected_markdown_path() -> Path:
    return FIXTURES_DIR / "expected_deterministic_2026-02-23.md"


def deterministic_argv() -> list[str]:
    return [
        "--issues-json",
        str(deterministic_issues_path()),
        "--closed-count",
        "10",
        "--generated-on",
        "2026-02-23",
    ]


def source_date_epoch_argv() -> list[str]:
    return [
        "--issues-json",
        str(deterministic_issues_path()),
        "--closed-count",
        "10",
    ]


def status_catalog_path(name: str) -> Path:
    return STATUS_INTEGRITY_FIXTURES / name


def subprocess_determinism_command() -> list[str]:
    return [
        sys.executable,
        str(SCRIPT_PATH),
        "--issues-json",
        str(deterministic_issues_path()),
        "--closed-count",
        "10",
        "--generated-on",
        "2026-02-23",
    ]


def run_subprocess_with_hash_seed(
    command: list[str],
    seed: str,
) -> subprocess.CompletedProcess[bytes]:
    env = os.environ.copy()
    env["PYTHONHASHSEED"] = seed
    env.pop("SOURCE_DATE_EPOCH", None)
    return subprocess.run(command, capture_output=True, check=False, env=env)
