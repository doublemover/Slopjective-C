#!/usr/bin/env python3
"""Validate the public-conformance workflow end to end."""

from __future__ import annotations

import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import require_json_object as load_json
from scripts.objc3c_workflow.public_command_api import (
    public_workflow_action_payload,
    public_workflow_command,
    public_workflow_has_actions,
)
from objc3c_tooling.subprocesses import python_script_command, run_capture


ROOT = Path(__file__).resolve().parents[1]
PUBLIC_COMMAND_SURFACE_PY = ROOT / "scripts" / "render_objc3c_public_command_surface.py"
TASK_HYGIENE_PY = ROOT / "scripts" / "ci" / "run_task_hygiene_gate.py"
SUPPORT_CLAIM_RUNNABLE_EVIDENCE_CATALOG = (
    ROOT / "tests" / "conformance" / "support_claim_runnable_evidence_catalog.json"
)
INTEGRATION_REPORT = ROOT / "tmp" / "reports" / "public-conformance" / "integration-summary.json"
EXTERNAL_VALIDATION_INTEGRATION_REPORT = (
    ROOT / "tmp" / "reports" / "external-validation" / "integration-summary.json"
)
END_TO_END_REPORT = ROOT / "tmp" / "reports" / "public-conformance" / "end-to-end-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.public_conformance_reporting.end_to_end.summary.v1"
EXTERNAL_VALIDATION_INTEGRATION_CONTRACT_ID = "objc3c.external_validation.integration.summary.v1"



def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def ensure_integration_report() -> dict[str, Any]:
    completed = run_capture(public_workflow_command("validate-public-conformance-reporting-integration"))
    expect(completed.returncode == 0, "validate-public-conformance-reporting-integration failed during end-to-end validation")
    return load_json(INTEGRATION_REPORT)


def validate_external_validation_prerequisite() -> dict[str, Any]:
    external_validation = load_json(EXTERNAL_VALIDATION_INTEGRATION_REPORT)
    expect(
        external_validation.get("contract_id") == EXTERNAL_VALIDATION_INTEGRATION_CONTRACT_ID,
        "external-validation integration summary contract drifted",
    )
    expect(
        external_validation.get("status") == "PASS",
        "external-validation integration summary did not pass",
    )
    return {
        "integration_report_path": repo_rel(EXTERNAL_VALIDATION_INTEGRATION_REPORT),
        "status": external_validation["status"],
    }


def describe_action(action: str) -> dict[str, Any]:
    payload = public_workflow_action_payload(action)
    expect(isinstance(payload, dict), f"--describe {action} did not return a JSON object")
    return payload


def validate_support_claim_runnable_evidence_catalog() -> dict[str, Any]:
    catalog = load_json(SUPPORT_CLAIM_RUNNABLE_EVIDENCE_CATALOG)
    expect(
        catalog.get("contract_id")
        == "objc3c.conformance.support_claim_runnable_evidence_catalog.v1",
        "support claim runnable evidence catalog contract drifted",
    )
    rows = catalog.get("rows")
    expect(isinstance(rows, list) and rows, "support claim runnable evidence catalog has no rows")
    object_foundation_rows = [
        row
        for row in rows
        if isinstance(row, dict)
        and row.get("support_claim")
        == "objc3c.behavior.runtime.object-model-interface-method-table"
    ]
    expect(
        len(object_foundation_rows) == 1,
        "object-foundation support claim traceability row is missing or duplicated",
    )
    row = object_foundation_rows[0]
    expect(
        row.get("runnable_command") == "npm run objc3c -- test-runtime-acceptance-fast",
        "object-foundation support claim row lost its runnable public command",
    )
    expect(
        row.get("conformance_fixture") == "tests/conformance/lowering_abi/OBJFND-8058-01.json",
        "object-foundation positive conformance fixture drifted",
    )
    expect(
        row.get("traceability_fixture") == "tests/conformance/lowering_abi/OBJFND-8059-01.json",
        "object-foundation traceability conformance fixture drifted",
    )
    expect(
        isinstance(row.get("positive_evidence"), list) and len(row["positive_evidence"]) >= 3,
        "object-foundation row missing positive runnable evidence",
    )
    expect(
        isinstance(row.get("negative_evidence"), list) and len(row["negative_evidence"]) >= 12,
        "object-foundation row missing negative runnable evidence",
    )
    return {
        "catalog_path": repo_rel(SUPPORT_CLAIM_RUNNABLE_EVIDENCE_CATALOG),
        "support_claim": row["support_claim"],
        "conformance_fixture": row["conformance_fixture"],
        "traceability_fixture": row["traceability_fixture"],
        "runnable_command": row["runnable_command"],
        "positive_evidence_count": len(row["positive_evidence"]),
        "negative_evidence_count": len(row["negative_evidence"]),
    }


def main() -> int:
    integration = ensure_integration_report()
    expect(integration.get("status") == "PASS", "public-conformance integration summary did not pass")
    external_validation_prerequisite = validate_external_validation_prerequisite()
    support_claim_traceability = validate_support_claim_runnable_evidence_catalog()

    validate_desc = describe_action("validate-public-conformance-reporting")
    integration_desc = describe_action("validate-public-conformance-reporting-integration")
    nightly_desc = describe_action("test-nightly")

    expect(
        validate_desc.get("action") == "validate-public-conformance-reporting",
        "validate-public-conformance-reporting description drifted",
    )
    expect(
        integration_desc.get("action") == "validate-public-conformance-reporting-integration",
        "validate-public-conformance-reporting-integration description drifted",
    )
    expect(nightly_desc.get("action") == "test-nightly", "test-nightly description drifted")

    expect(
        public_workflow_has_actions(["validate-public-conformance-reporting"]),
        "workflow registry no longer references validate-public-conformance-reporting",
    )

    render_check = run_capture(python_script_command(PUBLIC_COMMAND_SURFACE_PY, "--check"))
    expect(render_check.returncode == 0, "public command surface check failed")

    hygiene = run_capture(python_script_command(TASK_HYGIENE_PY))
    expect(hygiene.returncode == 0, "task hygiene gate failed")

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "integration_report_path": repo_rel(INTEGRATION_REPORT),
        "validate_action": validate_desc["action"],
        "integration_action": integration_desc["action"],
        "nightly_action": nightly_desc["action"],
        "command_surface_check": "scripts/render_objc3c_public_command_surface.py --check",
        "task_hygiene_gate": "scripts/ci/run_task_hygiene_gate.py",
        "support_claim_traceability": support_claim_traceability,
        "external_validation_prerequisite": external_validation_prerequisite,
        "nightly_wiring_present": True,
    }
    END_TO_END_REPORT.parent.mkdir(parents=True, exist_ok=True)
    END_TO_END_REPORT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(END_TO_END_REPORT)}")
    print("objc3c-public-conformance-end-to-end: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
