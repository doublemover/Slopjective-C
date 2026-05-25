"""Shared constants for hosted LLVM developer-tooling contracts."""

from __future__ import annotations

from typing import Final

from objc3c_tooling.artifact_identity import current_host_artifact_identity

ARTIFACT_IDENTITY = current_host_artifact_identity()
LLVM_CAPABILITY_PROBE_BACKEND: Final[str] = (
    "python:scripts/probe_objc3c_llvm_capabilities.py"
)
FRONTEND_RUNNER_BACKEND: Final[str] = (
    f"runner-internal + {ARTIFACT_IDENTITY.frontend_runner_relative_path}"
)
CAPABILITY_EXPLORER_ACTION: Final[str] = "inspect-capability-explorer"
CAPABILITY_EXPLORER_DUMP_FILENAME: Final[str] = "capability-explorer.json"
CAPABILITY_ROUTED_PARITY_ACTION: Final[str] = "test-capability-routed-source-parity"
CHECK_LLVM_CAPABILITIES_ACTION: Final[str] = "check-llvm-capabilities"
CHECK_HOSTED_LLVM_CAPABILITIES_ACTION: Final[str] = "check-hosted-llvm-capabilities"
HOSTED_LLVM_CAPABILITY_MODE: Final[str] = "objc3c-llvm-capabilities-v2"
HOSTED_LLVM_CAPABILITY_TRUTH_SOURCE: Final[str] = "hosted-llvm-summary"
LOCAL_LLVM_DIAGNOSTIC_SOURCE: Final[str] = "local-llvm-probe-summary"

PARITY_SOURCE: Final[str] = "tests/tooling/fixtures/native/hello.objc3"
PARITY_CLI_BIN: Final[str] = ARTIFACT_IDENTITY.native_executable_relative_path
PARITY_C_API_BIN: Final[str] = ARTIFACT_IDENTITY.frontend_runner_relative_path
PARITY_WORK_DIR: Final[str] = (
    "tmp/artifacts/compilation/objc3c-native/m144/library-cli-parity/work"
)
PARITY_SUMMARY_OUT: Final[str] = (
    "tmp/artifacts/compilation/objc3c-native/m144/library-cli-parity/summary.json"
)
DEFAULT_LLVM_CAPABILITIES_SUMMARY_OUT: Final[str] = (
    "tmp/artifacts/objc3c-native/llvm_capabilities/summary.json"
)
HOSTED_LLVM_CAPABILITIES_SUMMARY_OUT: Final[str] = (
    "tmp/artifacts/objc3c-native/m144/llvm_capabilities/summary.json"
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

__all__ = [
    "CAPABILITY_EXPLORER_ACTION",
    "CAPABILITY_EXPLORER_DUMP_FILENAME",
    "CAPABILITY_ROUTED_PARITY_ACTION",
    "CHECK_HOSTED_LLVM_CAPABILITIES_ACTION",
    "CHECK_LLVM_CAPABILITIES_ACTION",
    "CLI_BIN_FLAG",
    "C_API_BIN_FLAG",
    "DEFAULT_LLVM_CAPABILITIES_SUMMARY_OUT",
    "FRONTEND_RUNNER_BACKEND",
    "HOSTED_LLVM_CAPABILITIES_SUMMARY_OUT",
    "HOSTED_LLVM_CAPABILITY_MODE",
    "HOSTED_LLVM_CAPABILITY_TRUTH_SOURCE",
    "LLVM_CAPABILITIES_SUMMARY_FLAG",
    "LLVM_CAPABILITY_PROBE_BACKEND",
    "LOCAL_LLVM_DIAGNOSTIC_SOURCE",
    "PARITY_CLI_BIN",
    "PARITY_C_API_BIN",
    "PARITY_SOURCE",
    "PARITY_SUMMARY_OUT",
    "PARITY_WORK_DIR",
    "ROUTE_CLI_BACKEND_FROM_CAPABILITIES_FLAG",
    "SOURCE_FLAG",
    "SUMMARY_OUT_FLAG",
    "WORK_DIR_FLAG",
]
