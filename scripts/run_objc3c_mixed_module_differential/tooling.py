"""Tooling imports used by the mixed-module differential runner."""

from __future__ import annotations

import sys
from pathlib import Path


def _ensure_scripts_path() -> None:
    scripts_root = Path(__file__).resolve().parents[1]
    scripts_root_text = str(scripts_root)
    if scripts_root_text not in sys.path:
        sys.path.insert(0, scripts_root_text)


try:
    from objc3c_tooling.json_io import load_json_object as load_json
    from objc3c_tooling.paths import repo_rel
except ModuleNotFoundError:
    _ensure_scripts_path()
    from objc3c_tooling.json_io import load_json_object as load_json
    from objc3c_tooling.paths import repo_rel

__all__ = ["load_json", "repo_rel"]
