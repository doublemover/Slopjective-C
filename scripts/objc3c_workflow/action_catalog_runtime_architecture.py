"""Runtime architecture action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

RUNTIME_ARCHITECTURE_ACTION_SPECS: dict[str, ActionSpec] = {
    "proof-runtime-architecture": ActionSpec(
        "proof-runtime-architecture",
        "emit the integrated runtime architecture evidence bundle",
        "python:scripts/check_objc3c_runtime_architecture_proof_packet.py",
    ),
    "validate-runtime-architecture": ActionSpec(
        "validate-runtime-architecture",
        "validate runtime architecture across the full public workflow and evidence bundle",
        "python:scripts/check_objc3c_runtime_architecture_integration.py",
        validation_tier="full",
        guarantee_owner="full public workflow and runtime architecture evidence bundle alignment",
    ),
    "validate-public-runtime-reflection-api": ActionSpec(
        "validate-public-runtime-reflection-api",
        "validate the public runtime reflection C API contract and probe surface",
        "python:scripts/check_objc3c_public_runtime_reflection_api.py",
        validation_tier="fast",
        guarantee_owner=(
            "public runtime reflection claims stay attached to the checked C API "
            "contract, public header, realized-state implementation, and public probe"
        ),
    ),
    "validate-advanced-runtime-closure": ActionSpec(
        "validate-advanced-runtime-closure",
        "validate the combined advanced runtime language-semantics closure contract",
        "python:scripts/check_objc3c_advanced_runtime_closure.py",
        validation_tier="fast",
        guarantee_owner=(
            "combined ownership, blocks, errors, concurrency, property behavior, "
            "metaprogramming provenance, and package replay closure stays tied to "
            "checked source truth and the public language-semantics runtime API"
        ),
    ),
}


__all__ = ["RUNTIME_ARCHITECTURE_ACTION_SPECS"]
