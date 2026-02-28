from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
IO_HEADER = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.h"
IO_SOURCE = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
FILE_IO_HEADER = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_file_io.h"
FILE_IO_SOURCE = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_file_io.cpp"
DIAG_HEADER = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_diagnostics_artifacts.h"
DIAG_SOURCE = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_diagnostics_artifacts.cpp"
MANIFEST_HEADER = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.h"
MANIFEST_SOURCE = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
MAIN_CPP = ROOT / "native" / "objc3c" / "src" / "main.cpp"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_compilation_driver.cpp"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_io_process_module_exists_and_main_uses_it() -> None:
    assert IO_HEADER.exists()
    assert IO_SOURCE.exists()
    assert FILE_IO_HEADER.exists()
    assert FILE_IO_SOURCE.exists()
    assert DIAG_HEADER.exists()
    assert DIAG_SOURCE.exists()
    assert MANIFEST_HEADER.exists()
    assert MANIFEST_SOURCE.exists()
    main_cpp = _read(MAIN_CPP)
    assert '#include "io/objc3_diagnostics_artifacts.h"' not in main_cpp
    assert '#include "io/objc3_file_io.h"' not in main_cpp
    assert '#include "io/objc3_process.h"' not in main_cpp
    assert "static int RunProcess(" not in main_cpp
    assert "static int RunObjectiveCCompile(" not in main_cpp
    assert "static int RunIRCompile(" not in main_cpp
    assert "static void WriteDiagnosticsArtifacts(" not in main_cpp

    driver_cpp = _read(DRIVER_CPP)
    assert '#include "io/objc3_diagnostics_artifacts.h"' in driver_cpp
    assert '#include "io/objc3_file_io.h"' in driver_cpp
    assert '#include "io/objc3_process.h"' in driver_cpp
    assert '#include "io/objc3_manifest_artifacts.h"' in driver_cpp


def test_cmake_registers_io_target() -> None:
    cmake = _read(CMAKE_FILE)
    assert "add_library(objc3c_io STATIC" in cmake
    assert "src/io/objc3_diagnostics_artifacts.cpp" in cmake
    assert "src/io/objc3_file_io.cpp" in cmake
    assert "src/io/objc3_manifest_artifacts.cpp" in cmake
    assert "src/io/objc3_process.cpp" in cmake
    assert "target_link_libraries(objc3c-native PRIVATE" in cmake
    assert "objc3c_io" in cmake
