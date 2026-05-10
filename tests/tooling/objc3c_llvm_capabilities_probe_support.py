from __future__ import annotations

import importlib.util
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "probe_objc3c_llvm_capabilities.py"
PACKAGE_JSON = ROOT / "package.json"

SPEC = importlib.util.spec_from_file_location(
    "probe_objc3c_llvm_capabilities",
    SCRIPT_PATH,
)
assert SPEC is not None and SPEC.loader is not None
probe = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = probe
SPEC.loader.exec_module(probe)
