from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"
BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_stage_libraries_define_forward_only_linkage_topology() -> None:
    cmake = _read(CMAKE_FILE)

    assert "target_link_libraries(objc3c_parse PUBLIC" in cmake
    assert "target_link_libraries(objc3c_sema PUBLIC" in cmake
    assert "target_link_libraries(objc3c_lower PUBLIC" in cmake
    assert "target_link_libraries(objc3c_ir PUBLIC" in cmake
    assert "target_link_libraries(objc3c_pipeline PUBLIC" in cmake
    assert "target_link_libraries(objc3c_frontend PUBLIC" in cmake
    assert "target_link_libraries(objc3c_driver PUBLIC" in cmake
    assert "src/libobjc3c_frontend/c_api.cpp" in cmake


def test_native_executable_links_through_driver_aggregate_target() -> None:
    cmake = _read(CMAKE_FILE)

    assert "add_executable(objc3c-native" in cmake
    assert "target_link_libraries(objc3c-native PRIVATE" in cmake
    assert "  objc3c_driver" in cmake
    assert "add_executable(objc3c-frontend-c-api-runner" in cmake
    assert "src/tools/objc3c_frontend_c_api_runner.cpp" in cmake
    assert "target_link_libraries(objc3c-frontend-c-api-runner PRIVATE" in cmake
    assert "  objc3c_frontend" in cmake


def test_build_script_includes_split_driver_entrypoint_source() -> None:
    build = _read(BUILD_SCRIPT)

    assert '"native/objc3c/src/main.cpp"' in build
    assert '"native/objc3c/src/driver/objc3_driver_main.cpp"' in build
    assert '"native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp"' in build
    assert '"native/objc3c/src/libobjc3c_frontend/c_api.cpp"' in build
    assert '"native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp"' in build
