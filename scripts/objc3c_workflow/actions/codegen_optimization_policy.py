"""Codegen optimization and direct-dispatch policy workflow action."""

from __future__ import annotations

import sys

from ..commands import run

CODEGEN_OPTIMIZATION_POLICY_ACTION = "validate-codegen-optimization-policy"
CODEGEN_OPTIMIZATION_POLICY_SUMMARY = (
    "validate semantic-preserving codegen optimization and direct-dispatch policy"
)
CODEGEN_OPTIMIZATION_POLICY_BACKEND = (
    "python:scripts/check_objc3c_codegen_optimization_direct_dispatch_policy.py"
)
CODEGEN_OPTIMIZATION_POLICY_VALIDATION_TIER = "policy"
CODEGEN_OPTIMIZATION_POLICY_GUARANTEE_OWNER = (
    "codegen optimization passes and direct-dispatch lowering stay semantic-preserving, "
    "matrix-backed, and fail-closed without retired adapter routes"
)
CODEGEN_OPTIMIZATION_POLICY_COMMAND = (
    sys.executable,
    "scripts/check_objc3c_codegen_optimization_direct_dispatch_policy.py",
)


def action_validate_codegen_optimization_policy(_: list[str]) -> int:
    return run([str(part) for part in CODEGEN_OPTIMIZATION_POLICY_COMMAND])


__all__ = [
    "CODEGEN_OPTIMIZATION_POLICY_ACTION",
    "CODEGEN_OPTIMIZATION_POLICY_BACKEND",
    "CODEGEN_OPTIMIZATION_POLICY_GUARANTEE_OWNER",
    "CODEGEN_OPTIMIZATION_POLICY_SUMMARY",
    "CODEGEN_OPTIMIZATION_POLICY_VALIDATION_TIER",
    "action_validate_codegen_optimization_policy",
]
