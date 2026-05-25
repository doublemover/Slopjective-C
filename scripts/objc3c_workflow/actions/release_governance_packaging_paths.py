"""Packaging-channel and platform-hardening workflow paths."""

from __future__ import annotations

from ..environment import ROOT

PACKAGING_CHANNELS_SOURCE_SURFACE_PY = (
    ROOT / "scripts" / "check_packaging_channels_source_surface.py"
)
PACKAGE_CHANNELS_BUILD_PY = ROOT / "scripts" / "build_objc3c_package_channels.py"
RELEASE_FOUNDATION_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_release_foundation_integration.py"
)
PACKAGING_CHANNELS_END_TO_END_PY = (
    ROOT / "scripts" / "check_objc3c_packaging_channels_end_to_end.py"
)
PLATFORM_SUPPORT_MATRIX_PY = (
    ROOT / "scripts" / "build_objc3c_platform_support_matrix.py"
)
PLATFORM_HOST_EVIDENCE_INGESTION_PY = (
    ROOT / "scripts" / "ingest_objc3c_platform_host_evidence.py"
)
PLATFORM_HOST_EVIDENCE_REVIEW_PY = (
    ROOT / "scripts" / "review_objc3c_platform_host_evidence.py"
)
PLATFORM_SUPPORT_PROMOTION_PY = (
    ROOT / "scripts" / "promote_objc3c_platform_support.py"
)
PLATFORM_HOST_PROMOTION_EVIDENCE_PY = (
    ROOT / "scripts" / "check_platform_host_promotion_evidence.py"
)
PLATFORM_HARDENING_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_platform_hardening_integration.py"
)
RUNNABLE_PLATFORM_HARDENING_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_platform_hardening_end_to_end.py"
)
