from __future__ import annotations

from pathlib import Path

from .constants import DEFAULT_INCLUDE_GLOBS, ROOT


def iter_spec_files() -> list[Path]:
    return iter_spec_files_for_globs(DEFAULT_INCLUDE_GLOBS)


def iter_spec_files_for_globs(globs: tuple[str, ...] | list[str]) -> list[Path]:
    matches: dict[Path, Path] = {}
    for raw_glob in globs:
        include_glob = raw_glob.replace("\\", "/")
        for path in ROOT.glob(include_glob):
            if path.is_file():
                resolved = path.resolve()
                matches[resolved] = resolved

    return sorted(matches.values(), key=lambda path: path.relative_to(ROOT).as_posix())
