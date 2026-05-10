from __future__ import annotations

import importlib.util
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_objc3c_library_cli_parity.py"
FIXTURE_ROOT = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "library_cli_parity"
)
SPEC = importlib.util.spec_from_file_location(
    "check_objc3c_library_cli_parity", SCRIPT_PATH
)
assert SPEC is not None and SPEC.loader is not None
parity = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = parity
SPEC.loader.exec_module(parity)
