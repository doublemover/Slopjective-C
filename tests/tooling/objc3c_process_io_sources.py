from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
IO_ROOT = ROOT / "native" / "objc3c" / "src" / "io"
IO_HEADER = IO_ROOT / "objc3_process.h"
IO_SOURCE = IO_ROOT / "objc3_process.cpp"
FILE_IO_HEADER = IO_ROOT / "objc3_file_io.h"
FILE_IO_SOURCE = IO_ROOT / "objc3_file_io.cpp"
DIAG_HEADER = IO_ROOT / "objc3_diagnostics_artifacts.h"
DIAG_SOURCE = IO_ROOT / "objc3_diagnostics_artifacts.cpp"
MANIFEST_HEADER = IO_ROOT / "objc3_manifest_artifacts.h"
MANIFEST_SOURCE = IO_ROOT / "objc3_manifest_artifacts.cpp"
RUNTIME_ARTIFACT_CONTRACTS_HEADER = IO_ROOT / "objc3_runtime_artifact_contracts.h"
RUNTIME_ARTIFACT_CONTRACTS_SOURCE = IO_ROOT / "objc3_runtime_artifact_contracts.cpp"
RUNTIME_REGISTRATION_DESCRIPTOR_ARTIFACT = (
    IO_ROOT / "objc3_runtime_registration_descriptor_artifact.cpp"
)
RUNTIME_REGISTRATION_DESCRIPTOR_DOCUMENT = (
    IO_ROOT / "objc3_runtime_registration_descriptor_document.cpp"
)
RUNTIME_REGISTRATION_MANIFEST_ARTIFACT = (
    IO_ROOT / "objc3_runtime_registration_manifest_artifact.cpp"
)
RUNTIME_REGISTRATION_MANIFEST_DOCUMENT = (
    IO_ROOT / "objc3_runtime_registration_manifest_document.cpp"
)
MAIN_CPP = ROOT / "native" / "objc3c" / "src" / "main.cpp"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_compilation_driver.cpp"
DRIVER_OBJC3_PATH_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DRIVER_OBJC_PATH_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objectivec_path.cpp"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def required_io_module_paths() -> list[Path]:
    return [
        IO_HEADER,
        IO_SOURCE,
        FILE_IO_HEADER,
        FILE_IO_SOURCE,
        DIAG_HEADER,
        DIAG_SOURCE,
        MANIFEST_HEADER,
        MANIFEST_SOURCE,
    ]
