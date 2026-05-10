"""Stable contract constants for governance sustainability evidence."""

from __future__ import annotations


CONTRACT_ID = "objc3c.governance.sustainability.evidence.v1"
SUPPORT_STATE = "stable-governance-process"
PACKAGE_BRIDGE = "objc3c"
PUBLIC_ACTIONS = [
    "validate-governance-sustainability",
    "publish-governance-sustainability",
]
OWNER_SPLIT = {
    "budget_inventory": (
        "tests/tooling/fixtures/governance_sustainability/budget_inventory.json"
    ),
    "budget_policy": (
        "tests/tooling/fixtures/governance_sustainability/"
        "sustainable_progress_policy.json"
    ),
    "waiver_registry": (
        "tests/tooling/fixtures/governance_sustainability/waiver_registry.json"
    ),
    "stewardship": (
        "tests/tooling/fixtures/governance_sustainability/stewardship_semantics.json"
    ),
    "extension_review": (
        "tests/tooling/fixtures/governance_sustainability/extension_review_policy.json"
    ),
    "anti_regression": (
        "tests/tooling/fixtures/governance_sustainability/"
        "anti_regression_reporting_contract.json"
    ),
    "metadata_publication": (
        "scripts/publish_objc3c_governance_sustainability_metadata.py"
    ),
}
OWNER_CONTRACTS = {
    "waiver_owner": {
        "source_contract": OWNER_SPLIT["waiver_registry"],
        "artifact_section": "budget.waiver_registry",
        "publication_projection": "stewardship_publication.budget.waiver_registry",
        "blocker_projection": "claim_audit.blocker_metadata.waiver_registry",
    },
    "budget_owner": {
        "source_contract": OWNER_SPLIT["budget_inventory"],
        "artifact_section": "budget.inventory_summary",
        "publication_projection": "stewardship_publication.budget.inventory_summary",
        "blocker_projection": "claim_audit.blocker_metadata.budget_inventory",
    },
    "stewardship_owner": {
        "source_contract": OWNER_SPLIT["stewardship"],
        "artifact_section": "stewardship",
        "publication_projection": "stewardship_publication.stewardship",
        "blocker_projection": "claim_audit.blocker_metadata.stewardship",
    },
    "extension_review_owner": {
        "source_contract": OWNER_SPLIT["extension_review"],
        "artifact_section": "extension_review",
        "publication_projection": "extension_review_publication.extension_review",
        "blocker_projection": "claim_audit.blocker_metadata.extension_review",
    },
    "sustainable_progress_owner": {
        "source_contract": OWNER_SPLIT["budget_policy"],
        "artifact_section": "stewardship.policy_summary",
        "publication_projection": "stewardship_publication.stewardship.policy_summary",
        "blocker_projection": (
            "claim_audit.blocker_metadata.sustainable_progress_policy"
        ),
    },
    "publication_owner": {
        "source_contract": OWNER_SPLIT["metadata_publication"],
        "artifact_section": "metadata_publication",
        "publication_projection": "stewardship_publication.metadata_publication",
        "blocker_projection": "claim_audit.blocker_metadata.metadata_publication",
    },
}
BLOCKER_METADATA = {
    "waiver_registry": {
        "owner": "waiver_owner",
        "blocked_when": (
            "expired, unowned, duplicate active, or evidence-free waiver is present"
        ),
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "budget_inventory": {
        "owner": "budget_owner",
        "blocked_when": (
            "public surface growth is not measured or exceeds a release-blocking "
            "budget"
        ),
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "stewardship": {
        "owner": "stewardship_owner",
        "blocked_when": (
            "review roles, entry surfaces, or required checks lose checked-in "
            "ownership"
        ),
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "extension_review": {
        "owner": "extension_review_owner",
        "blocked_when": (
            "new work proposal lacks review class, evidence surfaces, owner, "
            "revert path, or publication plan"
        ),
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "sustainable_progress_policy": {
        "owner": "sustainable_progress_owner",
        "blocked_when": (
            "new work lacks measured budget delta, owner, exception status, or "
            "follow-on ratchet"
        ),
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "metadata_publication": {
        "owner": "publication_owner",
        "blocked_when": (
            "publication presents governance stability wider than generated evidence"
        ),
        "release_blocker_field": "claim_audit.release_blockers",
    },
}


__all__ = [
    "BLOCKER_METADATA",
    "CONTRACT_ID",
    "OWNER_CONTRACTS",
    "OWNER_SPLIT",
    "PACKAGE_BRIDGE",
    "PUBLIC_ACTIONS",
    "SUPPORT_STATE",
]
