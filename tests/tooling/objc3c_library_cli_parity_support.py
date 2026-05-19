from __future__ import annotations

import importlib.util
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_ROOT = ROOT / "scripts"
SCRIPT_PATH = ROOT / "scripts" / "check_objc3c_library_cli_parity.py"
FIXTURE_ROOT = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "library_cli_parity"
)
script_root_text = str(SCRIPT_ROOT)
if script_root_text not in sys.path:
    sys.path.insert(0, script_root_text)
SPEC = importlib.util.spec_from_file_location(
    "check_objc3c_library_cli_parity", SCRIPT_PATH
)
assert SPEC is not None and SPEC.loader is not None
parity = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = parity
SPEC.loader.exec_module(parity)
parity_subprocesses = __import__(
    "objc3c_library_cli_parity.subprocesses",
    fromlist=["subprocess"],
)
parity.subprocess = parity_subprocesses.subprocess
