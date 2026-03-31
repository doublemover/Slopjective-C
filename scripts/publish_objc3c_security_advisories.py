#!/usr/bin/env python3
"""Publish the live security advisory artifacts and summary."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
POSTURE_BUILD = ROOT / "scripts" / "build_objc3c_security_posture.py"
POSTURE_PATH = ROOT / "tmp" / "artifacts" / "security-hardening" / "posture" / "objc3c-security-posture.json"
POSTURE_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "security-posture-summary.json"
ADVISORY_INDEX_PATH = ROOT / "tmp" / "artifacts" / "security-hardening" / "advisories" / "objc3c-security-advisory-index.json"
ADVISORY_REPORT_PATH = ROOT / "tmp" / "artifacts" / "security-hardening" / "advisories" / "objc3c-security-advisory-report.md"
PUBLICATION_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "publication-summary.json"


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


def ensure_posture() -> tuple[dict[str, Any], dict[str, Any]]:
    if not POSTURE_PATH.is_file() or not POSTURE_SUMMARY.is_file():
        result = run_capture([sys.executable, str(POSTURE_BUILD)])
        if result.returncode != 0:
            raise RuntimeError("failed to build security posture")
    posture = load_json(POSTURE_PATH)
    summary = load_json(POSTURE_SUMMARY)
    if posture.get("status") != "PASS" or summary.get("status") != "PASS":
        raise RuntimeError("security posture did not pass")
    return posture, summary


def advisory_status(security_state: str) -> str:
    if security_state == "ready":
        return "monitoring"
    if security_state == "degraded":
        return "caution"
    return "blocked"


def main() -> int:
    try:
        posture, _ = ensure_posture()
    except RuntimeError as exc:
        print(f"objc3c-security-advisories: FAIL\n- {exc}", file=sys.stderr)
        return 1

    security_state = str(posture["security_state"])
    status = advisory_status(security_state)
    trust_boundaries = posture.get("trust_boundaries", [])
    evidence_paths = posture.get("evidence_paths", [])

    advisories = [
        {
            "advisory_id": "OBJC3C-SEC-0001",
            "severity": "high",
            "status": status,
            "headline": "Macro and package provenance trust remains bounded to checked-in compiler and runtime evidence.",
            "source_paths": [
                "tests/tooling/fixtures/security_hardening/macro_package_provenance_trust_policy.json",
                "tmp/reports/security-hardening/macro-trust-policy/macro_trust_policy_summary.json"
            ]
        },
        {
            "advisory_id": "OBJC3C-SEC-0002",
            "severity": "high",
            "status": status,
            "headline": "Release, installer, update, and trust publication claims remain tied to one coherent runnable payload family.",
            "source_paths": [
                "tests/tooling/fixtures/security_hardening/installer_update_release_key_hardening_policy.json",
                "tmp/reports/security-hardening/supply-chain-audit-summary.json"
            ]
        },
        {
            "advisory_id": "OBJC3C-SEC-0003",
            "severity": "high",
            "status": status,
            "headline": "Runtime hardening claims stay limited to checked-in acceptance, packaged runnable validation, and response evidence.",
            "source_paths": [
                "tests/tooling/fixtures/security_hardening/security_response_disclosure_policy.json",
                "tmp/artifacts/security-hardening/posture/objc3c-security-posture.json"
            ]
        },
        {
            "advisory_id": "OBJC3C-SEC-0004",
            "severity": "medium",
            "status": "bounded-non-goal",
            "headline": "No hosted advisory feed, remote key custody, or signed-installer service is claimed on the checked-in surface.",
            "source_paths": [
                "docs/runbooks/objc3c_security_hardening.md",
                "tmp/artifacts/security-hardening/posture/objc3c-security-posture.json"
            ]
        }
    ]

    advisory_index = {
        "contract_id": "objc3c.security.hardening.advisory.index.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "security_state": security_state,
        "advisories": advisories,
    }
    ADVISORY_INDEX_PATH.parent.mkdir(parents=True, exist_ok=True)
    ADVISORY_INDEX_PATH.write_text(json.dumps(advisory_index, indent=2) + "\n", encoding="utf-8")

    markdown_lines = [
        "# Objective-C 3 Security Advisory Report",
        "",
        f"- Security state: `{security_state}`",
        f"- Posture artifact: `{repo_rel(POSTURE_PATH)}`",
        "",
        str(posture.get("headline", "")),
        "",
        "## Advisory Index",
        "",
    ]
    for advisory in advisories:
        markdown_lines.extend(
            [
                f"### {advisory['advisory_id']}",
                "",
                f"- Severity: `{advisory['severity']}`",
                f"- Status: `{advisory['status']}`",
                f"- Headline: {advisory['headline']}",
                "",
                "Evidence:",
                *[f"- `{path}`" for path in advisory["source_paths"]],
                "",
            ]
        )
    markdown_lines.extend(["## Trust Boundaries", ""])
    for boundary in trust_boundaries:
        if isinstance(boundary, dict):
            markdown_lines.append(
                f"- `{boundary.get('boundary_id')}`: `{boundary.get('status')}` from `{boundary.get('source_path')}`"
            )
    markdown_lines.extend(["", "## Evidence", ""])
    markdown_lines.extend(f"- `{path}`" for path in evidence_paths if isinstance(path, str))
    ADVISORY_REPORT_PATH.write_text("\n".join(markdown_lines) + "\n", encoding="utf-8")

    publication_payload = {
        "contract_id": "objc3c.security.hardening.publication.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "security_state": security_state,
        "posture_summary_path": repo_rel(POSTURE_SUMMARY),
        "security_posture_json": repo_rel(POSTURE_PATH),
        "security_advisory_index_json": repo_rel(ADVISORY_INDEX_PATH),
        "security_advisory_report_md": repo_rel(ADVISORY_REPORT_PATH),
        "advisory_count": len(advisories),
        "headline": str(posture.get("headline", "")),
        "evidence_paths": evidence_paths,
    }
    PUBLICATION_SUMMARY.parent.mkdir(parents=True, exist_ok=True)
    PUBLICATION_SUMMARY.write_text(json.dumps(publication_payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(PUBLICATION_SUMMARY)}")
    print(f"published_advisory_index: {repo_rel(ADVISORY_INDEX_PATH)}")
    print(f"published_advisory_report: {repo_rel(ADVISORY_REPORT_PATH)}")
    print("objc3c-security-advisories: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
