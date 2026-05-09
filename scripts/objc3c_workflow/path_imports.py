"""Import-root mutation helpers for workflow entrypoints."""

from __future__ import annotations

import sys
from collections.abc import Iterable
from pathlib import Path


def install_import_roots(import_roots: Iterable[Path]) -> None:
    for import_root in import_roots:
        import_root_text = str(import_root)
        if import_root_text not in sys.path:
            sys.path.insert(0, import_root_text)


__all__ = ["install_import_roots"]
