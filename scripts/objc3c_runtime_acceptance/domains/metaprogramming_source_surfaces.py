"""Metaprogramming source runtime acceptance contract surfaces."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult

RUNTIME_METAPROGRAMMING_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.metaprogramming.source.surface.v1"
)


RUNTIME_METAPROGRAMMING_PACKAGE_PROVENANCE_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.metaprogramming.package.provenance.source.surface.v1"
)


def build_runtime_metaprogramming_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"metaprogramming-source-surface"}
    ]
    return {
        "contract_id": RUNTIME_METAPROGRAMMING_SOURCE_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            "objc3c.metaprogramming.metaprogramming.source.closure.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/token/objc3_token_contract.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
        ],
        "authoritative_source_fields": [
            "Objc3InterfaceDecl.objc_derive_declared",
            "Objc3InterfaceDecl.objc_derive_name",
            "Objc3FunctionDecl.objc_macro_declared",
            "Objc3FunctionDecl.objc_macro_name",
            "Objc3PropertyDecl.property_behavior_name",
            "frontend.pipeline.semantic_surface.objc_metaprogramming_derive_macro_property_behavior_source_closure",
        ],
        "source_surface_model": (
            "derive-markers-macro-markers-and-property-behavior-markers-are-live-parser-owned-source-surfaces-before-semantic-expansion-lowering-or-runtime-materialization"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/expansion_lowering_positive.objc3",
        ],
        "explicit_non_goals": [
            "no-runnable-macro-execution-claim",
            "no-derived-method-body-materialization-claim",
            "no-property-behavior-runtime-hook-claim",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
    }


def build_runtime_metaprogramming_package_provenance_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"metaprogramming-package-provenance-source-surface"}
    ]
    return {
        "contract_id": RUNTIME_METAPROGRAMMING_PACKAGE_PROVENANCE_SOURCE_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            "objc3c.metaprogramming.macro.package.provenance.source.completion.v1",
            "objc3c.metaprogramming.property.behavior.source.completion.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/token/objc3_token_contract.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
        ],
        "authoritative_source_fields": [
            "Objc3FunctionDecl.objc_macro_package_declared",
            "Objc3FunctionDecl.objc_macro_package_name",
            "Objc3FunctionDecl.objc_macro_provenance_declared",
            "Objc3FunctionDecl.objc_macro_provenance_name",
            "Objc3PropertyDecl.property_behavior_name",
            "Objc3PropertyDecl.executable_synthesized_binding_symbol",
            "Objc3PropertyDecl.effective_getter_selector",
            "Objc3PropertyDecl.effective_setter_selector",
            "frontend.pipeline.semantic_surface.objc_metaprogramming_macro_package_and_provenance_source_completion",
            "frontend.pipeline.semantic_surface.objc_metaprogramming_property_behavior_and_synthesized_declaration_source_completion",
        ],
        "source_surface_model": (
            "macro-package-macro-provenance-and-property-behavior-source-completion-freezes-expansion-visible-and-synthesized-declaration-state-before-semantic-expansion-lowering-or-runtime-materialization"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/expansion_lowering_positive.objc3",
            "tests/tooling/fixtures/native/property_behavior_source_completion_positive.objc3",
        ],
        "explicit_non_goals": [
            "no-macro-sandbox-execution-claim",
            "no-property-behavior-runtime-hook-claim",
            "no-executable-synthesized-declaration-materialization-claim",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
    }
