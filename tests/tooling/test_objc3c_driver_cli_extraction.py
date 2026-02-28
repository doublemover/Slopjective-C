from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DRIVER_HEADER = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.h"
DRIVER_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.cpp"
DRIVER_RUNTIME_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_compilation_driver.cpp"
DRIVER_MAIN_HEADER = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_driver_main.h"
DRIVER_MAIN_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_driver_main.cpp"
DRIVER_OBJC3_PATH_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
MAIN_CPP = ROOT / "native" / "objc3c" / "src" / "main.cpp"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_driver_cli_module_exists_and_main_calls_it() -> None:
    assert DRIVER_HEADER.exists()
    assert DRIVER_SOURCE.exists()
    assert DRIVER_RUNTIME_SOURCE.exists()
    assert DRIVER_MAIN_HEADER.exists()
    assert DRIVER_MAIN_SOURCE.exists()
    driver_main_cpp = _read(DRIVER_MAIN_SOURCE)
    main_cpp = _read(MAIN_CPP)
    assert '#include "driver/objc3_driver_main.h"' in main_cpp
    assert "RunObjc3DriverMain(argc, argv)" in main_cpp
    assert '#include "driver/objc3_cli_options.h"' in driver_main_cpp
    assert '#include "driver/objc3_compilation_driver.h"' in driver_main_cpp
    assert "ParseObjc3CliOptions(argc, argv, cli_options, cli_error)" in driver_main_cpp
    assert "RunObjc3CompilationDriver(cli_options)" in driver_main_cpp
    assert 'if (extension == ".objc3")' not in main_cpp
    assert "clang_parseTranslationUnit" not in main_cpp
    assert "for (int i = 2; i < argc; ++i)" not in main_cpp


def test_cmake_registers_driver_target() -> None:
    cmake = _read(CMAKE_FILE)
    assert "add_library(objc3c_driver STATIC" in cmake
    assert "src/driver/objc3_cli_options.cpp" in cmake
    assert "src/driver/objc3_driver_main.cpp" in cmake
    assert "src/driver/objc3_compilation_driver.cpp" in cmake
    assert "objc3c_driver" in cmake
    assert "target_link_libraries(objc3c-native PRIVATE" in cmake
    assert "objc3c_driver" in cmake


def test_cmake_target_linkage_topology_is_split_by_stage() -> None:
    cmake = _read(CMAKE_FILE)

    assert "target_link_libraries(objc3c_parse PUBLIC" in cmake
    assert "target_link_libraries(objc3c_sema PUBLIC" in cmake
    assert "target_link_libraries(objc3c_lower PUBLIC" in cmake
    assert "target_link_libraries(objc3c_ir PUBLIC" in cmake
    assert "target_link_libraries(objc3c_io PUBLIC" in cmake
    assert "objc3c_runtime_abi" in cmake
    assert "target_link_libraries(objc3c_pipeline PUBLIC" in cmake


def test_cli_exposes_ir_object_backend_flag_and_enum() -> None:
    header = _read(DRIVER_HEADER)
    source = _read(DRIVER_SOURCE)
    runtime = _read(DRIVER_RUNTIME_SOURCE)
    objc3_path = _read(DRIVER_OBJC3_PATH_SOURCE)

    assert "enum class Objc3IrObjectBackend" in header
    assert "Objc3IrObjectBackend::kLLVMDirect" in header
    assert "kLLVMDirect" in header

    assert "[--llc <path>]" in source
    assert "--objc3-ir-object-backend <clang|llvm-direct>" in source
    assert "ParseIrObjectBackend" in source
    assert "invalid --objc3-ir-object-backend (expected clang|llvm-direct): " in source

    assert "RunIRCompileLLVMDirect" in objc3_path
    assert "RunIRCompileLLVMDirect(cli_options.llc_path, ir_out, object_out, backend_error)" in objc3_path
    assert ".object-backend.txt" in objc3_path
    assert "RunObjc3LanguagePath(cli_options)" in runtime


def test_cli_default_out_dir_is_tmp_governed() -> None:
    header = _read(DRIVER_HEADER)

    assert 'std::filesystem::path("tmp") / "artifacts" / "compilation" / "objc3c-native"' in header
