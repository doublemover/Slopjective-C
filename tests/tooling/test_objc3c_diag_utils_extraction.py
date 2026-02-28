from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HEADER = ROOT / "native" / "objc3c" / "src" / "diag" / "objc3_diag_utils.h"
SOURCE = ROOT / "native" / "objc3c" / "src" / "diag" / "objc3_diag_utils.cpp"
MAIN_CPP = ROOT / "native" / "objc3c" / "src" / "main.cpp"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_diag_utils_module_exists_and_main_uses_it() -> None:
    assert HEADER.exists()
    assert SOURCE.exists()
    main_cpp = _read(MAIN_CPP)
    assert '#include "diag/objc3_diag_utils.h"' in main_cpp
    assert "static DiagSortKey ParseDiagSortKey(" not in main_cpp
    assert "static void NormalizeDiagnostics(" not in main_cpp


def test_cmake_registers_diag_target() -> None:
    cmake = _read(CMAKE_FILE)
    assert "add_library(objc3c_diag STATIC" in cmake
    assert "src/diag/objc3_diag_utils.cpp" in cmake
    assert "objc3c_diag" in cmake
