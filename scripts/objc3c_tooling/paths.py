"""Repository path helpers with stable Windows-safe display semantics."""

from __future__ import annotations

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]


def normalize_rel_path(raw_path: str) -> str:
    """Normalize a repository-relative path string for portable reports."""

    return raw_path.replace("\\", "/")


def display_path(path: Path | str, *, root: Path = ROOT) -> str:
    """Render a path relative to the repo when possible, otherwise absolute."""

    candidate = Path(path)
    absolute = candidate.resolve()
    root_resolved = root.resolve()
    try:
        return absolute.relative_to(root_resolved).as_posix()
    except ValueError:
        return absolute.as_posix()


def repo_rel(path: Path | str, *, root: Path = ROOT) -> str:
    """Render a path relative to the repo, failing clearly for outside paths."""

    candidate = Path(path)
    root_path = Path(root)
    try:
        return candidate.relative_to(root_path).as_posix()
    except ValueError:
        pass

    absolute = candidate.resolve()
    root_resolved = root_path.resolve()
    try:
        return absolute.relative_to(root_resolved).as_posix()
    except ValueError as exc:
        raise ValueError(
            f"path is outside repository root {root_resolved.as_posix()}: "
            f"{absolute.as_posix()}"
        ) from exc


def resolve_repo_path(path: Path | str, *, root: Path = ROOT) -> Path:
    """Resolve user input as an absolute path, relative to the repo if needed."""

    candidate = Path(path)
    return candidate if candidate.is_absolute() else Path(root) / candidate


def resolve_repo_path_inside(path: Path | str, *, root: Path = ROOT) -> Path:
    """Resolve user input and require the final path to stay under the repo."""

    resolved = resolve_repo_path(path, root=root).resolve()
    root_resolved = Path(root).resolve()
    try:
        resolved.relative_to(root_resolved)
    except ValueError as exc:
        raise ValueError(
            f"path must be under repository root {root_resolved.as_posix()}: "
            f"{resolved.as_posix()}"
        ) from exc
    return resolved

