"""Capability explorer workflow action."""

from __future__ import annotations

from ..commands import run
from .developer_tooling_llvm_contracts import (
    capability_explorer_dump_path,
    capability_probe_command,
    repo_relative,
)


def action_inspect_capability_explorer(rest: list[str]) -> int:
    dump_path = capability_explorer_dump_path()
    dump_path.parent.mkdir(parents=True, exist_ok=True)
    rc = run(
        [
            *capability_probe_command(dump_path),
            *rest,
        ]
    )
    if rc == 0:
        print(f"summary_path: {repo_relative(dump_path)}")
        print(f"dump_path: {repo_relative(dump_path)}")
    return rc
