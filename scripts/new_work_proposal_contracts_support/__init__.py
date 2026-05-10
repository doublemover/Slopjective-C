"""Governance new-work proposal contract internals."""

from __future__ import annotations

from .model import (
    JsonObject,
    ProposalPolicyInputs,
    ProposalRenderResult,
    ProposalValidationResult,
)
from .publication import build_publication_summary
from .rendering import (
    build_issue_payload,
    render_issue_body,
    render_issue_title,
    render_proposal_artifacts,
)
from .validation import (
    MILESTONE_CODE_RE,
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
