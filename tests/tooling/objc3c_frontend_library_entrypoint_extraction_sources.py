from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FRONTEND_ROOT = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend"
SOURCE_ROOT = ROOT / "native" / "objc3c" / "src"

FRONTEND_HEADER = FRONTEND_ROOT / "objc3c_frontend.h"
OPTIONS_HEADER = FRONTEND_ROOT / "objc3c_frontend_options.h"
ANCHOR_CPP = FRONTEND_ROOT / "frontend_anchor.cpp"
CLI_HEADER = FRONTEND_ROOT / "objc3_cli_frontend.h"
CLI_CPP = FRONTEND_ROOT / "objc3_cli_frontend.cpp"


def read_source_with_local_includes(path: Path) -> str:
    text = path.read_text(encoding="utf-8")
    expanded: list[str] = []
    for line in text.splitlines():
        expanded.append(line)
        stripped = line.strip()
        if not stripped.startswith('#include "'):
            continue
        include_path = stripped.split('"', 2)[1]
        target = SOURCE_ROOT / include_path
        if target.exists():
            expanded.append(target.read_text(encoding="utf-8"))
    return "\n".join(expanded)


def frontend_anchor_source() -> str:
    return read_source_with_local_includes(ANCHOR_CPP)


def cli_frontend_texts() -> tuple[str, str]:
    return (
        read_source_with_local_includes(CLI_HEADER),
        read_source_with_local_includes(CLI_CPP),
    )


def public_api_header_texts() -> tuple[str, str]:
    return (
        read_source_with_local_includes(FRONTEND_HEADER),
        read_source_with_local_includes(OPTIONS_HEADER),
    )
