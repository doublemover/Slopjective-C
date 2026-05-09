"""Metaprogramming lowering runtime acceptance contract surfaces."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.metaprogramming_semantic_surfaces import (
    RUNTIME_METAPROGRAMMING_SEMANTICS_SURFACE_CONTRACT_ID,
)

RUNTIME_METAPROGRAMMING_LOWERING_HOST_CACHE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.metaprogramming.lowering.host.cache.surface.v1"
)


RUNTIME_CROSS_MODULE_METAPROGRAMMING_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.cross.module.metaprogramming.artifact.preservation.surface.v1"
)


def build_runtime_metaprogramming_lowering_host_cache_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "metaprogramming-macro-safety-cache-diagnostics",
            "metaprogramming-lowering-host-cache-surface",
            "metaprogramming-executable-lowering",
        }
    ]
    return {
        "contract_id": RUNTIME_METAPROGRAMMING_LOWERING_HOST_CACHE_SURFACE_CONTRACT_ID,
        "compile_manifest_artifact": "<emit-prefix>.manifest.json",
        "runtime_import_surface_artifact": "<emit-prefix>.runtime-import-surface.json",
        "host_cache_artifact": "<emit-prefix>.metaprogramming-macro-host-cache.json",
        "object_artifact": "<emit-prefix>.obj",
        "backend_artifact": "<emit-prefix>.ll",
        "runtime_metaprogramming_semantics_surface_contract_id": (
            RUNTIME_METAPROGRAMMING_SEMANTICS_SURFACE_CONTRACT_ID
        ),
        "expansion_lowering_contract_id": (
            "objc3c.metaprogramming.expansion.lowering.contract.v1"
        ),
        "synthesized_ast_ir_emission_contract_id": (
            "objc3c.metaprogramming.synthesized.ast.ir.emission.v1"
        ),
        "module_interface_replay_preservation_contract_id": (
            "objc3c.metaprogramming.module.interface.replay.preservation.v1"
        ),
        "macro_host_process_cache_runtime_integration_contract_id": (
            "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1"
        ),
        "host_runtime_boundary_contract_id": (
            "objc3c.metaprogramming.expansion.host.runtime.boundary.v1"
        ),
        "lowering_host_cache_surface_model": (
            "metaprogramming-lowering-emission-replay-and-host-cache-packets-freeze-one-live-compile-coupled-boundary-before-runnable-expansion-runtime-package-loading-or-public-abi-widening"
        ),
        "semantic_surface_paths": [
            "frontend.pipeline.semantic_surface.objc_metaprogramming_expansion_and_lowering_contract",
            "frontend.pipeline.semantic_surface.objc_metaprogramming_synthesized_ast_and_ir_emission",
            "frontend.pipeline.semantic_surface.objc_metaprogramming_module_interface_and_replay_preservation",
            "frontend.pipeline.semantic_surface.objc_metaprogramming_macro_host_process_and_cache_runtime_integration",
        ],
        "runtime_import_surface_paths": [
            "objc_metaprogramming_module_interface_and_replay_preservation",
            "objc_metaprogramming_macro_host_process_and_cache_runtime_integration",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp",
        ],
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/expansion_lowering_positive.objc3",
            "tests/tooling/fixtures/native/synthesized_ast_ir_macro_positive.objc3",
            "tests/tooling/fixtures/native/expansion_host_runtime_boundary_positive.objc3",
            "tests/tooling/fixtures/native/macro_host_process_provider.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/expansion_host_runtime_boundary_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-runnable-macro-execution-claim",
            "no-runtime-package-loader-readiness-claim",
            "no-public-abi-widening",
        ],
        "requires_runtime_import_surface": True,
        "requires_host_cache_artifact": True,
        "requires_real_compile_output": True,
    }


def build_runtime_cross_module_metaprogramming_artifact_preservation_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"cross-module-metaprogramming-artifact-preservation"}
    ]
    return {
        "contract_id": (
            RUNTIME_CROSS_MODULE_METAPROGRAMMING_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID
        ),
        "source_contract_id": (
            RUNTIME_METAPROGRAMMING_LOWERING_HOST_CACHE_SURFACE_CONTRACT_ID
        ),
        "runtime_import_surface_artifact": "<emit-prefix>.runtime-import-surface.json",
        "cross_module_link_plan_artifact": "<emit-prefix>.cross-module-runtime-link-plan.json",
        "module_interface_replay_preservation_contract_id": (
            "objc3c.metaprogramming.module.interface.replay.preservation.v1"
        ),
        "macro_host_process_cache_runtime_integration_contract_id": (
            "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1"
        ),
        "surface_model": (
            "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-preserve-metaprogramming-replay-facts-and-host-cache-compatibility-beyond-local-object-emission"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp",
        ],
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/preservation_provider.objc3",
            "tests/tooling/fixtures/native/preservation_consumer.objc3",
        ],
        "requires_runtime_import_surface": True,
        "requires_cross_module_link_plan": True,
        "requires_real_compile_output": True,
    }
