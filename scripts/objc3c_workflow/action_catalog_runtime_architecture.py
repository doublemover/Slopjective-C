"""Runtime architecture action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

RUNTIME_ARCHITECTURE_ACTION_SPECS: dict[str, ActionSpec] = {
    "proof-runtime-architecture": ActionSpec("proof-runtime-architecture", "emit the integrated runtime architecture evidence bundle", "python:scripts/check_objc3c_runtime_architecture_proof_packet.py"),
    "validate-runtime-architecture": ActionSpec("validate-runtime-architecture", "validate runtime architecture across the full public workflow and evidence bundle", "python:scripts/check_objc3c_runtime_architecture_integration.py", validation_tier="full", guarantee_owner="full public workflow and runtime architecture evidence bundle alignment"),
}


__all__ = ["RUNTIME_ARCHITECTURE_ACTION_SPECS"]
