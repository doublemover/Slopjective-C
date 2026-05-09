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
RUNTIME_ARTIFACT_CONTRACTS_HEADER = (
    ROOT / "native" / "objc3c" / "src" / "io" / "objc3_runtime_artifact_contracts.h"
)
RUNTIME_ARTIFACT_CONTRACTS_SOURCE = (
    ROOT / "native" / "objc3c" / "src" / "io" / "objc3_runtime_artifact_contracts.cpp"
)
RUNTIME_REGISTRATION_DESCRIPTOR_ARTIFACT = (
    ROOT / "native" / "objc3c" / "src" / "io" / "objc3_runtime_registration_descriptor_artifact.cpp"
)
RUNTIME_REGISTRATION_DESCRIPTOR_DOCUMENT = (
    ROOT / "native" / "objc3c" / "src" / "io" / "objc3_runtime_registration_descriptor_document.cpp"
)
RUNTIME_REGISTRATION_MANIFEST_ARTIFACT = (
    ROOT / "native" / "objc3c" / "src" / "io" / "objc3_runtime_registration_manifest_artifact.cpp"
)
RUNTIME_REGISTRATION_MANIFEST_DOCUMENT = (
    ROOT / "native" / "objc3c" / "src" / "io" / "objc3_runtime_registration_manifest_document.cpp"
)
MAIN_CPP = ROOT / "native" / "objc3c" / "src" / "main.cpp"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_compilation_driver.cpp"
DRIVER_OBJC3_PATH_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DRIVER_OBJC_PATH_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objectivec_path.cpp"
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
    objc3_path_cpp = _read(DRIVER_OBJC3_PATH_CPP)
    objc_path_cpp = _read(DRIVER_OBJC_PATH_CPP)
    assert '#include "io/objc3_diagnostics_artifacts.h"' in objc3_path_cpp
    assert '#include "io/objc3_file_io.h"' in objc3_path_cpp
    assert '#include "io/objc3_process.h"' in objc3_path_cpp
    assert '#include "io/objc3_manifest_artifacts.h"' in objc3_path_cpp
    assert '#include "io/objc3_diagnostics_artifacts.h"' in objc_path_cpp
    assert '#include "io/objc3_process.h"' in objc_path_cpp
    assert '#include "io/objc3_manifest_artifacts.h"' in objc_path_cpp
    assert '#include "io/objc3_diagnostics_artifacts.h"' not in driver_cpp


def test_cmake_registers_io_target() -> None:
    cmake = _read(CMAKE_FILE)
    assert "add_library(objc3c_io STATIC" in cmake
    assert "src/io/objc3_diagnostics_artifacts.cpp" in cmake
    assert "src/io/objc3_file_io.cpp" in cmake
    assert "src/io/objc3_manifest_artifacts.cpp" in cmake
    assert "target_link_libraries(objc3c_io PUBLIC" in cmake
    assert "objc3c_runtime_abi" in cmake


def test_cmake_registers_runtime_abi_target() -> None:
    cmake = _read(CMAKE_FILE)
    assert "add_library(objc3c_runtime_abi STATIC" in cmake
    assert "src/io/objc3_process.cpp" in cmake
    assert "target_link_libraries(objc3c-native PRIVATE" in cmake
    assert "objc3c_io" in cmake


def test_io_process_exposes_llc_direct_object_emission_path() -> None:
    io_header = _read(IO_HEADER)
    io_source = _read(IO_SOURCE)
    cmake = _read(CMAKE_FILE)

    assert "RunIRCompileLLVMDirect(const std::filesystem::path &llc_path" in io_header
    assert "{\"-filetype=obj\", \"-o\", object_out.string(), ir_path.string()}" in io_source
    assert "llc executable not found" in io_source
    assert "-cc1" not in io_source
    assert "OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION" in cmake


def test_runtime_registration_symbol_json_uses_owner_record_contract() -> None:
    io_header = _read(IO_HEADER)
    contracts_header = _read(RUNTIME_ARTIFACT_CONTRACTS_HEADER)
    contracts_source = _read(RUNTIME_ARTIFACT_CONTRACTS_SOURCE)
    descriptor_artifact = _read(RUNTIME_REGISTRATION_DESCRIPTOR_ARTIFACT)
    descriptor_document = _read(RUNTIME_REGISTRATION_DESCRIPTOR_DOCUMENT)
    manifest_artifact = _read(RUNTIME_REGISTRATION_MANIFEST_ARTIFACT)
    manifest_document = _read(RUNTIME_REGISTRATION_MANIFEST_DOCUMENT)

    assert "struct Objc3RuntimeRegistrationSymbolOwnerRecord" in io_header
    assert "constructor_root_symbol;" in io_header
    assert "constructor_init_stub_symbol;" in io_header
    assert "bootstrap_registration_table_symbol;" in io_header
    assert "bootstrap_image_local_init_state_symbol;" in io_header

    assert "BuildRuntimeTranslationUnitRegistrationSymbolOwnerRecord" in contracts_header
    assert "BuildRuntimeRegistrationDescriptorSymbolOwnerRecord" in contracts_header
    assert "BuildRuntimeRegistrationSymbolOwnerRecord(" in contracts_source
    assert "MakeIdentifierSafeSuffix(" in contracts_source
    assert "translation_unit_identity_model !=" in contracts_source
    assert "inputs.translation_unit_identity_model" in contracts_source

    assert "BuildRuntimeRegistrationDescriptorSymbolOwnerRecord" in descriptor_artifact
    assert "BuildRuntimeTranslationUnitRegistrationSymbolOwnerRecord" in manifest_artifact
    assert "MakeIdentifierSafeSuffix(" not in descriptor_artifact
    assert "MakeIdentifierSafeSuffix(" not in manifest_document
    assert "constructor_init_stub_symbol =" not in descriptor_artifact
    assert "bootstrap_registration_table_symbol =" not in descriptor_artifact

    assert "const Objc3RuntimeRegistrationSymbolOwnerRecord &symbol_owner_record" in descriptor_document
    assert "const Objc3RuntimeRegistrationSymbolOwnerRecord &symbol_owner_record" in manifest_document
    assert 'StringField("constructor_root_symbol",' in descriptor_document
    assert "symbol_owner_record.constructor_root_symbol" in descriptor_document
    assert "symbol_owner_record.constructor_init_stub_symbol" in descriptor_document
    assert "symbol_owner_record.bootstrap_registration_table_symbol" in descriptor_document
    assert "symbol_owner_record.bootstrap_image_local_init_state_symbol" in descriptor_document
    assert "EscapeJsonString(symbol_owner_record.constructor_root_symbol)" in manifest_document
    assert "EscapeJsonString(symbol_owner_record.constructor_init_stub_symbol)" in manifest_document
    assert "symbol_owner_record.bootstrap_registration_table_symbol" in manifest_document
    assert "symbol_owner_record.bootstrap_image_local_init_state_symbol" in manifest_document
