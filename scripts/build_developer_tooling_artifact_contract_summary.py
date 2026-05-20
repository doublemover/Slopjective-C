#!/usr/bin/env python3
from __future__ import annotations

from objc3c_tooling.json_io import write_json_file
import json
from pathlib import Path
from typing import Any
from scripts.objc3c_workflow.public_command_api import public_workflow_action_names

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/developer_tooling/editor_protocol_debug_artifact_contract.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_developer_tooling.md"
PACKAGE_JSON = ROOT / "package.json"
OUT_DIR = ROOT / "tmp/reports/developer-tooling/artifact-contract"
JSON_OUT = OUT_DIR / "editor_protocol_debug_artifact_contract_summary.json"
MD_OUT = OUT_DIR / "editor_protocol_debug_artifact_contract_summary.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def summary_anchor(contract: dict[str, Any]) -> str:
    return str(
        contract.get("summary_script")
        or contract.get("summary_implementation_anchor")
        or ""
    )


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    package_payload = read_json(PACKAGE_JSON)
    schema_paths = [ROOT / path for path in contract["schema_paths"]]
    scripts = package_payload.get("scripts", {})
    required_actions = [str(action) for action in contract["public_actions"]]
    registered_actions = set(public_workflow_action_names())
    package_bridge = str(contract["package_bridge"])
    package_bridge_exists = package_bridge in scripts
    missing_actions = [action for action in required_actions if action not in registered_actions]

    checks = {
        "summary_script_link_matches": summary_anchor(contract) == "scripts/build_developer_tooling_artifact_contract_summary.py",
        "all_schema_paths_exist": all(path.is_file() for path in schema_paths),
        "runbook_has_artifact_contract_heading": "## Editor Protocol And Debug Artifact Contract" in runbook_text,
        "runbook_mentions_combined_surface": "one machine-owned editor tooling surface" in runbook_text,
        "runbook_mentions_tmp_report_family": "tmp/reports/developer-tooling/" in runbook_text,
        "runbook_mentions_public_workflow_actions": "inspect-editor-tooling" in runbook_text and "format-objc3c" in runbook_text,
        "workflow_registry_exposes_declared_spine": "validate-developer-tooling" in registered_actions,
        "package_exposes_objc3c_bridge": package_bridge_exists,
        "all_required_actions_registered": not missing_actions,
    }

    summary = {
        "issue": "developer-tooling-editor-protocol-debug-artifact-contract",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "schema_path_count": len(contract["schema_paths"]),
        "canonical_report_path_count": len(contract["canonical_report_paths"]),
        "required_action_count": len(required_actions),
        "package_bridge_count": 1 if package_bridge_exists else 0,
        "required_actions": required_actions,
        "missing_actions": missing_actions,
        "package_bridge": package_bridge,
        "missing_package_bridge": [] if package_bridge_exists else [package_bridge],
        "artifact_section_count": len(contract["artifact_sections"]),
        "source_artifact_count": len(contract["source_artifacts"]),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    write_json_file(JSON_OUT, summary)
    MD_OUT.write_text(
        "# Developer Tooling Editor Protocol Artifact Contract Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Schemas: `{summary['schema_path_count']}`\n"
        f"- Canonical report paths: `{summary['canonical_report_path_count']}`\n"
        f"- Required actions: `{summary['required_action_count']}`\n"
        f"- Package bridge count: `{summary['package_bridge_count']}`\n"
        f"- Artifact sections: `{summary['artifact_section_count']}`\n"
        f"- Source artifacts: `{summary['source_artifact_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
