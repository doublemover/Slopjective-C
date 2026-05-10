from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

from c_api_smoke_support import (
    ARTIFACT_H,
    C_API_H,
    C_API_HEADER_PATHS,
    CONTEXT_H,
    DIAGNOSTIC_H,
    ERROR_H,
    FRONTEND_H,
    OPTIONS_H,
    RESULT_H,
    SRC_ROOT,
    STRING_H,
    VERSION_H,
    c_api_surface_text,
    read_text,
)


@dataclass(frozen=True)
class CApiHeaderSurface:
    header: str
    c_api_surface: str
    frontend_header: str
    options_header: str
    result_header: str
    string_header: str


def load_c_api_header_surface() -> CApiHeaderSurface:
    return CApiHeaderSurface(
        header=read_text(C_API_H),
        c_api_surface=c_api_surface_text(),
        frontend_header=read_text(FRONTEND_H),
        options_header=read_text(OPTIONS_H),
        result_header=read_text(RESULT_H),
        string_header=read_text(STRING_H),
    )


def removed_domain_headers() -> list[Path]:
    return [
        SRC_ROOT / "libobjc3c_frontend" / "api.h",
        SRC_ROOT / "libobjc3c_frontend" / "version.h",
    ]


def public_frontend_header_paths() -> list[Path]:
    return [
        FRONTEND_H,
        VERSION_H,
        CONTEXT_H,
        OPTIONS_H,
        RESULT_H,
        DIAGNOSTIC_H,
        STRING_H,
        ARTIFACT_H,
        ERROR_H,
        *C_API_HEADER_PATHS,
    ]


def public_frontend_surface_paths() -> list[Path]:
    return [
        path
        for path in (SRC_ROOT / "libobjc3c_frontend").glob("**/*")
        if path.suffix in {".h", ".cpp", ".inc"}
    ]
