"""Error lowering surface builders."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.errors_surface_support import (
    authoritative_case_ids,
)

from ..runtime_contract_errors import RUNTIME_ERROR_LOWERING_UNWIND_BRIDGE_HELPER_SURFACE_CONTRACT_ID


def build_runtime_error_lowering_unwind_bridge_helper_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    return {
        "contract_id": RUNTIME_ERROR_LOWERING_UNWIND_BRIDGE_HELPER_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "source_contract_ids": [
            "objc3c.ns.error.bridging.lowering.v1",
            "objc3c.unwind.cleanup.lowering.v1",
            "objc3c.error_handling.throws.abi.propagation.lowering.v1",
            "objc3c.error_handling.result.and.bridging.artifact.replay.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/lower/objc3_lowering_contract.h",
            "native/objc3c/src/ir/objc3_ir_emitter.h",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp",
        ],
        "authoritative_case_ids": authoritative_case_ids(
            results,
            {"error-lowering-unwind-bridge-helper-surface"},
        ),
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/error_out_abi_positive.objc3",
            "tests/tooling/fixtures/native/error_runtime_bridge_helper_positive.objc3",
            "tests/tooling/runtime/error_runtime_bridge_helper_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-milestone-specific-scaffolding",
            "no-sidecar-only-proof",
            "no-runtime-helper-abi-claims-before-lane-d",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": False,
    }


__all__ = ["build_runtime_error_lowering_unwind_bridge_helper_surface"]
