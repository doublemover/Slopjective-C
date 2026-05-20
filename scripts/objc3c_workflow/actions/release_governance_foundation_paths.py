"""Release-foundation workflow paths."""

from __future__ import annotations

from ..environment import ROOT

RELEASE_FOUNDATION_SOURCE_SURFACE_PY = (
    ROOT / "scripts" / "check_release_foundation_source_surface.py"
)
RELEASE_ABI_API_DRIFT_PY = ROOT / "scripts" / "check_objc3c_release_abi_api_drift.py"
RELEASE_MANIFEST_PY = ROOT / "scripts" / "build_objc3c_release_manifest.py"
RELEASE_PROVENANCE_PY = ROOT / "scripts" / "publish_objc3c_release_provenance.py"
RELEASE_EVIDENCE_PY = ROOT / "scripts" / "check_release_evidence.py"
