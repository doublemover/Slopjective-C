#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/block_arc_closure/escaping_block_byref_ownership_semantic_model.json"
OUT_DIR = ROOT / "tmp/reports/m320/M320-B001"
JSON_OUT = OUT_DIR / "escaping_block_byref_ownership_semantic_summary.json"
MD_OUT = OUT_DIR / "escaping_block_byref_ownership_semantic_summary.md"
DOC_PATH = ROOT / "docs/objc3c-native.md"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_block_arc_closure.md"
RUNTIME_PATH = ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runtime_text = RUNTIME_PATH.read_text(encoding="utf-8")
    doc_text = DOC_PATH.read_text(encoding="utf-8")
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")

    fixture_paths = [ROOT / path for path in contract["authoritative_fixture_paths"]]
    probe_paths = [ROOT / path for path in contract["authoritative_probe_paths"]]
    code_paths = [ROOT / path for path in contract["authoritative_code_paths"]]
    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_block_arc_closure_semantic_summary.py",
        "all_authoritative_code_paths_exist": all(path.is_file() for path in code_paths),
        "all_authoritative_fixture_paths_exist": all(path.is_file() for path in fixture_paths),
        "all_authoritative_probe_paths_exist": all(path.is_file() for path in probe_paths),
        "runtime_exports_block_promote": 'extern "C" int objc3_runtime_promote_block_i32(' in runtime_text,
        "runtime_exports_block_invoke": 'extern "C" int objc3_runtime_invoke_block_i32(' in runtime_text,
        "runtime_keeps_byref_forwarding_anchor": "byref-forwarding/heap-promotion/ownership-interop anchor:" in runtime_text,
        "docs_publish_block_arc_unified_source_surface": "## Block/ARC Unified Source Surface" in doc_text,
        "docs_publish_capture_family_surface": "## Ownership Transfer And Capture-Family Source Surface" in doc_text,
        "runbook_mentions_executable_ownership_story": "byref forwarding, copy/dispose execution, and heap-promotion behavior over the live runtime path" in runbook_text,
    }

    summary = {
        "issue": "M320-B001",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "canonical_surface_count": len(contract["canonical_surfaces"]),
        "semantic_surface_path_count": len(contract["semantic_surface_paths"]),
        "capture_profile_field_count": len(contract["capture_profile_fields"]),
        "frozen_semantic_model_count": len(contract["frozen_semantic_models"]),
        "claim_narrowing_constraint_count": len(contract["claim_narrowing_constraints"]),
        "authoritative_case_id_count": len(contract["authoritative_case_ids"]),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# M320-B001 Escaping Block Byref Ownership Semantic Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Canonical surfaces: `{summary['canonical_surface_count']}`\n"
        f"- Semantic surface paths: `{summary['semantic_surface_path_count']}`\n"
        f"- Capture profile fields: `{summary['capture_profile_field_count']}`\n"
        f"- Frozen semantic models: `{summary['frozen_semantic_model_count']}`\n"
        f"- Claim-narrowing constraints: `{summary['claim_narrowing_constraint_count']}`\n"
        f"- Authoritative case ids: `{summary['authoritative_case_id_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
