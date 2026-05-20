"""Migration analyzer and rewrite workflow action entrypoints."""

from __future__ import annotations

import sys

from scripts.objc3c_workflow.commands import run
from scripts.objc3c_workflow.environment import ROOT


def _run_migration_script(script_name: str, rest: list[str]) -> int:
    return run([sys.executable, str(ROOT / "scripts" / script_name), *rest])


def action_analyze_migration_source(rest: list[str]) -> int:
    return _run_migration_script("analyze_objc3c_migration.py", rest)


def action_rewrite_migration_source(rest: list[str]) -> int:
    return _run_migration_script("rewrite_objc3c_migration.py", rest)


def action_validate_migration_workflow(rest: list[str]) -> int:
    return _run_migration_script("check_objc3c_migration_workflow.py", rest)
