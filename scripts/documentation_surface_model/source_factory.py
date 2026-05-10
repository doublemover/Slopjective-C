"""Shared constants and source construction for documentation surface checks."""

from __future__ import annotations

from pathlib import Path

from documentation_surface import paths as docs_paths
from documentation_surface.model import DocumentationSurfaceSource

CHECKER_NAME = docs_paths.CHECKER_NAME
ROOT = docs_paths.ROOT
DOCUMENTATION_SURFACE_OWNER = docs_paths.DOCUMENTATION_SURFACE_OWNER
DOCUMENTATION_SURFACE_OWNER_SURFACE = docs_paths.DOCUMENTATION_SURFACE_OWNER_SURFACE
DOCUMENTATION_SURFACE_BLOCKER_METADATA = docs_paths.DOCUMENTATION_SURFACE_BLOCKER_METADATA


def _source(
    path: Path,
    *,
    required_tokens: tuple[str, ...] = (),
    forbidden_tokens: tuple[str, ...] = (),
) -> DocumentationSurfaceSource:
    return DocumentationSurfaceSource(
        path=path,
        root=ROOT,
        required_tokens=required_tokens,
        forbidden_tokens=forbidden_tokens,
        owner_id=DOCUMENTATION_SURFACE_OWNER,
        owner_surface=DOCUMENTATION_SURFACE_OWNER_SURFACE,
    )


__all__ = (
    "CHECKER_NAME",
    "DOCUMENTATION_SURFACE_BLOCKER_METADATA",
    "DOCUMENTATION_SURFACE_OWNER",
    "DOCUMENTATION_SURFACE_OWNER_SURFACE",
    "ROOT",
    "_source",
)
