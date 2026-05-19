from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PIPELINE_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "CMakeLists.txt"
PIPELINE_SOURCE_CLOSURE_DIR = (
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "source_closure"
)
SOURCE_CLOSURE_REPLAY_KEY_SOURCES = (
    "frontend_source_closure_replay_keys_concurrency.cpp",
    "frontend_source_closure_replay_keys_core.cpp",
    "frontend_source_closure_replay_keys_metaprogramming_interop.cpp",
    "frontend_source_closure_replay_keys_ownership_dispatch.cpp",
    "frontend_source_closure_replay_keys_tooling_symbol_graph.cpp",
)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _cmake_block(cmake: str, start: str) -> str:
    block = cmake[cmake.index(start) :]
    return block[: block.index("\n)")]


def test_source_closure_replay_keys_are_owned_by_split_pipeline_module() -> None:
    pipeline_cmake = _read(PIPELINE_CMAKE_FILE)
    replay_key_target = _cmake_block(
        pipeline_cmake,
        "add_library(objc3c_pipeline_source_closure_replay_keys STATIC",
    )
    pipeline_target = _cmake_block(
        pipeline_cmake,
        "add_library(objc3c_pipeline STATIC",
    )

    for source in SOURCE_CLOSURE_REPLAY_KEY_SOURCES:
        assert (PIPELINE_SOURCE_CLOSURE_DIR / source).exists()
        assert f"source_closure/{source}" in replay_key_target
        assert source not in pipeline_target

    assert "frontend_source_closure_replay_keys.h" in pipeline_cmake
    assert (
        "target_link_libraries(objc3c_pipeline_source_closure_replay_keys PUBLIC"
        in pipeline_cmake
    )
    assert "  objc3c_sema_model" in pipeline_cmake
    assert "  objc3c_pipeline_source_closure_replay_keys" in pipeline_cmake
