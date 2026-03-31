#!/usr/bin/env python3
"""Build the machine-owned objc3c security posture from live hardening evidence."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
SOURCE_CHECK = ROOT / "scripts" / "check_security_hardening_source_surface.py"
SCHEMA_CHECK = ROOT / "scripts" / "check_security_hardening_schema_surface.py"
RESPONSE_SUMMARY_BUILD = ROOT / "scripts" / "build_security_hardening_response_policy_summary.py"
MACRO_SUMMARY_BUILD = ROOT / "scripts" / "build_security_hardening_macro_trust_policy_summary.py"
RELEASE_KEY_SUMMARY_BUILD = ROOT / "scripts" / "build_security_hardening_release_key_policy_summary.py"
ARTIFACT_CONTRACT_SUMMARY_BUILD = ROOT / "scripts" / "build_security_hardening_artifact_contract_summary.py"
SUPPLY_CHAIN_AUDIT = ROOT / "scripts" / "check_security_hardening_supply_chain_audit.py"
SOURCE_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "source-surface-summary.json"
SCHEMA_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "schema-surface-summary.json"
RESPONSE_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "response-policy" / "response_policy_summary.json"
MACRO_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "macro-trust-policy" / "macro_trust_policy_summary.json"
RELEASE_KEY_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "release-key-policy" / "release_key_policy_summary.json"
ARTIFACT_CONTRACT_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "artifact-contract" / "artifact_contract_summary.json"
SUPPLY_CHAIN_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "supply-chain-audit-summary.json"
DISTRIBUTION_TRUST_REPORT = ROOT / "tmp" / "artifacts" / "distribution-credibility" / "report" / "objc3c-distribution-trust-report.json"
POSTURE_PATH = ROOT / "tmp" / "artifacts" / "security-hardening" / "posture" / "objc3c-security-posture.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "security-hardening" / "security-posture-summary.json"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def run_capture(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(list(command), cwd=ROOT, check=False, text=True, capture_output=True)
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def ensure_success(path: Path, script: Path) -> dict[str, Any]:
    if not path.is_file():
        result = run_capture([sys.executable, str(script)])
        if result.returncode != 0:
            raise RuntimeError(f"failed to build required report via {repo_rel(script)}")
    payload = load_json(path)
    if payload.get("status") not in {"PASS", "OK"} and payload.get("ok") is not True:
        raise RuntimeError(f"required report did not pass: {repo_rel(path)}")
    return payload


def boundary_status_from_summary(summary: dict[str, Any], *, warn_on_degraded: bool = False, trust_state: str | None = None) -> str:
    if summary.get("status") == "FAIL" or summary.get("ok") is False:
        return "FAIL"
    if warn_on_degraded and trust_state == "degraded":
        return "WARN"
    return "PASS"


def main() -> int:
    try:
        source_summary = ensure_success(SOURCE_SUMMARY, SOURCE_CHECK)
        schema_summary = ensure_success(SCHEMA_SUMMARY, SCHEMA_CHECK)
        response_summary = ensure_success(RESPONSE_SUMMARY, RESPONSE_SUMMARY_BUILD)
        macro_summary = ensure_success(MACRO_SUMMARY, MACRO_SUMMARY_BUILD)
        release_key_summary = ensure_success(RELEASE_KEY_SUMMARY, RELEASE_KEY_SUMMARY_BUILD)
        artifact_contract_summary = ensure_success(ARTIFACT_CONTRACT_SUMMARY, ARTIFACT_CONTRACT_SUMMARY_BUILD)
        supply_chain_summary = ensure_success(SUPPLY_CHAIN_SUMMARY, SUPPLY_CHAIN_AUDIT)
    except RuntimeError as exc:
        print(f"objc3c-security-posture: FAIL\n- {exc}", file=sys.stderr)
        return 1

    if not DISTRIBUTION_TRUST_REPORT.is_file():
        print(
            f"objc3c-security-posture: FAIL\n- missing distribution trust report {repo_rel(DISTRIBUTION_TRUST_REPORT)}",
            file=sys.stderr,
        )
        return 1
    trust_report = load_json(DISTRIBUTION_TRUST_REPORT)
    trust_state = str(trust_report.get("trust_state", "blocked"))

    trust_boundaries = [
        {
            "boundary_id": "checked-in-security-source-surface",
            "status": boundary_status_from_summary(source_summary),
            "source_path": repo_rel(SOURCE_SUMMARY),
            "detail": "checked-in runbook, policy, and workflow surfaces remain complete",
        },
        {
            "boundary_id": "checked-in-security-schema-surface",
            "status": boundary_status_from_summary(schema_summary),
            "source_path": repo_rel(SCHEMA_SUMMARY),
            "detail": "security posture and advisory outputs stay on checked-in schema contracts",
        },
        {
            "boundary_id": "response-disclosure-policy",
            "status": boundary_status_from_summary(response_summary, warn_on_degraded=True, trust_state=trust_state),
            "source_path": repo_rel(RESPONSE_SUMMARY),
            "detail": "response classes and disclosure narrowing rules remain replayable",
        },
        {
            "boundary_id": "macro-package-provenance-trust",
            "status": boundary_status_from_summary(macro_summary),
            "source_path": repo_rel(MACRO_SUMMARY),
            "detail": "macro package and provenance enforcement stays fail-closed on checked-in surfaces",
        },
        {
            "boundary_id": "release-update-lineage-hardening",
            "status": boundary_status_from_summary(release_key_summary, warn_on_degraded=True, trust_state=trust_state),
            "source_path": repo_rel(RELEASE_KEY_SUMMARY),
            "detail": "release manifest, SBOM, attestation, update, and channel lineage remain coherent",
        },
        {
            "boundary_id": "security-artifact-publication-contract",
            "status": boundary_status_from_summary(artifact_contract_summary),
            "source_path": repo_rel(ARTIFACT_CONTRACT_SUMMARY),
            "detail": "security posture and advisory publication artifacts stay canonical and replayable",
        },
        {
            "boundary_id": "supply-chain-audit",
            "status": boundary_status_from_summary(supply_chain_summary, warn_on_degraded=True, trust_state=trust_state),
            "source_path": repo_rel(SUPPLY_CHAIN_SUMMARY),
            "detail": "release evidence, update metadata, and trust-report lineage remain coherent",
        },
        {
            "boundary_id": "distribution-trust-signal",
            "status": "PASS" if trust_state == "ready" else "WARN" if trust_state == "degraded" else "FAIL",
            "source_path": repo_rel(DISTRIBUTION_TRUST_REPORT),
            "detail": str(trust_report.get("headline", "")),
        },
    ]

    statuses = {entry["status"] for entry in trust_boundaries}
    if "FAIL" in statuses:
        security_state = "blocked"
    elif "WARN" in statuses:
        security_state = "degraded"
    else:
        security_state = "ready"

    if security_state == "ready":
        headline = "Objective-C 3 security posture is publishable on the current checked-in trust, package, and runtime evidence."
    elif security_state == "degraded":
        headline = "Objective-C 3 security posture is publishable with caution while trust or response signals remain narrow."
    else:
        headline = "Objective-C 3 security posture is blocked until trust, response, or supply-chain regressions are resolved."

    evidence_paths = [
        repo_rel(SOURCE_SUMMARY),
        repo_rel(SCHEMA_SUMMARY),
        repo_rel(RESPONSE_SUMMARY),
        repo_rel(MACRO_SUMMARY),
        repo_rel(RELEASE_KEY_SUMMARY),
        repo_rel(ARTIFACT_CONTRACT_SUMMARY),
        repo_rel(SUPPLY_CHAIN_SUMMARY),
        repo_rel(DISTRIBUTION_TRUST_REPORT),
        *[str(path) for path in trust_report.get("evidence_paths", []) if isinstance(path, str)],
    ]

    posture_payload = {
        "contract_id": "objc3c.security.hardening.posture.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "security_state": security_state,
        "headline": headline,
        "trust_boundaries": trust_boundaries,
        "evidence_paths": evidence_paths,
        "publication_surface": {
            "inspect_security_posture_command": "inspect:objc3c:security-posture",
            "publish_security_advisories_command": "publish:objc3c:security-advisories",
            "validate_security_hardening_command": "test:objc3c:security-hardening",
            "validate_security_hardening_end_to_end_command": "test:objc3c:security-hardening:e2e"
        }
    }
    POSTURE_PATH.parent.mkdir(parents=True, exist_ok=True)
    POSTURE_PATH.write_text(json.dumps(posture_payload, indent=2) + "\n", encoding="utf-8")

    summary_payload = {
        "contract_id": "objc3c.security.hardening.posture.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "security_state": security_state,
        "headline": headline,
        "trust_boundary_count": len(trust_boundaries),
        "posture_path": repo_rel(POSTURE_PATH),
        "evidence_paths": evidence_paths,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary_payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print(f"published_posture: {repo_rel(POSTURE_PATH)}")
    print("objc3c-security-posture: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
