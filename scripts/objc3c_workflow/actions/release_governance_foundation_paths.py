"""Release-foundation workflow paths."""

from __future__ import annotations

from ..environment import ROOT

RELEASE_FOUNDATION_SOURCE_SURFACE_PY = (
    ROOT / "scripts" / "check_release_foundation_source_surface.py"
)
ABI_GOVERNANCE_PY = ROOT / "scripts" / "check_objc3c_abi_governance.py"
PERFORMANCE_GOVERNANCE_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_performance_governance_integration.py"
)
RELEASE_ABI_API_DRIFT_PY = ROOT / "scripts" / "check_objc3c_release_abi_api_drift.py"
RELEASE_MANIFEST_PY = ROOT / "scripts" / "build_objc3c_release_manifest.py"
RELEASE_PROVENANCE_PY = ROOT / "scripts" / "publish_objc3c_release_provenance.py"
RELEASE_EVIDENCE_PY = ROOT / "scripts" / "check_release_evidence.py"
