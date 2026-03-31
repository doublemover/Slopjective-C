#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/block_arc_closure/byref_promotion_copy_dispose_forwarding_contract.json"
OUT_DIR = ROOT / "tmp/reports/block-arc-closure/byref-promotion-forwarding"
JSON_OUT = OUT_DIR / "byref_promotion_copy_dispose_forwarding_summary.json"
MD_OUT = OUT_DIR / "byref_promotion_copy_dispose_forwarding_summary.md"
RUNTIME_PATH = ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_block_arc_closure.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runtime_text = RUNTIME_PATH.read_text(encoding="utf-8")
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    proof_paths = [ROOT / path for path in contract["authoritative_proof_paths"]]
    fixture_paths = [ROOT / path for path in contract["authoritative_fixture_paths"]]
    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_block_arc_closure_byref_summary.py",
        "all_authoritative_proof_paths_exist": all(path.is_file() for path in proof_paths),
        "all_authoritative_fixture_paths_exist": all(path.is_file() for path in fixture_paths),
        "all_authoritative_runtime_symbols_exported": all(symbol in runtime_text for symbol in contract["authoritative_runtime_symbols"]),
        "runtime_keeps_byref_forwarding_anchor": "byref-forwarding/heap-promotion/ownership-interop anchor:" in runtime_text,
        "runbook_mentions_byref_forwarding_runtime_path": "escaping byref behavior is only supported through the runtime-owned promotion, forwarding, copy, and dispose helper path already targeted by lowering" in runbook_text,
    }

    summary = {
        "issue": "block-arc-closure-byref-promotion-forwarding",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "authoritative_runtime_symbol_count": len(contract["authoritative_runtime_symbols"]),
        "implementation_invariant_count": len(contract["implementation_invariants"]),
        "authoritative_proof_path_count": len(contract["authoritative_proof_paths"]),
        "authoritative_fixture_path_count": len(contract["authoritative_fixture_paths"]),
        "claim_narrowing_constraint_count": len(contract["claim_narrowing_constraints"]),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Block ARC Closure Byref Forwarding Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Runtime symbols: `{summary['authoritative_runtime_symbol_count']}`\n"
        f"- Implementation invariants: `{summary['implementation_invariant_count']}`\n"
        f"- Proof paths: `{summary['authoritative_proof_path_count']}`\n"
        f"- Fixture paths: `{summary['authoritative_fixture_path_count']}`\n"
        f"- Claim-narrowing constraints: `{summary['claim_narrowing_constraint_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
