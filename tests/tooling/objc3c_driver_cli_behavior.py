from __future__ import annotations

from objc3c_driver_cli_assertions import (
    assert_contains_all,
    assert_excludes_all,
    assert_paths_exist,
)
from objc3c_driver_cli_sources import (
    CLI_ENTRYPOINT_SOURCE,
    DRIVER_CLI_OPTION_SOURCES,
    DRIVER_CMAKE_FILE,
    DRIVER_HEADER,
    DRIVER_MAIN_SOURCE,
    DRIVER_OBJECT_BACKEND_SOURCE,
    DRIVER_RUNTIME_SOURCE,
    MAIN_CPP,
    SRC_CMAKE_FILE,
    read_expanded_source,
    removed_driver_cli_monolith,
    required_driver_module_paths,
)


def assert_driver_cli_module_exists_and_main_calls_it() -> None:
    assert_paths_exist(required_driver_module_paths())

    driver_main_cpp = read_expanded_source(DRIVER_MAIN_SOURCE)
    cli_entrypoint_cpp = read_expanded_source(CLI_ENTRYPOINT_SOURCE)
    main_cpp = read_expanded_source(MAIN_CPP)
    assert_contains_all(
        main_cpp,
        [
            '#include "cli/objc3c_native_entrypoint.h"',
            "RunObjc3NativeCli(argc, argv)",
        ],
    )
    assert_contains_all(
        cli_entrypoint_cpp,
        [
            '#include "driver/objc3_driver_main.h"',
            "RunObjc3DriverMain(argc, argv)",
        ],
    )
    assert_contains_all(
        driver_main_cpp,
        [
            '#include "driver/objc3_cli_options.h"',
            '#include "driver/objc3_compilation_driver.h"',
            '#include "driver/objc3_llvm_capability_routing.h"',
            "ParseObjc3CliOptions(argc, argv, cli_options, cli_error)",
            "ApplyObjc3LLVMCapabilityRouting(cli_options, cli_error)",
            "RunObjc3CompilationDriver(cli_options)",
        ],
    )
    assert_excludes_all(
        main_cpp,
        [
            'if (extension == ".objc3")',
            "clang_parseTranslationUnit",
            "for (int i = 2; i < argc; ++i)",
        ],
    )


def assert_cmake_registers_driver_target() -> None:
    cmake = read_expanded_source(DRIVER_CMAKE_FILE)
    src_cmake = read_expanded_source(SRC_CMAKE_FILE)
    removed_monolith = removed_driver_cli_monolith()

    assert "add_library(objc3c_driver STATIC" in cmake
    assert not removed_monolith.exists()
    assert removed_monolith.name not in cmake
    for driver_cli_option_source in DRIVER_CLI_OPTION_SOURCES:
        assert driver_cli_option_source.name in cmake
    assert_contains_all(
        cmake,
        [
            "objc3_driver_main.cpp",
            "objc3_llvm_capability_routing.cpp",
            "objc3_compilation_driver.cpp",
            "objc3c_driver",
        ],
    )
    assert_contains_all(
        src_cmake,
        [
            "target_link_libraries(objc3c-native PRIVATE",
            "objc3c_driver",
        ],
    )


def assert_cmake_target_linkage_topology_is_split_by_stage() -> None:
    cmake = read_expanded_source(SRC_CMAKE_FILE)
    assert_contains_all(
        cmake,
        [
            "add_subdirectory(parse)",
            "add_subdirectory(sema)",
            "add_subdirectory(lower)",
            "add_subdirectory(ir)",
            "add_subdirectory(io)",
            "add_subdirectory(runtime)",
            "add_subdirectory(pipeline)",
        ],
    )


def assert_cli_exposes_ir_object_backend_flag_and_enum() -> None:
    header = read_expanded_source(DRIVER_HEADER)
    source = "\n".join(read_expanded_source(path) for path in DRIVER_CLI_OPTION_SOURCES)
    runtime = read_expanded_source(DRIVER_RUNTIME_SOURCE)
    object_backend = read_expanded_source(DRIVER_OBJECT_BACKEND_SOURCE)

    assert_contains_all(
        header,
        [
            "enum class Objc3IrObjectBackend",
            "Objc3IrObjectBackend::kLLVMDirect",
            "std::uint32_t language_version = objc3c::config::kCanonicalLanguageVersion;",
            "kLLVMDirect",
        ],
    )
    assert_excludes_all(
        header,
        [
            "enum class Objc3CompatMode",
            "retired_mode_assist",
        ],
    )

    assert_contains_all(
        source,
        [
            "[--llc <path>]",
            "[-fobjc-version=<N>]",
            "--objc3-ir-object-backend <clang|llvm-direct>",
            "--llvm-capabilities-summary <path>",
            "--objc3-route-backend-from-capabilities",
            "ParseObjc3CliIrObjectBackend",
            "ParseIrObjectBackendToken",
            "ParseObjc3LanguageVersion",
            'if (flag.rfind("-fobjc-version=", 0) == 0)',
            "options.language_version = parsed_version;",
            '#include "diagnostics/modes/canonical_rejections.h"',
            "BuildCanonicalModeRejectionDiagnostic(",
            "objc3c::config::UnsupportedLanguageVersionDiagnostic(",
            "invalid --objc3-ir-object-backend (expected clang|llvm-direct): ",
            "options.route_backend_from_capabilities = true;",
            "options.llvm_capabilities_summary = value;",
        ],
    )
    assert_excludes_all(
        source,
        [
            "--objc3-language-version",
            "[--objc3-retired-mode <canonical|legacy>] [--objc3-retired-mode-assist]",
            "ParseCompatMode",
            'flag == "-fobjc-version" || flag == "--objc3-language-version"',
            "options.retired_mode_assist = true;",
        ],
    )

    assert_contains_all(
        object_backend,
        [
            "RunIRCompileLLVMDirect",
            "RunIRCompileLLVMDirect(",
            "cli_options.llc_path",
            ".object-backend.txt",
        ],
    )
    assert "DispatchObjc3DriverCommand(cli_options, input_kind)" in runtime


def assert_cli_default_out_dir_is_tmp_governed() -> None:
    header = read_expanded_source(DRIVER_HEADER)
    expected_path = (
        'std::filesystem::path("tmp") / "artifacts" / "compilation" / '
        '"objc3c-native"'
    )
    assert expected_path in header
