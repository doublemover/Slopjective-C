"""Constants and claim-matching patterns for public claim drift checks."""

from __future__ import annotations

import re

SUMMARY_CONTRACT_ID = "objc3c.public.claim.drift.summary.v1"
PUBLIC_CLAIM_DRIFT_OWNER = "public-claim-drift.owner"
PUBLIC_CLAIM_DRIFT_OWNER_SURFACE = "scripts/check_objc3c_public_claim_drift.py"
PUBLIC_CLAIM_DRIFT_BLOCKER_METADATA = {
    "blocker_contract": "hard-cutover-public-claim-drift-fail-closed",
    "blocker_scope": "public-claim-surfaces-and-support-evidence-mapping",
    "blocker_owner": PUBLIC_CLAIM_DRIFT_OWNER,
    "blocker_owner_surface": PUBLIC_CLAIM_DRIFT_OWNER_SURFACE,
}

CLAIM_KEYWORD_RE = re.compile(
    r"\b("
    r"claim|claimable|support|supported|supports|validated|validation|"
    r"evidence|proof|runtime|release-ready|production-strength|stable|"
    r"complete|full|runnable|live"
    r")\b",
    re.IGNORECASE,
)
PROMOTIONAL_CLAIM_RE = re.compile(
    r"\b("
    r"supported|supports|claimable|release-ready|production-strength|"
    r"stable|complete|full|validated|runnable|live"
    r")\b",
    re.IGNORECASE,
)
GUARD_RE = re.compile(
    r"\b("
    r"incomplete|not fully|not supported|unsupported|fail-closed|outside|"
    r"non-goal|non-goals|do not|does not|must not|cannot|deferred|later|"
    r"narrower|limited to|not yet|still incomplete|no blanket claim|"
    r"no widening|no public|no hosted|no cross-platform|no manual|"
    r"do not claim|must stay|remains unsupported"
    r")\b",
    re.IGNORECASE,
)
FORBIDDEN_PATTERNS: tuple[tuple[str, re.Pattern[str]], ...] = (
    (
        "full-live-object-model-runtime",
        re.compile(r"\bfull live objective-c 3\.0 object-model runtime behavior\b", re.IGNORECASE),
    ),
    (
        "full-property-ivar-reflection-closure",
        re.compile(r"\bfull property/ivar/runtime reflection closure\b", re.IGNORECASE),
    ),
    (
        "full-escaping-block-byref-arc",
        re.compile(r"\bfull escaping block/byref (runtime behavior|and arc automation)\b", re.IGNORECASE),
    ),
    (
        "complete-runtime-backed-semantics",
        re.compile(r"\bcomplete runtime-backed (semantics|closure)\b", re.IGNORECASE),
    ),
    (
        "production-strength-completeness-stability",
        re.compile(r"\bproduction-strength completeness and stability claims\b", re.IGNORECASE),
    ),
    (
        "every-behavior-fully-complete",
        re.compile(r"\bevery\b.*\bfully complete\b", re.IGNORECASE),
    ),
    (
        "all-intended-topologies-supported",
        re.compile(r"\ball intended (future )?topolog(?:y|ies).*\bsupported\b", re.IGNORECASE),
    ),
    (
        "universal-foreign-topology-support",
        re.compile(r"\buniversal foreign topolog(?:y|ies) support\b", re.IGNORECASE),
    ),
    (
        "foreign-topologies-supported",
        re.compile(r"\bforeign topolog(?:y|ies).*\bsupported\b", re.IGNORECASE),
    ),
    (
        "full-runtime-abi",
        re.compile(r"\bfull runtime abi\b", re.IGNORECASE),
    ),
    (
        "runtime-abi-widening",
        re.compile(r"\bruntime abi\b.*\b(foreign|universal|all|widened|widening)\b", re.IGNORECASE),
    ),
    (
        "cross-platform-support",
        re.compile(r"\bcross-platform\b.*\bsupport\b", re.IGNORECASE),
    ),
    (
        "indefinite-or-cross-major-support",
        re.compile(r"\b(indefinite support|cross-major forward compatibility)\b", re.IGNORECASE),
    ),
    (
        "hosted-release-service",
        re.compile(r"\b(hosted updater|hosted update service|hosted trust service|hosted artifact registry)\b", re.IGNORECASE),
    ),
)

__all__ = [
    "CLAIM_KEYWORD_RE",
    "FORBIDDEN_PATTERNS",
    "GUARD_RE",
    "PROMOTIONAL_CLAIM_RE",
    "PUBLIC_CLAIM_DRIFT_BLOCKER_METADATA",
    "PUBLIC_CLAIM_DRIFT_OWNER",
    "PUBLIC_CLAIM_DRIFT_OWNER_SURFACE",
    "SUMMARY_CONTRACT_ID",
]
