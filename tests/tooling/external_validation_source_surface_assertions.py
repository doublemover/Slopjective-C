from __future__ import annotations

from types import ModuleType
from typing import Any


def assert_owner_modules_match_checker(checker: ModuleType) -> None:
    import external_validation_source_surface.paths as paths
    import external_validation_source_surface.publication as publication
    import external_validation_source_surface.rendering as rendering
    import external_validation_source_surface.source_model as source_model
    import external_validation_source_surface.validation as validation

    assert checker.ROOT == paths.ROOT
    assert checker.SOURCE_SURFACE == paths.SOURCE_SURFACE
    assert checker.SUMMARY_PATH == paths.SUMMARY_PATH
    assert checker.SUMMARY_CONTRACT_ID == source_model.SUMMARY_CONTRACT_ID
    assert callable(validation.validate_source_surface)
    assert callable(rendering.render_summary)
    assert callable(publication.publish_summary)


def assert_named_summary_fields(summary: dict[str, Any], checker: ModuleType) -> None:
    assert summary["contract_id"] == checker.SUMMARY_CONTRACT_ID
    assert summary["status"] == "PASS"
    assert summary["source_surface"] == (
        "tests/tooling/fixtures/external_validation/source_surface.json"
    )
    assert summary["runbook"] == checker.EXPECTED_REQUIRED_PATHS["runbook"]
    assert summary["source_root"] == checker.EXPECTED_REQUIRED_PATHS["source_root"]
    assert summary["source_readme"] == checker.EXPECTED_REQUIRED_PATHS["source_readme"]
    assert (
        summary["source_check_script"]
        == checker.EXPECTED_REQUIRED_PATHS["source_check_script"]
    )
    assert summary["trust_policy"] == checker.EXPECTED_REQUIRED_PATHS["trust_policy"]
    assert (
        summary["intake_manifest"]
        == checker.EXPECTED_REQUIRED_PATHS["intake_manifest"]
    )
    assert (
        summary["quarantine_manifest"]
        == checker.EXPECTED_REQUIRED_PATHS["quarantine_manifest"]
    )
    assert summary["repro_corpus"] == checker.EXPECTED_REQUIRED_PATHS["repro_corpus"]
    assert (
        summary["support_claim_gate"]
        == checker.EXPECTED_REQUIRED_PATHS["support_claim_gate"]
    )
    assert (
        summary["support_claim_gate_script"]
        == checker.EXPECTED_REQUIRED_PATHS["support_claim_gate_script"]
    )
    assert (
        summary["artifact_surface"]
        == checker.EXPECTED_REQUIRED_PATHS["artifact_surface"]
    )
    assert (
        summary["workflow_surface"]
        == checker.EXPECTED_REQUIRED_PATHS["workflow_surface"]
    )
    assert summary["checked_in_roots"] == list(checker.EXPECTED_ROOTS)
    assert summary["expected_family_ids"] == list(checker.EXPECTED_FAMILY_IDS)
    assert summary["artifact_root"] == checker.EXPECTED_ARTIFACT_ROOT
    assert summary["report_root"] == checker.EXPECTED_REPORT_ROOT
    assert summary["checked_path_count"] == len(summary["checked_paths"])


def assert_fail_closed_without_summary(checker: ModuleType) -> None:
    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()
