from __future__ import annotations

from typing import Any

from objc3c_parser_draft_syntax_conformance.compiler import run_compiler
from objc3c_parser_draft_syntax_conformance.diagnostics import find_parser_manifest
from objc3c_parser_draft_syntax_conformance.diagnostics import parse_replay_key
from objc3c_parser_draft_syntax_conformance.matrix import build_surface_results
from objc3c_parser_draft_syntax_conformance.matrix import collect_negative_fixture_paths
from objc3c_parser_draft_syntax_conformance.paths import CONFORMANCE_MANIFEST
from objc3c_parser_draft_syntax_conformance.paths import CONTRACT_ID
from objc3c_parser_draft_syntax_conformance.paths import POSITIVE_FIXTURE
from objc3c_parser_draft_syntax_conformance.paths import TMP_ROOT
from objc3c_parser_draft_syntax_conformance.paths import rel
from objc3c_parser_draft_syntax_conformance.source_loading import REPLAY_KEY_FIELDS
from objc3c_parser_draft_syntax_conformance.source_loading import REQUIRED_SURFACES
from objc3c_parser_draft_syntax_conformance.source_loading import build_surface_index
from objc3c_parser_draft_syntax_conformance.source_loading import load_draft_syntax_sources
from objc3c_parser_draft_syntax_conformance.source_loading import static_source_truth_paths


def build_summary() -> dict[str, Any]:
    sources = load_draft_syntax_sources()
    manifest = sources["manifest"]
    positive_text = sources["positive_text"]
    parser_text = sources["parser_text"]
    readme_text = sources["readme_text"]
    surfaces = manifest["surfaces"]
    by_id = build_surface_index(surfaces)
    missing_surfaces = [surface_id for surface_id in REQUIRED_SURFACES if surface_id not in by_id]
    extra_surfaces = [surface["id"] for surface in surfaces if surface["id"] not in REQUIRED_SURFACES]

    positive_run = run_compiler(POSITIVE_FIXTURE, TMP_ROOT / "positive")
    parser_manifest = find_parser_manifest(positive_run.get("manifest")) if positive_run.get("manifest") else None
    replay_fields = parse_replay_key(parser_manifest.get("draft_syntax_surface_handoff_key", "")) if parser_manifest else {}

    surface_results = build_surface_results(surfaces, positive_text, replay_fields)

    source_truth_paths = [
        *static_source_truth_paths(),
        *collect_negative_fixture_paths(surfaces),
    ]
    no_tmp_source_truth = all(not rel(path).startswith("tmp/") for path in source_truth_paths)
    checks = {
        "manifest_contract_matches": manifest.get("contract_id") == CONTRACT_ID,
        "all_required_surfaces_listed": not missing_surfaces,
        "no_unknown_surfaces_listed": not extra_surfaces,
        "positive_fixture_compiles": positive_run["exit_code"] == 0,
        "positive_manifest_has_parser_surface": parser_manifest is not None,
        "positive_manifest_deterministic": bool(parser_manifest and parser_manifest.get("draft_syntax_surface_deterministic")),
        "positive_manifest_total_covers_surface_counts": bool(
            parser_manifest
            and int(parser_manifest.get("draft_syntax_surface_count", 0))
            >= sum(replay_fields.get(REPLAY_KEY_FIELDS[surface_id], 0) for surface_id in REQUIRED_SURFACES)
        ),
        "parser_has_property_behavior_payload_guard": "O3P355" in parser_text,
        "readme_references_conformance_manifest": rel(CONFORMANCE_MANIFEST) in readme_text,
        "no_tmp_source_truth": no_tmp_source_truth,
        "all_surface_results_pass": all(result["status"] == "PASS" for result in surface_results),
    }
    status = "PASS" if all(checks.values()) else "FAIL"
    return {
        "contract_id": CONTRACT_ID,
        "issue": "#8012",
        "status": status,
        "checks": checks,
        "missing_surfaces": missing_surfaces,
        "extra_surfaces": extra_surfaces,
        "source_truth_paths": [rel(path) for path in source_truth_paths],
        "positive_fixture": rel(POSITIVE_FIXTURE),
        "positive_compile_exit_code": positive_run["exit_code"],
        "positive_manifest_parser_surface": parser_manifest,
        "replay_key_counts": replay_fields,
        "surface_results": surface_results,
        "validation_commands": [
            "python scripts/build_objc3c_parser_draft_syntax_conformance.py --check",
            "python -m pytest tests/tooling/test_build_objc3c_parser_draft_syntax_conformance.py",
            "npm run objc3c -- test-fixture-matrix",
            "npm run objc3c -- test-negative-expectations",
            "npm run objc3c -- test-execution-replay",
        ],
    }
