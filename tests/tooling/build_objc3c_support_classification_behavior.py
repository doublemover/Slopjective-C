from __future__ import annotations

from build_objc3c_support_classification_support import builder


def assert_positive_fixture_summary_payload(payload: dict[str, object]) -> None:
    assert payload["contract_id"] == builder.SUMMARY_CONTRACT_ID
    assert payload["status"] == "PASS"
    assert payload["support_class_count"] == 4
    assert payload["evidence_family_count"] == 1
    assert payload["classification_count"] == 2
    assert payload["class_counts"] == {"supported": 1, "unsupported": 1}
    assert payload["tmp_source_truth_paths"] == []
    assert payload["checks"]["source_truth_excludes_tmp"] is True
    assert "fixture-unsupported-surface" in payload["fail_closed_surfaces"]


def assert_positive_fixture_report_text(report_text: str) -> None:
    assert "# Objective-C 3.0 Support Classification Summary" in report_text
    assert (
        "| fixture-supported-surface | supported | production-claimable | fixture-evidence |"
        in report_text
    )


def assert_unknown_surface_class_rejection(stderr: str) -> None:
    assert "claimed-without-evidence" in stderr


def assert_default_contract_summary_payload(payload: dict[str, object]) -> None:
    assert payload["source_contract_id"] == (
        "objc3c.full_envelope.claimability.support.matrix.claim.taxonomy.v1"
    )
    assert payload["summary_script"] == "scripts/build_objc3c_support_classification.py"
    assert payload["legacy_support_matrix_summary_script"] == (
        "scripts/build_full_envelope_claimability_support_matrix_summary.py"
    )
    assert payload["checks"]["canonical_support_classes_defined"] is True
    assert payload["checks"]["source_truth_excludes_tmp"] is True
    assert payload["tmp_source_truth_paths"] == []
    assert payload["class_counts"]["supported"] >= 1
    assert payload["class_counts"]["unsupported"] >= 1


def assert_check_mode_drift_rejection(stderr: str) -> None:
    assert "support classification output drift" in stderr
