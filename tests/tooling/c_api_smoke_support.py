from __future__ import annotations

import re
import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SRC_ROOT = ROOT / "native" / "objc3c" / "src"
FRONTEND_H = SRC_ROOT / "libobjc3c_frontend" / "objc3c_frontend.h"
VERSION_H = SRC_ROOT / "libobjc3c_frontend" / "objc3c_frontend_version.h"
OPTIONS_H = SRC_ROOT / "libobjc3c_frontend" / "objc3c_frontend_options.h"
RESULT_H = SRC_ROOT / "libobjc3c_frontend" / "objc3c_frontend_result.h"
DIAGNOSTIC_H = SRC_ROOT / "libobjc3c_frontend" / "objc3c_frontend_diagnostic.h"
CONTEXT_H = SRC_ROOT / "libobjc3c_frontend" / "objc3c_frontend_context.h"
ERROR_H = SRC_ROOT / "libobjc3c_frontend" / "objc3c_frontend_error.h"
ARTIFACT_H = SRC_ROOT / "libobjc3c_frontend" / "objc3c_frontend_artifact.h"
STRING_H = SRC_ROOT / "libobjc3c_frontend" / "objc3c_frontend_string.h"
C_API_H = SRC_ROOT / "libobjc3c_frontend" / "c_api.h"
C_API_CONTRACT_H = SRC_ROOT / "libobjc3c_frontend" / "c_api_contract.h"
C_API_TYPES_H = SRC_ROOT / "libobjc3c_frontend" / "c_api_types.h"
C_API_VERSION_H = SRC_ROOT / "libobjc3c_frontend" / "c_api_version.h"
C_API_LIFECYCLE_H = SRC_ROOT / "libobjc3c_frontend" / "c_api_lifecycle.h"
C_API_COMPILE_H = SRC_ROOT / "libobjc3c_frontend" / "c_api_compile.h"
C_API_RESULT_H = SRC_ROOT / "libobjc3c_frontend" / "c_api_result.h"
C_API_STRING_H = SRC_ROOT / "libobjc3c_frontend" / "c_api_string.h"
C_API_STAGE_SUMMARY_H = SRC_ROOT / "libobjc3c_frontend" / "c_api_stage_summary.h"
C_API_HEADER_PATHS = [
    C_API_H,
    C_API_CONTRACT_H,
    C_API_TYPES_H,
    C_API_VERSION_H,
    C_API_LIFECYCLE_H,
    C_API_COMPILE_H,
    C_API_RESULT_H,
    C_API_STRING_H,
    C_API_STAGE_SUMMARY_H,
]
RESULT_OWNERSHIP_CPP = (
    SRC_ROOT / "libobjc3c_frontend" / "objc3c_frontend_result_ownership.cpp"
)
FRONTEND_COMPILE_CONTRACT_CPP = (
    SRC_ROOT / "libobjc3c_frontend" / "objc3c_frontend_compile_contract.cpp"
)
C_API_SOURCES = [
    SRC_ROOT / "libobjc3c_frontend" / "c_api_abi.cpp",
    SRC_ROOT / "libobjc3c_frontend" / "c_api_compile.cpp",
    SRC_ROOT / "libobjc3c_frontend" / "c_api_lifecycle.cpp",
    SRC_ROOT / "libobjc3c_frontend" / "c_api_result_artifacts.cpp",
    SRC_ROOT / "libobjc3c_frontend" / "c_api_result_error.cpp",
    SRC_ROOT / "libobjc3c_frontend" / "c_api_result_lifecycle.cpp",
    SRC_ROOT / "libobjc3c_frontend" / "c_api_stage_summary.cpp",
    SRC_ROOT / "libobjc3c_frontend" / "c_api_string_bridge.cpp",
]
C_API_HELPER_CONTRACT = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "frontend_c_api_helper_contract.json"
)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def c_api_surface_text() -> str:
    return "\n".join(read_text(path) for path in C_API_HEADER_PATHS)


def c_api_source_text() -> str:
    return "\n".join(read_text(source_path) for source_path in C_API_SOURCES)


def squash_ws(text: str) -> str:
    return re.sub(r"\s+", " ", text)


def find_compiler(candidates: list[str]) -> str | None:
    for candidate in candidates:
        resolved = shutil.which(candidate)
        if resolved:
            return resolved
    return None
