"""Public-conformance reporting workflow paths."""

from __future__ import annotations

from ..environment import ROOT

PUBLIC_CONFORMANCE_SOURCE_SURFACE_PY = (
    ROOT / "scripts" / "check_public_conformance_reporting_source_surface.py"
)
PUBLIC_CONFORMANCE_SCORECARD_PY = (
    ROOT / "scripts" / "build_objc3c_public_conformance_scorecard.py"
)
PUBLIC_CONFORMANCE_REPORT_PY = (
    ROOT / "scripts" / "publish_objc3c_public_conformance_report.py"
)
PUBLIC_CONFORMANCE_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_public_conformance_reporting_integration.py"
)
PUBLIC_CONFORMANCE_END_TO_END_PY = (
    ROOT / "scripts" / "check_objc3c_public_conformance_reporting_end_to_end.py"
)
