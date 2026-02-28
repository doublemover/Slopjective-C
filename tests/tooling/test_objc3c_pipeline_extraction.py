from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PIPELINE_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.h"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
MAIN_CPP = ROOT / "native" / "objc3c" / "src" / "main.cpp"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_pipeline_module_exists_and_main_uses_it() -> None:
    assert PIPELINE_HEADER.exists()
    assert PIPELINE_SOURCE.exists()

    main_cpp = _read(MAIN_CPP)
    assert '#include "pipeline/objc3_frontend_pipeline.h"' in main_cpp
    assert "static Objc3FrontendPipelineResult RunObjc3FrontendPipeline(" not in main_cpp


def test_cmake_registers_pipeline_target() -> None:
    cmake = _read(CMAKE_FILE)
    assert "add_library(objc3c_pipeline STATIC" in cmake
    assert "src/pipeline/objc3_frontend_pipeline.cpp" in cmake
    assert "objc3c_pipeline" in cmake
