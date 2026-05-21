#!/usr/bin/env python3
"""Validate the checked-in distribution-credibility source surface."""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from scripts.distribution_credibility_source_surface_check.runner import main


if __name__ == "__main__":
    raise SystemExit(main())
