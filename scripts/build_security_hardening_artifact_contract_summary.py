#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "security_hardening" / "artifact_reporting_contract.json"
RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_security_hardening.md"
OUT_DIR = ROOT / "tmp" / "reports" / "security-hardening" / "artifact-contract"
JSON_OUT = OUT_DIR / "artifact_contract_summary.json"
MD_OUT = OUT_DIR / "artifact_contract_summary.md"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON object expected at {path}")
    return payload


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")

    checks = {
      "all_source_contracts_exist": all((ROOT / path).is_file() for path in contract["source_contracts"]),
      "all_required_schemas_exist": all((ROOT / path).is_file() for path in contract["required_schema_paths"]),
      "artifact_paths_cover_posture_and_advisories": all(
          key in contract["artifact_paths"]
          for key in (
              "security_posture_json",
              "security_advisory_index_json",
              "security_advisory_report_md",
              "security_posture_summary_json",
              "security_publication_summary_json",
          )
      ),
      "required_publication_surface_fields_present": len(contract["required_publication_surface_fields"]) == 4,
      "runbook_mentions_generated_security_outputs": "Generated security posture and advisory outputs must stay under:" in runbook_text,
      "runbook_mentions_security_posture_schema": "schemas/objc3c-security-posture-v1.schema.json" in runbook_text,
      "runbook_mentions_security_advisory_schema": "schemas/objc3c-security-advisory-index-v1.schema.json" in runbook_text,
    }

    payload = {
        "contract_id": "objc3c.security.hardening.artifact.reporting.contract.summary.v1",
        "source_contract_id": contract["contract_id"],
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/build_security_hardening_artifact_contract_summary.py",
        "source_contract_count": len(contract["source_contracts"]),
        "required_schema_count": len(contract["required_schema_paths"]),
        "artifact_path_count": len(contract["artifact_paths"]),
        "required_publication_surface_field_count": len(contract["required_publication_surface_fields"]),
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Security Hardening Artifact Contract Summary\n\n"
        f"- Contract: `{payload['source_contract_id']}`\n"
        f"- Source contracts: `{payload['source_contract_count']}`\n"
        f"- Required schemas: `{payload['required_schema_count']}`\n"
        f"- Artifact paths: `{payload['artifact_path_count']}`\n"
        f"- Publication surface fields: `{payload['required_publication_surface_field_count']}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
