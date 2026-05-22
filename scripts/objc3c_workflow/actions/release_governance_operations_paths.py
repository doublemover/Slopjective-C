"""Release-operations workflow paths."""

from __future__ import annotations

from ..environment import ROOT

RELEASE_OPERATIONS_SOURCE_SURFACE_PY = (
    ROOT / "scripts" / "check_release_operations_source_surface.py"
)
RELEASE_OPERATIONS_SCHEMA_SURFACE_PY = (
    ROOT / "scripts" / "check_release_operations_schema_surface.py"
)
PACKAGING_CHANNELS_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_packaging_channels_integration.py"
)
UPDATE_MANIFEST_PY = ROOT / "scripts" / "build_objc3c_update_manifest.py"
RELEASE_OPERATIONS_PUBLICATION_PY = (
    ROOT / "scripts" / "publish_objc3c_release_operations_metadata.py"
)
RELEASE_OPERATIONS_END_TO_END_PY = (
    ROOT / "scripts" / "check_objc3c_release_operations_end_to_end.py"
)
