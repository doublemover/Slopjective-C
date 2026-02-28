from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PIPELINE_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.h"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
CLI_FRONTEND_SOURCE = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "objc3_cli_frontend.cpp"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


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
    cmake = _read(CMAKE_FILE)
    assert "add_library(objc3c_pipeline STATIC" in cmake
    assert "src/pipeline/objc3_frontend_pipeline.cpp" in cmake
    assert "src/pipeline/objc3_frontend_artifacts.cpp" in cmake
    assert "objc3c_pipeline" in cmake
