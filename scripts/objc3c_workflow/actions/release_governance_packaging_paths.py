"""Packaging-channel and platform-hardening workflow paths."""

from __future__ import annotations

from ..environment import ROOT

PACKAGING_CHANNELS_SOURCE_SURFACE_PY = (
    ROOT / "scripts" / "check_packaging_channels_source_surface.py"
)
PACKAGE_CHANNELS_BUILD_PY = ROOT / "scripts" / "build_objc3c_package_channels.py"
PACKAGING_CHANNELS_END_TO_END_PY = (
    ROOT / "scripts" / "check_objc3c_packaging_channels_end_to_end.py"
)
PLATFORM_SUPPORT_MATRIX_PY = (
    ROOT / "scripts" / "build_objc3c_platform_support_matrix.py"
)
PLATFORM_HARDENING_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_platform_hardening_integration.py"
)
RUNNABLE_PLATFORM_HARDENING_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_platform_hardening_end_to_end.py"
)
