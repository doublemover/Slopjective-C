from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_c_api_source_ownership_assertions import (
    assert_contains_all,
    assert_contract_collection_in_text,
    assert_excludes_all,
    assert_paths_exist,
)


def assert_c_api_source_paths_exist(paths: list[Path]) -> None:
    assert_paths_exist(paths)


def assert_c_api_cpp_pins_core_compile_and_c_only_helper_owners(
    source: str,
    squashed_source: str,
) -> None:
    assert_contains_all(
        source,
        [
            '#include "libobjc3c_frontend/c_api.h"',
            "return objc3c_frontend_is_abi_compatible(requested_abi_version);",
            "return objc3c_frontend_abi_version();",
            "return objc3c_frontend_version();",
            "return objc3c_frontend_version_string();",
            "new (std::nothrow) objc3c_frontend_c_context_t();",
            "delete context;",
            "return objc3c_frontend_compile_file(context, options, result);",
            "return objc3c_frontend_compile_source(context, options, result);",
            "return objc3c_frontend_copy_last_error(context, buffer, buffer_size);",
            "OBJC3C_FRONTEND_C_API_OWNER_RESULT_LIFECYCLE == 1",
            "OBJC3C_FRONTEND_C_API_POLICY_C_NAMES_OWN_PACKAGE_SURFACE == 8",
            "objc3c_frontend_c_result_destroy(",
            "ReleaseCompileResultOwnedStrings(result);",
            "*result = {};",
            "objc3c_frontend_c_result_artifact_path(",
            "switch (artifact_kind)",
            "case OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS:",
            "return result->diagnostics_path;",
            "return nullptr;",
            "objc3c_frontend_c_result_artifact_path_view(",
            "objc3c_frontend_c_result_has_artifact(",
            "return view.data != nullptr && view.size != 0u ? 1u : 0u;",
            "objc3c_frontend_c_result_error_message(",
            "return result == nullptr ? nullptr : result->error_message;",
            "objc3c_frontend_c_result_error_message_view(",
            "objc3c_frontend_c_string_view(",
            "if (string == nullptr || string->data == nullptr)",
            "return {string->data, string->size};",
            "objc3c_frontend_c_string_release(",
            "ReleaseOwnedFrontendString(string);",
            "objc3c_frontend_c_stage_summary_is_well_formed(",
            "IsDefinedFrontendCApiStage(expected_stage)",
            "if (summary == nullptr || summary->stage != expected_stage)",
            "summary->attempted > 1u || summary->skipped > 1u",
            "severity_total == summary->diagnostics_total ? 1u : 0u",
            "objc3c_frontend_c_stage_summary_has_diagnostics(",
            "objc3c_frontend_c_stage_summary_has_errors(",
        ],
    )
    assert_contains_all(
        squashed_source,
        [
            (
                "static_assert(std::is_same_v<objc3c_frontend_c_string_t, "
                "objc3c_frontend_string_t>"
            ),
            (
                "static_assert(std::is_same_v<objc3c_frontend_c_stage_summary_t, "
                "objc3c_frontend_stage_summary_t>"
            ),
        ],
    )


def assert_c_api_helper_contract_tracks_result_error_artifact_stage_helpers(
    c_api_surface: str,
    source: str,
    implementation: str,
    contract: dict[str, Any],
) -> None:
    assert contract["contract_id"] == "objc3c.frontend.c_api.helper.contract.v1"
    assert contract["header_path"] == "native/objc3c/src/libobjc3c_frontend/c_api.h"
    assert contract["header_paths"] == [
        "native/objc3c/src/libobjc3c_frontend/c_api.h",
        "native/objc3c/src/libobjc3c_frontend/c_api_contract.h",
        "native/objc3c/src/libobjc3c_frontend/c_api_types.h",
        "native/objc3c/src/libobjc3c_frontend/c_api_version.h",
        "native/objc3c/src/libobjc3c_frontend/c_api_lifecycle.h",
        "native/objc3c/src/libobjc3c_frontend/c_api_compile.h",
        "native/objc3c/src/libobjc3c_frontend/c_api_result.h",
        "native/objc3c/src/libobjc3c_frontend/c_api_string.h",
        "native/objc3c/src/libobjc3c_frontend/c_api_stage_summary.h",
    ]
    assert contract["source_paths"] == [
        "native/objc3c/src/libobjc3c_frontend/c_api_abi.cpp",
        "native/objc3c/src/libobjc3c_frontend/c_api_compile.cpp",
        "native/objc3c/src/libobjc3c_frontend/c_api_lifecycle.cpp",
        "native/objc3c/src/libobjc3c_frontend/c_api_result_artifacts.cpp",
        "native/objc3c/src/libobjc3c_frontend/c_api_result_error.cpp",
        "native/objc3c/src/libobjc3c_frontend/c_api_result_lifecycle.cpp",
        "native/objc3c/src/libobjc3c_frontend/c_api_stage_summary.cpp",
        "native/objc3c/src/libobjc3c_frontend/c_api_string_bridge.cpp",
    ]

    for alias in contract["required_type_aliases"]:
        assert alias in c_api_surface
        assert f"std::is_same_v<{alias}," in source

    assert_contract_collection_in_text(
        contract["required_helpers"],
        c_api_surface,
        suffix="(",
    )
    assert_contract_collection_in_text(
        contract["required_helpers"],
        source,
        suffix="(",
    )
    assert_contract_collection_in_text(
        contract["required_header_ownership_phrases"],
        c_api_surface,
    )
    assert_contract_collection_in_text(
        contract["required_source_fail_closed_snippets"],
        implementation,
    )


def assert_frontend_result_ownership_releases_immutable_owned_strings(
    ownership: str,
    compile_contract: str,
) -> None:
    assert_contains_all(
        ownership,
        [
            "CloneOwnedFrontendString(const std::string &text)",
            "string->data = data;",
            "std::free(const_cast<char *>(string->data));",
            "ReleaseCompileResultOwnedStrings(",
            "PopulateCompileResultOwnedPayload(",
            "failed to allocate frontend result-owned string storage.",
        ],
    )
    assert_contains_all(
        compile_contract,
        [
            "IsMissingFrontendBorrowedPath(",
            "BorrowedFrontendPathToFilesystemPath(",
        ],
    )
    assert_excludes_all(ownership, ["IsNullOrEmpty("])
    assert_excludes_all(compile_contract, ["IsNullOrEmpty("])
