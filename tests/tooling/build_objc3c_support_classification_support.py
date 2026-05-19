from __future__ import annotations

import importlib.util
import sys
from pathlib import Path

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2]
    / "scripts"
    / "build_objc3c_support_classification.py"
)
SPEC = importlib.util.spec_from_file_location(
    "build_objc3c_support_classification", SCRIPT_PATH
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/build_objc3c_support_classification.py")
builder = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = builder
SPEC.loader.exec_module(builder)

FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "support_classification"
