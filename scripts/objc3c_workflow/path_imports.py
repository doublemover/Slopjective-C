"""Import-root mutation helpers for workflow entrypoints."""

from __future__ import annotations

import sys
from collections.abc import Iterable
from pathlib import Path

if __package__:
    from .path_policy import missing_import_roots
else:
    from path_policy import missing_import_roots


def install_import_roots(import_roots: Iterable[Path]) -> None:
    for root_text in reversed(missing_import_roots(sys.path, import_roots)):
        sys.path.insert(0, root_text)


__all__ = ["install_import_roots"]
