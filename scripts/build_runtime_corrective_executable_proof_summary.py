#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/runtime_corrective/executable_proof_abi_contract.json"
OUT_DIR = ROOT / "tmp/reports/runtime-corrective/executable-proof-abi"
SUMMARY_PATH = OUT_DIR / "executable_proof_abi_contract_summary.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_runtime_corrective.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")

    shared_entrypoints = [ROOT / path for path in contract["shared_acceptance_entrypoints"]]
    dispatch_cases = contract["dispatch_runtime_cases"]
    synthesized_cases = contract["synthesized_accessor_runtime_cases"]

    summary = {
        "issue": "runtime-corrective-executable-proof-abi",
        "contract_id": contract["contract_id"],
        "runbook_mentions_contract": "executable_proof_abi_contract.json" in runbook_text,
        "shared_acceptance_entrypoint_count": len(shared_entrypoints),
        "shared_acceptance_entrypoints_exist": all(path.is_file() for path in shared_entrypoints),
        "required_compile_artifact_count": len(contract["required_compile_artifacts"]),
        "runtime_abi_contract_count": len(contract["runtime_abi_contracts"]),
        "dispatch_runtime_case_count": len(dispatch_cases),
        "synthesized_accessor_runtime_case_count": len(synthesized_cases),
        "dispatch_runtime_case_inputs_exist": all(
            (ROOT / case["fixture_path"]).is_file() and (ROOT / case["probe_path"]).is_file()
            for case in dispatch_cases
        ),
        "synthesized_accessor_runtime_case_inputs_exist": all(
            (ROOT / case["fixture_path"]).is_file() and (ROOT / case["probe_path"]).is_file()
            for case in synthesized_cases
        ),
        "proof_rules_count": len(contract["proof_rules"]),
        "explicit_non_goal_count": len(contract["explicit_non_goals"]),
    }
    summary["ok"] = all(
        [
            summary["runbook_mentions_contract"],
            summary["shared_acceptance_entrypoints_exist"],
            summary["dispatch_runtime_case_inputs_exist"],
            summary["synthesized_accessor_runtime_case_inputs_exist"],
            summary["required_compile_artifact_count"] == 5,
            summary["runtime_abi_contract_count"] == 3,
            summary["dispatch_runtime_case_count"] == 2,
            summary["synthesized_accessor_runtime_case_count"] == 3,
        ]
    )

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
