from __future__ import annotations

import importlib.util
import sys
from pathlib import Path

SCRIPTS_ROOT = Path(__file__).resolve().parents[2] / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2]
    / "scripts"
    / "build_objc3c_claimability_dashboard_release_blocker_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "build_objc3c_claimability_dashboard_release_blocker_contract", SCRIPT_PATH
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/build_objc3c_claimability_dashboard_release_blocker_contract.py"
    )
builder = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = builder
SPEC.loader.exec_module(builder)
