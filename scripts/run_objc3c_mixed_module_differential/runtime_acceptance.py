"""Runtime acceptance imports used by the mixed-module differential runner."""

from __future__ import annotations

import sys
from pathlib import Path


def _ensure_scripts_path() -> None:
    scripts_root = Path(__file__).resolve().parents[1]
    scripts_root_text = str(scripts_root)
    if scripts_root_text not in sys.path:
        sys.path.insert(0, scripts_root_text)


try:
    from objc3c_runtime_acceptance.case_result import CaseResult
    from objc3c_runtime_acceptance.domains import interop_packaging
    from objc3c_runtime_acceptance.native_binaries import find_clangxx
except ModuleNotFoundError:
    _ensure_scripts_path()
    from objc3c_runtime_acceptance.case_result import CaseResult
    from objc3c_runtime_acceptance.domains import interop_packaging
    from objc3c_runtime_acceptance.native_binaries import find_clangxx

__all__ = ["CaseResult", "find_clangxx", "interop_packaging"]
