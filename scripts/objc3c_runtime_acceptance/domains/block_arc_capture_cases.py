"""Block/ARC capture legality runtime acceptance cases facade."""

from __future__ import annotations

from pathlib import Path

_HELPER_MODULE_DIR = Path(__file__).with_suffix("")
__path__ = [str(_HELPER_MODULE_DIR)]

from .block_arc_capture_cases.cases import (  # noqa: E402
    check_escaping_block_capture_legality_case,
)


check_escaping_block_capture_legality_case.__module__ = __name__


__all__ = ["check_escaping_block_capture_legality_case"]
