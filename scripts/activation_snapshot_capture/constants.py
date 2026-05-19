"""Constants for activation snapshot capture."""

from __future__ import annotations

from pathlib import Path

PACKAGE_DIR = Path(__file__).resolve().parent
SCRIPTS_DIR = PACKAGE_DIR.parent
ROOT = SCRIPTS_DIR.parent

SCRIPT_NAME = "capture_activation_snapshots.py"
PROGRAM_NAME = "capture-activation-snapshots"
CAPTURE_SNAPSHOTS_SCRIPT_PATH = SCRIPTS_DIR / SCRIPT_NAME

ISSUES_ENDPOINT = "repos/{owner}/{repo}/issues?state=open&per_page=100"
MILESTONES_ENDPOINT = "repos/{owner}/{repo}/milestones?state=open&per_page=100"
ISSUES_SOURCE = (
    "gh api repos/{owner}/{repo}/issues?state=open&per_page=100 "
    "--paginate --slurp (pull_request filtered)"
)
MILESTONES_SOURCE = (
    "gh api repos/{owner}/{repo}/milestones?state=open&per_page=100 "
    "--paginate --slurp"
)


__all__ = [
    "CAPTURE_SNAPSHOTS_SCRIPT_PATH",
    "ISSUES_ENDPOINT",
    "ISSUES_SOURCE",
    "MILESTONES_ENDPOINT",
    "MILESTONES_SOURCE",
    "PACKAGE_DIR",
    "PROGRAM_NAME",
    "ROOT",
    "SCRIPT_NAME",
    "SCRIPTS_DIR",
]
