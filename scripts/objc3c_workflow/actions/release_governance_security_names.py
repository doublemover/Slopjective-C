"""Security-hardening action names and hard-cutover guardrails."""

from __future__ import annotations

CHECK_SECURITY_HARDENING_SURFACE = "check-security-hardening-surface"
CHECK_SECURITY_HARDENING_SCHEMA_SURFACE = "check-security-hardening-schema-surface"
CHECK_SECURITY_SANITIZER_VALIDATION = "check-security-sanitizer-validation"
CHECK_SECURITY_LANGUAGE_RUNTIME_THREAT_MODEL = "check-security-language-runtime-threat-model"
CHECK_SECURITY_RESPONSE_DRILL = "check-security-response-drill"
CHECK_SECURITY_RUNTIME_HARDENING = "check-security-runtime-hardening"
BUILD_SECURITY_POSTURE = "build-security-posture"
PUBLISH_SECURITY_ADVISORIES = "publish-security-advisories"
VALIDATE_SECURITY_HARDENING = "validate-security-hardening"
VALIDATE_SECURITY_HARDENING_END_TO_END = "validate-security-hardening-end-to-end"

SECURITY_HARDENING_HARD_CUTOVER_GUARDRAILS: tuple[tuple[str, object], ...] = (
    ("evidence_log_security_proof_allowed", False),
    ("generated_report_capability_truth_allowed", False),
    ("local_tabletop_capability_truth_allowed", False),
    ("retired_route_claims_allowed", False),
    ("trust_bypass_claims_allowed", False),
    ("wrapper_only_security_actions_allowed", False),
    ("security_positive_claim_requires_source_contract", True),
    ("security_positive_claim_requires_runtime_or_release_evidence", True),
)

__all__ = [
    "BUILD_SECURITY_POSTURE",
    "CHECK_SECURITY_HARDENING_SCHEMA_SURFACE",
    "CHECK_SECURITY_HARDENING_SURFACE",
    "CHECK_SECURITY_LANGUAGE_RUNTIME_THREAT_MODEL",
    "CHECK_SECURITY_RESPONSE_DRILL",
    "CHECK_SECURITY_RUNTIME_HARDENING",
    "CHECK_SECURITY_SANITIZER_VALIDATION",
    "PUBLISH_SECURITY_ADVISORIES",
    "SECURITY_HARDENING_HARD_CUTOVER_GUARDRAILS",
    "VALIDATE_SECURITY_HARDENING",
    "VALIDATE_SECURITY_HARDENING_END_TO_END",
]
