"""Activation preflight runner path defaults."""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_ROOT = ROOT / "scripts"

DEFAULT_CATALOG_JSON = ROOT / "tmp" / "reports" / "remaining_task_review_catalog.json"
DEFAULT_OPEN_BLOCKERS_ROOT = ROOT
DEFAULT_OUTPUT_DIR = ROOT / "tmp" / "reports" / "activation_preflight"

ACTIVATION_CHECK_SCRIPT_PATH = ROOT / "scripts" / "check_activation_triggers.py"
SPEC_LINT_SCRIPT_PATH = ROOT / "scripts" / "spec_lint.py"
CAPTURE_SNAPSHOTS_SCRIPT_PATH = ROOT / "scripts" / "capture_activation_snapshots.py"
EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH = ROOT / "scripts" / "extract_open_blockers.py"

OPEN_BLOCKERS_REFRESH_RELATIVE_PATH = Path("inputs") / "open_blockers.snapshot.json"
DEFAULT_ACTIONABLE_STATUSES: tuple[str, ...] = ("open", "open-blocked", "blocked")


def default_open_blockers_output_path(output_dir: Path) -> Path:
    return output_dir / OPEN_BLOCKERS_REFRESH_RELATIVE_PATH


__all__ = [
    "ACTIVATION_CHECK_SCRIPT_PATH",
    "CAPTURE_SNAPSHOTS_SCRIPT_PATH",
    "DEFAULT_ACTIONABLE_STATUSES",
    "DEFAULT_CATALOG_JSON",
    "DEFAULT_OPEN_BLOCKERS_ROOT",
    "DEFAULT_OUTPUT_DIR",
    "EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH",
    "OPEN_BLOCKERS_REFRESH_RELATIVE_PATH",
    "ROOT",
    "SCRIPT_ROOT",
    "SPEC_LINT_SCRIPT_PATH",
    "default_open_blockers_output_path",
]
