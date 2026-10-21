"""Typed contracts for developer-tooling frontend JSON dump actions."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Final

from .developer_tooling_llvm_contract_constants import FRONTEND_RUNNER_BACKEND

SUMMARY_OUT_FLAG: Final[str] = "--summary-out"
DUMP_SUMMARY_JSON_FLAG: Final[str] = "--dump-summary-json"
OBSERVABILITY_DUMP_FLAG: Final[str] = "--dump-observability-json"
PLAYGROUND_REPRO_DUMP_FLAG: Final[str] = "--dump-playground-repro-json"
RUNTIME_INSPECTOR_DUMP_FLAG: Final[str] = "--dump-runtime-inspector-json"
STAGE_TRACE_DUMP_FLAG: Final[str] = "--dump-stage-trace-json"

MANAGED_DEVELOPER_TOOLING_DUMP_FLAGS: Final[tuple[str, ...]] = (
    SUMMARY_OUT_FLAG,
    DUMP_SUMMARY_JSON_FLAG,
    OBSERVABILITY_DUMP_FLAG,
    PLAYGROUND_REPRO_DUMP_FLAG,
    RUNTIME_INSPECTOR_DUMP_FLAG,
    STAGE_TRACE_DUMP_FLAG,
)


@dataclass(frozen=True)
class DeveloperToolingDumpContract:
    action: str
    summary: str
    dump_flag: str
    dump_filename: str
    guarantee_owner: str
    backend: str = FRONTEND_RUNNER_BACKEND
    validation_tier: str = "repo"
    pass_through_args: bool = True


COMPILE_OBSERVABILITY_DUMP: Final[DeveloperToolingDumpContract] = (
    DeveloperToolingDumpContract(
        action="inspect-compile-observability",
        summary=(
            "compile one source through the frontend C API runner and dump the "
            "structured observability object"
        ),
        dump_flag=OBSERVABILITY_DUMP_FLAG,
        dump_filename="compile-observability.json",
        guarantee_owner=(
            "developer-facing compile observability stays tied to the real "
            "frontend runner summary and emitted artifacts"
        ),
    )
)
RUNTIME_INSPECTOR_DUMP: Final[DeveloperToolingDumpContract] = (
    DeveloperToolingDumpContract(
        action="inspect-runtime-inspector",
        summary=(
            "compile one source through the frontend C API runner and dump the "
            "runtime inspector object"
        ),
        dump_flag=RUNTIME_INSPECTOR_DUMP_FLAG,
        dump_filename="runtime-inspector.json",
        guarantee_owner=(
            "developer-facing runtime inspection stays tied to the real emitted "
            "object artifact and runtime ABI boundary models"
        ),
    )
)
COMPILE_STAGE_TRACE_DUMP: Final[DeveloperToolingDumpContract] = (
    DeveloperToolingDumpContract(
        action="trace-compile-stages",
        summary=(
            "compile one source through the frontend C API runner and dump the "
            "stage trace object"
        ),
        dump_flag=STAGE_TRACE_DUMP_FLAG,
        dump_filename="compile-stage-trace.json",
        guarantee_owner=(
            "developer-facing compile stage traces stay tied to the real frontend "
            "runner stage summaries and process exit semantics"
        ),
    )
)

DEVELOPER_TOOLING_DUMP_CONTRACTS: Final[tuple[DeveloperToolingDumpContract, ...]] = (
    COMPILE_OBSERVABILITY_DUMP,
    RUNTIME_INSPECTOR_DUMP,
    COMPILE_STAGE_TRACE_DUMP,
)
DEVELOPER_TOOLING_DUMP_CONTRACTS_BY_ACTION: Final[
    dict[str, DeveloperToolingDumpContract]
] = {contract.action: contract for contract in DEVELOPER_TOOLING_DUMP_CONTRACTS}
