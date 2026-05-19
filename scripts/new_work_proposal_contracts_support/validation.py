"""Validation rules for new-work proposal publication."""

from __future__ import annotations

import re
from typing import Any, Mapping

from .model import JsonObject, ProposalPolicyInputs, ProposalValidationResult


MILESTONE_CODE_RE = re.compile(r"^(M\d{3})")


def validate_required_fields(
    proposal: Mapping[str, Any],
    template: Mapping[str, Any],
) -> list[str]:
    failures: list[str] = []
    for field in template["required_issue_fields"]:
        if field not in proposal:
            failures.append(f"missing required proposal field `{field}`")
    return failures


def validate_proposal(
    proposal: Mapping[str, Any],
    policies: ProposalPolicyInputs,
) -> ProposalValidationResult:
    template = policies.template
    failures = validate_required_fields(proposal, template)
    allowed_fields = set(template["required_issue_fields"]) | set(
        template["optional_issue_fields"]
    )
    for key in proposal:
        if key not in allowed_fields:
            failures.append(f"unknown proposal field `{key}`")

    issue_code = str(proposal.get("issue_code", ""))
    resolved: JsonObject = {
        "milestone_code": "",
        "short_code": "",
        "lane": "",
        "waiver_status": None,
    }
    if not re.match(template["issue_code_pattern"], issue_code):
        failures.append("issue_code does not match required Mxxx-Lnnn pattern")
    else:
        milestone_code, short_code = issue_code.split("-", 1)
        lane = short_code[0]
        resolved.update(
            {
                "milestone_code": milestone_code,
                "short_code": short_code,
                "lane": lane,
            }
        )
        if lane not in "ABCDE":
            failures.append("issue_code lane must be A-E")

    if proposal.get("validation_posture") not in set(template["validation_postures"]):
        failures.append("validation_posture is not allowed by template")
    if proposal.get("budget_impact") not in set(template["budget_impact_options"]):
        failures.append("budget_impact is not allowed by template")
    if (
        not isinstance(proposal.get("acceptance_criteria"), list)
        or not proposal["acceptance_criteria"]
    ):
        failures.append("acceptance_criteria must be a non-empty list")
    if (
        not isinstance(proposal.get("primary_implementation_surfaces"), list)
        or not proposal["primary_implementation_surfaces"]
    ):
        failures.append("primary_implementation_surfaces must be a non-empty list")
    if not isinstance(proposal.get("dependencies"), list):
        failures.append("dependencies must be a list")
    if (
        not isinstance(proposal.get("label_names"), list)
        or not proposal["label_names"]
    ):
        failures.append("label_names must be a non-empty list")
    if (
        not isinstance(proposal.get("evidence_surfaces"), list)
        or not proposal["evidence_surfaces"]
    ):
        failures.append("evidence_surfaces must be a non-empty list")

    if "issue_numbers" in proposal or "milestone_numbers" in proposal:
        failures.append("proposal must not carry predicted GitHub numeric identifiers")
    forbidden_source_keys = {"issue_numbers", "milestone_numbers"}
    if forbidden_source_keys & set(proposal.keys()):
        failures.append("proposal includes forbidden source keys")

    if proposal.get("budget_impact") not in set(template["budget_impact_options"]):
        failures.append("budget impact not allowed by template")
    waiver_id = proposal.get("waiver_id")
    if proposal.get("budget_impact") == "requires_exception_record":
        if not waiver_id:
            failures.append(
                "waiver_id is required when budget_impact is requires_exception_record"
            )
        else:
            match = next(
                (
                    item
                    for item in policies.waiver_registry.get("waivers", [])
                    if item.get("waiver_id") == waiver_id
                ),
                None,
            )
            if match is None:
                failures.append(f"waiver `{waiver_id}` was not found in the waiver registry")
            else:
                resolved["waiver_status"] = match.get("status")
                if match.get("status") != "active":
                    failures.append(f"waiver `{waiver_id}` is not active")
    elif waiver_id:
        match = next(
            (
                item
                for item in policies.waiver_registry.get("waivers", [])
                if item.get("waiver_id") == waiver_id
            ),
            None,
        )
        resolved["waiver_status"] = match.get("status") if match else "missing"

    if proposal.get("milestone_title"):
        milestone_code = resolved["milestone_code"]
        title_code = MILESTONE_CODE_RE.match(str(proposal["milestone_title"]))
        if title_code and title_code.group(1) != milestone_code:
            failures.append("milestone_title code must match issue_code milestone")

    review_classes = {
        str(entry.get("review_class")): entry
        for entry in policies.extension_policy.get("review_classes", [])
        if isinstance(entry, dict)
    }
    review_class = str(proposal.get("review_class", ""))
    selected_review_class = review_classes.get(review_class)
    if selected_review_class is None:
        failures.append("review_class is not allowed by extension review policy")
    else:
        required_surfaces = {
            str(path)
            for path in selected_review_class.get("required_evidence_surfaces", [])
        }
        proposal_surfaces = {str(path) for path in proposal.get("evidence_surfaces", [])}
        missing_surfaces = sorted(required_surfaces - proposal_surfaces)
        if missing_surfaces:
            failures.append(
                "evidence_surfaces missing required review surfaces: "
                + ", ".join(missing_surfaces)
            )
    if not str(proposal.get("support_boundary_classification", "")).strip():
        failures.append("support_boundary_classification is required")
    if not str(proposal.get("revert_or_demote_path", "")).strip():
        failures.append("revert_or_demote_path is required")

    execution_order = proposal.get("execution_order")
    if execution_order is not None:
        if not isinstance(execution_order, dict):
            failures.append("execution_order must be an object when present")
        else:
            missing = [
                field
                for field in template["execution_order_fields"]
                if field not in execution_order
            ]
            if missing:
                failures.append(
                    "execution_order is missing required fields: " + ", ".join(missing)
                )

    if not policies.governance_policy.get("exception_requirements", {}).get(
        "expiry_required", False
    ):
        failures.append("governance policy drifted: expiry requirement must stay enabled")
    return ProposalValidationResult(resolved=resolved, failures=failures)
