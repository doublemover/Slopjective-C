#!/usr/bin/env python3
"""Validate the language/runtime threat model and mitigation backlog."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.json_io import write_report_json
from objc3c_tooling.paths import repo_rel
from scripts.objc3c_workflow.public_command_api import public_workflow_action_names


CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "security_hardening"
    / "language_runtime_threat_model_backlog.json"
)
SUMMARY_PATH = (
    ROOT
    / "tmp"
    / "reports"
    / "security-hardening"
    / "language-runtime-threat-model-summary.json"
)
CONTRACT_ID = "objc3c.security.hardening.language.runtime.threat_model.backlog.v1"
SUMMARY_CONTRACT_ID = "objc3c.security.hardening.language.runtime.threat_model.summary.v1"
EXPECTED_EVIDENCE_CONTRACT_IDS = {
    "macro_trust_policy": "objc3c.security.hardening.macro.package.provenance.trust.policy.v1",
    "macro_supply_chain_trust_registry": "objc3c.security.hardening.macro.supply-chain.trust.registry.v1",
    "runtime_hardening_contract": "objc3c.security.hardening.runtime.contract.v1",
    "sanitizer_validation_contract": "objc3c.security.hardening.sanitizer.validation.contract.v1",
    "security_source_surface": "objc3c.security.hardening.source.surface.v1",
    "security_workflow_surface": "objc3c.security.hardening.workflow.surface.v1",
}


def fail(message: str) -> int:
    print(f"security-language-runtime-threat-model: {message}", file=sys.stderr)
    return 1


def require_path(raw_path: str) -> Path:
    path = ROOT / raw_path
    if not path.is_file():
        raise RuntimeError(f"missing required file {raw_path}")
    return path


def require_action_surfaces(contract: dict[str, Any]) -> None:
    action_name = str(contract.get("public_action", ""))
    if not action_name:
        raise RuntimeError("public_action must be non-empty")
    if action_name not in set(public_workflow_action_names()):
        raise RuntimeError(f"workflow registry missing public action {action_name}")

    source_surface = load_json(ROOT / "tests/tooling/fixtures/security_hardening/source_surface.json")
    workflow_surface = load_json(ROOT / "tests/tooling/fixtures/security_hardening/workflow_surface.json")
    for payload, field in (
        (source_surface, "public_actions"),
        (source_surface.get("owner_policy", {}), "owned_actions"),
        (workflow_surface, "required_actions"),
        (workflow_surface.get("owner_policy", {}), "owned_actions"),
        (workflow_surface, "validation_child_actions"),
        (workflow_surface.get("owner_policy", {}), "workflow_child_actions"),
    ):
        actions = payload.get(field) if isinstance(payload, dict) else None
        if not isinstance(actions, list) or action_name not in actions:
            raise RuntimeError(f"{field} missing {action_name}")


def validate_evidence_roots(contract: dict[str, Any]) -> dict[str, str]:
    roots = contract.get("evidence_roots")
    if not isinstance(roots, dict):
        raise RuntimeError("evidence_roots must be an object")
    checked: dict[str, str] = {}
    for key, expected_contract_id in EXPECTED_EVIDENCE_CONTRACT_IDS.items():
        raw_path = roots.get(key)
        if not isinstance(raw_path, str) or not raw_path:
            raise RuntimeError(f"evidence_roots missing {key}")
        path = require_path(raw_path)
        payload = load_json(path)
        if payload.get("contract_id") != expected_contract_id:
            raise RuntimeError(f"{key} drifted from expected contract id {expected_contract_id}")
        checked[key] = repo_rel(path)
    return checked


def validate_runbook(contract: dict[str, Any]) -> str:
    runbook_path = require_path(str(contract.get("runbook", "")))
    runbook_text = runbook_path.read_text(encoding="utf-8")
    required_phrases = [
        "Canonical source truth",
        "check-security-language-runtime-threat-model",
        "language-runtime-threat-model-summary.json",
        "does not claim hostile macro execution safety",
    ]
    missing = [phrase for phrase in required_phrases if phrase not in runbook_text]
    if missing:
        raise RuntimeError(f"runbook missing required phrases: {', '.join(missing)}")
    return repo_rel(runbook_path)


def validate_threats_and_backlog(contract: dict[str, Any]) -> tuple[list[dict[str, str]], list[dict[str, str]]]:
    threats = contract.get("threats")
    backlog = contract.get("mitigation_backlog")
    if not isinstance(threats, list) or not threats:
        raise RuntimeError("threats must be a non-empty list")
    if not isinstance(backlog, list) or not backlog:
        raise RuntimeError("mitigation_backlog must be a non-empty list")

    evidence_keys = set(EXPECTED_EVIDENCE_CONTRACT_IDS)
    required_categories = {str(category) for category in contract.get("required_categories", [])}
    threat_ids: set[str] = set()
    mitigation_ids: set[str] = set()
    threat_rows: list[dict[str, str]] = []
    backlog_rows: list[dict[str, str]] = []

    for threat in threats:
        if not isinstance(threat, dict):
            raise RuntimeError("threat entries must be objects")
        threat_id = str(threat.get("threat_id", ""))
        category = str(threat.get("category", ""))
        owner = str(threat.get("owner", ""))
        if not threat_id or threat_id in threat_ids:
            raise RuntimeError(f"duplicate or missing threat_id {threat_id}")
        if category not in required_categories:
            raise RuntimeError(f"{threat_id} uses unknown category {category}")
        if not owner:
            raise RuntimeError(f"{threat_id} missing owner")
        linked_evidence = {str(key) for key in threat.get("evidence_keys", [])}
        if not linked_evidence or not linked_evidence <= evidence_keys:
            raise RuntimeError(f"{threat_id} has invalid evidence_keys")
        required_mitigations = [str(value) for value in threat.get("required_mitigation_ids", [])]
        if not required_mitigations:
            raise RuntimeError(f"{threat_id} missing required_mitigation_ids")
        threat_ids.add(threat_id)
        threat_rows.append({"threat_id": threat_id, "category": category, "owner": owner})

    categories = {row["category"] for row in threat_rows}
    missing_categories = sorted(required_categories - categories)
    if missing_categories:
        raise RuntimeError(f"missing required threat categories: {', '.join(missing_categories)}")

    for mitigation in backlog:
        if not isinstance(mitigation, dict):
            raise RuntimeError("mitigation_backlog entries must be objects")
        mitigation_id = str(mitigation.get("mitigation_id", ""))
        owner = str(mitigation.get("owner", ""))
        status = str(mitigation.get("status", ""))
        closure_action = str(mitigation.get("closure_action", ""))
        if not mitigation_id or mitigation_id in mitigation_ids:
            raise RuntimeError(f"duplicate or missing mitigation_id {mitigation_id}")
        if status not in {"active", "blocked"}:
            raise RuntimeError(f"{mitigation_id} has unsupported status {status}")
        if not owner or not closure_action:
            raise RuntimeError(f"{mitigation_id} missing owner or closure_action")
        linked_threats = {str(value) for value in mitigation.get("threat_ids", [])}
        if not linked_threats or not linked_threats <= threat_ids:
            raise RuntimeError(f"{mitigation_id} references unknown threat_ids")
        linked_evidence = {str(key) for key in mitigation.get("evidence_required", [])}
        if not linked_evidence or not linked_evidence <= evidence_keys:
            raise RuntimeError(f"{mitigation_id} has invalid evidence_required")
        mitigation_ids.add(mitigation_id)
        backlog_rows.append(
            {
                "mitigation_id": mitigation_id,
                "owner": owner,
                "status": status,
                "closure_action": closure_action,
            }
        )

    required_by_threat = {
        str(mitigation_id)
        for threat in threats
        if isinstance(threat, dict)
        for mitigation_id in threat.get("required_mitigation_ids", [])
    }
    missing_mitigations = sorted(required_by_threat - mitigation_ids)
    if missing_mitigations:
        raise RuntimeError(f"missing required mitigations: {', '.join(missing_mitigations)}")

    registered_actions = set(public_workflow_action_names())
    missing_actions = [
        row["closure_action"]
        for row in backlog_rows
        if row["closure_action"] not in registered_actions
    ]
    if missing_actions:
        raise RuntimeError(f"mitigation closure actions are not registered: {', '.join(missing_actions)}")

    return threat_rows, backlog_rows


def main() -> int:
    try:
        contract = load_json(CONTRACT_PATH)
        if contract.get("contract_id") != CONTRACT_ID:
            raise RuntimeError("unexpected language/runtime threat model contract_id")
        if str(contract.get("summary_path", "")) != repo_rel(SUMMARY_PATH):
            raise RuntimeError("summary_path drifted from language/runtime threat model output")
        require_action_surfaces(contract)
        evidence_roots = validate_evidence_roots(contract)
        runbook = validate_runbook(contract)
        threats, backlog = validate_threats_and_backlog(contract)
    except RuntimeError as exc:
        return fail(str(exc))

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "source_contract": repo_rel(CONTRACT_PATH),
        "runbook": runbook,
        "public_action": contract["public_action"],
        "evidence_roots": evidence_roots,
        "threat_count": len(threats),
        "mitigation_count": len(backlog),
        "threats": threats,
        "mitigation_backlog": backlog,
        "forbidden_claims": contract["forbidden_claims"],
    }
    write_report_json(SUMMARY_PATH, payload, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("security-language-runtime-threat-model: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
