from __future__ import annotations

import importlib.util
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_shared.json_io import load_json_object
from objc3c_shared.schema_registry import schema_path

SCRIPT_PATH = ROOT / "scripts" / "check_packaging_channels_schema_surface.py"


def _load_checker():
    spec = importlib.util.spec_from_file_location("check_packaging_channels_schema_surface", SCRIPT_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError("Unable to load scripts/check_packaging_channels_schema_surface.py")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def test_packaging_channels_schema_surface_uses_registered_schemas() -> None:
    checker = _load_checker()
    checker.SUMMARY_PATH = ROOT / "tmp" / "tests" / "packaging-channels-schema-surface-summary.json"
    checker.SUMMARY_PATH.unlink(missing_ok=True)

    try:
        assert checker.main() == 0

        summary = load_json_object(checker.SUMMARY_PATH)
        assert summary["contract_id"] == "objc3c.packaging.channels.schema.surface.summary.v1"
        assert summary["status"] == "PASS"
        assert summary["metadata_surface"] == "tests/tooling/fixtures/packaging_channels/metadata_surface.json"
        assert summary["schema_surface"] == "tests/tooling/fixtures/packaging_channels/schema_surface.json"
        assert summary["schema_count"] == 2
        assert summary["schema_ids"] == [
            "https://objc3c.dev/schemas/objc3c-package-channels-manifest-v1.schema.json",
            "https://objc3c.dev/schemas/objc3c-package-install-receipt-v1.schema.json",
        ]
        assert [
            schema_path("objc3c-package-channels-manifest-v1").relative_to(ROOT).as_posix(),
            schema_path("objc3c-package-install-receipt-v1").relative_to(ROOT).as_posix(),
        ] == [
            "schemas/objc3c-package-channels-manifest-v1.schema.json",
            "schemas/objc3c-package-install-receipt-v1.schema.json",
        ]
    finally:
        checker.SUMMARY_PATH.unlink(missing_ok=True)
