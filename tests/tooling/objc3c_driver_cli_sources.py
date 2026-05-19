from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DRIVER_ROOT = ROOT / "native" / "objc3c" / "src" / "driver"
DRIVER_HEADER = DRIVER_ROOT / "objc3_cli_options.h"
DRIVER_PARSE_SOURCE = DRIVER_ROOT / "objc3_cli_parse.cpp"
DRIVER_USAGE_SOURCE = DRIVER_ROOT / "objc3_cli_usage.cpp"
DRIVER_OPTION_APPLICATION_SOURCE = DRIVER_ROOT / "objc3_cli_option_application.cpp"
DRIVER_LANGUAGE_OPTIONS_SOURCE = DRIVER_ROOT / "objc3_cli_language_options.cpp"
DRIVER_LANGUAGE_VERSION_SOURCE = DRIVER_ROOT / "objc3_cli_language_version.cpp"
DRIVER_TOOLCHAIN_OPTIONS_SOURCE = DRIVER_ROOT / "objc3_cli_toolchain_options.cpp"
DRIVER_IR_BACKEND_OPTIONS_SOURCE = DRIVER_ROOT / "objc3_cli_ir_backend_options.cpp"
DRIVER_OPTION_VALIDATION_SOURCE = DRIVER_ROOT / "objc3_cli_option_validation.cpp"
DRIVER_RUNTIME_SOURCE = DRIVER_ROOT / "objc3_compilation_driver.cpp"
DRIVER_MAIN_HEADER = DRIVER_ROOT / "objc3_driver_main.h"
DRIVER_MAIN_SOURCE = DRIVER_ROOT / "objc3_driver_main.cpp"
DRIVER_CAPABILITY_ROUTING_SOURCE = DRIVER_ROOT / "objc3_llvm_capability_routing.cpp"
DRIVER_OBJECT_BACKEND_SOURCE = DRIVER_ROOT / "objc3_driver_object_backend.cpp"
DRIVER_OBJC3_PATH_SOURCE = DRIVER_ROOT / "objc3_objc3_path.cpp"
CLI_ENTRYPOINT_SOURCE = ROOT / "native" / "objc3c" / "src" / "cli" / "objc3c_native_entrypoint.cpp"
MAIN_CPP = ROOT / "native" / "objc3c" / "src" / "main.cpp"
DRIVER_CMAKE_FILE = DRIVER_ROOT / "CMakeLists.txt"
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


def read_expanded_source(path: Path) -> str:
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


def required_driver_module_paths() -> list[Path]:
    return [
        CLI_ENTRYPOINT_SOURCE,
        DRIVER_HEADER,
        *DRIVER_CLI_OPTION_SOURCES,
        DRIVER_OBJECT_BACKEND_SOURCE,
        DRIVER_RUNTIME_SOURCE,
        DRIVER_MAIN_HEADER,
        DRIVER_MAIN_SOURCE,
        DRIVER_CAPABILITY_ROUTING_SOURCE,
    ]


def removed_driver_cli_monolith() -> Path:
    return DRIVER_HEADER.with_name("objc3_cli_" + "options.cpp")
