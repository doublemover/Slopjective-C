from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
PIPELINE_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_lowering_contract_module_is_wired() -> None:
    assert HEADER.exists()
    assert SOURCE.exists()
    assert '#include "lower/objc3_lowering_contract.h"' in _read(PIPELINE_TYPES)
    header = _read(HEADER)
    source = _read(SOURCE)
    assert "TryGetCompoundAssignmentBinaryOpcode" in header
    assert '#include "lex/objc3_lexer.h"' not in header
    assert '#include "lex/objc3_lexer.h"' not in source
    assert '#include "parse/objc3_parser.h"' not in header
    assert '#include "parse/objc3_parser.h"' not in source
    assert '#include "parse/objc3_parser_contract.h"' not in header
    assert '#include "parse/objc3_parser_contract.h"' not in source
    assert '#include "token/objc3_token.h"' not in header
    assert '#include "token/objc3_token.h"' not in source


def test_cmake_links_lowering_contract_target() -> None:
    cmake = _read(CMAKE_FILE)
    assert "add_library(objc3c_lower STATIC" in cmake
    assert "src/lower/objc3_lowering_contract.cpp" in cmake
    assert "target_link_libraries(objc3c-native PRIVATE" in cmake
    assert "objc3c_lower" in cmake
