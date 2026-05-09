"""Claim-boundary fields for composite public workflow reports."""

from __future__ import annotations


def composite_claim_boundary() -> dict[str, object]:
    return {
        "contract_id": "objc3c.runtime.execution.claim.boundary.v1",
        "reports_are_authoritative_only_when_child_steps_are_compile-coupled": True,
        "authoritative_child_surfaces": [
            "scripts/check_objc3c_runtime_acceptance.py",
            "scripts/check_objc3c_execution_replay_proof.ps1",
            "scripts/check_objc3c_native_execution_smoke.ps1",
        ],
        "non_authoritative_inputs": [
            "integrated report paths by themselves",
            "sidecar-only summaries with no matching emitted object/probe path",
            "synthetic or hand-authored llvm ir used without coupled compile output",
        ],
    }


__all__ = ["composite_claim_boundary"]
