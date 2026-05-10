"""Repository path handling for planning issue publication."""

from __future__ import annotations

from pathlib import Path

from .constants import ROOT
from .contracts import PublicationError


def repo_path(path: Path) -> Path:
    return path if path.is_absolute() else ROOT / path


def is_under(child: Path, parent: Path) -> bool:
    try:
        child.resolve().relative_to(parent.resolve())
        return True
    except ValueError:
        return False


def _repo_display_path(path: Path) -> str:
    try:
        return path.resolve().relative_to(ROOT.resolve()).as_posix()
    except ValueError:
        return path.as_posix()


def assert_not_tmp_source(path: Path) -> None:
    if is_under(path, ROOT / "tmp"):
        raise PublicationError(
            f"refusing transient planning input: {_repo_display_path(path)}"
        )
