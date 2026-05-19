"""Filesystem constants for the validation surface inventory builder."""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
REPORT_DIR = ROOT / "tmp" / "reports" / "m313" / "validation-surface-inventory"
JSON_OUT = REPORT_DIR / "validation_surface_inventory.json"
MD_OUT = REPORT_DIR / "validation_surface_inventory.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_GATE = ROOT / "scripts" / "ci" / "run_task_hygiene_gate.py"
ACCEPTANCE_HARNESS = ROOT / "scripts" / "shared_compiler_runtime_acceptance_harness.py"
CHECK_ROOTS = [
    ROOT / "scripts",
    ROOT / "tests",
    ROOT / "native",
    ROOT / "docs",
    ROOT / "showcase",
    ROOT / "stdlib",
]
