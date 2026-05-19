from __future__ import annotations

from objc3c_process_io_assertions import (
    assert_contains_all,
    assert_excludes_all,
    assert_paths_exist,
)
from objc3c_process_io_sources import (
    CMAKE_FILE,
    DRIVER_CPP,
    DRIVER_OBJC3_PATH_CPP,
    DRIVER_OBJC_PATH_CPP,
    IO_HEADER,
    IO_SOURCE,
    MAIN_CPP,
    RUNTIME_ARTIFACT_CONTRACTS_HEADER,
    RUNTIME_ARTIFACT_CONTRACTS_SOURCE,
    RUNTIME_REGISTRATION_DESCRIPTOR_ARTIFACT,
    RUNTIME_REGISTRATION_DESCRIPTOR_DOCUMENT,
    RUNTIME_REGISTRATION_MANIFEST_ARTIFACT,
    RUNTIME_REGISTRATION_MANIFEST_DOCUMENT,
    read_text,
    required_io_module_paths,
)


def assert_io_process_module_exists_and_main_uses_it() -> None:
    assert_paths_exist(required_io_module_paths())

    main_cpp = read_text(MAIN_CPP)
    assert_excludes_all(
        main_cpp,
        [
            '#include "io/objc3_diagnostics_artifacts.h"',
            '#include "io/objc3_file_io.h"',
            '#include "io/objc3_process.h"',
            "static int RunProcess(",
            "static int RunObjectiveCCompile(",
            "static int RunIRCompile(",
            "static void WriteDiagnosticsArtifacts(",
        ],
    )

    driver_cpp = read_text(DRIVER_CPP)
    objc3_path_cpp = read_text(DRIVER_OBJC3_PATH_CPP)
    objc_path_cpp = read_text(DRIVER_OBJC_PATH_CPP)
    assert_contains_all(
        objc3_path_cpp,
        [
            '#include "io/objc3_diagnostics_artifacts.h"',
            '#include "io/objc3_file_io.h"',
            '#include "io/objc3_process.h"',
            '#include "io/objc3_manifest_artifacts.h"',
        ],
    )
    assert_contains_all(
        objc_path_cpp,
        [
            '#include "io/objc3_diagnostics_artifacts.h"',
            '#include "io/objc3_process.h"',
            '#include "io/objc3_manifest_artifacts.h"',
        ],
    )
    assert '#include "io/objc3_diagnostics_artifacts.h"' not in driver_cpp


def assert_cmake_registers_io_target() -> None:
    cmake = read_text(CMAKE_FILE)
    assert_contains_all(
        cmake,
        [
            "add_library(objc3c_io STATIC",
            "src/io/objc3_diagnostics_artifacts.cpp",
            "src/io/objc3_file_io.cpp",
            "src/io/objc3_manifest_artifacts.cpp",
            "target_link_libraries(objc3c_io PUBLIC",
            "objc3c_runtime_abi",
        ],
    )


def assert_cmake_registers_runtime_abi_target() -> None:
    cmake = read_text(CMAKE_FILE)
    assert_contains_all(
        cmake,
        [
            "add_library(objc3c_runtime_abi STATIC",
            "src/io/objc3_process.cpp",
            "target_link_libraries(objc3c-native PRIVATE",
            "objc3c_io",
        ],
    )


def assert_io_process_exposes_llc_direct_object_emission_path() -> None:
    io_header = read_text(IO_HEADER)
    io_source = read_text(IO_SOURCE)
    cmake = read_text(CMAKE_FILE)

    assert_contains_all(
        io_header,
        ["RunIRCompileLLVMDirect(const std::filesystem::path &llc_path"],
    )
    assert_contains_all(
        io_source,
        [
            "{\"-filetype=obj\", \"-o\", object_out.string(), ir_path.string()}",
            "llc executable not found",
        ],
    )
    assert "-cc1" not in io_source
    assert "OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION" in cmake


def assert_runtime_registration_symbol_json_uses_owner_record_contract() -> None:
    io_header = read_text(IO_HEADER)
    contracts_header = read_text(RUNTIME_ARTIFACT_CONTRACTS_HEADER)
    contracts_source = read_text(RUNTIME_ARTIFACT_CONTRACTS_SOURCE)
    descriptor_artifact = read_text(RUNTIME_REGISTRATION_DESCRIPTOR_ARTIFACT)
    descriptor_document = read_text(RUNTIME_REGISTRATION_DESCRIPTOR_DOCUMENT)
    manifest_artifact = read_text(RUNTIME_REGISTRATION_MANIFEST_ARTIFACT)
    manifest_document = read_text(RUNTIME_REGISTRATION_MANIFEST_DOCUMENT)

    assert_contains_all(
        io_header,
        [
            "struct Objc3RuntimeRegistrationSymbolOwnerRecord",
            "constructor_root_symbol;",
            "constructor_init_stub_symbol;",
            "bootstrap_registration_table_symbol;",
            "bootstrap_image_local_init_state_symbol;",
        ],
    )
    assert_contains_all(
        contracts_header,
        [
            "BuildRuntimeTranslationUnitRegistrationSymbolOwnerRecord",
            "BuildRuntimeRegistrationDescriptorSymbolOwnerRecord",
        ],
    )
    assert_contains_all(
        contracts_source,
        [
            "BuildRuntimeRegistrationSymbolOwnerRecord(",
            "MakeIdentifierSafeSuffix(",
            "translation_unit_identity_model !=",
            "inputs.translation_unit_identity_model",
        ],
    )

    assert_contains_all(
        descriptor_artifact,
        ["BuildRuntimeRegistrationDescriptorSymbolOwnerRecord"],
    )
    assert_contains_all(
        manifest_artifact,
        ["BuildRuntimeTranslationUnitRegistrationSymbolOwnerRecord"],
    )
    assert_excludes_all(
        descriptor_artifact,
        [
            "MakeIdentifierSafeSuffix(",
            "constructor_init_stub_symbol =",
            "bootstrap_registration_table_symbol =",
        ],
    )
    assert "MakeIdentifierSafeSuffix(" not in manifest_document

    assert_contains_all(
        descriptor_document,
        [
            "const Objc3RuntimeRegistrationSymbolOwnerRecord &symbol_owner_record",
            'StringField("constructor_root_symbol",',
            "symbol_owner_record.constructor_root_symbol",
            "symbol_owner_record.constructor_init_stub_symbol",
            "symbol_owner_record.bootstrap_registration_table_symbol",
            "symbol_owner_record.bootstrap_image_local_init_state_symbol",
        ],
    )
    assert_contains_all(
        manifest_document,
        [
            "const Objc3RuntimeRegistrationSymbolOwnerRecord &symbol_owner_record",
            "EscapeJsonString(symbol_owner_record.constructor_root_symbol)",
            "EscapeJsonString(symbol_owner_record.constructor_init_stub_symbol)",
            "symbol_owner_record.bootstrap_registration_table_symbol",
            "symbol_owner_record.bootstrap_image_local_init_state_symbol",
        ],
    )
