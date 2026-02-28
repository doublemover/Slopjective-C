from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PARSER_HEADER = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
PIPELINE_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_parser_module_exists_and_pipeline_uses_api() -> None:
    assert PARSER_HEADER.exists()
    assert PARSER_SOURCE.exists()

    pipeline_cpp = _read(PIPELINE_CPP)
    assert '#include "parse/objc3_parser.h"' in pipeline_cpp
    assert "class Objc3Parser {" not in pipeline_cpp
    assert "Objc3ParseResult parse_result = ParseObjc3Program(tokens);" in pipeline_cpp


def test_cmake_registers_parse_target() -> None:
    cmake = _read(CMAKE_FILE)
    assert "add_library(objc3c_parse STATIC" in cmake
    assert "src/parse/objc3_parser.cpp" in cmake
    assert "target_link_libraries(objc3c-native PRIVATE" in cmake
    assert "objc3c_parse" in cmake
