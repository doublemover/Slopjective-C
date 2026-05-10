#!/usr/bin/env python3
"""Validate the checked-in conformance corpus surface and emit a summary."""

from __future__ import annotations

if __package__:
    from .conformance_corpus_surface_model import (
        ROOT,
        ConformanceCorpusPaths,
        ConformanceCorpusSurfaceModel,
        ConformanceCorpusSurfaceReportWriter,
        SurfaceValidationError,
        main,
    )
else:
    from conformance_corpus_surface_model import (
        ROOT,
        ConformanceCorpusPaths,
        ConformanceCorpusSurfaceModel,
        ConformanceCorpusSurfaceReportWriter,
        SurfaceValidationError,
        main,
    )

__all__ = (
    "ROOT",
    "ConformanceCorpusPaths",
    "ConformanceCorpusSurfaceModel",
    "ConformanceCorpusSurfaceReportWriter",
    "SurfaceValidationError",
    "main",
)


if __name__ == "__main__":
    raise SystemExit(main())
