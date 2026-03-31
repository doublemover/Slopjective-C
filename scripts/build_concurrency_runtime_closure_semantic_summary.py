#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/concurrency_runtime_closure/unified_concurrency_runtime_semantic_model.json"
OUT_DIR = ROOT / "tmp/reports/concurrency-runtime-closure/unified-concurrency-runtime-semantic"
JSON_OUT = OUT_DIR / "unified_concurrency_runtime_semantic_summary.json"
MD_OUT = OUT_DIR / "unified_concurrency_runtime_semantic_summary.md"
DOC_PATH = ROOT / "docs/objc3c-native.md"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_concurrency_runtime_closure.md"
CONFORMANCE_REPORT = ROOT / "tmp/reports/runtime/runnable-concurrency-conformance/summary.json"
STDLIB_ADVANCED_PATH = ROOT / "stdlib/advanced_helper_package_surface.json"
STDLIB_PROGRAM_PATH = ROOT / "stdlib/program_surface.json"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    doc_text = DOC_PATH.read_text(encoding="utf-8")
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    conformance_report = read_json(CONFORMANCE_REPORT)
    advanced_surface_text = STDLIB_ADVANCED_PATH.read_text(encoding="utf-8")
    program_surface_text = STDLIB_PROGRAM_PATH.read_text(encoding="utf-8")

    fixture_paths = [ROOT / path for path in contract["authoritative_fixture_paths"]]
    probe_paths = [ROOT / path for path in contract["authoritative_probe_paths"]]
    code_paths = [ROOT / path for path in contract["authoritative_code_paths"]]

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_concurrency_runtime_closure_semantic_summary.py",
        "all_authoritative_code_paths_exist": all(path.is_file() for path in code_paths),
        "all_authoritative_fixture_paths_exist": all(path.is_file() for path in fixture_paths),
        "all_authoritative_probe_paths_exist": all(path.is_file() for path in probe_paths),
        "docs_publish_unified_concurrency_source_surface": "## Unified Concurrency Runtime Source Surface" in doc_text,
        "docs_publish_normalization_surface": "## Async/Task/Actor Normalization Completion Surface" in doc_text,
        "docs_publish_lowering_surface": "## Unified Concurrency Lowering/Metadata Surface" in doc_text,
        "runbook_keeps_runtime_responsibility_explicit": "scheduler, executor, continuation, task, and actor semantics are runtime responsibilities" in runbook_text,
        "runbook_keeps_task_and_actor_tracks_explicit": "task runtime and actor runtime are separate internal tracks" in runbook_text,
        "runbook_mentions_successor_bounded_interactions": "interaction with ARC and thrown-error cleanup remains a successor concern" in runbook_text,
        "conformance_report_passes": conformance_report.get("status") == "PASS",
        "conformance_report_publishes_required_surfaces": all(
            isinstance(conformance_report.get(surface_key), dict) for surface_key in contract["canonical_surfaces"]
        ),
        "conformance_report_requires_all_case_ids": all(
            case_id in conformance_report.get("required_case_ids", []) for case_id in contract["authoritative_case_ids"]
        ),
        "stdlib_advanced_surface_mentions_concurrency_module": "\"objc3.concurrency\"" in advanced_surface_text,
        "stdlib_program_surface_keeps_actor_shaped_claim_narrowing": "actor-shaped-messaging" in program_surface_text and "not-yet-runnable capabilities must be framed as actor-shaped comparison or migration guidance rather than runnable parity claims" in program_surface_text,
    }

    summary = {
        "issue": "concurrency-runtime-closure-unified-concurrency-runtime-semantic",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "canonical_surface_count": len(contract["canonical_surfaces"]),
        "semantic_model_count": len(contract["semantic_models"]),
        "claim_narrowing_constraint_count": len(contract["claim_narrowing_constraints"]),
        "authoritative_case_id_count": len(contract["authoritative_case_ids"]),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Concurrency Runtime Closure Semantic Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Canonical surfaces: `{summary['canonical_surface_count']}`\n"
        f"- Semantic models: `{summary['semantic_model_count']}`\n"
        f"- Claim narrowing constraints: `{summary['claim_narrowing_constraint_count']}`\n"
        f"- Authoritative case ids: `{summary['authoritative_case_id_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
