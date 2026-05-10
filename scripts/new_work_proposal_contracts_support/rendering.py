"""Issue title, body, and payload rendering for new-work proposals."""

from __future__ import annotations

from typing import Any, Mapping

from .model import JsonObject, ProposalRenderResult


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
