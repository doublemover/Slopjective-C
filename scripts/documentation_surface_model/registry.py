"""Structured model and report writer for the documentation surface checker."""

from __future__ import annotations

from documentation_surface.model import (
    DocumentationSurfaceModel,
    DocumentationSurfaceReportWriter,
    DocumentationSurfaceSource,
)

from .overview_sources import overview_documentation_sources
from .runbook_sources import runbook_documentation_sources
from .source_factory import (
    CHECKER_NAME,
    DOCUMENTATION_SURFACE_BLOCKER_METADATA,
    DOCUMENTATION_SURFACE_OWNER,
    DOCUMENTATION_SURFACE_OWNER_SURFACE,
    ROOT,
    _source,
)
from .stdlib_sources import stdlib_documentation_sources
from .tutorial_sources import tutorial_documentation_sources


def _documentation_sources() -> tuple[DocumentationSurfaceSource, ...]:
    return (
        *overview_documentation_sources(),
        *tutorial_documentation_sources(),
        *stdlib_documentation_sources(),
        *runbook_documentation_sources(),
    )


DOCUMENTATION_SURFACE_MODEL = DocumentationSurfaceModel(
    checker_name=CHECKER_NAME,
    sources=_documentation_sources(),
    owner_id=DOCUMENTATION_SURFACE_OWNER,
    owner_surface=DOCUMENTATION_SURFACE_OWNER_SURFACE,
    blocker_metadata=DOCUMENTATION_SURFACE_BLOCKER_METADATA,
)

__all__ = (
    "CHECKER_NAME",
    "DOCUMENTATION_SURFACE_BLOCKER_METADATA",
    "DOCUMENTATION_SURFACE_MODEL",
    "DOCUMENTATION_SURFACE_OWNER",
    "DOCUMENTATION_SURFACE_OWNER_SURFACE",
    "DocumentationSurfaceModel",
    "DocumentationSurfaceReportWriter",
    "DocumentationSurfaceSource",
    "ROOT",
    "_source",
)
