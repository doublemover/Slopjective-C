"""Importable runtime architecture integration package."""

from __future__ import annotations

from .cli import main
from .contracts import (
    CLAIM_BOUNDARY_CONTRACT_ID,
    FULL_HARNESS_SUMMARY_PATH,
    HARNESS_SCRIPT,
    HARNESS_SUMMARY_CONTRACT_ID,
    INTEGRATION_CONTRACT_ID,
    INTEGRATION_PAYLOAD_SURFACE_KEYS,
    INTEGRATION_SUMMARY_PATH,
    INTEGRATION_SURFACE_CONTRACT_ID,
    PROOF_PACKET_CONTRACT_ID,
    PROOF_PACKET_PATH,
    PROOF_PACKET_SCRIPT,
    REQUIRED_STEP_ACTION_GROUPS,
    ROOT,
    SURFACE_KEYS,
)
from .fixtures import (
    load_harness_summary,
    load_proof_packet,
    load_public_workflow_report,
    load_runtime_acceptance_report,
    run_runtime_architecture_inputs,
)
from .rendering import render_summary_path, write_integration_summary
from .summary import build_integration_summary
from .validation_checks import (
    ValidatedRuntimeArchitecture,
    collect_step_details,
    expect,
    validate_runtime_architecture_reports,
)

__all__ = [
    "CLAIM_BOUNDARY_CONTRACT_ID",
    "FULL_HARNESS_SUMMARY_PATH",
    "HARNESS_SCRIPT",
    "HARNESS_SUMMARY_CONTRACT_ID",
    "INTEGRATION_CONTRACT_ID",
    "INTEGRATION_PAYLOAD_SURFACE_KEYS",
    "INTEGRATION_SUMMARY_PATH",
    "INTEGRATION_SURFACE_CONTRACT_ID",
    "PROOF_PACKET_CONTRACT_ID",
    "PROOF_PACKET_PATH",
    "PROOF_PACKET_SCRIPT",
    "REQUIRED_STEP_ACTION_GROUPS",
    "ROOT",
    "SURFACE_KEYS",
    "ValidatedRuntimeArchitecture",
    "build_integration_summary",
    "collect_step_details",
    "expect",
    "load_harness_summary",
    "load_proof_packet",
    "load_public_workflow_report",
    "load_runtime_acceptance_report",
    "main",
    "render_summary_path",
    "run_runtime_architecture_inputs",
    "validate_runtime_architecture_reports",
    "write_integration_summary",
]
