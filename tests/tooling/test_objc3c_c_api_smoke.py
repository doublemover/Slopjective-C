from __future__ import annotations

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
C_API_CPP = SRC_ROOT / "libobjc3c_frontend" / "c_api.cpp"
ANCHOR_PART_001 = SRC_ROOT / "libobjc3c_frontend" / "frontend_anchor_parts" / "frontend_anchor_part_001.inc"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _find_compiler(candidates: list[str]) -> str | None:
    for candidate in candidates:
        resolved = shutil.which(candidate)
        if resolved:
            return resolved
    return None


def test_c_api_header_exposes_wrapper_surface() -> None:
    header = _read(C_API_H)
    frontend_header = _read(FRONTEND_H)
    options_header = _read(OPTIONS_H)
    result_header = _read(RESULT_H)
    string_header = _read(STRING_H)

    assert "#include \"objc3c_frontend.h\"" in header
    assert "#include \"objc3c_frontend_options.h\"" in frontend_header
    assert "#define OBJC3C_FRONTEND_LANGUAGE_VERSION_OBJECTIVE_C_3 3u" in options_header
    assert "#define OBJC3C_FRONTEND_LANGUAGE_VERSION_DEFAULT OBJC3C_FRONTEND_LANGUAGE_VERSION_OBJECTIVE_C_3" in options_header
    assert "OBJC3C_FRONTEND_COMPATIBILITY_MODE" not in options_header
    assert "uint8_t language_version;" in options_header
    assert "uint8_t compatibility_mode;" not in options_header
    assert "uint8_t migration_assist;" not in options_header
    assert "uint8_t reserved1;" in options_header
    assert "uint8_t reserved2;" in options_header
    assert "const char * fields are borrowed caller storage for the duration of the call." in options_header
    assert "#define OBJC3C_FRONTEND_C_API_ABI_VERSION 1u" in header
    assert "typedef objc3c_frontend_context_t objc3c_frontend_c_context_t;" in header
    assert "typedef objc3c_frontend_compile_options_t objc3c_frontend_c_compile_options_t;" in header
    assert "typedef objc3c_frontend_compile_result_t objc3c_frontend_c_compile_result_t;" in header

    assert "uint32_t objc3c_frontend_c_api_abi_version(void);" in header
    assert "uint8_t objc3c_frontend_c_is_abi_compatible(" in header
    assert "objc3c_frontend_c_status_t objc3c_frontend_c_compile_file(" in header
    assert "objc3c_frontend_c_status_t objc3c_frontend_c_compile_source(" in header
    assert "size_t objc3c_frontend_c_copy_last_error(" in header
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


def test_c_api_cpp_delegates_to_core_frontend_api() -> None:
    source = _read(C_API_CPP)

    assert '#include "libobjc3c_frontend/c_api.h"' in source
    assert "return objc3c_frontend_is_abi_compatible(requested_abi_version);" in source
    assert "return objc3c_frontend_abi_version();" in source
    assert "return objc3c_frontend_version();" in source
    assert "return objc3c_frontend_version_string();" in source
    assert "return objc3c_frontend_context_create();" in source
    assert "objc3c_frontend_context_destroy(context);" in source
    assert "return objc3c_frontend_compile_file(context, options, result);" in source
    assert "return objc3c_frontend_compile_source(context, options, result);" in source
    assert "return objc3c_frontend_copy_last_error(context, buffer, buffer_size);" in source


def test_frontend_anchor_releases_immutable_owned_strings() -> None:
    source = _read(ANCHOR_PART_001)

    assert "string->data = data;" in source
    assert "std::free(const_cast<char *>(string->data));" in source
    assert "Copy context strings into result-owned immutable string objects." in source


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


def test_c_api_cpp_compiles_when_cxx_compiler_available(tmp_path: Path) -> None:
    cxx_compiler = _find_compiler(["c++", "clang++", "g++"])
    if cxx_compiler is None:
        pytest.skip("no C++ compiler available in PATH")

    cxx_object = tmp_path / "c_api_wrapper.o"
    command = [
        cxx_compiler,
        "-std=c++20",
        "-c",
        str(C_API_CPP),
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
