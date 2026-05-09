"""Command strings published by runtime acceptance contract surfaces."""

from __future__ import annotations


RUNTIME_ACCEPTANCE_COMMAND = "python scripts/check_objc3c_runtime_acceptance.py"
VALIDATE_RUNTIME_ARCHITECTURE_COMMAND = (
    "npm run objc3c -- validate-runtime-architecture"
)


__all__ = [
    "RUNTIME_ACCEPTANCE_COMMAND",
    "VALIDATE_RUNTIME_ARCHITECTURE_COMMAND",
]
