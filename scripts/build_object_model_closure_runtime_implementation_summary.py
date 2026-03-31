#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/object_model_closure/realized_object_graph_runtime_implementation_contract.json"
RUNTIME_PATH = ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp"
OUT_DIR = ROOT / "tmp/reports/object-model-closure/runtime-implementation"
JSON_OUT = OUT_DIR / "realized_object_graph_runtime_implementation_summary.json"
MD_OUT = OUT_DIR / "realized_object_graph_runtime_implementation_summary.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runtime_text = RUNTIME_PATH.read_text(encoding="utf-8")
    proof_paths = [ROOT / path for path in contract["authoritative_proof_paths"]]
    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_object_model_closure_runtime_implementation_summary.py",
        "all_authoritative_proof_paths_exist": all(path.is_file() for path in proof_paths),
        "all_authoritative_runtime_symbols_exported": all(
            symbol in runtime_text for symbol in contract["authoritative_runtime_symbols"]
        ),
        "runtime_registers_replay_and_reset_surface": all(
            needle in runtime_text
            for needle in (
                "int objc3_runtime_replay_registered_images_for_testing(",
                "void objc3_runtime_reset_for_testing(",
            )
        ),
    }
    summary = {
        "issue": "object-model-closure-runtime-implementation",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "authoritative_runtime_symbol_count": len(contract["authoritative_runtime_symbols"]),
        "implementation_invariant_count": len(contract["implementation_invariants"]),
        "authoritative_proof_path_count": len(contract["authoritative_proof_paths"]),
        "checks": checks,
        "ok": all(checks.values())
    }
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Object-Model Closure Runtime Implementation Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Runtime symbols: `{summary['authoritative_runtime_symbol_count']}`\n"
        f"- Implementation invariants: `{summary['implementation_invariant_count']}`\n"
        f"- Proof paths: `{summary['authoritative_proof_path_count']}`\n"
        f"- Status: `{'PASS' if summary['ok'] else 'FAIL'}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
