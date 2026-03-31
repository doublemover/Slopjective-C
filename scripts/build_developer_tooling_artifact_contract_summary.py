#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/developer_tooling/editor_protocol_debug_artifact_contract.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_developer_tooling.md"
RUNNER_PATH = ROOT / "scripts/objc3c_public_workflow_runner.py"
PACKAGE_JSON = ROOT / "package.json"
OUT_DIR = ROOT / "tmp/reports/developer-tooling/artifact-contract"
JSON_OUT = OUT_DIR / "editor_protocol_debug_artifact_contract_summary.json"
MD_OUT = OUT_DIR / "editor_protocol_debug_artifact_contract_summary.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    runner_text = RUNNER_PATH.read_text(encoding="utf-8")
    package_payload = read_json(PACKAGE_JSON)
    schema_paths = [ROOT / path for path in contract["schema_paths"]]
    scripts = package_payload.get("scripts", {})

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_developer_tooling_artifact_contract_summary.py",
        "all_schema_paths_exist": all(path.is_file() for path in schema_paths),
        "runbook_has_artifact_contract_heading": "## Editor Protocol And Debug Artifact Contract" in runbook_text,
        "runbook_mentions_combined_surface": "one machine-owned editor tooling surface" in runbook_text,
        "runbook_mentions_tmp_report_family": "tmp/reports/developer-tooling/" in runbook_text,
        "runbook_mentions_public_runner_entrypoint": "inspect-editor-tooling" in runbook_text and "format-objc3c" in runbook_text,
        "runner_can_be_extended_at_declared_spine": "validate-developer-tooling" in runner_text,
        "package_retains_developer_tooling_script": scripts.get("test:objc3c:developer-tooling") == "python scripts/objc3c_public_workflow_runner.py validate-developer-tooling",
    }

    summary = {
        "issue": "developer-tooling-editor-protocol-debug-artifact-contract",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "schema_path_count": len(contract["schema_paths"]),
        "canonical_report_path_count": len(contract["canonical_report_paths"]),
        "public_action_count": len(contract["public_actions"]),
        "public_script_count": len(contract["public_scripts"]),
        "artifact_section_count": len(contract["artifact_sections"]),
        "source_artifact_count": len(contract["source_artifacts"]),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Developer Tooling Editor Protocol Artifact Contract Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Schemas: `{summary['schema_path_count']}`\n"
        f"- Canonical report paths: `{summary['canonical_report_path_count']}`\n"
        f"- Public actions: `{summary['public_action_count']}`\n"
        f"- Public scripts: `{summary['public_script_count']}`\n"
        f"- Artifact sections: `{summary['artifact_section_count']}`\n"
        f"- Source artifacts: `{summary['source_artifact_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
