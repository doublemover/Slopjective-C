"""Runtime acceptance command surface definitions."""

from __future__ import annotations

RUNTIME_ACCEPTANCE_SCRIPT = "scripts/check_objc3c_runtime_acceptance.py"
RUNTIME_ACCEPTANCE_COMMAND = f"python {RUNTIME_ACCEPTANCE_SCRIPT}"
VALIDATE_RUNTIME_ARCHITECTURE_COMMAND = (
    "npm run objc3c -- validate-runtime-architecture"
)


def published_runtime_acceptance_commands() -> tuple[str, ...]:
    return (
        RUNTIME_ACCEPTANCE_COMMAND,
        VALIDATE_RUNTIME_ARCHITECTURE_COMMAND,
    )


__all__ = [
    "RUNTIME_ACCEPTANCE_COMMAND",
    "RUNTIME_ACCEPTANCE_SCRIPT",
    "VALIDATE_RUNTIME_ARCHITECTURE_COMMAND",
    "published_runtime_acceptance_commands",
]
