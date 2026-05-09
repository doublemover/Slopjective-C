"""Metaprogramming semantic runtime acceptance contract surfaces."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult

RUNTIME_METAPROGRAMMING_SEMANTICS_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.metaprogramming.semantics.surface.v1"
)


def build_runtime_metaprogramming_semantics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "metaprogramming-semantics",
            "metaprogramming-derive-property-behavior-semantics",
            "metaprogramming-macro-safety-cache-diagnostics",
        }
    ]
    return {
        "contract_id": RUNTIME_METAPROGRAMMING_SEMANTICS_SURFACE_CONTRACT_ID,
        "semantic_contract_ids": [
            "objc3c.metaprogramming.expansion.behavior.semantic.model.v1",
            "objc3c.metaprogramming.macro.safety.sandbox.determinism.semantics.v1",
        ],
        "source_dependency_contract_ids": [
            "objc3c.metaprogramming.metaprogramming.source.closure.v1",
            "objc3c.metaprogramming.macro.package.provenance.source.completion.v1",
            "objc3c.metaprogramming.property.behavior.source.completion.v1",
        ],
        "cache_runtime_contract_ids": [
            "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp",
        ],
        "semantic_surface_model": (
            "derive-macro-package-provenance-property-behavior-and-macro-safety-packets-share-one-deterministic-sema-boundary-before-lowering-runtime-host-cache-integration-or-runtime-hooks"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/expansion_behavior_semantic_model_positive.objc3",
            "tests/tooling/fixtures/native/expansion_lowering_positive.objc3",
            "tests/tooling/fixtures/native/derive_expansion_inventory_positive.objc3",
            "tests/tooling/fixtures/native/macro_host_process_provider.objc3",
            "tests/tooling/fixtures/native/macro_safety_sandbox_negative_missing_metadata.objc3",
            "tests/tooling/fixtures/native/macro_safety_sandbox_negative_orphan_metadata.objc3",
            "tests/tooling/fixtures/native/macro_safety_sandbox_negative_invalid_package.objc3",
            "tests/tooling/fixtures/native/macro_safety_sandbox_negative_invalid_provenance.objc3",
            "tests/tooling/fixtures/native/macro_safety_sandbox_negative_nonpure.objc3",
            "tests/tooling/fixtures/native/macro_safety_sandbox_negative_method_topology.objc3",
            "tests/tooling/fixtures/native/property_behavior_legality_positive.objc3",
        ],
        "requires_real_compile_output": True,
    }
