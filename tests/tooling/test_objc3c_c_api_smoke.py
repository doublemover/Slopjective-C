from __future__ import annotations

import json
import re
import shutil
import subprocess
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SRC_ROOT = ROOT / "native" / "objc3c" / "src"
FRONTEND_H = SRC_ROOT / "libobjc3c_frontend" / "objc3c_frontend.h"
VERSION_H = SRC_ROOT / "libobjc3c_frontend" / "objc3c_frontend_version.h"
OPTIONS_H = SRC_ROOT / "libobjc3c_frontend" / "objc3c_frontend_options.h"
RESULT_H = SRC_ROOT / "libobjc3c_frontend" / "objc3c_frontend_result.h"
DIAGNOSTIC_H = SRC_ROOT / "libobjc3c_frontend" / "objc3c_frontend_diagnostic.h"
CONTEXT_H = SRC_ROOT / "libobjc3c_frontend" / "objc3c_frontend_context.h"
ERROR_H = SRC_ROOT / "libobjc3c_frontend" / "objc3c_frontend_error.h"
ARTIFACT_H = SRC_ROOT / "libobjc3c_frontend" / "objc3c_frontend_artifact.h"
STRING_H = SRC_ROOT / "libobjc3c_frontend" / "objc3c_frontend_string.h"
C_API_H = SRC_ROOT / "libobjc3c_frontend" / "c_api.h"
C_API_CONTRACT_H = SRC_ROOT / "libobjc3c_frontend" / "c_api_contract.h"
C_API_TYPES_H = SRC_ROOT / "libobjc3c_frontend" / "c_api_types.h"
C_API_VERSION_H = SRC_ROOT / "libobjc3c_frontend" / "c_api_version.h"
C_API_LIFECYCLE_H = SRC_ROOT / "libobjc3c_frontend" / "c_api_lifecycle.h"
C_API_COMPILE_H = SRC_ROOT / "libobjc3c_frontend" / "c_api_compile.h"
C_API_RESULT_H = SRC_ROOT / "libobjc3c_frontend" / "c_api_result.h"
C_API_STRING_H = SRC_ROOT / "libobjc3c_frontend" / "c_api_string.h"
C_API_STAGE_SUMMARY_H = SRC_ROOT / "libobjc3c_frontend" / "c_api_stage_summary.h"
C_API_HEADER_PATHS = [
    C_API_H,
    C_API_CONTRACT_H,
    C_API_TYPES_H,
    C_API_VERSION_H,
    C_API_LIFECYCLE_H,
    C_API_COMPILE_H,
    C_API_RESULT_H,
    C_API_STRING_H,
    C_API_STAGE_SUMMARY_H,
]
RESULT_OWNERSHIP_CPP = SRC_ROOT / "libobjc3c_frontend" / "objc3c_frontend_result_ownership.cpp"
FRONTEND_COMPILE_CONTRACT_CPP = SRC_ROOT / "libobjc3c_frontend" / "objc3c_frontend_compile_contract.cpp"
C_API_SOURCES = [
    SRC_ROOT / "libobjc3c_frontend" / "c_api_abi.cpp",
    SRC_ROOT / "libobjc3c_frontend" / "c_api_compile.cpp",
    SRC_ROOT / "libobjc3c_frontend" / "c_api_lifecycle.cpp",
    SRC_ROOT / "libobjc3c_frontend" / "c_api_result_artifacts.cpp",
    SRC_ROOT / "libobjc3c_frontend" / "c_api_result_error.cpp",
    SRC_ROOT / "libobjc3c_frontend" / "c_api_result_lifecycle.cpp",
    SRC_ROOT / "libobjc3c_frontend" / "c_api_stage_summary.cpp",
    SRC_ROOT / "libobjc3c_frontend" / "c_api_string_bridge.cpp",
]
C_API_HELPER_CONTRACT = ROOT / "tests" / "tooling" / "fixtures" / "native" / "frontend_c_api_helper_contract.json"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _c_api_surface_text() -> str:
    return "\n".join(_read(path) for path in C_API_HEADER_PATHS)


def _squash_ws(text: str) -> str:
    return re.sub(r"\s+", " ", text)


def _find_compiler(candidates: list[str]) -> str | None:
    for candidate in candidates:
        resolved = shutil.which(candidate)
        if resolved:
            return resolved
    return None


def test_c_api_header_exposes_wrapper_surface() -> None:
    header = _read(C_API_H)
    c_api_surface = _c_api_surface_text()
    frontend_header = _read(FRONTEND_H)
    options_header = _read(OPTIONS_H)
    result_header = _read(RESULT_H)
    string_header = _read(STRING_H)

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
        text = _read(path).lower()
        for term in forbidden_terms:
            assert term not in text, f"{term!r} remains in {path}"


def test_c_api_cpp_pins_core_compile_and_c_only_helper_owners() -> None:
    for source_path in C_API_SOURCES:
        assert source_path.exists(), source_path

    source = "\n".join(_read(source_path) for source_path in C_API_SOURCES)
    squashed_source = _squash_ws(source)

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
    c_api_surface = _c_api_surface_text()
    source = "\n".join(_read(source_path) for source_path in C_API_SOURCES)
    implementation = "\n".join(
        [
            source,
            _read(RESULT_OWNERSHIP_CPP),
        ]
    )
    contract = json.loads(_read(C_API_HELPER_CONTRACT))

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
    ownership = _read(RESULT_OWNERSHIP_CPP)
    compile_contract = _read(FRONTEND_COMPILE_CONTRACT_CPP)

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


def test_c_api_header_compiles_from_c_when_compiler_available(tmp_path: Path) -> None:
    c_compiler = _find_compiler(["cc", "clang", "gcc"])
    if c_compiler is None:
        pytest.skip("no C compiler available in PATH")

    c_source = tmp_path / "c_api_embed_smoke.c"
    c_object = tmp_path / "c_api_embed_smoke.o"
    c_source.write_text(
        "\n".join(
            [
                '#include "libobjc3c_frontend/c_api.h"',
                "static int smoke(void) {",
                "  objc3c_frontend_c_compile_result_t result = {0};",
                "  objc3c_frontend_c_string_view_t empty_error =",
                "      objc3c_frontend_c_result_error_message_view(&result);",
                "  objc3c_frontend_c_string_view_t missing_artifact =",
                "      objc3c_frontend_c_result_artifact_path_view(",
                "          &result, OBJC3C_FRONTEND_ARTIFACT_OBJECT);",
                "  objc3c_frontend_c_stage_summary_t lex = {0};",
                "  lex.stage = OBJC3C_FRONTEND_STAGE_LEX;",
                "  lex.skipped = 1u;",
                "  if (empty_error.data != 0 || empty_error.size != 0u) return 2;",
                "  if (missing_artifact.data != 0 || missing_artifact.size != 0u) return 3;",
                "  if (objc3c_frontend_c_result_has_artifact(",
                "          &result, OBJC3C_FRONTEND_ARTIFACT_OBJECT) != 0u) return 4;",
                "  if (objc3c_frontend_c_stage_summary_is_well_formed(",
                "          &lex, OBJC3C_FRONTEND_STAGE_LEX) == 0u) return 5;",
                "  if (objc3c_frontend_c_stage_summary_has_diagnostics(&lex) != 0u) return 6;",
                "  if (objc3c_frontend_c_stage_summary_has_errors(&lex) != 0u) return 7;",
                "  objc3c_frontend_c_result_destroy(&result);",
                "  objc3c_frontend_c_string_release(0);",
                "  return (int)objc3c_frontend_c_api_abi_version();",
                "}",
                "",
            ]
        ),
        encoding="utf-8",
    )

    command = [
        c_compiler,
        "-std=c11",
        "-c",
        str(c_source),
        "-I",
        str(SRC_ROOT),
        "-o",
        str(c_object),
    ]
    result = subprocess.run(command, capture_output=True, text=True, check=False)
    if result.returncode != 0:
        diagnostics = (result.stderr + "\n" + result.stdout).strip()
        if not diagnostics:
            pytest.skip("C compiler invocation is unavailable in this environment")
        pytest.fail(diagnostics)
    assert c_object.exists()


def test_c_api_split_owner_sources_compile_when_cxx_compiler_available(tmp_path: Path) -> None:
    cxx_compiler = _find_compiler(["c++", "clang++", "g++"])
    if cxx_compiler is None:
        pytest.skip("no C++ compiler available in PATH")

    for source_path in C_API_SOURCES:
        cxx_object = tmp_path / f"{source_path.stem}.o"
        command = [
            cxx_compiler,
            "-std=c++20",
            "-c",
            str(source_path),
            "-I",
            str(SRC_ROOT),
            "-o",
            str(cxx_object),
        ]
        result = subprocess.run(command, capture_output=True, text=True, check=False)
        if result.returncode != 0:
            diagnostics = (result.stderr + "\n" + result.stdout).strip()
            if not diagnostics:
                pytest.skip("C++ compiler invocation is unavailable in this environment")
            pytest.fail(diagnostics)
        assert cxx_object.exists()
