"""Paths used by the security posture builder."""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SOURCE_CHECK = ROOT / "scripts" / "check_security_hardening_source_surface.py"
SCHEMA_CHECK = ROOT / "scripts" / "check_security_hardening_schema_surface.py"
RESPONSE_SUMMARY_BUILD = ROOT / "scripts" / "build_security_hardening_response_policy_summary.py"
MACRO_SUMMARY_BUILD = ROOT / "scripts" / "build_security_hardening_macro_trust_policy_summary.py"
RELEASE_KEY_SUMMARY_BUILD = ROOT / "scripts" / "build_security_hardening_release_key_policy_summary.py"
ARTIFACT_CONTRACT_SUMMARY_BUILD = ROOT / "scripts" / "build_security_hardening_artifact_contract_summary.py"
SUPPLY_CHAIN_AUDIT = ROOT / "scripts" / "check_security_hardening_supply_chain_audit.py"
RUNTIME_HARDENING_CHECK = ROOT / "scripts" / "check_security_hardening_runtime_hardening.py"
SOURCE_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "source-surface-summary.json"
SCHEMA_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "schema-surface-summary.json"
RESPONSE_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "response-policy" / "response_policy_summary.json"
MACRO_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "macro-trust-policy" / "macro_trust_policy_summary.json"
RELEASE_KEY_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "release-key-policy" / "release_key_policy_summary.json"
ARTIFACT_CONTRACT_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "artifact-contract" / "artifact_contract_summary.json"
SUPPLY_CHAIN_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "supply-chain-audit-summary.json"
RUNTIME_HARDENING_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "runtime-hardening-summary.json"
DISTRIBUTION_TRUST_REPORT = ROOT / "tmp" / "artifacts" / "distribution-credibility" / "report" / "objc3c-distribution-trust-report.json"
POSTURE_PATH = ROOT / "tmp" / "artifacts" / "security-hardening" / "posture" / "objc3c-security-posture.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "security-hardening" / "security-posture-summary.json"
