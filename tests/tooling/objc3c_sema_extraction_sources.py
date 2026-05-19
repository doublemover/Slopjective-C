from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SRC_ROOT = ROOT / "native" / "objc3c" / "src"
SEMA_ROOT = SRC_ROOT / "sema"
PIPELINE_ROOT = SRC_ROOT / "pipeline"

SEMA_CONTRACT = SEMA_ROOT / "objc3_sema_contract.h"
SEMA_HEADER = SEMA_ROOT / "objc3_semantic_passes.h"
SEMA_SOURCE = SEMA_ROOT / "objc3_semantic_passes.cpp"
SEMA_PM_HEADER = SEMA_ROOT / "objc3_sema_pass_manager.h"
SEMA_PM_SOURCE = SEMA_ROOT / "objc3_sema_pass_manager.cpp"
PIPELINE_ORCHESTRATION = PIPELINE_ROOT / "frontend_pipeline_orchestration.cpp"
PIPELINE_SEMA_STAGE_RUNNER = PIPELINE_ROOT / "frontend_pipeline_sema_stage_runner.cpp"
PIPELINE_TYPES = PIPELINE_ROOT / "objc3_frontend_types.h"
SEMA_CMAKE_FILE = SEMA_ROOT / "CMakeLists.txt"


def sema_required_paths() -> tuple[Path, ...]:
    return (
        SEMA_CONTRACT,
        SEMA_HEADER,
        SEMA_SOURCE,
        SEMA_PM_HEADER,
        SEMA_PM_SOURCE,
        PIPELINE_ORCHESTRATION,
        PIPELINE_SEMA_STAGE_RUNNER,
    )


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def pipeline_sema_api_texts() -> tuple[str, str]:
    return read_text(PIPELINE_SEMA_STAGE_RUNNER), read_text(PIPELINE_ORCHESTRATION)


def pipeline_types_text() -> str:
    return read_text(PIPELINE_TYPES)


def sema_contract_texts() -> tuple[str, str, str]:
    return read_text(SEMA_CONTRACT), read_text(SEMA_HEADER), read_text(SEMA_SOURCE)


def sema_cmake_text() -> str:
    return read_text(SEMA_CMAKE_FILE)
