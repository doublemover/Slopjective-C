"""Import path setup for script and package entry points."""

from __future__ import annotations

import sys

from .config import ROOT, SCRIPTS_ROOT


def ensure_import_paths() -> None:
    for path in (ROOT, SCRIPTS_ROOT):
        path_text = str(path)
        if path_text not in sys.path:
            sys.path.insert(0, path_text)


__all__ = ["ensure_import_paths"]
