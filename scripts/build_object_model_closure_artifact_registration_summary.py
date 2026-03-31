#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/object_model_closure/object_model_reflection_artifact_runtime_registration_contract.json"
ACCEPTANCE_SCRIPT = ROOT / "scripts/check_objc3c_runtime_acceptance.py"
OUT_DIR = ROOT / "tmp/reports/object-model-closure/artifact-registration"
JSON_OUT = OUT_DIR / "object_model_reflection_artifact_runtime_registration_summary.json"
MD_OUT = OUT_DIR / "object_model_reflection_artifact_runtime_registration_summary.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    acceptance_text = ACCEPTANCE_SCRIPT.read_text(encoding="utf-8")
    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_object_model_closure_artifact_registration_summary.py",
        "all_generation_paths_exist": all((ROOT / path).is_file() for path in contract["authoritative_generation_paths"]),
        "all_compile_manifest_surface_keys_are_emitted": all(
            f"\"{key}\"" in acceptance_text for key in contract["compile_manifest_surface_keys"]
        ),
        "all_acceptance_builder_functions_exist": all(
            f"def {name}(" in acceptance_text for name in contract["acceptance_builder_functions"]
        ),
    }
    summary = {
        "issue": "object-model-closure-artifact-registration",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "compile_manifest_surface_key_count": len(contract["compile_manifest_surface_keys"]),
        "acceptance_builder_function_count": len(contract["acceptance_builder_functions"]),
        "required_compile_artifact_count": len(contract["required_compile_artifacts"]),
        "authoritative_generation_path_count": len(contract["authoritative_generation_paths"]),
        "checks": checks,
        "ok": all(checks.values()),
    }
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Object-Model Closure Artifact Registration Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Surface keys: `{summary['compile_manifest_surface_key_count']}`\n"
        f"- Builder functions: `{summary['acceptance_builder_function_count']}`\n"
        f"- Required compile artifacts: `{summary['required_compile_artifact_count']}`\n"
        f"- Generation paths: `{summary['authoritative_generation_path_count']}`\n"
        f"- Status: `{'PASS' if summary['ok'] else 'FAIL'}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
