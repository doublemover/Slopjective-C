"""Command builders for hosted LLVM developer-tooling actions."""

from __future__ import annotations

import sys
from pathlib import Path

from ..environment import ROOT
from .developer_tooling_llvm_contract_constants import (
    CAPABILITY_EXPLORER_DUMP_FILENAME,
    C_API_BIN_FLAG,
    CLI_BIN_FLAG,
    DEFAULT_LLVM_CAPABILITIES_SUMMARY_OUT,
    LLVM_CAPABILITIES_SUMMARY_FLAG,
    PARITY_C_API_BIN,
    PARITY_CLI_BIN,
    PARITY_SOURCE,
    PARITY_SUMMARY_OUT,
    PARITY_WORK_DIR,
    ROUTE_CLI_BACKEND_FROM_CAPABILITIES_FLAG,
    SOURCE_FLAG,
    SUMMARY_OUT_FLAG,
    WORK_DIR_FLAG,
)
from .developer_tooling_paths import (
    HOSTED_LLVM_CAPABILITIES_SUMMARY,
    LIBRARY_CLI_PARITY_PY,
    LLVM_CAPABILITIES_PROBE_PY,
    PUBLIC_WORKFLOW_REPORT_ROOT,
)


def repo_relative(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def capability_probe_command(summary_out: str | Path) -> list[str]:
    return [
        sys.executable,
        str(LLVM_CAPABILITIES_PROBE_PY),
        SUMMARY_OUT_FLAG,
        str(summary_out),
    ]


def capability_explorer_dump_path() -> Path:
    return PUBLIC_WORKFLOW_REPORT_ROOT / CAPABILITY_EXPLORER_DUMP_FILENAME


def hosted_llvm_probe_command() -> list[str]:
    return capability_probe_command(repo_relative(HOSTED_LLVM_CAPABILITIES_SUMMARY))


def default_llvm_capabilities_command() -> list[str]:
    return capability_probe_command(DEFAULT_LLVM_CAPABILITIES_SUMMARY_OUT)


def capability_routed_parity_command() -> list[str]:
    return [
        sys.executable,
        str(LIBRARY_CLI_PARITY_PY),
        SOURCE_FLAG,
        PARITY_SOURCE,
        CLI_BIN_FLAG,
        PARITY_CLI_BIN,
        C_API_BIN_FLAG,
        PARITY_C_API_BIN,
        WORK_DIR_FLAG,
        PARITY_WORK_DIR,
        SUMMARY_OUT_FLAG,
        PARITY_SUMMARY_OUT,
        LLVM_CAPABILITIES_SUMMARY_FLAG,
        repo_relative(HOSTED_LLVM_CAPABILITIES_SUMMARY),
        ROUTE_CLI_BACKEND_FROM_CAPABILITIES_FLAG,
    ]


__all__ = [
    "capability_explorer_dump_path",
    "capability_probe_command",
    "capability_routed_parity_command",
    "default_llvm_capabilities_command",
    "hosted_llvm_probe_command",
    "repo_relative",
]
