from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
IO_HEADER = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.h"
IO_SOURCE = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
MAIN_CPP = ROOT / "native" / "objc3c" / "src" / "main.cpp"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_io_process_module_exists_and_main_uses_it() -> None:
    assert IO_HEADER.exists()
    assert IO_SOURCE.exists()
    main_cpp = _read(MAIN_CPP)
    assert '#include "io/objc3_process.h"' in main_cpp
    assert "static int RunProcess(" not in main_cpp
    assert "static int RunObjectiveCCompile(" not in main_cpp
    assert "static int RunIRCompile(" not in main_cpp


def test_cmake_registers_io_target() -> None:
    cmake = _read(CMAKE_FILE)
    assert "add_library(objc3c_io STATIC" in cmake
    assert "src/io/objc3_process.cpp" in cmake
    assert "target_link_libraries(objc3c-native PRIVATE objc3c_lex objc3c_lower objc3c_ir objc3c_io objc3c_frontend)" in cmake
