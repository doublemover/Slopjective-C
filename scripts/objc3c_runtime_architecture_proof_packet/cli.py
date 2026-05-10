"""CLI orchestration for runtime architecture proof-packet emission."""

from __future__ import annotations

from .fixtures import run_public_smoke_harness
from .rendering import render_summary_path, write_proof_packet
from .summary import build_proof_packet
from .validation_checks import validate_runtime_architecture_proof_packet


def main() -> int:
    run_public_smoke_harness()
    validated = validate_runtime_architecture_proof_packet()
    payload = build_proof_packet(validated)
    summary_path = write_proof_packet(payload)
    render_summary_path(summary_path)
    return 0
