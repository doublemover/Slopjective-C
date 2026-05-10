from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from .errors import ContractError
from .input_loading import require_string, require_string_list
from .source_contracts import (
    DASHBOARD_DECISION_FIELDS,
    DASHBOARD_SUMMARY,
    NON_PRODUCTION_PUBLIC_CLAIM_CLASSES,
    PUBLIC_SUMMARY,
    PUBLIC_SUMMARY_DECISION_FIELDS,
    ROLLOUT_CLASS_CANDIDATE,
    ROLLOUT_CLASS_PREVIEW,
)


@dataclass(frozen=True)
class DashboardReleaseBlockerProjection:
    blocker: str
    dashboard_summary_path: str
    public_summary_path: str
    blocking_public_claim_classes: list[str]
    blocking_rollout_classes: list[str]
    required_dashboard_fields: list[str]
    required_public_summary_fields: list[str]
    source_owned_decision_fields: list[str]

    def to_payload(self) -> dict[str, Any]:
        return {
            "blocker": self.blocker,
            "dashboard_summary_path": self.dashboard_summary_path,
            "public_summary_path": self.public_summary_path,
            "blocking_public_claim_classes": self.blocking_public_claim_classes,
            "blocking_rollout_classes": self.blocking_rollout_classes,
            "required_dashboard_fields": self.required_dashboard_fields,
            "required_public_summary_fields": self.required_public_summary_fields,
            "source_owned_decision_fields": self.source_owned_decision_fields,
        }


def parse_projection(payload: dict[str, Any]) -> DashboardReleaseBlockerProjection:
    projection = payload.get("dashboard_release_blocker_projection")
    if not isinstance(projection, dict):
        raise ContractError("policy missing dashboard_release_blocker_projection")
    return DashboardReleaseBlockerProjection(
        blocker=require_string(projection, "blocker"),
        dashboard_summary_path=require_string(projection, "dashboard_summary_path"),
        public_summary_path=require_string(projection, "public_summary_path"),
        blocking_public_claim_classes=require_string_list(
            projection, "blocking_public_claim_classes"
        ),
        blocking_rollout_classes=require_string_list(
            projection, "blocking_rollout_classes"
        ),
        required_dashboard_fields=require_string_list(
            projection, "required_dashboard_fields"
        ),
        required_public_summary_fields=require_string_list(
            projection, "required_public_summary_fields"
        ),
        source_owned_decision_fields=require_string_list(
            projection, "source_owned_decision_fields"
        ),
    )


def evaluate_contract_checks(
    projection: DashboardReleaseBlockerProjection,
    *,
    release_blocker_text: str,
    dashboard_text: str,
    runbook_text: str,
    tmp_source_truth_paths: list[str],
) -> dict[str, bool]:
    return {
        "projection_has_blocker_id": projection.blocker
        == "claimability-dashboard-not-production-strength",
        "projection_names_dashboard_outputs": projection.dashboard_summary_path
        == DASHBOARD_SUMMARY
        and projection.public_summary_path
        == PUBLIC_SUMMARY,
        "projection_blocks_non_production_claim_classes": set(
            projection.blocking_public_claim_classes
        )
        == set(NON_PRODUCTION_PUBLIC_CLAIM_CLASSES),
        "projection_blocks_non_stable_rollouts": set(
            projection.blocking_rollout_classes
        )
        == {ROLLOUT_CLASS_PREVIEW, ROLLOUT_CLASS_CANDIDATE},
        "projection_requires_dashboard_decision_fields": set(
            DASHBOARD_DECISION_FIELDS
        ).issubset(projection.required_dashboard_fields),
        "projection_requires_public_summary_decision_fields": set(
            PUBLIC_SUMMARY_DECISION_FIELDS
        ).issubset(projection.required_public_summary_fields),
        "projection_has_source_owned_decision_fields": set(
            DASHBOARD_DECISION_FIELDS
        ).issubset(projection.source_owned_decision_fields)
        and set(PUBLIC_SUMMARY_DECISION_FIELDS).issubset(
            projection.source_owned_decision_fields
        ),
        "release_blocker_script_emits_projection": all(
            marker in release_blocker_text
            for marker in (
                "build_dashboard_release_blocker_projection",
                "dashboard_release_blocker_projection",
                "blocks_production_strength_claim",
            )
        ),
        "dashboard_script_consumes_projection": all(
            marker in dashboard_text
            for marker in (
                "dashboard_release_blocker_projection",
                "release blocker summary missing dashboard_release_blocker_projection",
                "dashboard release-blocker projection public claim class drifted",
            )
        ),
        "runbook_documents_dashboard_blocker_projection": all(
            marker in runbook_text
            for marker in (
                "dashboard release-blocker projection",
                "`dashboard_release_blocker_projection`",
                "blocks production-strength release claims",
            )
        ),
        "source_truth_excludes_tmp": not tmp_source_truth_paths,
    }
