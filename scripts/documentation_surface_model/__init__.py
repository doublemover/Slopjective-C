"""Public import surface for the documentation surface model checker."""

from __future__ import annotations

from documentation_surface.model import (
    DocumentationSurfaceModel,
    DocumentationSurfaceReport,
    DocumentationSurfaceReportWriter,
    DocumentationSurfaceSource,
)

from .registry import (
    CHECKER_NAME,
    DOCUMENTATION_SURFACE_BLOCKER_METADATA,
    DOCUMENTATION_SURFACE_MODEL,
    DOCUMENTATION_SURFACE_OWNER,
    DOCUMENTATION_SURFACE_OWNER_SURFACE,
    ROOT,
    _source,
)

__all__ = (
    "CHECKER_NAME",
    "DOCUMENTATION_SURFACE_BLOCKER_METADATA",
    "DOCUMENTATION_SURFACE_MODEL",
    "DOCUMENTATION_SURFACE_OWNER",
    "DOCUMENTATION_SURFACE_OWNER_SURFACE",
    "DocumentationSurfaceModel",
    "DocumentationSurfaceReport",
    "DocumentationSurfaceReportWriter",
    "DocumentationSurfaceSource",
    "ROOT",
    "_source",
)
