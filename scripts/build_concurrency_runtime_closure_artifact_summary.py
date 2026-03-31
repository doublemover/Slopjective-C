#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/concurrency_runtime_closure/lowering_runtime_abi_contract.json"
ACCEPTANCE_SCRIPT = ROOT / "scripts/check_objc3c_runtime_acceptance.py"
ACCEPTANCE_REPORT = ROOT / "tmp/reports/runtime/acceptance/summary.json"
FRONTEND_ARTIFACTS = ROOT / "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp"
WORKFLOW_RUNNER = ROOT / "scripts/objc3c_public_workflow_runner.py"
OUT_DIR = ROOT / "tmp/reports/concurrency-runtime-closure/lowering-runtime-abi"
JSON_OUT = OUT_DIR / "concurrency_lowering_runtime_abi_summary.json"
MD_OUT = OUT_DIR / "concurrency_lowering_runtime_abi_summary.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    acceptance_text = ACCEPTANCE_SCRIPT.read_text(encoding="utf-8")
    acceptance_report = read_json(ACCEPTANCE_REPORT)
    frontend_text = FRONTEND_ARTIFACTS.read_text(encoding="utf-8")
    workflow_text = WORKFLOW_RUNNER.read_text(encoding="utf-8")

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_concurrency_runtime_closure_artifact_summary.py",
        "all_generation_paths_exist": all((ROOT / path).is_file() for path in contract["authoritative_generation_paths"]),
        "all_compile_manifest_surface_keys_are_emitted_in_script": all(
            f'"{key}"' in acceptance_text for key in contract["compile_manifest_surface_keys"]
        ),
        "all_acceptance_builder_functions_exist": all(
            f"def {name}(" in acceptance_text for name in contract["acceptance_builder_functions"]
        ),
        "acceptance_report_passes": acceptance_report.get("status") == "PASS",
        "acceptance_report_emits_all_surface_keys": all(
            key in acceptance_report for key in contract["compile_manifest_surface_keys"]
        ),
        "frontend_surface_fields_are_emitted": all(
            field in frontend_text for field in contract["frontend_surface_fields"]
        ),
        "workflow_runner_exports_required_actions": all(
            f"def {name}(" in workflow_text for name in contract["required_public_workflow_actions"]
        ),
    }

    summary = {
        "issue": "concurrency-runtime-closure-lowering-runtime-abi",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "compile_manifest_surface_key_count": len(contract["compile_manifest_surface_keys"]),
        "acceptance_builder_function_count": len(contract["acceptance_builder_functions"]),
        "frontend_surface_field_count": len(contract["frontend_surface_fields"]),
        "required_compile_artifact_count": len(contract["required_compile_artifacts"]),
        "authoritative_generation_path_count": len(contract["authoritative_generation_paths"]),
        "required_public_workflow_action_count": len(contract["required_public_workflow_actions"]),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Concurrency Runtime Closure Lowering Runtime ABI Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Surface keys: `{summary['compile_manifest_surface_key_count']}`\n"
        f"- Builder functions: `{summary['acceptance_builder_function_count']}`\n"
        f"- Frontend surface fields: `{summary['frontend_surface_field_count']}`\n"
        f"- Required compile artifacts: `{summary['required_compile_artifact_count']}`\n"
        f"- Generation paths: `{summary['authoritative_generation_path_count']}`\n"
        f"- Public workflow actions: `{summary['required_public_workflow_action_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
