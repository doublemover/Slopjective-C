"""Capability explorer workflow action."""

from __future__ import annotations

import sys

from ..commands import run
from ..environment import ROOT
from .developer_tooling_paths import LLVM_CAPABILITIES_PROBE_PY, PUBLIC_WORKFLOW_REPORT_ROOT


def action_inspect_capability_explorer(rest: list[str]) -> int:
    dump_path = PUBLIC_WORKFLOW_REPORT_ROOT / "capability-explorer.json"
    dump_path.parent.mkdir(parents=True, exist_ok=True)
    rc = run(
        [
            sys.executable,
            str(LLVM_CAPABILITIES_PROBE_PY),
            "--summary-out",
            str(dump_path),
            *rest,
        ]
    )
    if rc == 0:
        print(f"summary_path: {dump_path.relative_to(ROOT).as_posix()}")
        print(f"dump_path: {dump_path.relative_to(ROOT).as_posix()}")
    return rc
