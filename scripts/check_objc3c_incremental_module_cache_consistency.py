#!/usr/bin/env python3
"""Validate incremental module-cache invalidation and runtime metadata consistency."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_tooling.paths import repo_rel


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "module_cache"
    / "incremental_runtime_metadata_consistency.json"
)
SUMMARY_PATH = (
    ROOT
    / "tmp"
    / "reports"
    / "module-cache"
    / "incremental-runtime-metadata-consistency-summary.json"
)


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def replay_key_for_surface(surface: dict[str, Any], *, lane_contract: str) -> str:
    return (
        "incremental_module_cache_invalidation_sites="
        f"{int(surface.get('incremental_module_cache_invalidation_sites', -1))}"
        f";namespace_segment_sites={int(surface.get('namespace_segment_sites', -1))}"
        f";import_edge_candidate_sites={int(surface.get('import_edge_candidate_sites', -1))}"
        f";object_pointer_type_sites={int(surface.get('object_pointer_type_sites', -1))}"
        f";pointer_declarator_sites={int(surface.get('pointer_declarator_sites', -1))}"
        f";normalized_sites={int(surface.get('normalized_sites', -1))}"
        f";cache_invalidation_candidate_sites={int(surface.get('cache_invalidation_candidate_sites', -1))}"
        f";contract_violation_sites={int(surface.get('contract_violation_sites', -1))}"
        f";deterministic={'true' if bool(surface.get('deterministic')) else 'false'}"
        f";lane_contract={lane_contract}"
    )


def surface_is_consistent(surface: dict[str, Any]) -> bool:
    sites = int(surface.get("incremental_module_cache_invalidation_sites", -1))
    normalized = int(surface.get("normalized_sites", -1))
    candidates = int(surface.get("cache_invalidation_candidate_sites", -1))
    violations = int(surface.get("contract_violation_sites", -1))
    deterministic = surface.get("deterministic", surface.get("deterministic_handoff"))
    return (
        sites >= 0
        and 0 <= normalized <= sites
        and 0 <= candidates <= sites
        and normalized + candidates == sites
        and violations == 0
        and bool(deterministic) is True
    )


def manifest_incremental_surface(manifest: dict[str, Any]) -> dict[str, Any]:
    frontend = manifest.get("frontend", {})
    pipeline = frontend.get("pipeline", {}) if isinstance(frontend, dict) else {}
    semantic_surface = pipeline.get("semantic_surface", {}) if isinstance(pipeline, dict) else {}
    surface = semantic_surface.get("objc_incremental_module_cache_invalidation_lowering_surface", {})
    if not isinstance(surface, dict):
        raise RuntimeError("manifest missing incremental module cache lowering surface")
    return surface


def manifest_replay_key(manifest: dict[str, Any]) -> str:
    lowering = manifest.get("lowering_incremental_module_cache_invalidation", {})
    if not isinstance(lowering, dict):
        raise RuntimeError("manifest missing lowering incremental module cache section")
    return str(lowering.get("replay_key", ""))


def native_sources_enforce_cache_partition(native_anchors: list[str]) -> dict[str, bool]:
    required_by_path = {
        "native/objc3c/src/lower/contracts/cross_module_lowering_contracts.cpp": [
            "contract.normalized_sites + contract.cache_invalidation_candidate_sites !=",
            "kObjc3IncrementalModuleCacheInvalidationLoweringLaneContract",
        ],
        "native/objc3c/src/sema/objc3_semantic_passes_body_validation_core_module_type_system.inc": [
            ".normalized_sites +",
            ".cache_invalidation_candidate_sites ==",
        ],
        "native/objc3c/src/sema/objc3_sema_pass_manager_module_summary_readiness.cpp": [
            ".normalized_sites +",
            ".cache_invalidation_candidate_sites ==",
        ],
        "native/objc3c/src/sema/objc3_sema_pass_manager_module_publication_flow.cpp": [
            ".normalized_sites +",
            ".cache_invalidation_candidate_sites ==",
        ],
        "native/objc3c/src/sema/objc3_sema_pass_manager_execution_module_error_concurrency.inc": [
            ".normalized_sites +",
            ".cache_invalidation_candidate_sites ==",
        ],
        "native/objc3c/src/ir/objc3_ir_frontend_metadata_module_cache_invalidation.h": [
            "lowering_incremental_module_cache_invalidation_replay_key",
            "incremental_module_cache_invalidation_lowering_cache_invalidation_candidate_sites",
        ],
    }
    checks: dict[str, bool] = {}
    for raw_path in native_anchors:
        path = ROOT / raw_path
        text = path.read_text(encoding="utf-8")
        checks[raw_path] = all(fragment in text for fragment in required_by_path.get(raw_path, ()))
    return checks


def build_summary() -> dict[str, Any]:
    contract = load_json(CONTRACT_PATH)
    failures: list[str] = []
    expect(
        contract.get("contract_id") == "objc3c.module_cache.incremental_runtime_metadata_consistency.v1",
        "module cache consistency contract id drifted",
        failures,
    )
    owner_policy = contract.get("owner_policy", {})
    expect(isinstance(owner_policy, dict), "owner policy missing", failures)
    expect(owner_policy.get("evidence_log_allowed") is False, "evidence log must not be source authority", failures)

    native_anchors = [str(path) for path in contract.get("native_anchors", [])]
    source_paths = [
        str(contract["runbook"]),
        *native_anchors,
        *[str(path) for path in contract.get("positive_replay_manifests", [])],
        *[str(path) for path in contract.get("positive_replay_ir", [])],
    ]
    missing_source_paths = [path for path in source_paths if not (ROOT / path).is_file()]
    expect(not missing_source_paths, "module cache contract references missing source paths", failures)

    manifests = [load_json(ROOT / str(path)) for path in contract.get("positive_replay_manifests", [])]
    surfaces = [manifest_incremental_surface(manifest) for manifest in manifests]
    replay_keys = [manifest_replay_key(manifest) for manifest in manifests]
    expect(len(surfaces) >= 2, "expected at least two replay manifests", failures)
    expect(len(set(replay_keys)) == 1, "incremental module cache replay keys drifted across runs", failures)
    expect(all(surface_is_consistent(surface) for surface in surfaces), "replay manifest incremental cache surfaces are inconsistent", failures)
    for ir_path, replay_key in zip(contract.get("positive_replay_ir", []), replay_keys):
        expect(replay_key in (ROOT / str(ir_path)).read_text(encoding="utf-8"), f"IR replay key missing from {ir_path}", failures)

    positive_surface = contract.get("positive_cache_candidate_surface", {})
    if not isinstance(positive_surface, dict):
        positive_surface = {}
    expected_positive_key = replay_key_for_surface(
        positive_surface,
        lane_contract="objc3c.incremental.module.cache.invalidation.lowering.v1",
    )
    expect(surface_is_consistent(positive_surface), "positive cache candidate surface is inconsistent", failures)
    expect(
        positive_surface.get("runtime_metadata_replay_key") == expected_positive_key,
        "positive cache candidate runtime metadata replay key drifted",
        failures,
    )

    negative_surfaces = contract.get("negative_surfaces", [])
    negative_codes = {
        str(entry.get("diagnostic_code"))
        for entry in negative_surfaces
        if isinstance(entry, dict)
    }
    expect({"O3INC8081", "O3INC8082"} <= negative_codes, "negative cache diagnostics missing stable codes", failures)
    iterable_negative_surfaces = negative_surfaces if isinstance(negative_surfaces, list) else []
    for entry in iterable_negative_surfaces:
        if not isinstance(entry, dict):
            failures.append("negative cache surface entry is not an object")
            continue
        surface = entry.get("surface", {})
        if not isinstance(surface, dict):
            failures.append("negative cache surface payload is not an object")
            continue
        expect(
            not surface_is_consistent(surface)
            or surface.get("runtime_metadata_replay_key")
            != replay_key_for_surface(
                surface,
                lane_contract="objc3c.incremental.module.cache.invalidation.lowering.v1",
            ),
            f"negative cache surface did not fail closed: {entry.get('case_id')}",
            failures,
        )

    native_checks = native_sources_enforce_cache_partition(native_anchors)
    expect(all(native_checks.values()), "native cache partition checks are not threaded through all anchors", failures)

    return {
        "contract_id": "objc3c.module_cache.incremental_runtime_metadata_consistency.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "contract": repo_rel(CONTRACT_PATH),
        "issues": contract.get("issues", []),
        "owner_policy": owner_policy,
        "blocker_metadata": contract.get("blocker_metadata", {}),
        "source_paths": source_paths,
        "missing_source_paths": missing_source_paths,
        "positive_replay_keys": replay_keys,
        "positive_replay_surfaces": surfaces,
        "positive_cache_candidate_surface": positive_surface,
        "native_checks": native_checks,
        "negative_diagnostic_codes": sorted(negative_codes),
        "failures": failures,
    }


def main() -> int:
    summary = build_summary()
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, summary)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    if summary["status"] != "PASS":
        print("objc3c-incremental-module-cache-consistency: FAIL")
        for failure in summary["failures"]:
            print(f"- {failure}")
        return 1
    print("objc3c-incremental-module-cache-consistency: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
