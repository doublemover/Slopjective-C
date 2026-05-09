from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PIPELINE_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.h"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "frontend_pipeline_orchestration.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "artifacts" / "objc3_frontend_artifacts.cpp"
CLI_FRONTEND_SOURCE = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "objc3_cli_frontend.cpp"
PIPELINE_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "CMakeLists.txt"
ARTIFACTS_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "artifacts" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_pipeline_module_exists_and_cli_frontend_uses_it() -> None:
    assert PIPELINE_HEADER.exists()
    assert PIPELINE_SOURCE.exists()
    assert ARTIFACTS_SOURCE.exists()
    assert CLI_FRONTEND_SOURCE.exists()

    cli_frontend_cpp = _read(CLI_FRONTEND_SOURCE)
    assert '#include "pipeline/objc3_frontend_pipeline.h"' in cli_frontend_cpp
    assert "RunObjc3FrontendPipeline(source, options)" in cli_frontend_cpp


def test_cmake_registers_pipeline_target() -> None:
    pipeline_cmake = _read(PIPELINE_CMAKE_FILE)
    artifacts_cmake = _read(ARTIFACTS_CMAKE_FILE)
    assert "add_library(objc3c_pipeline STATIC" in pipeline_cmake
    assert "frontend_pipeline_orchestration.cpp" in pipeline_cmake
    assert "frontend_pipeline_stage_runner.cpp" in pipeline_cmake
    assert "frontend_pipeline_sema_stage_runner.cpp" in pipeline_cmake
    assert "add_library(objc3c_artifacts STATIC" in artifacts_cmake
    assert "objc3_frontend_artifacts.cpp" in artifacts_cmake
    assert "target_link_libraries(objc3c_artifacts PUBLIC" in artifacts_cmake
    assert "objc3c_pipeline" in artifacts_cmake
