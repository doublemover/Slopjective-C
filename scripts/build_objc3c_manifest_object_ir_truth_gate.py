from __future__ import annotations

import sys as _sys
from pathlib import Path as _Path

_ROOT = _Path(__file__).resolve().parents[1]
if str(_ROOT) not in _sys.path:
    _sys.path.insert(0, str(_ROOT))

from scripts.objc3c_manifest_object_ir_truth_gate import *  # noqa: F401,F403
from scripts.objc3c_manifest_object_ir_truth_gate import main


if __name__ == "__main__":
    raise SystemExit(main())
