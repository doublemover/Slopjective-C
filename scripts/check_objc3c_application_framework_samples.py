#!/usr/bin/env python3
"""Validate issue #8178 application framework samples."""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from scripts.objc3c_application_framework_samples.runner import main


if __name__ == "__main__":
    raise SystemExit(main())
