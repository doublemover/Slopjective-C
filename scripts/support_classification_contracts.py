from __future__ import annotations

from support_classification.constants import CANONICAL_SUPPORT_CLASSES
from support_classification.constants import SUMMARY_CONTRACT_ID
from support_classification.constants import SUMMARY_SCRIPT_PATH
from support_classification.models import ContractError
from support_classification.models import SupportClassificationReport
from support_classification.models import SupportClassificationSource
from support_classification.rendering import render_support_classification_markdown
from support_classification.reporting import build_support_classification_report
from support_classification.summary import build_support_classification_summary
from support_classification.validation import classify_surfaces
from support_classification.validation import normalize_checked_in_path
from support_classification.validation import require_existing_contract_path
from support_classification.validation import require_list
from support_classification.validation import require_string
from support_classification.validation import validate_demotion_triggers
from support_classification.validation import validate_evidence_families
from support_classification.validation import validate_public_claim_surfaces
from support_classification.validation import validate_support_classes
from support_classification.validation import validate_surface_paths

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
    "validate_surface_paths",
]
