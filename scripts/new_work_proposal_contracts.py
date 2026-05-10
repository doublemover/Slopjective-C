"""Governance new-work proposal contract shaping."""

from __future__ import annotations

import re
from dataclasses import dataclass
from typing import Any, Mapping


JsonObject = dict[str, Any]
MILESTONE_CODE_RE = re.compile(r"^(M\d{3})")


@dataclass(frozen=True)
class ProposalPolicyInputs:
    template: Mapping[str, Any]
    governance_policy: Mapping[str, Any]
    waiver_registry: Mapping[str, Any]
    extension_policy: Mapping[str, Any]


@dataclass(frozen=True)
class ProposalValidationResult:
    resolved: JsonObject
    failures: list[str]


@dataclass(frozen=True)
class ProposalRenderResult:
    title: str
    body: str
    payload: JsonObject


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


def render_issue_title(
    proposal: Mapping[str, Any],
    template: Mapping[str, Any],
    resolved: Mapping[str, Any],
) -> str:
    return template["title_format"].format(
        milestone_code=resolved["milestone_code"],
        lane=resolved["lane"],
        short_code=resolved["short_code"],
        title_core=proposal["title_core"],
    )


def render_issue_body(proposal: Mapping[str, Any]) -> str:
    lines = [
        "## Outcome",
        (
            f"Deliver `{proposal['issue_code']}` for **{proposal['title_core']}** "
            f"within the milestone focus: {proposal['milestone_focus_summary']}"
        ),
        "",
        "## Why this matters",
        proposal["why_it_matters"],
    ]
    if proposal.get("design_corrections_folded_in"):
        lines += ["", "## Design corrections folded in"] + [
            f"- {item}" for item in proposal["design_corrections_folded_in"]
        ]
    lines += ["", "## Acceptance criteria"] + [
        f"- {item}" for item in proposal["acceptance_criteria"]
    ]
    lines += ["", "## Primary implementation surfaces"] + [
        f"- `{item}`" for item in proposal["primary_implementation_surfaces"]
    ]
    lines += ["", "## Dependencies"]
    lines += (
        [f"- `{item}`" for item in proposal["dependencies"]]
        if proposal["dependencies"]
        else ["- None."]
    )
    lines += [
        "",
        "## Validation posture",
        f"- Class: `{proposal['validation_posture']}`",
        f"- Budget impact: `{proposal['budget_impact']}`",
    ]
    lines += ["", "## Extension review"]
    lines += [
        f"- Review class: `{proposal['review_class']}`",
        (
            "- Support-boundary classification: "
            f"`{proposal['support_boundary_classification']}`"
        ),
        f"- Revert or demotion path: {proposal['revert_or_demote_path']}",
    ]
    lines += ["- Evidence surfaces:"] + [
        f"  - `{item}`" for item in proposal["evidence_surfaces"]
    ]
    if proposal.get("waiver_id"):
        lines.append(f"- Waiver: `{proposal['waiver_id']}`")
    if proposal.get("boundary_note"):
        lines += ["", "## Boundary note", proposal["boundary_note"]]
    if proposal.get("execution_order"):
        order = proposal["execution_order"]
        lines += [
            "",
            "<!-- EXECUTION-ORDER-START -->",
            "## Execution Order",
            f"- Cleanup program slot: `{order['cleanup_program_slot']}`",
            f"- Cleanup program phase: `{order['cleanup_program_phase']}`",
            f"- Global cleanup-first sequence: `{order['global_sequence']}`",
            "- Blocked by prior cleanup milestones: "
            + (
                ", ".join(f"`{item}`" for item in order["blocked_by_prior_milestones"])
                if order["blocked_by_prior_milestones"]
                else "`None`"
            ),
            "- Direct issue blockers: "
            + (
                ", ".join(order["direct_issue_blockers"])
                if order["direct_issue_blockers"]
                else "`None`"
            ),
            "- Directly unblocks: "
            + (
                ", ".join(order["directly_unblocks"])
                if order["directly_unblocks"]
                else "`None`"
            ),
            f"- Execution instruction: {order['instruction']}",
            "<!-- EXECUTION-ORDER-END -->",
        ]
    if proposal.get("notes"):
        lines += ["", "## Notes"] + [f"- {item}" for item in proposal["notes"]]
    return "\n".join(lines) + "\n"


def build_issue_payload(
    *,
    title: str,
    body_path: str,
    proposal: Mapping[str, Any],
) -> JsonObject:
    return {
        "title": title,
        "body_path": body_path,
        "labels": proposal.get("label_names", []),
        "milestone_title": proposal.get("milestone_title"),
        "issue_code": proposal.get("issue_code"),
    }


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


def render_proposal_artifacts(
    proposal: Mapping[str, Any],
    template: Mapping[str, Any],
    resolved: Mapping[str, Any],
    body_path: str,
    failures: list[str],
) -> ProposalRenderResult:
    title = render_issue_title(proposal, template, resolved) if not failures else ""
    body = render_issue_body(proposal) if not failures else ""
    payload = build_issue_payload(title=title, body_path=body_path, proposal=proposal)
    return ProposalRenderResult(title=title, body=body, payload=payload)


__all__ = [
    "ProposalPolicyInputs",
    "ProposalRenderResult",
    "ProposalValidationResult",
    "build_publication_summary",
    "render_proposal_artifacts",
    "validate_proposal",
]
