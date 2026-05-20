from __future__ import annotations

import importlib.util
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_shared.json_io import load_json_object, write_json_file

SCRIPT_PATH = ROOT / "scripts" / "check_release_foundation_source_surface.py"


def _load_checker():
    spec = importlib.util.spec_from_file_location("check_release_foundation_source_surface", SCRIPT_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError("Unable to load scripts/check_release_foundation_source_surface.py")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def test_release_foundation_source_surface_writes_named_summary_fields() -> None:
    checker = _load_checker()
    checker.SUMMARY_PATH = ROOT / "tmp" / "tests" / "release-foundation-source-surface-summary.json"
    checker.SUMMARY_PATH.unlink(missing_ok=True)

    try:
        assert checker.main() == 0

        summary = load_json_object(checker.SUMMARY_PATH)
        assert summary["contract_id"] == checker.SUMMARY_CONTRACT_ID
        assert summary["status"] == "PASS"
        assert summary["source_surface"] == (
            "tests/tooling/fixtures/release_foundation/source_surface.json"
        )
        assert summary["runbook"] == checker.EXPECTED_RUNBOOK
        assert summary["artifact_taxonomy"] == checker.EXPECTED_REQUIRED_PATHS["artifact_taxonomy"]
        assert summary["abi_api_governance"] == checker.EXPECTED_REQUIRED_PATHS["abi_api_governance"]
        assert summary["workflow_surface"] == checker.EXPECTED_REQUIRED_PATHS["workflow_surface"]
        assert summary["schema_surface"] == checker.EXPECTED_REQUIRED_PATHS["schema_surface"]
        assert summary["checked_in_sources"] == list(checker.EXPECTED_CHECKED_IN_SOURCES)
        assert summary["upstream_surfaces"] == list(checker.EXPECTED_UPSTREAM_SURFACES)
        assert summary["build_scripts"] == list(checker.EXPECTED_BUILD_SCRIPTS)
        assert summary["machine_owned_output_roots"] == list(
            checker.EXPECTED_MACHINE_OWNED_OUTPUT_ROOTS
        )
        assert summary["explicit_non_goals"] == list(checker.EXPECTED_EXPLICIT_NON_GOALS)
        assert summary["checked_path_count"] == len(summary["checked_paths"])
    finally:
        checker.SUMMARY_PATH.unlink(missing_ok=True)


def test_release_foundation_source_surface_rejects_path_drift(tmp_path: Path) -> None:
    checker = _load_checker()
    surface = load_json_object(checker.SOURCE_SURFACE)
    surface["artifact_taxonomy"] = "tests/tooling/fixtures/release_foundation/artifact_taxonomy_old.json"

    checker.SOURCE_SURFACE = tmp_path / "source_surface.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_json_file(checker.SOURCE_SURFACE, surface, sort_keys=True)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_release_foundation_source_surface_rejects_machine_owned_root_drift(
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    surface = load_json_object(checker.SOURCE_SURFACE)
    surface["machine_owned_output_roots"] = [
        *checker.EXPECTED_MACHINE_OWNED_OUTPUT_ROOTS[:-1],
        "tmp/pkg/release-foundation-compat-package",
    ]

    checker.SOURCE_SURFACE = tmp_path / "source_surface.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_json_file(checker.SOURCE_SURFACE, surface, sort_keys=True)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_release_foundation_source_surface_rejects_source_boundary_drift(
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    surface = load_json_object(checker.SOURCE_SURFACE)
    surface["checked_in_sources"] = [
        *checker.EXPECTED_CHECKED_IN_SOURCES,
        "docs/runbooks/objc3c_legacy_release_foundation.md",
    ]

    checker.SOURCE_SURFACE = tmp_path / "source_surface.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_json_file(checker.SOURCE_SURFACE, surface, sort_keys=True)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()
