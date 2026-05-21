"""Runtime acceptance and architecture workflow actions."""

from __future__ import annotations

import sys
from pathlib import Path

from ..commands import run
from ..environment import ROOT
from .runtime_acceptance_routes import (
    RUNTIME_ACCEPTANCE_PY,
    run_runtime_acceptance_action,
)

RUNTIME_ARCHITECTURE_PROOF_PACKET_PY = (
    ROOT / "scripts" / "check_objc3c_runtime_architecture_proof_packet.py"
)
RUNTIME_ARCHITECTURE_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_runtime_architecture_integration.py"
)
PUBLIC_RUNTIME_REFLECTION_API_PY = (
    ROOT / "scripts" / "check_objc3c_public_runtime_reflection_api.py"
)


def run_python_check(script: Path, *args: str) -> int:
    return run([sys.executable, str(script), *args])


def action_test_runtime_acceptance(_: list[str]) -> int:
    return run_runtime_acceptance_action("test-runtime-acceptance")


def action_test_runtime_acceptance_fast(_: list[str]) -> int:
    return run_runtime_acceptance_action("test-runtime-acceptance-fast")


def action_test_runtime_acceptance_diagnostics(_: list[str]) -> int:
    return run_runtime_acceptance_action("test-runtime-acceptance-diagnostics")


def action_test_runtime_acceptance_cross_module(_: list[str]) -> int:
    return run_runtime_acceptance_action("test-runtime-acceptance-cross-module")


def action_test_runtime_acceptance_block_arc(_: list[str]) -> int:
    return run_runtime_acceptance_action("test-runtime-acceptance-block-arc")


def action_test_runtime_acceptance_arc_cleanup_integration(_: list[str]) -> int:
    return run_runtime_acceptance_action(
        "test-runtime-acceptance-arc-cleanup-integration"
    )


def action_test_runtime_acceptance_concurrency(_: list[str]) -> int:
    return run_runtime_acceptance_action("test-runtime-acceptance-concurrency")


def action_proof_runtime_architecture(_: list[str]) -> int:
    return run_python_check(RUNTIME_ARCHITECTURE_PROOF_PACKET_PY)


def action_validate_runtime_architecture(_: list[str]) -> int:
    return run_python_check(RUNTIME_ARCHITECTURE_INTEGRATION_PY)


def action_validate_public_runtime_reflection_api(_: list[str]) -> int:
    return run_python_check(PUBLIC_RUNTIME_REFLECTION_API_PY)
