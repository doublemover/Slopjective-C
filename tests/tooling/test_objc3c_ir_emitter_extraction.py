from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
IR_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
MAIN_CPP = ROOT / "native" / "objc3c" / "src" / "main.cpp"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_ir_emitter_module_exists_and_main_uses_api() -> None:
    assert IR_HEADER.exists()
    assert IR_SOURCE.exists()

    main_cpp = _read(MAIN_CPP)
    assert '#include "ir/objc3_ir_emitter.h"' in main_cpp
    assert "class Objc3IREmitter {" not in main_cpp
    assert "EmitObjc3IRText(program, lowering_contract, ir, error)" in main_cpp


def test_cmake_registers_ir_target() -> None:
    cmake = _read(CMAKE_FILE)
    assert "add_library(objc3c_ir STATIC" in cmake
    assert "src/ir/objc3_ir_emitter.cpp" in cmake
    assert "target_link_libraries(objc3c-native PRIVATE objc3c_lex objc3c_lower objc3c_ir objc3c_frontend)" in cmake
