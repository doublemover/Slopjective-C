"""Public facade for governance new-work proposal contracts."""

from __future__ import annotations

if __package__:
    from .new_work_proposal_contracts_support import (
        MILESTONE_CODE_RE,
        JsonObject,
        ProposalPolicyInputs,
        ProposalRenderResult,
        ProposalValidationResult,
        build_issue_payload,
        build_publication_summary,
        render_issue_body,
        render_issue_title,
        render_proposal_artifacts,
        validate_proposal,
        validate_required_fields,
    )
else:
    from new_work_proposal_contracts_support import (
        MILESTONE_CODE_RE,
        JsonObject,
        ProposalPolicyInputs,
        ProposalRenderResult,
        ProposalValidationResult,
        build_issue_payload,
        build_publication_summary,
        render_issue_body,
        render_issue_title,
        render_proposal_artifacts,
        validate_proposal,
        validate_required_fields,
    )


__all__ = [
    "JsonObject",
    "MILESTONE_CODE_RE",
    "ProposalPolicyInputs",
    "ProposalRenderResult",
    "ProposalValidationResult",
    "build_issue_payload",
    "build_publication_summary",
    "render_issue_body",
    "render_issue_title",
    "render_proposal_artifacts",
    "validate_proposal",
    "validate_required_fields",
]
