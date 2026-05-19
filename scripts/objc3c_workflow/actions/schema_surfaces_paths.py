"""Schema-surface workflow paths."""

from __future__ import annotations

from ..environment import ROOT

PUBLIC_CONFORMANCE_SCHEMA_SURFACE_PY = (
    ROOT / "scripts" / "check_public_conformance_schema_surface.py"
)
RELEASE_FOUNDATION_SCHEMA_SURFACE_PY = (
    ROOT / "scripts" / "check_release_foundation_schema_surface.py"
)
PACKAGING_CHANNELS_SCHEMA_SURFACE_PY = (
    ROOT / "scripts" / "check_packaging_channels_schema_surface.py"
)
RELEASE_OPERATIONS_SCHEMA_SURFACE_PY = (
    ROOT / "scripts" / "check_release_operations_schema_surface.py"
)
DISTRIBUTION_CREDIBILITY_SCHEMA_SURFACE_PY = (
    ROOT / "scripts" / "check_distribution_credibility_schema_surface.py"
)
SECURITY_HARDENING_SCHEMA_SURFACE_PY = (
    ROOT / "scripts" / "check_security_hardening_schema_surface.py"
)
