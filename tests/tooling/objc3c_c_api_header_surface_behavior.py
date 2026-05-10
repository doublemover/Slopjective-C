from __future__ import annotations

import re

from c_api_smoke_support import read_text
from objc3c_c_api_header_surface_assertions import (
    assert_contains_all,
    assert_excludes_all,
    assert_forbidden_terms_absent,
    assert_paths_absent,
    assert_paths_exist,
    assert_regex_absent,
    assert_regex_present,
)
from objc3c_c_api_header_surface_sources import (
    CApiHeaderSurface,
    public_frontend_header_paths,
    public_frontend_surface_paths,
    removed_domain_headers,
)


def assert_c_api_header_exposes_public_surface(surface: CApiHeaderSurface) -> None:
    expected_default_language_version = (
        "#define OBJC3C_FRONTEND_LANGUAGE_VERSION_DEFAULT "
        "OBJC3C_FRONTEND_LANGUAGE_VERSION_OBJECTIVE_C_3"
    )

    assert_contains_all(
        surface.header,
        [
            '#include "c_api_types.h"',
            '#include "c_api_contract.h"',
            '#include "c_api_version.h"',
            '#include "c_api_lifecycle.h"',
            '#include "c_api_compile.h"',
            '#include "c_api_result.h"',
            '#include "c_api_string.h"',
            '#include "c_api_stage_summary.h"',
        ],
    )
    assert_contains_all(
        surface.c_api_surface,
        [
            '#include "objc3c_frontend.h"',
            "#define OBJC3C_FRONTEND_C_API_ABI_VERSION OBJC3C_FRONTEND_ABI_VERSION",
            "typedef objc3c_frontend_context_t objc3c_frontend_c_context_t;",
            (
                "typedef objc3c_frontend_compile_options_t\n"
                "    objc3c_frontend_c_compile_options_t;"
            ),
            (
                "typedef objc3c_frontend_compile_result_t "
                "objc3c_frontend_c_compile_result_t;"
            ),
            "typedef objc3c_frontend_stage_id_t objc3c_frontend_c_stage_id_t;",
            (
                "typedef objc3c_frontend_artifact_kind_t "
                "objc3c_frontend_c_artifact_kind_t;"
            ),
            "typedef objc3c_frontend_string_t objc3c_frontend_c_string_t;",
            (
                "typedef objc3c_frontend_string_view_t "
                "objc3c_frontend_c_string_view_t;"
            ),
            (
                "typedef objc3c_frontend_stage_summary_t "
                "objc3c_frontend_c_stage_summary_t;"
            ),
            "#define OBJC3C_FRONTEND_C_API_CONTRACT_ID",
            "objc3c_frontend_c_api_owner_t",
            "objc3c_frontend_c_api_policy_t",
            "OBJC3C_FRONTEND_C_API_RESULT_LIFECYCLE_OWNER owns result destruction",
            "OBJC3C_FRONTEND_C_API_STRING_OWNER owns standalone string release/view",
            "OBJC3C_FRONTEND_C_API_DIAGNOSTICS_OWNER owns diagnostics artifact access",
            "OBJC3C_FRONTEND_C_API_ARTIFACT_OWNER owns artifact selectors",
            "OBJC3C_FRONTEND_C_API_STAGE_SUMMARY_OWNER owns stage summary predicates",
            "OBJC3C_FRONTEND_C_API_NULL_INVALID_INPUT_OWNER owns fail-closed NULL",
            "OBJC3C_FRONTEND_C_API_ABI_VERSION_OWNER owns exact ABI version gating",
            "OBJC3C_FRONTEND_C_API_PUBLIC_PRIVATE_PARTITION_OWNER owns the rule",
            "uint32_t objc3c_frontend_c_api_abi_version(void);",
            "uint8_t objc3c_frontend_c_is_abi_compatible(",
            "objc3c_frontend_c_status_t objc3c_frontend_c_compile_file(",
            "objc3c_frontend_c_status_t\nobjc3c_frontend_c_compile_source(",
            "size_t objc3c_frontend_c_copy_last_error(",
            "void objc3c_frontend_c_result_destroy(",
            "objc3c_frontend_c_result_artifact_path(",
            "objc3c_frontend_c_result_artifact_path_view(",
            "objc3c_frontend_c_result_has_artifact(",
            "objc3c_frontend_c_result_error_message(",
            "objc3c_frontend_c_result_error_message_view(",
            "objc3c_frontend_c_string_view(",
            "void objc3c_frontend_c_string_release(",
            "objc3c_frontend_c_stage_summary_is_well_formed(",
            "objc3c_frontend_c_stage_summary_has_diagnostics(",
            "objc3c_frontend_c_stage_summary_has_errors(",
            "compile_result storage is caller-owned",
            (
                "result payload strings are released only by "
                "objc3c_frontend_c_result_destroy()"
            ),
            "undefined artifact kinds and NULL inputs fail closed as NULL/empty/0",
        ],
    )
    assert_contains_all(
        surface.frontend_header,
        ['#include "objc3c_frontend_options.h"'],
    )
    assert_contains_all(
        surface.options_header,
        [
            "#define OBJC3C_FRONTEND_LANGUAGE_VERSION_OBJECTIVE_C_3 3u",
            expected_default_language_version,
            "typedef const char *objc3c_frontend_borrowed_c_string_t;",
            (
                "typedef objc3c_frontend_borrowed_c_string_t "
                "objc3c_frontend_borrowed_path_t;"
            ),
            (
                "typedef objc3c_frontend_borrowed_c_string_t "
                "objc3c_frontend_borrowed_text_t;"
            ),
            "objc3c_frontend_borrowed_path_t input_path;",
            "objc3c_frontend_borrowed_text_t source_text;",
            "objc3c_frontend_borrowed_path_t out_dir;",
            "objc3c_frontend_borrowed_path_t clang_path;",
            "objc3c_frontend_borrowed_path_t llc_path;",
            "uint8_t language_version;",
            "uint8_t reserved1;",
            "uint8_t reserved2;",
            (
                "Borrowed option values are caller-owned storage for the duration "
                "of the call."
            ),
        ],
    )
    assert_excludes_all(
        surface.options_header,
        [
            "OBJC3C_FRONTEND_RETIRED_MODE",
            "uint8_t retired_mode;",
            "uint8_t retired_mode_assist;",
        ],
    )
    assert_regex_absent(r"const char \*.*path", surface.options_header)

    assert_contains_all(
        surface.result_header,
        [
            "objc3c_frontend_string_t *diagnostics_path;",
            "callers must not release them directly.",
            "Returns a borrowed pointer to a result-owned artifact path string.",
            "objc3c_frontend_result_destroy(",
            "objc3c_frontend_result_artifact_path(",
            "objc3c_frontend_result_error_message(",
        ],
    )
    assert "const char *diagnostics_path;" not in surface.result_header

    assert_contains_all(
        surface.string_header,
        [
            "Owned immutable string returned by libobjc3c_frontend.",
            "objc3c_frontend_string_release(",
            "objc3c_frontend_string_view(",
        ],
    )
    assert_regex_present(r"^\s+const char \*data;$", surface.string_header, re.MULTILINE)
    assert_regex_absent(r"^\s+char \*data;$", surface.string_header, re.MULTILINE)


def assert_frontend_public_headers_are_domain_owned() -> None:
    assert_paths_absent(removed_domain_headers())
    assert_paths_exist(public_frontend_header_paths())


def assert_public_frontend_surface_has_no_lane_or_roadmap_comments() -> None:
    forbidden_terms = ["lane", "milestone", "roadmap", "reserved future"]
    for path in public_frontend_surface_paths():
        assert_forbidden_terms_absent(
            path,
            read_text(path).lower(),
            forbidden_terms,
        )
