#!/usr/bin/env python3
"""Run the security-hardening closeout gate over the live hardening and publication evidence."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
OUT_DIR = ROOT / "tmp" / "reports" / "security-hardening" / "closeout-gate"
JSON_OUT = OUT_DIR / "security_hardening_closeout_gate.json"
MD_OUT = OUT_DIR / "security_hardening_closeout_gate.md"
SUMMARY_CONTRACT_ID = "objc3c.security.hardening.closeout.gate.v1"

STEPS = [
    ("build-boundary-inventory-summary", [sys.executable, "scripts/build_security_hardening_boundary_inventory_summary.py"]),
    ("build-response-policy-summary", [sys.executable, "scripts/build_security_hardening_response_policy_summary.py"]),
    ("build-macro-trust-policy-summary", [sys.executable, "scripts/build_security_hardening_macro_trust_policy_summary.py"]),
    ("build-release-key-policy-summary", [sys.executable, "scripts/build_security_hardening_release_key_policy_summary.py"]),
    ("build-artifact-contract-summary", [sys.executable, "scripts/build_security_hardening_artifact_contract_summary.py"]),
    ("check-security-hardening-surface", [sys.executable, "scripts/check_security_hardening_source_surface.py"]),
    ("check-security-hardening-schema-surface", [sys.executable, "scripts/check_security_hardening_schema_surface.py"]),
    ("check-security-hardening-supply-chain-audit", [sys.executable, "scripts/check_security_hardening_supply_chain_audit.py"]),
    ("check-security-response-drill", [sys.executable, "scripts/check_security_hardening_response_drill.py"]),
    ("check-security-runtime-hardening", [sys.executable, "scripts/check_security_hardening_runtime_hardening.py"]),
    ("build-security-posture", [sys.executable, "scripts/build_objc3c_security_posture.py"]),
    ("publish-security-advisories", [sys.executable, "scripts/publish_objc3c_security_advisories.py"]),
    ("check-documentation-surface", [sys.executable, "scripts/check_documentation_surface.py"]),
    ("check-repo-superclean-surface", [sys.executable, "scripts/check_repo_superclean_surface.py"]),
    ("check-source-hygiene-authenticity", [sys.executable, "scripts/check_source_hygiene_authenticity.py"]),
]

WORKFLOW_REPORT = ROOT / "tmp" / "reports" / "objc3c-public-workflow" / "validate-security-hardening.json"
RESPONSE_DRILL_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "response-drill-summary.json"
RUNTIME_HARDENING_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "runtime-hardening-summary.json"
POSTURE_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "security-posture-summary.json"
POSTURE_JSON = ROOT / "tmp" / "artifacts" / "security-hardening" / "posture" / "objc3c-security-posture.json"
PUBLICATION_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "publication-summary.json"
ADVISORY_INDEX = ROOT / "tmp" / "artifacts" / "security-hardening" / "advisories" / "objc3c-security-advisory-index.json"
INTEGRATION_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "integration-summary.json"
END_TO_END_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "end-to-end-summary.json"

EXPECTED_WORKFLOW_STEPS = [
    "check-security-response-drill",
    "check-security-runtime-hardening",
    "check-security-hardening-surface",
    "check-security-hardening-schema-surface",
    "build-security-posture",
    "publish-security-advisories",
]


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def run_step(name: str, command: list[str]) -> dict[str, object]:
    result = subprocess.run(command, cwd=ROOT, text=True, capture_output=True, check=False)
    return {
        "name": name,
        "command": command,
        "exit_code": result.returncode,
        "stdout": result.stdout,
        "stderr": result.stderr,
    }


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"{repo_rel(path)} did not contain a JSON object")
    return payload


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def summary_passes(payload: dict[str, Any]) -> bool:
    return payload.get("status") in {"PASS", "OK"} or payload.get("ok") is True


def main() -> int:
    steps: list[dict[str, object]] = []
    failures: list[str] = []
    for name, command in STEPS:
        step = run_step(name, command)
        steps.append(step)
        expect(step["exit_code"] == 0, f"{name} failed", failures)
        if step["exit_code"] != 0:
            break

    required_paths = (
        WORKFLOW_REPORT,
        RESPONSE_DRILL_SUMMARY,
        RUNTIME_HARDENING_SUMMARY,
        POSTURE_SUMMARY,
        POSTURE_JSON,
        PUBLICATION_SUMMARY,
        ADVISORY_INDEX,
        INTEGRATION_SUMMARY,
        END_TO_END_SUMMARY,
    )
    for path in required_paths:
        expect(path.is_file(), f"missing expected artifact {repo_rel(path)}", failures)

    workflow_report = load_json(WORKFLOW_REPORT) if WORKFLOW_REPORT.is_file() else {}
    response_drill_summary = load_json(RESPONSE_DRILL_SUMMARY) if RESPONSE_DRILL_SUMMARY.is_file() else {}
    runtime_hardening_summary = load_json(RUNTIME_HARDENING_SUMMARY) if RUNTIME_HARDENING_SUMMARY.is_file() else {}
    posture_summary = load_json(POSTURE_SUMMARY) if POSTURE_SUMMARY.is_file() else {}
    posture_json = load_json(POSTURE_JSON) if POSTURE_JSON.is_file() else {}
    publication_summary = load_json(PUBLICATION_SUMMARY) if PUBLICATION_SUMMARY.is_file() else {}
    advisory_index = load_json(ADVISORY_INDEX) if ADVISORY_INDEX.is_file() else {}
    integration_summary = load_json(INTEGRATION_SUMMARY) if INTEGRATION_SUMMARY.is_file() else {}
    end_to_end_summary = load_json(END_TO_END_SUMMARY) if END_TO_END_SUMMARY.is_file() else {}

    expect(summary_passes(response_drill_summary), "response drill summary did not report PASS", failures)
    expect(summary_passes(runtime_hardening_summary), "runtime hardening summary did not report PASS", failures)
    expect(summary_passes(posture_summary), "security posture summary did not report PASS", failures)
    expect(summary_passes(posture_json), "security posture artifact did not report PASS", failures)
    expect(summary_passes(publication_summary), "security publication summary did not report PASS", failures)
    expect(summary_passes(advisory_index), "security advisory index did not report PASS", failures)
    expect(summary_passes(integration_summary), "security integration summary did not report PASS", failures)
    expect(summary_passes(end_to_end_summary), "security end-to-end summary did not report PASS", failures)
    expect(workflow_report.get("status") == "PASS", "validate-security-hardening workflow report did not report PASS", failures)

    workflow_steps = workflow_report.get("steps", [])
    workflow_actions = [str(step.get("action")) for step in workflow_steps if isinstance(step, dict)]
    expect(workflow_actions == EXPECTED_WORKFLOW_STEPS, "security workflow steps drifted", failures)

    security_state = posture_summary.get("security_state")
    expect(security_state in {"ready", "degraded"}, "security posture state must stay publishable or cautionary, not blocked", failures)
    expect(security_state == publication_summary.get("security_state"), "publication summary drifted from posture state", failures)
    expect(security_state == advisory_index.get("security_state"), "advisory index drifted from posture state", failures)
    expect(security_state == end_to_end_summary.get("security_state"), "end-to-end summary drifted from posture state", failures)

    response_trust_state = response_drill_summary.get("trust_state")
    expect(response_trust_state in {"ready", "degraded"}, "response drill trust state must stay publishable or cautionary", failures)
    expect(runtime_hardening_summary.get("memory_safety_boundary") == "bounded-by-runtime-acceptance-and-packaged-release-candidate-evidence", "runtime hardening boundary drifted", failures)
    expect(runtime_hardening_summary.get("missing_runtime_case_ids") == [], "runtime hardening summary reported missing runtime case ids", failures)

    trust_boundaries = posture_json.get("trust_boundaries", [])
    runtime_boundary = next(
        (entry for entry in trust_boundaries if isinstance(entry, dict) and entry.get("boundary_id") == "runtime-hardening-memory-safety-regression"),
        None,
    )
    expect(isinstance(runtime_boundary, dict), "security posture missing runtime hardening trust boundary", failures)
    if isinstance(runtime_boundary, dict):
        expect(runtime_boundary.get("source_path") == repo_rel(RUNTIME_HARDENING_SUMMARY), "runtime hardening trust boundary source drifted", failures)
        expect(runtime_boundary.get("status") == "PASS", "runtime hardening trust boundary must report PASS", failures)

    evidence_paths = posture_json.get("evidence_paths", [])
    expect(repo_rel(RUNTIME_HARDENING_SUMMARY) in evidence_paths, "security posture evidence is missing runtime hardening summary", failures)
    expect(publication_summary.get("runtime_hardening_summary_path") == repo_rel(RUNTIME_HARDENING_SUMMARY), "publication summary runtime hardening link drifted", failures)
    expect(publication_summary.get("runtime_hardening_memory_safety_boundary") == runtime_hardening_summary.get("memory_safety_boundary"), "publication summary memory-safety boundary drifted", failures)

    advisories = advisory_index.get("advisories", [])
    runtime_advisory = next(
        (entry for entry in advisories if isinstance(entry, dict) and entry.get("advisory_id") == "OBJC3C-SEC-0003"),
        None,
    )
    expect(isinstance(runtime_advisory, dict), "security advisory index missing runtime hardening advisory", failures)
    if isinstance(runtime_advisory, dict):
        source_paths = runtime_advisory.get("source_paths", [])
        expect("tests/tooling/fixtures/security_hardening/runtime_hardening_contract.json" in source_paths, "runtime hardening advisory missing contract source", failures)
        expect(repo_rel(RUNTIME_HARDENING_SUMMARY) in source_paths, "runtime hardening advisory missing summary source", failures)

    expect(integration_summary.get("validated_steps") == EXPECTED_WORKFLOW_STEPS, "integration summary step inventory drifted", failures)
    expect(end_to_end_summary.get("validated_steps") == EXPECTED_WORKFLOW_STEPS, "end-to-end summary step inventory drifted", failures)

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS" if not failures else "FAIL",
        "steps": steps,
        "failures": failures,
        "security_state": security_state,
        "response_trust_state": response_trust_state,
        "artifacts": {
            "workflow_report": repo_rel(WORKFLOW_REPORT),
            "response_drill_summary": repo_rel(RESPONSE_DRILL_SUMMARY),
            "runtime_hardening_summary": repo_rel(RUNTIME_HARDENING_SUMMARY),
            "security_posture_summary": repo_rel(POSTURE_SUMMARY),
            "security_posture_artifact": repo_rel(POSTURE_JSON),
            "security_publication_summary": repo_rel(PUBLICATION_SUMMARY),
            "security_advisory_index": repo_rel(ADVISORY_INDEX),
            "integration_summary": repo_rel(INTEGRATION_SUMMARY),
            "end_to_end_summary": repo_rel(END_TO_END_SUMMARY),
        },
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Security Hardening Closeout Gate\n\n"
        f"- Contract: `{payload['contract_id']}`\n"
        f"- Status: `{payload['status']}`\n"
        f"- Security state: `{security_state}`\n"
        f"- Response trust state: `{response_trust_state}`\n"
        f"- Workflow steps: `{', '.join(EXPECTED_WORKFLOW_STEPS)}`\n",
        encoding="utf-8",
    )
    print(f"summary_path: {repo_rel(JSON_OUT)}")
    print("m327-security-hardening-closeout-gate: PASS" if not failures else "m327-security-hardening-closeout-gate: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
