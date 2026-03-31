#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
ACTOR_CONTRACT_PATH = ROOT / "tests/tooling/fixtures/concurrency_runtime_closure/actor_isolation_sendability_hop_contract.json"
ABI_CONTRACT_PATH = ROOT / "tests/tooling/fixtures/concurrency_runtime_closure/lowering_runtime_abi_contract.json"
ACCEPTANCE_REPORT = ROOT / "tmp/reports/runtime/acceptance/summary.json"
CONFORMANCE_REPORT = ROOT / "tmp/reports/runtime/runnable-concurrency-conformance/summary.json"
E2E_REPORT = ROOT / "tmp/reports/runtime/runnable-concurrency-e2e/summary.json"
CONFORMANCE_SCRIPT = ROOT / "scripts/check_objc3c_runnable_concurrency_conformance.py"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def contains_case(report: dict[str, Any], case_id: str) -> bool:
    for case in report.get("cases", []):
        if isinstance(case, dict) and case.get("case_id") == case_id and case.get("passed") is True:
            return True
    return False


def main() -> int:
    actor_contract = read_json(ACTOR_CONTRACT_PATH)
    abi_contract = read_json(ABI_CONTRACT_PATH)
    acceptance_report = read_json(ACCEPTANCE_REPORT)
    conformance_report = read_json(CONFORMANCE_REPORT)
    e2e_report = read_json(E2E_REPORT)
    conformance_text = CONFORMANCE_SCRIPT.read_text(encoding="utf-8")

    actor_payload = e2e_report.get("probe_payloads", {}).get("actor", {})
    lowering_surface = conformance_report.get("runtime_unified_concurrency_lowering_metadata_surface", {})
    abi_surface = conformance_report.get("runtime_unified_concurrency_runtime_abi_surface", {})

    checks = {
        "acceptance_report_passes": acceptance_report.get("status") == "PASS",
        "conformance_report_passes": conformance_report.get("status") == "PASS",
        "acceptance_contains_lowering_case": contains_case(acceptance_report, "unified-concurrency-lowering-metadata-surface"),
        "acceptance_contains_runtime_abi_case": contains_case(acceptance_report, "unified-concurrency-runtime-abi"),
        "acceptance_contains_live_runtime_case": contains_case(acceptance_report, "live-unified-concurrency-runtime-implementation"),
        "acceptance_contains_cross_module_actor_case": contains_case(acceptance_report, "cross-module-concurrency-actor-artifact-preservation"),
        "conformance_emits_required_surface_keys": all(
            key in conformance_report for key in abi_contract["compile_manifest_surface_keys"]
        ),
        "lowering_surface_preserves_actor_contract": (
            isinstance(lowering_surface, dict)
            and "objc3c.concurrency.actor.lowering.and.metadata.contract.v1" in lowering_surface.get("lowering_contract_ids", [])
            and "frontend.pipeline.semantic_surface.objc_concurrency_actor_lowering_and_metadata_contract" in lowering_surface.get("authoritative_surface_fields", [])
        ),
        "abi_surface_preserves_actor_snapshot_symbol": (
            isinstance(abi_surface, dict)
            and abi_surface.get("actor_runtime_state_snapshot_symbol") == "objc3_runtime_copy_actor_runtime_state_for_testing"
            and abi_surface.get("actor_runtime_model") == "actor-isolation-nonisolated-hop-replay-race-guard-mailbox-and-testing-snapshots-stay-on-bootstrap-internal-runtime-entrypoints"
        ),
        "actor_payload_matches_actor_contract": all(
            actor_payload.get(key) == value for key, value in actor_contract["required_actor_probe_payload"].items()
        ),
        "conformance_script_binds_actor_runtime_contract": (
            "unified concurrency lowering surface drifted from the actor lowering metadata contract" in conformance_text
            and "unified concurrency runtime ABI surface drifted from the actor runtime snapshot symbol" in conformance_text
        ),
        "conformance_script_binds_cross_module_actor_case": "cross-module-concurrency-actor-artifact-preservation" in conformance_text,
    }

    summary = {
        "issue": "concurrency-runtime-closure-actor-lowering-proof",
        "actor_contract_id": actor_contract["contract_id"],
        "abi_contract_id": abi_contract["contract_id"],
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
