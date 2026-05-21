#!/usr/bin/env python3
"""Validate incremental module-cache invalidation and runtime metadata consistency."""

from __future__ import annotations

import hashlib
import json
from pathlib import Path
from pathlib import PureWindowsPath
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_tooling.paths import display_path


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_ID = "objc3c.module_cache.incremental_runtime_metadata_consistency.v1"
SUMMARY_CONTRACT_ID = "objc3c.module_cache.incremental_runtime_metadata_consistency.summary.v1"
LANE_CONTRACT = "objc3c.incremental.module.cache.invalidation.lowering.v1"

DIAG_MISSING_INPUT = "O3INC8163-MISSING-INPUT"
DIAG_STALE_INPUT = "O3INC8163-STALE-INPUT"
DIAG_TMP_INPUT = "O3INC8163-TMP-INPUT"
DIAG_MALFORMED_INPUT = "O3INC8163-MALFORMED-INPUT"
DIAG_SOURCE_PATH = "O3INC8163-SOURCE-PATH"

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


def add_diagnostic(
    diagnostics: list[dict[str, str]],
    code: str,
    message: str,
    *,
    path: str | None = None,
    detail: str | None = None,
) -> None:
    diagnostic = {"code": code, "message": message}
    if path is not None:
        diagnostic["path"] = path
    if detail is not None:
        diagnostic["detail"] = detail
    diagnostics.append(diagnostic)


def diagnostic_failures(diagnostics: list[dict[str, str]]) -> list[str]:
    failures: list[str] = []
    for diagnostic in diagnostics:
        message = f"{diagnostic['code']}: {diagnostic['message']}"
        if "path" in diagnostic:
            message += f" [{diagnostic['path']}]"
        if "detail" in diagnostic:
            message += f" ({diagnostic['detail']})"
        failures.append(message)
    return failures


def surface_int(surface: dict[str, Any], key: str) -> int:
    try:
        return int(surface.get(key, -1))
    except (TypeError, ValueError):
        return -1


def replay_key_for_surface(surface: dict[str, Any], *, lane_contract: str) -> str:
    deterministic = surface.get("deterministic", surface.get("deterministic_handoff"))
    return (
        "incremental_module_cache_invalidation_sites="
        f"{surface_int(surface, 'incremental_module_cache_invalidation_sites')}"
        f";namespace_segment_sites={surface_int(surface, 'namespace_segment_sites')}"
        f";import_edge_candidate_sites={surface_int(surface, 'import_edge_candidate_sites')}"
        f";object_pointer_type_sites={surface_int(surface, 'object_pointer_type_sites')}"
        f";pointer_declarator_sites={surface_int(surface, 'pointer_declarator_sites')}"
        f";normalized_sites={surface_int(surface, 'normalized_sites')}"
        f";cache_invalidation_candidate_sites={surface_int(surface, 'cache_invalidation_candidate_sites')}"
        f";contract_violation_sites={surface_int(surface, 'contract_violation_sites')}"
        f";deterministic={'true' if deterministic is True else 'false'}"
        f";lane_contract={lane_contract}"
    )


def surface_is_consistent(surface: dict[str, Any]) -> bool:
    sites = surface_int(surface, "incremental_module_cache_invalidation_sites")
    normalized = surface_int(surface, "normalized_sites")
    candidates = surface_int(surface, "cache_invalidation_candidate_sites")
    violations = surface_int(surface, "contract_violation_sites")
    deterministic = surface.get("deterministic", surface.get("deterministic_handoff"))
    return (
        sites >= 0
        and 0 <= normalized <= sites
        and 0 <= candidates <= sites
        and normalized + candidates == sites
        and violations == 0
        and deterministic is True
    )


def normalize_contract_source_path(
    raw_path: Any,
    diagnostics: list[dict[str, str]],
    *,
    field: str,
) -> str | None:
    if not isinstance(raw_path, str) or not raw_path.strip():
        add_diagnostic(
            diagnostics,
            DIAG_SOURCE_PATH,
            f"{field} must be a non-empty repository-relative source path",
            path=str(raw_path),
        )
        return None

    normalized = raw_path.replace("\\", "/")
    parts = normalized.split("/")
    if (
        normalized.startswith("/")
        or normalized.startswith("\\")
        or PureWindowsPath(raw_path).drive
        or any(part in {"", ".", ".."} for part in parts)
    ):
        add_diagnostic(
            diagnostics,
            DIAG_SOURCE_PATH,
            f"{field} must stay inside checked-in repository source paths",
            path=normalized,
        )
        return None

    if parts[0] == "tmp":
        add_diagnostic(
            diagnostics,
            DIAG_TMP_INPUT,
            f"{field} must not use generated tmp artifacts as source truth",
            path=normalized,
        )
        return None

    resolved = (ROOT / normalized).resolve()
    try:
        resolved.relative_to(ROOT.resolve())
    except ValueError:
        add_diagnostic(
            diagnostics,
            DIAG_SOURCE_PATH,
            f"{field} resolved outside the repository root",
            path=normalized,
        )
        return None

    return normalized


def contract_path_list(
    contract: dict[str, Any],
    key: str,
    diagnostics: list[dict[str, str]],
    *,
    contract_rel: str,
) -> list[str]:
    raw_paths = contract.get(key, [])
    if not isinstance(raw_paths, list):
        add_diagnostic(
            diagnostics,
            DIAG_MALFORMED_INPUT,
            f"{key} must be a list of repository-relative source paths",
            path=contract_rel,
        )
        return []

    paths: list[str] = []
    for index, raw_path in enumerate(raw_paths):
        normalized = normalize_contract_source_path(raw_path, diagnostics, field=f"{key}[{index}]")
        if normalized is not None:
            paths.append(normalized)
    return paths


def contract_runbook_path(contract: dict[str, Any], diagnostics: list[dict[str, str]]) -> str | None:
    return normalize_contract_source_path(contract.get("runbook"), diagnostics, field="runbook")


def require_source_files(paths: list[str], diagnostics: list[dict[str, str]]) -> dict[str, Path]:
    existing: dict[str, Path] = {}
    for rel_path in paths:
        path = ROOT / rel_path
        if not path.is_file():
            add_diagnostic(
                diagnostics,
                DIAG_MISSING_INPUT,
                "checked-in module cache source input is missing",
                path=rel_path,
            )
            continue
        existing[rel_path] = path
    return existing


def load_checked_json(
    path: Path,
    diagnostics: list[dict[str, str]],
    *,
    rel_path: str,
) -> dict[str, Any] | None:
    try:
        return load_json(path)
    except json.JSONDecodeError as exc:
        add_diagnostic(
            diagnostics,
            DIAG_MALFORMED_INPUT,
            "checked-in module cache JSON input is malformed",
            path=rel_path,
            detail=exc.msg,
        )
    except RuntimeError as exc:
        add_diagnostic(
            diagnostics,
            DIAG_MALFORMED_INPUT,
            "checked-in module cache JSON input is not an object",
            path=rel_path,
            detail=str(exc),
        )
    except OSError as exc:
        add_diagnostic(
            diagnostics,
            DIAG_MISSING_INPUT,
            "checked-in module cache JSON input could not be read",
            path=rel_path,
            detail=str(exc),
        )
    return None


def checked_text(
    path: Path,
    diagnostics: list[dict[str, str]],
    *,
    rel_path: str,
) -> str | None:
    try:
        return path.read_text(encoding="utf-8")
    except OSError as exc:
        add_diagnostic(
            diagnostics,
            DIAG_MISSING_INPUT,
            "checked-in module cache text input could not be read",
            path=rel_path,
            detail=str(exc),
        )
    return None


def file_sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def validate_cache_input_digests(
    contract: dict[str, Any],
    source_files: dict[str, Path],
    diagnostics: list[dict[str, str]],
    *,
    contract_rel: str,
    expected_cache_inputs: list[str],
) -> dict[str, str]:
    source_truth = contract.get("checked_in_source_truth", {})
    if not isinstance(source_truth, dict):
        add_diagnostic(
            diagnostics,
            DIAG_MALFORMED_INPUT,
            "checked_in_source_truth must be an object",
            path=contract_rel,
        )
        return {}

    raw_cache_inputs = source_truth.get("cache_inputs", [])
    if not isinstance(raw_cache_inputs, list):
        add_diagnostic(
            diagnostics,
            DIAG_MALFORMED_INPUT,
            "checked_in_source_truth.cache_inputs must be a list",
            path=contract_rel,
        )
        raw_cache_inputs = []

    cache_inputs: list[str] = []
    for index, raw_path in enumerate(raw_cache_inputs):
        normalized = normalize_contract_source_path(
            raw_path,
            diagnostics,
            field=f"checked_in_source_truth.cache_inputs[{index}]",
        )
        if normalized is not None:
            cache_inputs.append(normalized)

    for rel_path in expected_cache_inputs:
        if rel_path not in cache_inputs:
            add_diagnostic(
                diagnostics,
                DIAG_STALE_INPUT,
                "checked-in module cache source input is not covered by cache input digest policy",
                path=rel_path,
            )

    for rel_path in cache_inputs:
        if rel_path not in expected_cache_inputs:
            add_diagnostic(
                diagnostics,
                DIAG_STALE_INPUT,
                "checked-in module cache digest policy references a non-replay input",
                path=rel_path,
            )

    expected_digests = source_truth.get("input_sha256", {})
    if not isinstance(expected_digests, dict):
        add_diagnostic(
            diagnostics,
            DIAG_MALFORMED_INPUT,
            "checked_in_source_truth.input_sha256 must be an object",
            path=contract_rel,
        )
        return {}

    observed_digests: dict[str, str] = {}
    for rel_path in cache_inputs:
        source_path = source_files.get(rel_path)
        if source_path is None:
            continue

        expected_digest = expected_digests.get(rel_path)
        if not isinstance(expected_digest, str) or len(expected_digest) != 64:
            add_diagnostic(
                diagnostics,
                DIAG_STALE_INPUT,
                "checked-in module cache source input is not pinned by sha256",
                path=rel_path,
            )
            continue

        observed_digest = file_sha256(source_path)
        observed_digests[rel_path] = observed_digest
        if observed_digest != expected_digest.lower():
            add_diagnostic(
                diagnostics,
                DIAG_STALE_INPUT,
                "checked-in module cache source input digest drifted",
                path=rel_path,
                detail=f"expected {expected_digest.lower()} observed {observed_digest}",
            )

    return observed_digests


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


def native_sources_enforce_cache_partition(
    native_anchors: list[str],
    diagnostics: list[dict[str, str]],
) -> dict[str, bool]:
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
        text = checked_text(path, diagnostics, rel_path=raw_path)
        if text is None:
            checks[raw_path] = False
            continue
        checks[raw_path] = all(fragment in text for fragment in required_by_path.get(raw_path, ()))
    return checks


def build_summary(contract_path: Path | str = CONTRACT_PATH) -> dict[str, Any]:
    contract_rel = display_path(contract_path)
    contract_diagnostics: list[dict[str, str]] = []
    contract = load_checked_json(Path(contract_path), contract_diagnostics, rel_path=contract_rel)
    diagnostics: list[dict[str, str]] = list(contract_diagnostics)
    failures: list[str] = []

    if contract is None:
        failures.extend(diagnostic_failures(diagnostics))
        return {
            "contract_id": SUMMARY_CONTRACT_ID,
            "status": "FAIL",
            "contract": contract_rel,
            "issues": [],
            "owner_policy": {},
            "blocker_metadata": {},
            "source_paths": [],
            "missing_source_paths": [],
            "source_sha256": {},
            "positive_replay_keys": [],
            "positive_replay_surfaces": [],
            "positive_cache_candidate_surface": {},
            "native_checks": {},
            "negative_diagnostic_codes": [],
            "diagnostics": diagnostics,
            "failures": failures,
        }

    expect(
        contract.get("contract_id") == CONTRACT_ID,
        "module cache consistency contract id drifted",
        failures,
    )
    owner_policy = contract.get("owner_policy", {})
    expect(isinstance(owner_policy, dict), "owner policy missing", failures)
    expect(owner_policy.get("evidence_log_allowed") is False, "evidence log must not be source authority", failures)
    source_truth = contract.get("checked_in_source_truth", {})
    expect(isinstance(source_truth, dict), "checked-in source truth policy missing", failures)
    if isinstance(source_truth, dict):
        expect(source_truth.get("issue") == "#8163", "checked-in source truth issue owner drifted", failures)
        expect(source_truth.get("from_nothing") is True, "from-nothing source truth policy missing", failures)
        expect(source_truth.get("forbid_tmp_inputs") is True, "tmp artifact source truth ban missing", failures)

    native_anchors = contract_path_list(
        contract,
        "native_anchors",
        diagnostics,
        contract_rel=contract_rel,
    )
    positive_replay_manifests = contract_path_list(
        contract,
        "positive_replay_manifests",
        diagnostics,
        contract_rel=contract_rel,
    )
    positive_replay_ir = contract_path_list(
        contract,
        "positive_replay_ir",
        diagnostics,
        contract_rel=contract_rel,
    )
    runbook = contract_runbook_path(contract, diagnostics)
    source_paths = [
        *([runbook] if runbook is not None else []),
        *native_anchors,
        *positive_replay_manifests,
        *positive_replay_ir,
    ]
    source_files = require_source_files(source_paths, diagnostics)
    missing_source_paths = [path for path in source_paths if path not in source_files]
    source_sha256 = validate_cache_input_digests(
        contract,
        source_files,
        diagnostics,
        contract_rel=contract_rel,
        expected_cache_inputs=[*positive_replay_manifests, *positive_replay_ir],
    )
    expect(not missing_source_paths, "module cache contract references missing source paths", failures)

    surfaces: list[dict[str, Any]] = []
    replay_keys: list[str] = []
    for manifest_path in positive_replay_manifests:
        manifest_source = source_files.get(manifest_path)
        if manifest_source is None:
            continue
        manifest = load_checked_json(manifest_source, diagnostics, rel_path=manifest_path)
        if manifest is None:
            continue
        try:
            surfaces.append(manifest_incremental_surface(manifest))
            replay_keys.append(manifest_replay_key(manifest))
        except RuntimeError as exc:
            add_diagnostic(
                diagnostics,
                DIAG_MALFORMED_INPUT,
                "checked-in module cache replay manifest is missing required metadata",
                path=manifest_path,
                detail=str(exc),
            )

    expect(len(surfaces) >= 2, "expected at least two replay manifests", failures)
    expect(len(set(replay_keys)) == 1, "incremental module cache replay keys drifted across runs", failures)
    expect(all(surface_is_consistent(surface) for surface in surfaces), "replay manifest incremental cache surfaces are inconsistent", failures)
    for ir_path, replay_key in zip(positive_replay_ir, replay_keys):
        ir_source = source_files.get(ir_path)
        if ir_source is None:
            continue
        ir_text = checked_text(ir_source, diagnostics, rel_path=ir_path)
        if ir_text is not None:
            expect(replay_key in ir_text, f"IR replay key missing from {ir_path}", failures)

    positive_surface = contract.get("positive_cache_candidate_surface", {})
    if not isinstance(positive_surface, dict):
        positive_surface = {}
    expected_positive_key = replay_key_for_surface(
        positive_surface,
        lane_contract=LANE_CONTRACT,
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
                lane_contract=LANE_CONTRACT,
            ),
            f"negative cache surface did not fail closed: {entry.get('case_id')}",
            failures,
        )

    native_checks = native_sources_enforce_cache_partition(native_anchors, diagnostics)
    expect(all(native_checks.values()), "native cache partition checks are not threaded through all anchors", failures)
    failures.extend(diagnostic_failures(diagnostics))

    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS" if not failures else "FAIL",
        "contract": contract_rel,
        "issues": contract.get("issues", []),
        "owner_policy": owner_policy,
        "blocker_metadata": contract.get("blocker_metadata", {}),
        "source_paths": source_paths,
        "missing_source_paths": missing_source_paths,
        "source_sha256": source_sha256,
        "positive_replay_keys": replay_keys,
        "positive_replay_surfaces": surfaces,
        "positive_cache_candidate_surface": positive_surface,
        "native_checks": native_checks,
        "negative_diagnostic_codes": sorted(negative_codes),
        "diagnostics": diagnostics,
        "failures": failures,
    }


def main() -> int:
    summary = build_summary()
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, summary)
    print(f"summary_path: {display_path(SUMMARY_PATH)}")
    if summary["status"] != "PASS":
        print("objc3c-incremental-module-cache-consistency: FAIL")
        for failure in summary["failures"]:
            print(f"- {failure}")
        return 1
    print("objc3c-incremental-module-cache-consistency: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
