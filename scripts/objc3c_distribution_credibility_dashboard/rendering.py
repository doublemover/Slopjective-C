from __future__ import annotations

from datetime import datetime, timezone
from typing import Any

from objc3c_distribution_credibility_dashboard.model import DistributionCredibilityDashboardModel
from objc3c_distribution_credibility_dashboard.paths import SUMMARY_CONTRACT_ID


def dashboard_summary_payload(
    model: DistributionCredibilityDashboardModel,
    *,
    generated_at_utc: datetime | None = None,
) -> dict[str, Any]:
    generated_at = generated_at_utc or datetime.now(timezone.utc)
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": generated_at.isoformat(),
        "status": model.status,
        "trust_state": model.trust_state,
        "release_id": model.release_id,
        "release_version": model.release_version,
        "warning_count": model.warning_count,
        "upstream_reports": model.upstream_reports,
        "trust_signals": model.trust_signals,
        "required_drill_steps": model.required_drill_steps,
        "install_docs": model.install_docs,
        "release_docs": model.release_docs,
        "operator_states": model.operator_states,
        "operator_actions": model.operator_actions,
        "release_drill_policy": model.release_drill_policy,
        "workflow_surface": model.workflow_surface,
        "dashboard_schema": model.dashboard_schema,
        "trust_report_schema": model.trust_report_schema,
        "artifact_surface": model.artifact_surface,
        "failures": model.failures,
    }
