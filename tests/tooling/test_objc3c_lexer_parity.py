from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
LEX_HEADER = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.h"
LEX_SOURCE = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
MAIN_CPP = ROOT / "native" / "objc3c" / "src" / "main.cpp"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_lexer_module_exists_and_main_consumes_it() -> None:
    assert LEX_HEADER.exists(), f"missing lexer header: {LEX_HEADER}"
    assert LEX_SOURCE.exists(), f"missing lexer source: {LEX_SOURCE}"

    main_cpp = read_text(MAIN_CPP)
    assert '#include "lex/objc3_lexer.h"' in main_cpp
    assert "class Objc3Lexer {" not in main_cpp


def test_cmake_registers_lexer_target() -> None:
    cmake = read_text(CMAKE_FILE)
    assert "add_library(objc3c_lex STATIC" in cmake
    assert "src/lex/objc3_lexer.cpp" in cmake
