"""Support classification contract helpers."""

from __future__ import annotations

from .models import ContractError
from .models import SupportClassificationReport
from .models import SupportClassificationSource
from .reporting import build_support_classification_report
from .rendering import render_support_classification_markdown
from .summary import build_support_classification_summary
from .validation import classify_surfaces
from .validation import normalize_checked_in_path
from .validation import require_existing_contract_path
from .validation import require_list
from .validation import require_string
from .validation import validate_demotion_triggers
from .validation import validate_evidence_families
from .validation import validate_public_claim_surfaces
from .validation import validate_support_classes
from .constants import CANONICAL_SUPPORT_CLASSES
from .constants import SUMMARY_CONTRACT_ID
from .constants import SUMMARY_SCRIPT_PATH

__all__ = [
    "CANONICAL_SUPPORT_CLASSES",
    "SUMMARY_CONTRACT_ID",
    "SUMMARY_SCRIPT_PATH",
    "ContractError",
    "SupportClassificationReport",
    "SupportClassificationSource",
    "build_support_classification_report",
    "build_support_classification_summary",
    "classify_surfaces",
    "normalize_checked_in_path",
    "render_support_classification_markdown",
    "require_existing_contract_path",
    "require_list",
    "require_string",
    "validate_demotion_triggers",
    "validate_evidence_families",
    "validate_public_claim_surfaces",
    "validate_support_classes",
]
