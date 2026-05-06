"""Runtime acceptance and runnable runtime workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from ..environment import ROOT

RUNTIME_ACCEPTANCE_PY = ROOT / "scripts" / "check_objc3c_runtime_acceptance.py"
RUNTIME_ARCHITECTURE_PROOF_PACKET_PY = (
    ROOT / "scripts" / "check_objc3c_runtime_architecture_proof_packet.py"
)
RUNTIME_ARCHITECTURE_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_runtime_architecture_integration.py"
)
RUNNABLE_BOOTSTRAP_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_bootstrap_end_to_end.py"
)
RUNNABLE_BLOCK_ARC_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_block_arc_conformance.py"
)
RUNNABLE_BLOCK_ARC_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_block_arc_end_to_end.py"
)
RUNNABLE_CONCURRENCY_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_concurrency_conformance.py"
)
RUNNABLE_CONCURRENCY_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_concurrency_end_to_end.py"
)
RUNNABLE_OBJECT_MODEL_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_object_model_conformance.py"
)
RUNNABLE_OBJECT_MODEL_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_object_model_end_to_end.py"
)
RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_storage_reflection_conformance.py"
)
RUNNABLE_STORAGE_REFLECTION_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_storage_reflection_end_to_end.py"
)
RUNNABLE_ERROR_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_error_conformance.py"
)
RUNNABLE_ERROR_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_error_end_to_end.py"
RUNNABLE_INTEROP_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_interop_conformance.py"
)
RUNNABLE_INTEROP_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_interop_end_to_end.py"
RUNNABLE_METAPROGRAMMING_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_metaprogramming_conformance.py"
)
RUNNABLE_METAPROGRAMMING_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_metaprogramming_end_to_end.py"
)
RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_release_candidate_conformance.py"
)
RUNNABLE_RELEASE_CANDIDATE_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_release_candidate_end_to_end.py"
)


def action_test_runtime_acceptance(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_ACCEPTANCE_PY)])


def action_test_runtime_acceptance_fast(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_ACCEPTANCE_PY), "--suite", "fast"])


def action_test_runtime_acceptance_diagnostics(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_ACCEPTANCE_PY), "--suite", "diagnostics"])


def action_test_runtime_acceptance_cross_module(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_ACCEPTANCE_PY), "--suite", "cross-module"])


def action_test_runtime_acceptance_block_arc(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_ACCEPTANCE_PY), "--suite", "block-arc"])


def action_test_runtime_acceptance_concurrency(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_ACCEPTANCE_PY), "--suite", "concurrency"])


def action_proof_runtime_architecture(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_ARCHITECTURE_PROOF_PACKET_PY)])


def action_validate_runtime_architecture(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_ARCHITECTURE_INTEGRATION_PY)])


def action_validate_runnable_bootstrap(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_BOOTSTRAP_E2E_PY)])


def action_validate_block_arc_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_BLOCK_ARC_CONFORMANCE_PY)])


def action_validate_runnable_block_arc(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_BLOCK_ARC_E2E_PY)])


def action_validate_concurrency_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_CONCURRENCY_CONFORMANCE_PY)])


def action_validate_runnable_concurrency(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_CONCURRENCY_E2E_PY)])


def action_validate_object_model_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_OBJECT_MODEL_CONFORMANCE_PY)])


def action_validate_runnable_object_model(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_OBJECT_MODEL_E2E_PY)])


def action_validate_storage_reflection_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_PY)])


def action_validate_runnable_storage_reflection(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STORAGE_REFLECTION_E2E_PY)])


def action_validate_error_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_ERROR_CONFORMANCE_PY)])


def action_validate_runnable_error(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_ERROR_E2E_PY)])


def action_validate_interop_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_INTEROP_CONFORMANCE_PY)])


def action_validate_runnable_interop(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_INTEROP_E2E_PY)])


def action_validate_metaprogramming_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_METAPROGRAMMING_CONFORMANCE_PY)])


def action_validate_runnable_metaprogramming(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_METAPROGRAMMING_E2E_PY)])


def action_validate_release_candidate_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_PY)])


def action_validate_runnable_release_candidate(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_RELEASE_CANDIDATE_E2E_PY)])
