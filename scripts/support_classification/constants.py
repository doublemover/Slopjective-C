"""Support classification contract constants."""

from __future__ import annotations

SUMMARY_CONTRACT_ID = "objc3c.support.classification.generator.summary.v1"
SUMMARY_SCRIPT_PATH = "scripts/build_objc3c_support_classification.py"

CANONICAL_SUPPORT_CLASSES = {
    "supported": {
        "claim_class": "production-claimable",
        "fail_closed": False,
        "release_blocking": False,
    },
    "experimental": {
        "claim_class": "preview-or-candidate-only",
        "fail_closed": False,
        "release_blocking": False,
    },
    "unsupported": {
        "claim_class": "fail-closed",
        "fail_closed": True,
        "release_blocking": False,
    },
    "release-blocking": {
        "claim_class": "blocked",
        "fail_closed": True,
        "release_blocking": True,
    },
}


__all__ = ["CANONICAL_SUPPORT_CLASSES", "SUMMARY_CONTRACT_ID", "SUMMARY_SCRIPT_PATH"]
