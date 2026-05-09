from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PARSER_HEADER = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.h"
AST_BUILDER_CONTRACT_SOURCE = (
    ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_ast_builder_contract.cpp"
)
PARSER_CORE_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser_core.cpp"
PIPELINE_STAGE_RUNNER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "frontend_pipeline_stage_runner.cpp"
PARSE_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "parse" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_parser_module_exists_and_pipeline_uses_api() -> None:
    assert PARSER_HEADER.exists()
    assert AST_BUILDER_CONTRACT_SOURCE.exists()
    assert PARSER_CORE_SOURCE.exists()

    pipeline_cpp = _read(PIPELINE_STAGE_RUNNER)
    assert '#include "parse/objc3_ast_builder_contract.h"' in pipeline_cpp
    assert '#include "parse/objc3_parser.h"' not in pipeline_cpp
    assert "class Objc3Parser {" not in pipeline_cpp
    assert "Objc3AstBuilderResult parse_result = BuildObjc3AstFromTokens(tokens);" in pipeline_cpp
    assert "result.program = std::move(parse_result.program);" in pipeline_cpp


def test_cmake_registers_parse_target() -> None:
    cmake = _read(PARSE_CMAKE_FILE)
    assert "add_library(objc3c_parse STATIC" in cmake
    assert "objc3_ast_builder_contract.cpp" in cmake
    assert "objc3_parser_core.cpp" in cmake
    assert "objc3_parser_declaration_surface.cpp" in cmake
    assert "objc3_parser_expression_surface.cpp" in cmake
    assert "objc3_parser_statement_surface.cpp" in cmake
    assert "target_link_libraries(objc3c_parse PUBLIC" in cmake
    assert "objc3c_parse" in cmake
