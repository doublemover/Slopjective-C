from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DRIVER_HEADER = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.h"
DRIVER_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.cpp"
MAIN_CPP = ROOT / "native" / "objc3c" / "src" / "main.cpp"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_driver_cli_module_exists_and_main_calls_it() -> None:
    assert DRIVER_HEADER.exists()
    assert DRIVER_SOURCE.exists()
    main_cpp = _read(MAIN_CPP)
    assert '#include "driver/objc3_cli_options.h"' in main_cpp
    assert "ParseObjc3CliOptions(argc, argv, cli_options, cli_error)" in main_cpp
    assert "for (int i = 2; i < argc; ++i)" not in main_cpp


def test_cmake_registers_driver_target() -> None:
    cmake = _read(CMAKE_FILE)
    assert "add_library(objc3c_driver STATIC" in cmake
    assert "src/driver/objc3_cli_options.cpp" in cmake
    assert "objc3c_driver" in cmake
