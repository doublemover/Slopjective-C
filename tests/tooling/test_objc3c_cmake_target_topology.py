from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SRC_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "CMakeLists.txt"
CLI_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "cli" / "CMakeLists.txt"
CONFIG_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "config" / "CMakeLists.txt"
PARSE_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "parse" / "CMakeLists.txt"
SEMA_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "sema" / "CMakeLists.txt"
LOWER_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "lower" / "CMakeLists.txt"
IR_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "ir" / "CMakeLists.txt"
PIPELINE_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "CMakeLists.txt"
ARTIFACTS_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "artifacts" / "CMakeLists.txt"
RUNTIME_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "runtime" / "CMakeLists.txt"
FRONTEND_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "CMakeLists.txt"
DRIVER_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "driver" / "CMakeLists.txt"
TOOLS_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "tools" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _combined_cmake() -> str:
    return "\n".join(
        _read(path)
        for path in [
            SRC_CMAKE_FILE,
            CLI_CMAKE_FILE,
            CONFIG_CMAKE_FILE,
            PARSE_CMAKE_FILE,
            SEMA_CMAKE_FILE,
            LOWER_CMAKE_FILE,
            IR_CMAKE_FILE,
            PIPELINE_CMAKE_FILE,
            ARTIFACTS_CMAKE_FILE,
            RUNTIME_CMAKE_FILE,
            FRONTEND_CMAKE_FILE,
            DRIVER_CMAKE_FILE,
            TOOLS_CMAKE_FILE,
        ]
    )


def test_stage_libraries_define_forward_only_linkage_topology() -> None:
    cmake = _combined_cmake()

    assert "target_link_libraries(objc3c_parse PUBLIC" in cmake
    assert "target_link_libraries(objc3c_sema PUBLIC" in cmake
    assert "target_link_libraries(objc3c_lower PUBLIC" in cmake
    assert "target_link_libraries(objc3c_ir PUBLIC" in cmake
    assert "target_link_libraries(objc3c_pipeline PUBLIC" in cmake
    assert "target_link_libraries(objc3c_frontend PUBLIC" in cmake
    assert "target_link_libraries(objc3c_driver PUBLIC" in cmake
    assert "add_library(objc3c_pipeline_results INTERFACE)" in cmake
    assert "add_library(objc3c_sema_model INTERFACE)" in cmake
    assert "add_library(objc3c_lower_model INTERFACE)" in cmake
    assert "add_library(objc3c_runtime_metadata INTERFACE)" in cmake
    assert "add_library(objc3c_artifacts_evidence INTERFACE)" in cmake
    assert "add_library(objc3c_artifacts_reports INTERFACE)" in cmake
    removed_c_api_monolith = "c_api" + ".cpp"
    assert removed_c_api_monolith not in _read(FRONTEND_CMAKE_FILE)


def test_native_executable_links_through_driver_aggregate_target() -> None:
    src_cmake = _read(SRC_CMAKE_FILE)
    cli_cmake = _read(CLI_CMAKE_FILE)
    tools_cmake = _read(TOOLS_CMAKE_FILE)

    assert "add_executable(objc3c-native" in cli_cmake
    assert '"${CMAKE_CURRENT_LIST_DIR}/../main.cpp"' in cli_cmake
    assert "target_link_libraries(objc3c-native PRIVATE" in src_cmake
    assert "  objc3c_driver" in src_cmake
    assert "add_executable(objc3c_tools_frontend_c_api_runner" in tools_cmake
    assert "OUTPUT_NAME objc3c-frontend-c-api-runner" in tools_cmake
    assert "objc3c_frontend_c_api_runner.cpp" in tools_cmake
    assert "target_link_libraries(objc3c_tools_frontend_c_api_runner PRIVATE" in tools_cmake
    assert "  objc3c_frontend" in tools_cmake


def test_frontend_cmake_includes_split_public_c_api_owner_sources() -> None:
    cmake = _read(FRONTEND_CMAKE_FILE)

    assert "c_api_abi.cpp" in cmake
    assert "c_api_compile.cpp" in cmake
    assert "c_api_lifecycle.cpp" in cmake
    assert "c_api_result_artifacts.cpp" in cmake
    assert "c_api_result_error.cpp" in cmake
    assert "c_api_result_lifecycle.cpp" in cmake
    assert "c_api_stage_summary.cpp" in cmake
    assert "c_api_string_bridge.cpp" in cmake
    assert "frontend_compile_entrypoints.cpp" in cmake
    assert "frontend_compile_pipeline.cpp" in cmake
    assert "frontend_anchor.cpp" not in cmake
