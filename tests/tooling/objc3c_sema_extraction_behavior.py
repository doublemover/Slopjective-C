from __future__ import annotations

from objc3c_sema_extraction_assertions import (
    assert_contains_all,
    assert_excludes_all,
    assert_in_order,
    assert_paths_exist,
)
from objc3c_sema_extraction_sources import (
    pipeline_sema_api_texts,
    pipeline_types_text,
    sema_cmake_text,
    sema_contract_texts,
    sema_required_paths,
)


def assert_sema_module_exists_and_pipeline_uses_api() -> None:
    assert_paths_exist(sema_required_paths())

    sema_stage_runner, pipeline_orchestration = pipeline_sema_api_texts()
    assert_contains_all(
        sema_stage_runner,
        [
            '#include "sema/objc3_sema_pass_manager.h"',
            "RunObjc3SemaPassManager(sema_input)",
        ],
    )
    assert_contains_all(
        pipeline_orchestration,
        ["result.sema_parity_surface = sema_result.parity_surface;"],
    )


def assert_pipeline_uses_sema_contract_types() -> None:
    pipeline_types = pipeline_types_text()
    assert_contains_all(pipeline_types, ['#include "sema/objc3_sema_contract.h"'])
    assert_excludes_all(
        pipeline_types,
        [
            '#include "sema/objc3_semantic_passes.h"',
            "struct FunctionInfo",
            "struct Objc3SemanticIntegrationSurface",
        ],
    )


def assert_sema_contract_exports_explicit_type_metadata_handoff_surface() -> None:
    contract, sema_header, sema_source = sema_contract_texts()

    assert_contains_all(
        contract,
        [
            "kObjc3SemaBoundaryContractVersionMajor",
            "struct Objc3SemanticFunctionTypeMetadata {",
            "struct Objc3SemanticTypeMetadataHandoff {",
            "std::vector<std::string> global_names_lexicographic;",
            (
                "std::vector<Objc3SemanticFunctionTypeMetadata> "
                "functions_lexicographic;"
            ),
            "Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(",
            "bool IsDeterministicSemanticTypeMetadataHandoff(",
        ],
    )
    assert_contains_all(
        sema_header,
        [
            (
                "BuildSemanticTypeMetadataHandoff("
                "const Objc3SemanticIntegrationSurface &surface);"
            ),
            (
                "IsDeterministicSemanticTypeMetadataHandoff("
                "const Objc3SemanticTypeMetadataHandoff &handoff);"
            ),
        ],
    )
    assert_contains_all(
        sema_source,
        [
            "Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(",
            (
                "std::sort(handoff.global_names_lexicographic.begin(), "
                "handoff.global_names_lexicographic.end());"
            ),
            "std::sort(function_names.begin(), function_names.end());",
            "metadata.param_types.size() == metadata.arity",
            "metadata.param_has_invalid_type_suffix.size() == metadata.arity",
            (
                "std::is_sorted(handoff.global_names_lexicographic.begin(), "
                "handoff.global_names_lexicographic.end())"
            ),
            (
                "std::is_sorted(handoff.functions_lexicographic.begin(), "
                "handoff.functions_lexicographic.end(),"
            ),
        ],
    )

    assert_in_order(
        sema_source,
        [
            "handoff.global_names_lexicographic.reserve(surface.globals.size());",
            (
                "std::sort(handoff.global_names_lexicographic.begin(), "
                "handoff.global_names_lexicographic.end());"
            ),
            "function_names.reserve(surface.functions.size());",
            "std::sort(function_names.begin(), function_names.end());",
            "handoff.functions_lexicographic.reserve(function_names.size());",
            "return handoff;",
        ],
    )


def assert_cmake_registers_sema_target() -> None:
    cmake = sema_cmake_text()
    assert_contains_all(
        cmake,
        [
            "add_library(objc3c_sema STATIC",
            "objc3_sema_pass_manager.cpp",
            "objc3_semantic_passes.cpp",
            "target_link_libraries(objc3c_sema PUBLIC",
            "objc3c_parse",
            "objc3c_diag",
            "add_library(objc3c_sema_type_system INTERFACE)",
            "target_link_libraries(objc3c_sema_type_system INTERFACE",
            "objc3c_sema_type_system",
        ],
    )

    assert_in_order(
        cmake,
        [
            "add_library(objc3c_sema STATIC",
            "objc3_sema_pass_manager.cpp",
            "target_link_libraries(objc3c_sema PUBLIC",
            "add_library(objc3c_sema_type_system INTERFACE)",
            "target_link_libraries(objc3c_sema_type_system INTERFACE",
        ],
    )
