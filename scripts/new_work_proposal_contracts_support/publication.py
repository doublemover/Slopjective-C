"""Publication summary shaping for new-work proposals."""

from __future__ import annotations

from typing import Any, Mapping

from .model import JsonObject


def build_publication_summary(
    *,
    template: Mapping[str, Any],
    proposal: Mapping[str, Any],
    resolved: Mapping[str, Any],
    proposal_path: str,
    template_path: str,
    publication_mode: str,
    preflight_mode: str,
    title: str,
    preflight_results: list[JsonObject],
    output_paths: JsonObject,
    published_issue_number: int | None,
    failures: list[str],
) -> JsonObject:
    return {
        "mode": template["mode"],
        "contract_id": template["contract_id"],
        "proposal_path": proposal_path,
        "template_path": template_path,
        "publication_mode": publication_mode,
        "preflight_mode": preflight_mode,
        "consumed_contract_ids": template["consumed_contract_ids"],
        "issue_code": proposal.get("issue_code"),
        "title": title,
        "labels": proposal.get("label_names", []),
        "milestone_title": proposal.get("milestone_title"),
        "budget_impact": proposal.get("budget_impact"),
        "validation_posture": proposal.get("validation_posture"),
        "waiver_id": proposal.get("waiver_id"),
        "waiver_status": resolved.get("waiver_status"),
        "governance_preflight": preflight_results,
        "output_paths": output_paths,
        "published_issue_number": published_issue_number,
        "ok": not failures,
        "failures": failures,
    }
