from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DRIVER_HEADER = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.h"
DRIVER_PARSE_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_parse.cpp"
DRIVER_USAGE_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_usage.cpp"
DRIVER_OPTION_APPLICATION_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_option_application.cpp"
DRIVER_LANGUAGE_OPTIONS_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_language_options.cpp"
DRIVER_LANGUAGE_VERSION_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_language_version.cpp"
DRIVER_TOOLCHAIN_OPTIONS_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_toolchain_options.cpp"
DRIVER_IR_BACKEND_OPTIONS_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_ir_backend_options.cpp"
DRIVER_OPTION_VALIDATION_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_option_validation.cpp"
DRIVER_RUNTIME_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_compilation_driver.cpp"
DRIVER_MAIN_HEADER = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_driver_main.h"
DRIVER_MAIN_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_driver_main.cpp"
DRIVER_CAPABILITY_ROUTING_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_llvm_capability_routing.cpp"
DRIVER_OBJC3_PATH_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
MAIN_CPP = ROOT / "native" / "objc3c" / "src" / "main.cpp"
DRIVER_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "driver" / "CMakeLists.txt"
SRC_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "CMakeLists.txt"
DRIVER_CLI_OPTION_SOURCES = (
    DRIVER_PARSE_SOURCE,
    DRIVER_USAGE_SOURCE,
    DRIVER_OPTION_APPLICATION_SOURCE,
    DRIVER_LANGUAGE_OPTIONS_SOURCE,
    DRIVER_LANGUAGE_VERSION_SOURCE,
    DRIVER_TOOLCHAIN_OPTIONS_SOURCE,
    DRIVER_IR_BACKEND_OPTIONS_SOURCE,
    DRIVER_OPTION_VALIDATION_SOURCE,
)


def _read(path: Path) -> str:
    text = path.read_text(encoding="utf-8")
    expanded: list[str] = []
    for line in text.splitlines():
        expanded.append(line)
        stripped = line.strip()
        if not stripped.startswith('#include "'):
            continue
        include_path = stripped.split('"', 2)[1]
        target = ROOT / "native" / "objc3c" / "src" / include_path
        if target.exists():
            expanded.append(target.read_text(encoding="utf-8"))
    return "\n".join(expanded)


def test_driver_cli_module_exists_and_main_calls_it() -> None:
    assert DRIVER_HEADER.exists()
    for driver_cli_option_source in DRIVER_CLI_OPTION_SOURCES:
        assert driver_cli_option_source.exists()
    assert DRIVER_RUNTIME_SOURCE.exists()
    assert DRIVER_MAIN_HEADER.exists()
    assert DRIVER_MAIN_SOURCE.exists()
    assert DRIVER_CAPABILITY_ROUTING_SOURCE.exists()
    driver_main_cpp = _read(DRIVER_MAIN_SOURCE)
    main_cpp = _read(MAIN_CPP)
    assert '#include "driver/objc3_driver_main.h"' in main_cpp
    assert "RunObjc3DriverMain(argc, argv)" in main_cpp
    assert '#include "driver/objc3_cli_options.h"' in driver_main_cpp
    assert '#include "driver/objc3_compilation_driver.h"' in driver_main_cpp
    assert '#include "driver/objc3_llvm_capability_routing.h"' in driver_main_cpp
    assert "ParseObjc3CliOptions(argc, argv, cli_options, cli_error)" in driver_main_cpp
    assert "ApplyObjc3LLVMCabilityRouting(cli_options, cli_error)" in driver_main_cpp
    assert "RunObjc3CompilationDriver(cli_options)" in driver_main_cpp
    assert 'if (extension == ".objc3")' not in main_cpp
    assert "clang_parseTranslationUnit" not in main_cpp
    assert "for (int i = 2; i < argc; ++i)" not in main_cpp


def test_cmake_registers_driver_target() -> None:
    cmake = _read(DRIVER_CMAKE_FILE)
    src_cmake = _read(SRC_CMAKE_FILE)
    assert "add_library(objc3c_driver STATIC" in cmake
    assert "objc3_cli_options.cpp" not in cmake
    for driver_cli_option_source in DRIVER_CLI_OPTION_SOURCES:
        assert driver_cli_option_source.name in cmake
    assert "objc3_driver_main.cpp" in cmake
    assert "objc3_llvm_capability_routing.cpp" in cmake
    assert "objc3_compilation_driver.cpp" in cmake
    assert "objc3c_driver" in cmake
    assert "target_link_libraries(objc3c-native PRIVATE" in src_cmake
    assert "objc3c_driver" in src_cmake


def test_cmake_target_linkage_topology_is_split_by_stage() -> None:
    cmake = _read(SRC_CMAKE_FILE)

    assert "add_subdirectory(parse)" in cmake
    assert "add_subdirectory(sema)" in cmake
    assert "add_subdirectory(lower)" in cmake
    assert "add_subdirectory(ir)" in cmake
    assert "add_subdirectory(io)" in cmake
    assert "add_subdirectory(runtime)" in cmake
    assert "add_subdirectory(pipeline)" in cmake


def test_cli_exposes_ir_object_backend_flag_and_enum() -> None:
    header = _read(DRIVER_HEADER)
    source = "\n".join(_read(path) for path in DRIVER_CLI_OPTION_SOURCES)
    runtime = _read(DRIVER_RUNTIME_SOURCE)
    objc3_path = _read(DRIVER_OBJC3_PATH_SOURCE)

    assert "enum class Objc3IrObjectBackend" in header
    assert "enum class Objc3CompatMode" not in header
    assert "Objc3IrObjectBackend::kLLVMDirect" in header
    assert "std::uint32_t language_version = objc3c::config::kCanonicalLanguageVersion;" in header
    assert "migration_assist" not in header
    assert "kLLVMDirect" in header

    assert "[--llc <path>]" in source
    assert "[-fobjc-version=<N>]" in source
    assert "--objc3-language-version" not in source
    assert "[--objc3-compat-mode <canonical|legacy>] [--objc3-migration-assist]" not in source
    assert "--objc3-ir-object-backend <clang|llvm-direct>" in source
    assert "--llvm-capabilities-summary <path>" in source
    assert "--objc3-route-backend-from-capabilities" in source
    assert "ParseObjc3CliIrObjectBackend" in source
    assert "ParseIrObjectBackendToken" in source
    assert "ParseCompatMode" not in source
    assert "ParseObjc3LanguageVersion" in source
    assert "if (flag.rfind(\"-fobjc-version=\", 0) == 0)" in source
    assert "flag == \"-fobjc-version\" || flag == \"--objc3-language-version\"" not in source
    assert "options.language_version = parsed_version;" in source
    assert '#include "diagnostics/modes/objc3_removed_mode_options.h"' in source
    assert "BuildRemovedModeOptionDiagnostic(flag," in source
    assert "options.migration_assist = true;" not in source
    assert "objc3c::config::UnsupportedLanguageVersionDiagnostic(" in source
    assert "invalid --objc3-ir-object-backend (expected clang|llvm-direct): " in source
    assert "options.route_backend_from_capabilities = true;" in source
    assert "options.llvm_capabilities_summary = value;" in source

    assert "RunIRCompileLLVMDirect" in objc3_path
    assert "RunIRCompileLLVMDirect(cli_options.llc_path, ir_out, object_out, backend_error)" in objc3_path
    assert ".object-backend.txt" in objc3_path
    assert "RunObjc3LanguagePath(cli_options)" in runtime


def test_cli_default_out_dir_is_tmp_governed() -> None:
    header = _read(DRIVER_HEADER)

    assert 'std::filesystem::path("tmp") / "artifacts" / "compilation" / "objc3c-native"' in header
