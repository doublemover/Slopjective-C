from __future__ import annotations

import importlib.util
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_shared.json_io import load_json_object, write_json_file

SCRIPT_PATH = ROOT / "scripts" / "check_external_validation_source_surface.py"


def _load_checker():
    spec = importlib.util.spec_from_file_location("check_external_validation_source_surface", SCRIPT_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError("Unable to load scripts/check_external_validation_source_surface.py")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def test_external_validation_source_surface_owner_modules_are_importable() -> None:
    import external_validation_source_surface.paths as paths
    import external_validation_source_surface.publication as publication
    import external_validation_source_surface.rendering as rendering
    import external_validation_source_surface.source_model as source_model
    import external_validation_source_surface.validation as validation

    checker = _load_checker()

    assert checker.ROOT == paths.ROOT
    assert checker.SOURCE_SURFACE == paths.SOURCE_SURFACE
    assert checker.SUMMARY_PATH == paths.SUMMARY_PATH
    assert checker.SUMMARY_CONTRACT_ID == source_model.SUMMARY_CONTRACT_ID
    assert callable(validation.validate_source_surface)
    assert callable(rendering.render_summary)
    assert callable(publication.publish_summary)


def test_external_validation_source_surface_writes_named_summary_fields() -> None:
    checker = _load_checker()
    checker.SUMMARY_PATH = ROOT / "tmp" / "tests" / "external-validation-source-surface-summary.json"
    checker.SUMMARY_PATH.unlink(missing_ok=True)

    try:
        assert checker.main() == 0

        summary = load_json_object(checker.SUMMARY_PATH)
        assert summary["contract_id"] == checker.SUMMARY_CONTRACT_ID
        assert summary["status"] == "PASS"
        assert summary["source_surface"] == (
            "tests/tooling/fixtures/external_validation/source_surface.json"
        )
        assert summary["runbook"] == checker.EXPECTED_REQUIRED_PATHS["runbook"]
        assert summary["source_root"] == checker.EXPECTED_REQUIRED_PATHS["source_root"]
        assert summary["source_readme"] == checker.EXPECTED_REQUIRED_PATHS["source_readme"]
        assert summary["source_check_script"] == checker.EXPECTED_REQUIRED_PATHS[
            "source_check_script"
        ]
        assert summary["trust_policy"] == checker.EXPECTED_REQUIRED_PATHS["trust_policy"]
        assert summary["intake_manifest"] == checker.EXPECTED_REQUIRED_PATHS["intake_manifest"]
        assert summary["quarantine_manifest"] == checker.EXPECTED_REQUIRED_PATHS[
            "quarantine_manifest"
        ]
        assert summary["artifact_surface"] == checker.EXPECTED_REQUIRED_PATHS["artifact_surface"]
        assert summary["workflow_surface"] == checker.EXPECTED_REQUIRED_PATHS[
            "workflow_surface"
        ]
        assert summary["checked_in_roots"] == list(checker.EXPECTED_ROOTS)
        assert summary["expected_family_ids"] == list(checker.EXPECTED_FAMILY_IDS)
        assert summary["artifact_root"] == checker.EXPECTED_ARTIFACT_ROOT
        assert summary["report_root"] == checker.EXPECTED_REPORT_ROOT
        assert summary["checked_path_count"] == len(summary["checked_paths"])
    finally:
        checker.SUMMARY_PATH.unlink(missing_ok=True)


def test_external_validation_source_surface_rejects_path_drift(tmp_path: Path) -> None:
    checker = _load_checker()
    surface = load_json_object(checker.SOURCE_SURFACE)
    surface["trust_policy"] = (
        "tests/tooling/fixtures/external_validation/legacy_trust_policy.json"
    )

    checker.SOURCE_SURFACE = tmp_path / "source_surface.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_json_file(checker.SOURCE_SURFACE, surface, sort_keys=True)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_external_validation_source_surface_rejects_checked_root_drift(
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    surface = load_json_object(checker.SOURCE_SURFACE)
    surface["checked_in_roots"] = [
        *checker.EXPECTED_ROOTS,
        "tests/tooling/fixtures/external_validation_compat",
    ]

    checker.SOURCE_SURFACE = tmp_path / "source_surface.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_json_file(checker.SOURCE_SURFACE, surface, sort_keys=True)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_external_validation_source_surface_rejects_family_inventory_drift(
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    surface = load_json_object(checker.SOURCE_SURFACE)
    source_families = list(surface["source_families"])
    surface["source_families"] = [source_families[1], source_families[0], source_families[2]]

    checker.SOURCE_SURFACE = tmp_path / "source_surface.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_json_file(checker.SOURCE_SURFACE, surface, sort_keys=True)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_external_validation_source_surface_rejects_family_path_drift(
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    surface = load_json_object(checker.SOURCE_SURFACE)
    source_families = []
    for family in surface["source_families"]:
        copied_family = dict(family)
        copied_family["source_paths"] = list(family["source_paths"])
        source_families.append(copied_family)
    source_families[0]["source_paths"].append(
        "tests/tooling/fixtures/external_validation/legacy_surface.json"
    )
    surface["source_families"] = source_families

    checker.SOURCE_SURFACE = tmp_path / "source_surface.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_json_file(checker.SOURCE_SURFACE, surface, sort_keys=True)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()
