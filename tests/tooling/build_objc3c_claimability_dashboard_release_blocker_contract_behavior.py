from __future__ import annotations

from build_objc3c_claimability_dashboard_release_blocker_contract_support import (
    builder,
)


def assert_dashboard_release_blocker_projection_payload(
    payload: dict[str, object],
) -> None:
    assert payload["contract_id"] == builder.SUMMARY_CONTRACT_ID
    assert payload["status"] == "PASS"
    assert payload["dashboard_release_blocker_projection"]["blocker"] == (
        "claimability-dashboard-not-production-strength"
    )
    assert payload["checks"]["dashboard_script_consumes_projection"] is True
    assert payload["checks"]["release_blocker_script_emits_projection"] is True
    assert payload["checks"]["projection_requires_public_summary_decision_fields"] is True
    assert payload["checks"]["projection_has_source_owned_decision_fields"] is True
    assert payload["checks"]["source_truth_excludes_tmp"] is True


def assert_dashboard_release_blocker_drift_rejection(stderr: str) -> None:
    assert "dashboard release-blocker contract output drift" in stderr
