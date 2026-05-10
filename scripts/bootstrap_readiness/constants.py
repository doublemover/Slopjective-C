from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_CATALOG_JSON = ROOT / "tmp" / "reports" / "remaining_task_review_catalog.json"
DEFAULT_OPEN_BLOCKERS_ROOT = ROOT
DEFAULT_OUTPUT_DIR = ROOT / "tmp" / "reports" / "bootstrap_readiness"
DEFAULT_REFRESH_OPEN_BLOCKERS_RELATIVE_PATH = Path("inputs") / "open_blockers.snapshot.json"
DEFAULT_COMMAND_TIMEOUT_SECONDS = 600

CHECK_BOOTSTRAP_READINESS_SCRIPT_PATH = ROOT / "scripts" / "check_bootstrap_readiness.py"
EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH = ROOT / "scripts" / "extract_open_blockers.py"
SPEC_LINT_SCRIPT_PATH = ROOT / "scripts" / "spec_lint.py"

EXIT_BOOTSTRAPPABLE = 0
EXIT_BLOCKED = 1
EXIT_RUNNER_ERROR = 2
