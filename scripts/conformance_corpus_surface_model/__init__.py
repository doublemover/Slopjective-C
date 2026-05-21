"""Public import surface for the conformance corpus surface model checker."""

from __future__ import annotations

from .cli import ROOT, main
from .contracts import (
    CHECKER_NAME,
    EXPECTED_PRIMARY_BUCKETS,
    EXPECTED_SUPPLEMENTAL_BUCKETS,
    EXPECTED_WORKFLOW_SURFACE,
    LONGITUDINAL_CONTRACT_ID,
    PUBLIC_SUITE_MANIFEST,
    SUMMARY_CONTRACT_ID,
    SURFACE_CONTRACT_ID,
    SURFACE_FIELDS,
    SurfaceField,
    SurfaceValidationError,
)
from .loading import (
    ConformanceCorpusPaths,
    load_longitudinal_suites,
    load_manifest_payload,
    load_surface,
)
from .reports import (
    ConformanceCorpusSurfaceReportWriter,
    ConformanceCorpusSurfaceSummary,
    ManifestBucketSummary,
    RetainedSuiteSummary,
)
from .validation import ConformanceCorpusSurfaceModel

__all__ = (
    "CHECKER_NAME",
    "EXPECTED_PRIMARY_BUCKETS",
    "EXPECTED_SUPPLEMENTAL_BUCKETS",
    "EXPECTED_WORKFLOW_SURFACE",
    "LONGITUDINAL_CONTRACT_ID",
    "PUBLIC_SUITE_MANIFEST",
    "ROOT",
    "SUMMARY_CONTRACT_ID",
    "SURFACE_CONTRACT_ID",
    "SURFACE_FIELDS",
    "ConformanceCorpusPaths",
    "ConformanceCorpusSurfaceModel",
    "ConformanceCorpusSurfaceReportWriter",
    "ConformanceCorpusSurfaceSummary",
    "ManifestBucketSummary",
    "RetainedSuiteSummary",
    "SurfaceField",
    "SurfaceValidationError",
    "load_longitudinal_suites",
    "load_manifest_payload",
    "load_surface",
    "main",
)
