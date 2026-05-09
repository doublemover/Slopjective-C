from __future__ import annotations

import subprocess
from pathlib import Path

import pytest

from c_api_smoke_support import C_API_SOURCES, SRC_ROOT, find_compiler


def test_c_api_header_compiles_from_c_when_compiler_available(tmp_path: Path) -> None:
    c_compiler = find_compiler(["cc", "clang", "gcc"])
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


def test_c_api_split_owner_sources_compile_when_cxx_compiler_available(
    tmp_path: Path,
) -> None:
    cxx_compiler = find_compiler(["c++", "clang++", "g++"])
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
