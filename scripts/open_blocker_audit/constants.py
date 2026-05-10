"""Open-blocker audit path constants."""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_AUDIT_ROOT = ROOT
DEFAULT_OUTPUT_DIR = ROOT / "tmp" / "reports" / "open_blocker_audit"

EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH = ROOT / "scripts" / "extract_open_blockers.py"
CHECK_OPEN_BLOCKER_AUDIT_CONTRACT_SCRIPT_PATH = (
    ROOT / "scripts" / "check_open_blocker_audit_contract.py"
)

__all__ = [
    "CHECK_OPEN_BLOCKER_AUDIT_CONTRACT_SCRIPT_PATH",
    "DEFAULT_AUDIT_ROOT",
    "DEFAULT_OUTPUT_DIR",
    "EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH",
    "ROOT",
]
