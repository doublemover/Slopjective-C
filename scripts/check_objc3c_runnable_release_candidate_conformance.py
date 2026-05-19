#!/usr/bin/env python3
"""Validate runnable release-candidate conformance against the integrated live workflow."""

from __future__ import annotations

import sys as _sys
from pathlib import Path as _Path

_ROOT = _Path(__file__).resolve().parents[1]
_SCRIPTS_ROOT = _ROOT / "scripts"
for _path in (_ROOT, _SCRIPTS_ROOT):
    _path_text = str(_path)
    if _path_text not in _sys.path:
        _sys.path.insert(0, _path_text)

from scripts.objc3c_runnable_release_candidate_conformance import *  # noqa: F401,F403
from scripts.objc3c_runnable_release_candidate_conformance import main


if __name__ == "__main__":
    raise SystemExit(main())
