from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HEADER = ROOT / "native" / "objc3c" / "src" / "diag" / "objc3_diag_utils.h"
SOURCE = ROOT / "native" / "objc3c" / "src" / "diag" / "objc3_diag_utils.cpp"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_compilation_driver.cpp"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_diag_utils_module_exists_and_driver_uses_it() -> None:
    assert HEADER.exists()
    assert SOURCE.exists()
    driver_cpp = _read(DRIVER_CPP)
    assert '#include "diag/objc3_diag_utils.h"' in driver_cpp
    assert "static DiagSortKey ParseDiagSortKey(" not in driver_cpp
    assert "static void NormalizeDiagnostics(" not in driver_cpp


def test_cmake_registers_diag_target() -> None:
    cmake = _read(CMAKE_FILE)
    assert "add_library(objc3c_diag STATIC" in cmake
    assert "src/diag/objc3_diag_utils.cpp" in cmake
    assert "objc3c_diag" in cmake
