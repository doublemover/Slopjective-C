from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
STATIC_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_static_analysis.h"
STATIC_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_static_analysis.cpp"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_static_analysis_module_exists_and_sema_passes_uses_it() -> None:
    assert STATIC_HEADER.exists()
    assert STATIC_SOURCE.exists()

    sema_passes = _read(SEMA_PASSES)
    assert '#include "sema/objc3_static_analysis.h"' in sema_passes
    assert "static bool TryEvalStaticScalarValue(" not in sema_passes
    assert "static bool StatementAlwaysReturns(" not in sema_passes
    assert "static bool BlockAlwaysReturns(" not in sema_passes


def test_cmake_registers_static_analysis_source() -> None:
    cmake = _read(CMAKE_FILE)
    assert "add_library(objc3c_sema STATIC" in cmake
    assert "src/sema/objc3_static_analysis.cpp" in cmake
