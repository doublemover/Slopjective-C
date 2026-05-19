"""Runtime acceptance claim boundary report surface."""

from __future__ import annotations

from typing import Any


def build_claim_boundary(public_runtime_abi_boundary: list[str]) -> dict[str, Any]:
    return {
        "contract_id": "objc3c.runtime.execution.claim.boundary.v1",
        "authoritative_claim_classes": {
            "linked-runtime-probe": {
                "requires_runtime_library_or_emitted_object": True,
                "requires_executable_probe": True,
                "requires_runtime_backed_execution_or_snapshot": True,
            },
            "compile-coupled-inspection": {
                "requires_real_compile": True,
                "requires_compile_output_truthfulness": True,
                "requires_coupled_registration_manifest": True,
            },
        },
        "non_authoritative_inputs": [
            "hand-authored llvm ir without matching compile output",
            "sidecar-only manifests or reports with no coupled object/probe path",
            "non-authoritative test surfaces without a coupled emitted object and runtime probe",
            "comment-only or placeholder-only capability claims",
        ],
        "public_runtime_abi_boundary": public_runtime_abi_boundary,
    }


__all__ = ["build_claim_boundary"]
