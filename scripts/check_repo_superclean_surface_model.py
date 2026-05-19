"""Structured model and report writer for the repo superclean surface checker."""

from __future__ import annotations

if __package__:
    from .repo_superclean_surface.contracts import (
        build_surface_payload,
        load_surface_payload,
        missing_surface_report,
        validate_surface_payload,
        write_surface_payload,
    )
    from .repo_superclean_surface.model import (
        CHECKER_NAME,
        FrontendContractArtifact,
        RepoSupercleanSurfaceModel,
        SurfaceField,
        SurfaceReport,
        SurfaceReportWriter,
    )
else:
    from repo_superclean_surface.contracts import (
        build_surface_payload,
        load_surface_payload,
        missing_surface_report,
        validate_surface_payload,
        write_surface_payload,
    )
    from repo_superclean_surface.model import (
        CHECKER_NAME,
        FrontendContractArtifact,
        RepoSupercleanSurfaceModel,
        SurfaceField,
        SurfaceReport,
        SurfaceReportWriter,
    )


__all__ = [
    "CHECKER_NAME",
    "FrontendContractArtifact",
    "RepoSupercleanSurfaceModel",
    "SurfaceField",
    "SurfaceReport",
    "SurfaceReportWriter",
    "build_surface_payload",
    "load_surface_payload",
    "missing_surface_report",
    "validate_surface_payload",
    "write_surface_payload",
]
