"""Typed contracts for hosted LLVM and capability-routed tooling actions."""

from __future__ import annotations

import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Final

from ..environment import ROOT
from .developer_tooling_paths import (
    HOSTED_LLVM_CAPABILITIES_SUMMARY,
    LIBRARY_CLI_PARITY_PY,
    LLVM_CAPABILITIES_PROBE_PY,
    PUBLIC_WORKFLOW_REPORT_ROOT,
)

LLVM_CAPABILITY_PROBE_BACKEND: Final[str] = (
    "python:scripts/probe_objc3c_llvm_capabilities.py"
)
FRONTEND_RUNNER_BACKEND: Final[str] = (
    "runner-internal + artifacts/bin/objc3c-frontend-c-api-runner.exe"
)
CAPABILITY_EXPLORER_ACTION: Final[str] = "inspect-capability-explorer"
CAPABILITY_EXPLORER_DUMP_FILENAME: Final[str] = "capability-explorer.json"
CAPABILITY_ROUTED_PARITY_ACTION: Final[str] = "test-capability-routed-source-parity"
CHECK_LLVM_CAPABILITIES_ACTION: Final[str] = "check-llvm-capabilities"
CHECK_HOSTED_LLVM_CAPABILITIES_ACTION: Final[str] = "check-hosted-llvm-capabilities"

PARITY_SOURCE: Final[str] = "tests/tooling/fixtures/native/hello.objc3"
PARITY_CLI_BIN: Final[str] = "artifacts/bin/objc3c-native.exe"
PARITY_C_API_BIN: Final[str] = "artifacts/bin/objc3c-frontend-c-api-runner.exe"
PARITY_WORK_DIR: Final[str] = (
    "tmp/artifacts/compilation/objc3c-native/m144/library-cli-parity/work"
)
PARITY_SUMMARY_OUT: Final[str] = (
    "tmp/artifacts/compilation/objc3c-native/m144/library-cli-parity/summary.json"
)
DEFAULT_LLVM_CAPABILITIES_SUMMARY_OUT: Final[str] = (
    "tmp/artifacts/objc3c-native/llvm_capabilities/summary.json"
)
SUMMARY_OUT_FLAG: Final[str] = "--summary-out"
SOURCE_FLAG: Final[str] = "--source"
CLI_BIN_FLAG: Final[str] = "--cli-bin"
C_API_BIN_FLAG: Final[str] = "--c-api-bin"
WORK_DIR_FLAG: Final[str] = "--work-dir"
LLVM_CAPABILITIES_SUMMARY_FLAG: Final[str] = "--llvm-capabilities-summary"
ROUTE_CLI_BACKEND_FROM_CAPABILITIES_FLAG: Final[str] = (
    "--route-cli-backend-from-capabilities"
)


@dataclass(frozen=True)
class CapabilityExplorerContract:
    action: str
    summary: str
    backend: str
    guarantee_owner: str
    validation_tier: str = "repo"
    pass_through_args: bool = True


CAPABILITY_EXPLORER_CONTRACT: Final[CapabilityExplorerContract] = (
    CapabilityExplorerContract(
        action=CAPABILITY_EXPLORER_ACTION,
        summary=(
            "probe LLVM and backend-routing capability state through the live "
            "capability explorer surface"
        ),
        backend=LLVM_CAPABILITY_PROBE_BACKEND,
        guarantee_owner=(
            "capability explorer payloads stay tied to the live LLVM probe and "
            "backend-routing contracts"
        ),
    )
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
