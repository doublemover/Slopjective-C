"""The checked-in public command contract must not depend on its render host."""

from __future__ import annotations

import json
from pathlib import Path
import subprocess
import sys


ROOT = Path(__file__).resolve().parents[2]


def test_public_command_contract_is_portable_and_execution_paths_remain_native() -> None:
    script = """
import json
import platform
import sys
platform.system = lambda: sys.argv[1]
platform.machine = lambda: sys.argv[2]
sys.path.insert(0, 'scripts')
from scripts.build_objc3c_public_command_contract import build_contract
from scripts.objc3c_workflow.actions.developer_tooling_llvm_contract_constants import (
    PARITY_CLI_BIN, PARITY_C_API_BIN,
)
print(json.dumps({
    'contract': build_contract(),
    'cli': PARITY_CLI_BIN,
    'runner': PARITY_C_API_BIN,
}))
"""
    payloads = []
    for system, machine in (("Windows", "AMD64"), ("Linux", "x86_64"), ("Darwin", "arm64")):
        completed = subprocess.run(
            [sys.executable, "-B", "-c", script, system, machine],
            cwd=ROOT,
            check=True,
            capture_output=True,
            text=True,
            encoding="utf-8",
        )
        payloads.append(json.loads(completed.stdout))

    assert payloads[0]["contract"] == payloads[1]["contract"] == payloads[2]["contract"]
    for field in ("cli", "runner"):
        assert payloads[0][field] == payloads[1][field] + ".exe"
        assert payloads[1][field] == payloads[2][field]
