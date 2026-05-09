from __future__ import annotations

import re

from c_api_smoke_support import (
    ARTIFACT_H,
    C_API_H,
    C_API_HEADER_PATHS,
    CONTEXT_H,
    DIAGNOSTIC_H,
    ERROR_H,
    FRONTEND_H,
    OPTIONS_H,
    RESULT_H,
    SRC_ROOT,
    STRING_H,
    VERSION_H,
    c_api_surface_text,
    read_text,
)


def test_c_api_header_exposes_public_surface() -> None:
    header = read_text(C_API_H)
    c_api_surface = c_api_surface_text()
    frontend_header = read_text(FRONTEND_H)
    options_header = read_text(OPTIONS_H)
    result_header = read_text(RESULT_H)
    string_header = read_text(STRING_H)

    assert "#include \"c_api_types.h\"" in header
    assert "#include \"c_api_contract.h\"" in header
    assert "#include \"c_api_version.h\"" in header
    assert "#include \"c_api_lifecycle.h\"" in header
    assert "#include \"c_api_compile.h\"" in header
    assert "#include \"c_api_result.h\"" in header
    assert "#include \"c_api_string.h\"" in header
    assert "#include \"c_api_stage_summary.h\"" in header
    assert "#include \"objc3c_frontend.h\"" in c_api_surface
    assert "#include \"objc3c_frontend_options.h\"" in frontend_header
    assert "#define OBJC3C_FRONTEND_LANGUAGE_VERSION_OBJECTIVE_C_3 3u" in options_header
    assert "#define OBJC3C_FRONTEND_LANGUAGE_VERSION_DEFAULT OBJC3C_FRONTEND_LANGUAGE_VERSION_OBJECTIVE_C_3" in options_header
    assert "typedef const char *objc3c_frontend_borrowed_c_string_t;" in options_header
    assert "typedef objc3c_frontend_borrowed_c_string_t objc3c_frontend_borrowed_path_t;" in options_header
    assert "typedef objc3c_frontend_borrowed_c_string_t objc3c_frontend_borrowed_text_t;" in options_header
    assert "OBJC3C_FRONTEND_COMPATIBILITY_MODE" not in options_header
    assert "objc3c_frontend_borrowed_path_t input_path;" in options_header
    assert "objc3c_frontend_borrowed_text_t source_text;" in options_header
    assert "objc3c_frontend_borrowed_path_t out_dir;" in options_header
    assert "objc3c_frontend_borrowed_path_t clang_path;" in options_header
    assert "objc3c_frontend_borrowed_path_t llc_path;" in options_header
    assert not re.search(r"const char \*.*path", options_header)
    assert "uint8_t language_version;" in options_header
    assert "uint8_t compatibility_mode;" not in options_header
    assert "uint8_t migration_assist;" not in options_header
    assert "uint8_t reserved1;" in options_header
    assert "uint8_t reserved2;" in options_header
    assert "Borrowed option values are caller-owned storage for the duration of the call." in options_header
    assert "#define OBJC3C_FRONTEND_C_API_ABI_VERSION OBJC3C_FRONTEND_ABI_VERSION" in c_api_surface
    assert "typedef objc3c_frontend_context_t objc3c_frontend_c_context_t;" in c_api_surface
    assert "typedef objc3c_frontend_compile_options_t\n    objc3c_frontend_c_compile_options_t;" in c_api_surface
    assert "typedef objc3c_frontend_compile_result_t objc3c_frontend_c_compile_result_t;" in c_api_surface
    assert "typedef objc3c_frontend_stage_id_t objc3c_frontend_c_stage_id_t;" in c_api_surface
    assert "typedef objc3c_frontend_artifact_kind_t objc3c_frontend_c_artifact_kind_t;" in c_api_surface
    assert "typedef objc3c_frontend_string_t objc3c_frontend_c_string_t;" in c_api_surface
    assert "typedef objc3c_frontend_string_view_t objc3c_frontend_c_string_view_t;" in c_api_surface
    assert "typedef objc3c_frontend_stage_summary_t objc3c_frontend_c_stage_summary_t;" in c_api_surface
    assert "#define OBJC3C_FRONTEND_C_API_CONTRACT_ID" in c_api_surface
    assert "objc3c_frontend_c_api_owner_t" in c_api_surface
    assert "objc3c_frontend_c_api_policy_t" in c_api_surface
    assert "OBJC3C_FRONTEND_C_API_RESULT_LIFECYCLE_OWNER owns result destruction" in c_api_surface
    assert "OBJC3C_FRONTEND_C_API_STRING_OWNER owns standalone string release/view" in c_api_surface
    assert "OBJC3C_FRONTEND_C_API_DIAGNOSTICS_OWNER owns diagnostics artifact access" in c_api_surface
    assert "OBJC3C_FRONTEND_C_API_ARTIFACT_OWNER owns artifact selectors" in c_api_surface
    assert "OBJC3C_FRONTEND_C_API_STAGE_SUMMARY_OWNER owns stage summary predicates" in c_api_surface
    assert "OBJC3C_FRONTEND_C_API_NULL_INVALID_INPUT_OWNER owns fail-closed NULL" in c_api_surface
    assert "OBJC3C_FRONTEND_C_API_ABI_VERSION_OWNER owns exact ABI version gating" in c_api_surface
    assert "OBJC3C_FRONTEND_C_API_PUBLIC_PRIVATE_PARTITION_OWNER owns the rule" in c_api_surface

    assert "uint32_t objc3c_frontend_c_api_abi_version(void);" in c_api_surface
    assert "uint8_t objc3c_frontend_c_is_abi_compatible(" in c_api_surface
    assert "objc3c_frontend_c_status_t objc3c_frontend_c_compile_file(" in c_api_surface
    assert "objc3c_frontend_c_status_t\nobjc3c_frontend_c_compile_source(" in c_api_surface
    assert "size_t objc3c_frontend_c_copy_last_error(" in c_api_surface
    assert "void objc3c_frontend_c_result_destroy(" in c_api_surface
    assert "objc3c_frontend_c_result_artifact_path(" in c_api_surface
    assert "objc3c_frontend_c_result_artifact_path_view(" in c_api_surface
    assert "objc3c_frontend_c_result_has_artifact(" in c_api_surface
    assert "objc3c_frontend_c_result_error_message(" in c_api_surface
    assert "objc3c_frontend_c_result_error_message_view(" in c_api_surface
    assert "objc3c_frontend_c_string_view(" in c_api_surface
    assert "void objc3c_frontend_c_string_release(" in c_api_surface
    assert "objc3c_frontend_c_stage_summary_is_well_formed(" in c_api_surface
    assert "objc3c_frontend_c_stage_summary_has_diagnostics(" in c_api_surface
    assert "objc3c_frontend_c_stage_summary_has_errors(" in c_api_surface
    assert "compile_result storage is caller-owned" in c_api_surface
    assert "result payload strings are released only by objc3c_frontend_c_result_destroy()" in c_api_surface
    assert "undefined artifact kinds and NULL inputs fail closed as NULL/empty/0" in c_api_surface
    assert "objc3c_frontend_string_t *diagnostics_path;" in result_header
    assert "const char *diagnostics_path;" not in result_header
    assert "callers must not release them directly." in result_header
    assert "Returns a borrowed pointer to a result-owned artifact path string." in result_header
    assert "objc3c_frontend_result_destroy(" in result_header
    assert "objc3c_frontend_result_artifact_path(" in result_header
    assert "objc3c_frontend_result_error_message(" in result_header
    assert "Owned immutable string returned by libobjc3c_frontend." in string_header
    assert re.search(r"^\s+const char \*data;$", string_header, re.MULTILINE)
    assert not re.search(r"^\s+char \*data;$", string_header, re.MULTILINE)
    assert "objc3c_frontend_string_release(" in string_header
    assert "objc3c_frontend_string_view(" in string_header


def test_frontend_public_headers_are_domain_owned() -> None:
    old_api_header = SRC_ROOT / "libobjc3c_frontend" / "api.h"
    old_version_header = SRC_ROOT / "libobjc3c_frontend" / "version.h"

    assert not old_api_header.exists()
    assert not old_version_header.exists()

    for header in [
        FRONTEND_H,
        VERSION_H,
        CONTEXT_H,
        OPTIONS_H,
        RESULT_H,
        DIAGNOSTIC_H,
        STRING_H,
        ARTIFACT_H,
        ERROR_H,
        *C_API_HEADER_PATHS,
    ]:
        assert header.exists(), header


def test_public_frontend_surface_has_no_lane_or_roadmap_comments() -> None:
    forbidden_terms = ["lane", "milestone", "roadmap", "reserved future"]
    for path in (SRC_ROOT / "libobjc3c_frontend").glob("**/*"):
        if path.suffix not in {".h", ".cpp", ".inc"}:
            continue
        text = read_text(path).lower()
        for term in forbidden_terms:
            assert term not in text, f"{term!r} remains in {path}"
