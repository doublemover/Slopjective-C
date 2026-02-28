from __future__ import annotations

import shutil
import subprocess
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SRC_ROOT = ROOT / "native" / "objc3c" / "src"
API_H = SRC_ROOT / "libobjc3c_frontend" / "api.h"
C_API_H = SRC_ROOT / "libobjc3c_frontend" / "c_api.h"
C_API_CPP = SRC_ROOT / "libobjc3c_frontend" / "c_api.cpp"


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
    api_header = _read(API_H)

    assert "#include \"api.h\"" in header
    assert "#define OBJC3C_FRONTEND_LANGUAGE_VERSION_OBJECTIVE_C_3 3u" in api_header
    assert "#define OBJC3C_FRONTEND_LANGUAGE_VERSION_DEFAULT OBJC3C_FRONTEND_LANGUAGE_VERSION_OBJECTIVE_C_3" in api_header
    assert "#define OBJC3C_FRONTEND_COMPATIBILITY_MODE_CANONICAL 0u" in api_header
    assert "#define OBJC3C_FRONTEND_COMPATIBILITY_MODE_LEGACY 1u" in api_header
    assert (
        "#define OBJC3C_FRONTEND_COMPATIBILITY_MODE_DEFAULT "
        "OBJC3C_FRONTEND_COMPATIBILITY_MODE_CANONICAL"
    ) in api_header
    assert "uint8_t language_version;" in api_header
    assert "uint8_t compatibility_mode;" in api_header
    assert "uint8_t migration_assist;" in api_header
    assert "#define OBJC3C_FRONTEND_C_API_ABI_VERSION 1u" in header
    assert "typedef objc3c_frontend_context_t objc3c_frontend_c_context_t;" in header
    assert "typedef objc3c_frontend_compile_options_t objc3c_frontend_c_compile_options_t;" in header
    assert "typedef objc3c_frontend_compile_result_t objc3c_frontend_c_compile_result_t;" in header

    assert "uint32_t objc3c_frontend_c_api_abi_version(void);" in header
    assert "uint8_t objc3c_frontend_c_is_abi_compatible(" in header
    assert "objc3c_frontend_c_status_t objc3c_frontend_c_compile_file(" in header
    assert "objc3c_frontend_c_status_t objc3c_frontend_c_compile_source(" in header
    assert "size_t objc3c_frontend_c_copy_last_error(" in header


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
