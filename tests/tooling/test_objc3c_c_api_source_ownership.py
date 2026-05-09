from __future__ import annotations

import json

from c_api_smoke_support import (
    C_API_HELPER_CONTRACT,
    C_API_SOURCES,
    FRONTEND_COMPILE_CONTRACT_CPP,
    RESULT_OWNERSHIP_CPP,
    c_api_source_text,
    c_api_surface_text,
    read_text,
    squash_ws,
)


def test_c_api_cpp_pins_core_compile_and_c_only_helper_owners() -> None:
    for source_path in C_API_SOURCES:
        assert source_path.exists(), source_path

    source = c_api_source_text()
    squashed_source = squash_ws(source)

    assert '#include "libobjc3c_frontend/c_api.h"' in source
    assert "return objc3c_frontend_is_abi_compatible(requested_abi_version);" in source
    assert "return objc3c_frontend_abi_version();" in source
    assert "return objc3c_frontend_version();" in source
    assert "return objc3c_frontend_version_string();" in source
    assert "new (std::nothrow) objc3c_frontend_c_context_t();" in source
    assert "delete context;" in source
    assert "return objc3c_frontend_compile_file(context, options, result);" in source
    assert "return objc3c_frontend_compile_source(context, options, result);" in source
    assert "return objc3c_frontend_copy_last_error(context, buffer, buffer_size);" in source
    assert "static_assert(std::is_same_v<objc3c_frontend_c_string_t, objc3c_frontend_string_t>" in squashed_source
    assert "static_assert(std::is_same_v<objc3c_frontend_c_stage_summary_t, objc3c_frontend_stage_summary_t>" in squashed_source
    assert "OBJC3C_FRONTEND_C_API_OWNER_RESULT_LIFECYCLE == 1" in source
    assert "OBJC3C_FRONTEND_C_API_POLICY_C_NAMES_OWN_PACKAGE_SURFACE == 8" in source
    assert "objc3c_frontend_c_result_destroy(" in source
    assert "ReleaseCompileResultOwnedStrings(result);" in source
    assert "*result = {};" in source
    assert "objc3c_frontend_c_result_artifact_path(" in source
    assert "switch (artifact_kind)" in source
    assert "case OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS:" in source
    assert "return result->diagnostics_path;" in source
    assert "return nullptr;" in source
    assert "objc3c_frontend_c_result_artifact_path_view(" in source
    assert "objc3c_frontend_c_result_has_artifact(" in source
    assert "return view.data != nullptr && view.size != 0u ? 1u : 0u;" in source
    assert "objc3c_frontend_c_result_error_message(" in source
    assert "return result == nullptr ? nullptr : result->error_message;" in source
    assert "objc3c_frontend_c_result_error_message_view(" in source
    assert "objc3c_frontend_c_string_view(" in source
    assert "if (string == nullptr || string->data == nullptr)" in source
    assert "return {string->data, string->size};" in source
    assert "objc3c_frontend_c_string_release(" in source
    assert "ReleaseOwnedFrontendString(string);" in source
    assert "objc3c_frontend_c_stage_summary_is_well_formed(" in source
    assert "IsDefinedFrontendCApiStage(expected_stage)" in source
    assert "if (summary == nullptr || summary->stage != expected_stage)" in source
    assert "summary->attempted > 1u || summary->skipped > 1u" in source
    assert "severity_total == summary->diagnostics_total ? 1u : 0u" in source
    assert "objc3c_frontend_c_stage_summary_has_diagnostics(" in source
    assert "objc3c_frontend_c_stage_summary_has_errors(" in source


def test_c_api_helper_contract_fixture_tracks_result_error_artifact_stage_helpers() -> None:
    c_api_surface = c_api_surface_text()
    source = c_api_source_text()
    implementation = "\n".join(
        [
            source,
            read_text(RESULT_OWNERSHIP_CPP),
        ]
    )
    contract = json.loads(read_text(C_API_HELPER_CONTRACT))

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

    for helper in contract["required_helpers"]:
        assert f"{helper}(" in c_api_surface
        assert f"{helper}(" in source

    for phrase in contract["required_header_ownership_phrases"]:
        assert phrase in c_api_surface

    for snippet in contract["required_source_fail_closed_snippets"]:
        assert snippet in implementation


def test_frontend_result_ownership_releases_immutable_owned_strings() -> None:
    ownership = read_text(RESULT_OWNERSHIP_CPP)
    compile_contract = read_text(FRONTEND_COMPILE_CONTRACT_CPP)

    assert "CloneOwnedFrontendString(const std::string &text)" in ownership
    assert "string->data = data;" in ownership
    assert "std::free(const_cast<char *>(string->data));" in ownership
    assert "ReleaseCompileResultOwnedStrings(" in ownership
    assert "PopulateCompileResultOwnedPayload(" in ownership
    assert "failed to allocate frontend result-owned string storage." in ownership
    assert "IsMissingFrontendBorrowedPath(" in compile_contract
    assert "BorrowedFrontendPathToFilesystemPath(" in compile_contract
    assert "IsNullOrEmpty(" not in ownership
    assert "IsNullOrEmpty(" not in compile_contract
