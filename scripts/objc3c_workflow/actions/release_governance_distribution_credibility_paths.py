"""Distribution credibility workflow paths."""

from __future__ import annotations

from ..environment import ROOT

DISTRIBUTION_CREDIBILITY_SOURCE_SURFACE_PY = (
    ROOT / "scripts" / "check_distribution_credibility_source_surface.py"
)
DISTRIBUTION_CREDIBILITY_DASHBOARD_PY = (
    ROOT / "scripts" / "build_objc3c_distribution_credibility_dashboard.py"
)
DISTRIBUTION_CREDIBILITY_PUBLICATION_PY = (
    ROOT / "scripts" / "publish_objc3c_distribution_trust_report.py"
)
DISTRIBUTION_CREDIBILITY_END_TO_END_PY = (
    ROOT / "scripts" / "check_objc3c_distribution_credibility_end_to_end.py"
)


__all__ = [
    "DISTRIBUTION_CREDIBILITY_DASHBOARD_PY",
    "DISTRIBUTION_CREDIBILITY_END_TO_END_PY",
    "DISTRIBUTION_CREDIBILITY_PUBLICATION_PY",
    "DISTRIBUTION_CREDIBILITY_SOURCE_SURFACE_PY",
]
