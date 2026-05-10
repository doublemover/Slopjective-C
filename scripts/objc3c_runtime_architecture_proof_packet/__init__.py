"""Importable runtime architecture proof-packet package."""

from __future__ import annotations

from .cli import main
from .contracts import (
    CLAIM_BOUNDARY_CONTRACT_ID,
    HARNESS_SCRIPT,
    HARNESS_SUMMARY_CONTRACT_ID,
    HARNESS_SUMMARY_PATH,
    PACKET_SURFACE_KEYS,
    PROOF_PACKET_CONTRACT_ID,
    PROOF_PACKET_PATH,
    PROOF_PACKET_SURFACE_CONTRACT_ID,
    PUBLIC_SMOKE_SUITE_ID,
    ROOT,
    SURFACE_KEYS,
)
from .fixtures import (
    load_harness_summary,
    load_public_workflow_report,
    load_runtime_acceptance_report,
    run_public_smoke_harness,
)
from .rendering import render_summary_path, write_proof_packet
from .summary import build_proof_packet
from .validation_checks import (
    ValidatedRuntimeArchitectureProofPacket,
    collect_child_report_paths,
    expect,
    validate_runtime_architecture_proof_packet,
)

__all__ = [
    "CLAIM_BOUNDARY_CONTRACT_ID",
    "HARNESS_SCRIPT",
    "HARNESS_SUMMARY_CONTRACT_ID",
    "HARNESS_SUMMARY_PATH",
    "PACKET_SURFACE_KEYS",
    "PROOF_PACKET_CONTRACT_ID",
    "PROOF_PACKET_PATH",
    "PROOF_PACKET_SURFACE_CONTRACT_ID",
    "PUBLIC_SMOKE_SUITE_ID",
    "ROOT",
    "SURFACE_KEYS",
    "ValidatedRuntimeArchitectureProofPacket",
    "build_proof_packet",
    "collect_child_report_paths",
    "expect",
    "load_harness_summary",
    "load_public_workflow_report",
    "load_runtime_acceptance_report",
    "main",
    "render_summary_path",
    "run_public_smoke_harness",
    "validate_runtime_architecture_proof_packet",
    "write_proof_packet",
]
