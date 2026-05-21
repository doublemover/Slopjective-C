#!/usr/bin/env python3
"""Validate the live developer-tooling inspect, formatter, and trace workflow."""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SCRIPT_ROOT = ROOT / "scripts"
for import_root in (ROOT, SCRIPT_ROOT):
    import_root_text = str(import_root)
    if import_root_text not in sys.path:
        sys.path.insert(0, import_root_text)

from scripts.objc3c_developer_tooling_integration_check.runner import main


if __name__ == "__main__":
    raise SystemExit(main())
