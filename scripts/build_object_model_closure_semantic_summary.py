#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/object_model_closure/realized_object_graph_reflection_semantic_model.json"
OUT_DIR = ROOT / "tmp/reports/m319/M319-B001"
JSON_OUT = OUT_DIR / "realized_object_graph_reflection_semantic_summary.json"
MD_OUT = OUT_DIR / "realized_object_graph_reflection_semantic_summary.md"
DOC_PATH = ROOT / "docs/objc3c-native.md"
RUNTIME_PATH = ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runtime_text = RUNTIME_PATH.read_text(encoding="utf-8")
    doc_text = DOC_PATH.read_text(encoding="utf-8")

    proof_paths = [ROOT / path for path in contract["authoritative_proof_paths"]]
    checks = {
      "summary_script_link_matches": contract["summary_script"] == "scripts/build_object_model_closure_semantic_summary.py",
      "all_authoritative_proof_paths_exist": all(path.is_file() for path in proof_paths),
      "aggregate_query_symbol_exported": "int objc3_runtime_copy_object_model_query_state_for_testing(" in runtime_text,
      "realized_class_entry_query_exported": "int objc3_runtime_copy_realized_class_entry_for_testing(" in runtime_text,
      "property_entry_query_exported": "int objc3_runtime_copy_property_entry_for_testing(" in runtime_text,
      "protocol_query_exported": "int objc3_runtime_copy_protocol_conformance_query_for_testing(" in runtime_text,
      "docs_publish_object_model_abi_query_boundary": "This is the authoritative object-model runtime ABI/query boundary." in doc_text,
      "docs_publish_realization_lookup_boundary": "This is the authoritative live implementation boundary for realization lookup" in doc_text,
      "docs_publish_private_reflection_constraint": "private-testing-snapshots-remain-the-only-reflection-visibility-surface-and-publish-runtime-owned-class-property-and-protocol-state" in doc_text
    }

    summary = {
        "issue": "M319-B001",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "canonical_surface_count": len(contract["canonical_surfaces"]),
        "public_runtime_abi_symbol_count": len(contract["public_runtime_abi"]),
        "private_runtime_query_symbol_count": len(contract["private_runtime_query_symbols"]),
        "frozen_semantic_model_count": len(contract["frozen_semantic_models"]),
        "claim_narrowing_constraint_count": len(contract["claim_narrowing_constraints"]),
        "checks": checks,
        "ok": all(checks.values())
    }
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# M319-B001 Realized Object Graph And Reflection Semantic Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Canonical surfaces: `{summary['canonical_surface_count']}`\n"
        f"- Public ABI symbols: `{summary['public_runtime_abi_symbol_count']}`\n"
        f"- Private query symbols: `{summary['private_runtime_query_symbol_count']}`\n"
        f"- Frozen semantic models: `{summary['frozen_semantic_model_count']}`\n"
        f"- Claim-narrowing constraints: `{summary['claim_narrowing_constraint_count']}`\n"
        f"- Status: `{'PASS' if summary['ok'] else 'FAIL'}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
