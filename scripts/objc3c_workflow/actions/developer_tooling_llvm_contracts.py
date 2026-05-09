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
HOSTED_LLVM_CAPABILITY_MODE: Final[str] = "objc3c-llvm-capabilities-v2"
HOSTED_LLVM_CAPABILITY_TRUTH_SOURCE: Final[str] = "hosted-llvm-summary"
LOCAL_LLVM_DIAGNOSTIC_SOURCE: Final[str] = "local-llvm-probe-summary"

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


@dataclass(frozen=True)
class CapabilityExplorerContract:
    action: str
    summary: str
    backend: str
    guarantee_owner: str
    validation_tier: str = "repo"
    pass_through_args: bool = True


@dataclass(frozen=True)
class ToolCapabilityOwnerContract:
    action: str
    owner: str
    proof_source: str
    claim_scope: str
    unsupported_claims: tuple[str, ...]
    requires_hosted_probe_summary: bool
    fails_closed_without_live_capability: bool


@dataclass(frozen=True)
class HostedLLVMCapabilityTruth:
    source_kind: str
    mode: object
    ok: bool
    clang_found: bool
    llc_found: bool
    llc_supports_filetype_obj: bool
    failure_reasons: tuple[str, ...]

    @property
    def is_hosted_source(self) -> bool:
        return self.source_kind == HOSTED_LLVM_CAPABILITY_TRUTH_SOURCE

    @property
    def hosted_execution_supported(self) -> bool:
        return (
            self.is_hosted_source
            and self.mode == HOSTED_LLVM_CAPABILITY_MODE
            and self.ok
            and self.clang_found
            and self.llc_found
            and self.llc_supports_filetype_obj
        )

    @property
    def hosted_source_parity_supported(self) -> bool:
        return self.hosted_execution_supported

    @property
    def local_probe_diagnostic_only(self) -> bool:
        return not self.is_hosted_source

    @property
    def fail_closed_without_live_capability(self) -> bool:
        return not self.hosted_execution_supported

    def as_payload(self) -> dict[str, object]:
        return {
            "source_kind": self.source_kind,
            "mode": self.mode,
            "ok": self.ok,
            "clang_found": self.clang_found,
            "llc_found": self.llc_found,
            "llc_supports_filetype_obj": self.llc_supports_filetype_obj,
            "local_probe_diagnostic_only": self.local_probe_diagnostic_only,
            "hosted_execution_supported": self.hosted_execution_supported,
            "hosted_source_parity_supported": self.hosted_source_parity_supported,
            "fail_closed_without_live_capability": (
                self.fail_closed_without_live_capability
            ),
            "failure_reasons": list(self.failure_reasons),
        }


def _summary_section(summary: dict[str, object], key: str) -> dict[str, object]:
    value = summary.get(key)
    return value if isinstance(value, dict) else {}


def hosted_llvm_capability_truth_from_summary(
    summary: dict[str, object],
    *,
    source_kind: str,
) -> HostedLLVMCapabilityTruth:
    clang = _summary_section(summary, "clang")
    llc = _summary_section(summary, "llc")
    llc_features = _summary_section(summary, "llc_features")
    mode = summary.get("mode")
    ok = summary.get("ok") is True
    clang_found = bool(clang.get("found"))
    llc_found = bool(llc.get("found"))
    llc_supports_filetype_obj = bool(llc_features.get("supports_filetype_obj"))

    failure_reasons: list[str] = []
    if source_kind != HOSTED_LLVM_CAPABILITY_TRUTH_SOURCE:
        failure_reasons.append("local LLVM probe summary is diagnostic-only")
    if mode != HOSTED_LLVM_CAPABILITY_MODE:
        failure_reasons.append("missing canonical LLVM capability summary mode")
    if not ok:
        failure_reasons.append("summary did not publish ok=true")
    if not clang_found:
        failure_reasons.append("clang availability missing")
    if not llc_found:
        failure_reasons.append("llc availability missing")
    elif not llc_supports_filetype_obj:
        failure_reasons.append("llc --filetype=obj support missing")

    return HostedLLVMCapabilityTruth(
        source_kind=source_kind,
        mode=mode,
        ok=ok,
        clang_found=clang_found,
        llc_found=llc_found,
        llc_supports_filetype_obj=llc_supports_filetype_obj,
        failure_reasons=tuple(failure_reasons),
    )


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

LLVM_TOOL_CAPABILITY_OWNER_CONTRACTS: Final[
    tuple[ToolCapabilityOwnerContract, ...]
] = (
    ToolCapabilityOwnerContract(
        action=CHECK_LLVM_CAPABILITIES_ACTION,
        owner="developer-tooling.llvm.local-probe",
        proof_source="tmp/artifacts/objc3c-native/llvm_capabilities/summary.json",
        claim_scope=(
            "local probe output records the current machine only and is never "
            "hosted-execution capability truth"
        ),
        unsupported_claims=(
            "hosted LLVM availability",
            "hosted source parity",
            "language-server execution fallback",
        ),
        requires_hosted_probe_summary=False,
        fails_closed_without_live_capability=True,
    ),
    ToolCapabilityOwnerContract(
        action=CHECK_HOSTED_LLVM_CAPABILITIES_ACTION,
        owner="developer-tooling.hosted-llvm.probe",
        proof_source=HOSTED_LLVM_CAPABILITIES_SUMMARY_OUT,
        claim_scope=(
            "hosted LLVM capability truth requires the hosted summary to report "
            "clang plus llc object-emission support"
        ),
        unsupported_claims=(
            "clang-only hosted execution",
            "local-only hosted execution proof",
            "hosted object parity without llc --filetype=obj",
        ),
        requires_hosted_probe_summary=True,
        fails_closed_without_live_capability=True,
    ),
    ToolCapabilityOwnerContract(
        action=CAPABILITY_ROUTED_PARITY_ACTION,
        owner="developer-tooling.hosted-llvm.parity",
        proof_source=HOSTED_LLVM_CAPABILITIES_SUMMARY_OUT,
        claim_scope=(
            "capability-routed parity may run only from hosted LLVM probe truth "
            "that exposes native object emission"
        ),
        unsupported_claims=(
            "parity success when the hosted route was skipped",
            "local-only source parity as hosted capability proof",
            "fallback language-server parity",
        ),
        requires_hosted_probe_summary=True,
        fails_closed_without_live_capability=True,
    ),
    ToolCapabilityOwnerContract(
        action=CAPABILITY_EXPLORER_ACTION,
        owner="developer-tooling.capability-explorer",
        proof_source=CAPABILITY_EXPLORER_DUMP_FILENAME,
        claim_scope=(
            "explorer payloads expose probe state only and do not upgrade "
            "unsupported language-server or hosted execution claims"
        ),
        unsupported_claims=(
            "rename/reference/semantic-token language-server support",
            "hosted execution support",
            "wrapper-only action capability",
        ),
        requires_hosted_probe_summary=False,
        fails_closed_without_live_capability=True,
    ),
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
