from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
MAIN_CPP = ROOT / "native" / "objc3c" / "src" / "main.cpp"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_lowering_contract_module_is_wired() -> None:
    assert HEADER.exists()
    assert SOURCE.exists()
    assert '#include "lower/objc3_lowering_contract.h"' in _read(MAIN_CPP)


def test_cmake_links_lowering_contract_target() -> None:
    cmake = _read(CMAKE_FILE)
    assert "add_library(objc3c_lower STATIC" in cmake
    assert "src/lower/objc3_lowering_contract.cpp" in cmake
    assert "target_link_libraries(objc3c-native PRIVATE" in cmake
    assert "objc3c_lower" in cmake
