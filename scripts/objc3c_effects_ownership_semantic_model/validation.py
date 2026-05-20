from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_effects_ownership_semantic_model.contracts import LANDED_FLAGS
from objc3c_effects_ownership_semantic_model.contracts import POSITIVE_MIN_COUNTS
from objc3c_effects_ownership_semantic_model.contracts import RUNTIME_REPLAY_SEGMENTS
from objc3c_effects_ownership_semantic_model.contracts import SUMMARY_FIELDS
from objc3c_effects_ownership_semantic_model.paths import CONFORMANCE_NEGATIVE
from objc3c_effects_ownership_semantic_model.paths import CONFORMANCE_POSITIVE
from objc3c_effects_ownership_semantic_model.paths import NEGATIVE_FIXTURE
from objc3c_effects_ownership_semantic_model.paths import POSITIVE_FIXTURE
from objc3c_effects_ownership_semantic_model.paths import rel


def diagnostic_matches(diagnostics: list[dict[str, Any]], code: str, line: int, column: int) -> bool:
    return any(
        diag.get("code") == code
        and int(diag.get("line", -1)) == line
        and int(diag.get("column", -1)) == column
        for diag in diagnostics
    )


def build_checks(
    *,
    positive_run: dict[str, Any],
    negative_run: dict[str, Any],
    model: dict[str, Any] | None,
    replay_key: str,
    semantic_inputs: dict[str, Any],
    static_presence: dict[str, dict[str, bool]],
    source_truth_paths: list[Path],
    missing_required_slices_run: dict[str, Any],
    missing_required_slices_model: dict[str, Any] | None,
) -> dict[str, bool]:
    manifest_text = semantic_inputs["manifest_text"]
    readme_text = semantic_inputs["readme_text"]
    stress_manifest_text = semantic_inputs["stress_manifest_text"]
    conformance_positive = semantic_inputs["conformance_positive"]
    conformance_negative = semantic_inputs["conformance_negative"]
    conformance_helper_symbols = semantic_inputs["conformance_helper_symbols"]

    return {
        "positive_fixture_compiles": positive_run["exit_code"] == 0,
        "positive_manifest_emitted": positive_run["manifest_path"] is not None,
        "positive_llvm_ir_emitted": positive_run["llvm_ir_path"] is not None,
        "manifest_has_effects_ownership_surface": model is not None,
        "all_summary_fields_emitted": bool(model) and all(field in model for field in SUMMARY_FIELDS),
        "positive_minimum_counts_observed": bool(model)
        and all(int(model.get(field, -1)) >= minimum for field, minimum in POSITIVE_MIN_COUNTS.items()),
        "positive_landed_flags_true": bool(model) and all(bool(model.get(flag)) for flag in LANDED_FLAGS),
        "positive_contract_violations_zero": bool(model) and int(model.get("contract_violation_sites", -1)) == 0,
        "positive_copy_dispose_helper_symbols_cover_required": bool(model)
        and int(model.get("copy_helper_symbolized_sites", -1))
        >= int(model.get("copy_helper_required_sites", -1))
        and int(model.get("dispose_helper_symbolized_sites", -1))
        >= int(model.get("dispose_helper_required_sites", -1)),
        "positive_ready_and_deterministic": bool(model)
        and bool(model.get("deterministic"))
        and bool(model.get("ready_for_lowering_and_runtime")),
        "missing_required_slices_fixture_compiles": missing_required_slices_run["exit_code"] == 0,
        "missing_required_slices_manifest_emitted": missing_required_slices_run["manifest_path"] is not None,
        "missing_required_slices_model_emitted": missing_required_slices_model is not None,
        "missing_required_slices_fail_closed_readiness": bool(missing_required_slices_model)
        and not bool(missing_required_slices_model.get("ready_for_lowering_and_runtime"))
        and any(not bool(missing_required_slices_model.get(flag)) for flag in LANDED_FLAGS),
        "missing_required_slices_failure_reason_observed": bool(missing_required_slices_model)
        and "missing required" in str(missing_required_slices_model.get("failure_reason", "")),
        "replay_key_covers_effects_axes": all(segment in replay_key for segment in RUNTIME_REPLAY_SEGMENTS),
        "negative_fixture_fails_closed": negative_run["exit_code"] != 0,
        "negative_diagnostics_json_emitted": negative_run["diagnostics_path"] is not None,
        "negative_async_throws_diagnostic_observed": diagnostic_matches(negative_run["diagnostics"], "O3S226", 4, 10),
        "semantic_manifest_indexes_eff_8014_01": "EFF-8014-01.json" in manifest_text,
        "semantic_manifest_indexes_eff_8014_02": "EFF-8014-02.json" in manifest_text,
        "semantic_manifest_indexes_eff_8014_03": "EFF-8014-03.json" in manifest_text,
        "semantic_readme_mentions_issue_8014": "#8014" in readme_text,
        "semantic_readme_mentions_positive_fixture": rel(POSITIVE_FIXTURE) in readme_text,
        "semantic_readme_mentions_negative_fixture": rel(NEGATIVE_FIXTURE) in readme_text,
        "positive_conformance_references_fixture": rel(POSITIVE_FIXTURE) in conformance_positive.get("references", []),
        "negative_conformance_references_fixture": rel(NEGATIVE_FIXTURE) in conformance_negative.get("references", []),
        "helper_symbol_conformance_references_summary": "tmp/reports/claimability/effects-ownership-semantic-model/effects_ownership_semantic_model_summary.json"
        in conformance_helper_symbols.get("references", []),
        "helper_symbol_conformance_requires_symbolized_counts": conformance_helper_symbols.get("expect", {})
        .get("manifest_surface_requirements", {})
        .get("copy_helper_symbolized_sites")
        == ">= copy_helper_required_sites"
        and conformance_helper_symbols.get("expect", {})
        .get("manifest_surface_requirements", {})
        .get("dispose_helper_symbolized_sites")
        == ">= dispose_helper_required_sites",
        "negative_conformance_expects_o3s226_location": conformance_negative.get("expect", {}).get("diagnostics")
        == [{"code": "O3S226", "line": 4, "column": 10}],
        "stress_manifest_compiles_positive_fixture": rel(POSITIVE_FIXTURE) in stress_manifest_text,
        "no_tmp_source_truth": all(not rel(path).startswith("tmp/") for path in source_truth_paths),
        "static_sources_thread_surface": all(all(values.values()) for values in static_presence.values()),
    }
