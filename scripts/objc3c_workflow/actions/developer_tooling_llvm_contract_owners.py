"""Owner-contract declarations for hosted LLVM developer-tooling actions."""

from __future__ import annotations

from typing import Final

from .developer_tooling_llvm_contract_constants import (
    CAPABILITY_EXPLORER_ACTION,
    CAPABILITY_EXPLORER_DUMP_FILENAME,
    CAPABILITY_ROUTED_PARITY_ACTION,
    CHECK_HOSTED_LLVM_CAPABILITIES_ACTION,
    CHECK_LLVM_CAPABILITIES_ACTION,
    HOSTED_LLVM_CAPABILITIES_SUMMARY_OUT,
    LLVM_CAPABILITY_PROBE_BACKEND,
)
from .developer_tooling_llvm_contract_models import (
    CapabilityExplorerContract,
    ToolCapabilityOwnerContract,
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
            "language-server execution retired route",
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
            "clang, clang++, native_object_emission_supported from "
            "llc --filetype=obj, coherent LLVM toolchain identity, llvm-ar "
            "archive tooling, and LLVM header/library discovery from "
            "llvm-config or an installed LLVM root"
        ),
        unsupported_claims=(
            "clang-only hosted execution",
            "local-only hosted execution proof",
            "hosted object parity without llc --filetype=obj",
            "hosted object parity with mixed LLVM roots or mismatched LLVM tool versions",
            "hosted package or execution support without llvm-ar and LLVM headers/libs",
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
            "retired route language-server parity",
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

__all__ = [
    "CAPABILITY_EXPLORER_CONTRACT",
    "LLVM_TOOL_CAPABILITY_OWNER_CONTRACTS",
]
