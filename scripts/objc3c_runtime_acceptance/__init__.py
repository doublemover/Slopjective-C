"""Importable runtime acceptance package."""

import sys
from pathlib import Path

SCRIPT_ROOT = Path(__file__).resolve().parents[1]
if str(SCRIPT_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPT_ROOT))
sys.modules.setdefault("objc3c_runtime_acceptance", sys.modules[__name__])

from .cli_orchestration import main

__all__ = ["main"]
