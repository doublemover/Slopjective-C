#!/usr/bin/env python3
"""Validate the source-backed module/ABI/interop completion contract."""

from __future__ import annotations

import argparse
import hashlib
import json
import sys
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "module_abi_interop"
    / "completion_contract.json"
)
SUMMARY_PATH = (
    ROOT
    / "tmp"
    / "reports"
    / "module-abi-interop"
    / "completion-contract-summary.json"
)
SUMMARY_CONTRACT_ID = "objc3c.module_abi_interop.completion_contract.summary.v1"
REQUIRED_ISSUES = {"#8163", "#8165", "#8173"}
REQUIRED_NATIVE_SOURCE_ANCHORS = {
    "native/objc3c/src/pipeline/objc3_module_interop_contract_surface.h",
    "native/objc3c/src/pipeline/objc3_module_interop_contract_surface.cpp",
    "native/objc3c/src/pipeline/objc3_runtime_import_surface.h",
    "native/objc3c/src/artifacts/objc3_frontend_interop_semantic_artifacts.cpp",
    "native/objc3c/src/artifacts/objc3_frontend_runtime_import_artifact_payload_json.cpp",
}
FORBIDDEN_SOURCE_PARTS = {"tmp", "temp"}


def _repo_rel(path: Path) -> str:
    return path.resolve().relative_to(ROOT.resolve()).as_posix()


def _repo_path(raw_path: object) -> Path:
    return ROOT / Path(str(raw_path))


def _as_object(value: object) -> dict[str, Any]:
    return value if isinstance(value, dict) else {}


def _as_list(value: object) -> list[Any]:
    return value if isinstance(value, list) else []


def _as_str_set(value: object) -> set[str]:
    return {str(item) for item in _as_list(value) if isinstance(item, str)}


def _load_json(path: Path) -> dict[str, Any]:
    with path.open(encoding="utf-8") as handle:
        payload = json.load(handle)
    if not isinstance(payload, dict):
        raise ValueError(f"{_repo_rel(path)} must contain a JSON object")
    return payload


def _write_json(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(
        json.dumps(payload, indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )


def _sha256_text(value: str) -> str:
    return hashlib.sha256(value.encode("utf-8")).hexdigest()


def _sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _is_portable_repo_path(raw_path: object) -> bool:
    if not isinstance(raw_path, str) or not raw_path:
        return False
    path = Path(raw_path)
    if path.is_absolute() or "\\" in raw_path:
        return False
    return all(part not in {"", ".", ".."} for part in path.parts)


def _reject_bad_path(raw_path: object, label: str, failures: list[str]) -> None:
    if not _is_portable_repo_path(raw_path):
        failures.append(f"{label} must be a portable repo-relative path: {raw_path!r}")
        return
    parts = {part.lower() for part in Path(str(raw_path)).parts}
    if parts & FORBIDDEN_SOURCE_PARTS:
        failures.append(f"{label} must not point at generated temp output: {raw_path}")


def _validate_source_anchors(
    payload: dict[str, Any],
    failures: list[str],
) -> dict[str, bool]:
    anchors = [_as_object(anchor) for anchor in _as_list(payload.get("source_anchors"))]
    observed: dict[str, bool] = {}
    seen: set[str] = set()
    for anchor in anchors:
        raw_path = anchor.get("path")
        _reject_bad_path(raw_path, "source anchor path", failures)
        if not isinstance(raw_path, str):
            continue
        if raw_path in seen:
            failures.append(f"source anchor duplicated: {raw_path}")
        seen.add(raw_path)
        path = _repo_path(raw_path)
        if not path.is_file():
            failures.append(f"source anchor missing: {raw_path}")
            observed[raw_path] = False
            continue
        expected_digest = anchor.get("sha256")
        actual_digest = _sha256_file(path)
        digest_ok = expected_digest == actual_digest
        if not digest_ok:
            failures.append(f"source anchor digest drifted for {raw_path}")
        text = path.read_text(encoding="utf-8")
        fragments = [fragment for fragment in _as_list(anchor.get("fragments")) if isinstance(fragment, str)]
        if not fragments:
            failures.append(f"source anchor has no required fragments: {raw_path}")
        missing_fragments = [fragment for fragment in fragments if fragment not in text]
        if missing_fragments:
            failures.append(
                f"source anchor fragments missing for {raw_path}: "
                + ", ".join(missing_fragments)
            )
        observed[raw_path] = digest_ok and not missing_fragments
    missing_native = sorted(REQUIRED_NATIVE_SOURCE_ANCHORS - seen)
    if missing_native:
        failures.append(
            "source anchors are missing native implementation files: "
            + ", ".join(missing_native)
        )
    return observed


def _load_contract_path(section: dict[str, Any], field: str, failures: list[str]) -> dict[str, Any]:
    raw_path = section.get(field)
    _reject_bad_path(raw_path, field, failures)
    if not isinstance(raw_path, str):
        return {}
    path = _repo_path(raw_path)
    if not path.is_file():
        failures.append(f"{field} references missing file: {raw_path}")
        return {}
    try:
        return _load_json(path)
    except Exception as exc:
        failures.append(f"{field} failed to load: {raw_path}: {exc}")
        return {}


def _validate_module_semantics(payload: dict[str, Any], failures: list[str]) -> dict[str, Any]:
    section = _as_object(payload.get("module_semantics"))
    contract = _load_contract_path(section, "contract_path", failures)
    if not contract:
        return {}

    identity = _as_object(contract.get("module_identity"))
    if identity.get("module_name") != section.get("module_name"):
        failures.append("module identity name drifted")
    if identity.get("metadata_version") != section.get("metadata_version"):
        failures.append("module metadata version drifted")
    if identity.get("abi_identity") != section.get("abi_identity"):
        failures.append("module ABI identity drifted")

    imports = [_as_object(entry) for entry in _as_list(contract.get("imports"))]
    import_names = {str(entry.get("module_name")) for entry in imports}
    missing_imports = sorted(_as_str_set(section.get("required_imports")) - import_names)
    if missing_imports:
        failures.append("module import graph lost required imports: " + ", ".join(missing_imports))

    public_reexports = {
        str(entry.get("module_name"))
        for entry in imports
        if entry.get("reexport") is True and entry.get("visibility") == "public"
    }
    if public_reexports != _as_str_set(section.get("required_public_reexports")):
        failures.append("module public reexport boundary drifted")
    for entry in imports:
        if entry.get("reexport") is True and entry.get("visibility") != "public":
            failures.append(f"hidden/private import was reexported: {entry.get('module_name')}")

    private_imports = {
        str(entry.get("module_name"))
        for entry in imports
        if entry.get("visibility") == "private"
    }
    if private_imports != _as_str_set(section.get("required_private_imports")):
        failures.append("module private import boundary drifted")

    graph = _as_object(contract.get("dependency_graph"))
    reexports = {str(value) for value in _as_list(graph.get("reexported_modules"))}
    if reexports != public_reexports:
        failures.append("dependency graph reexports do not match public import reexports")

    exports = _as_object(contract.get("exports"))
    public_exports = {str(value) for value in _as_list(exports.get("public"))}
    private_exports = {str(value) for value in _as_list(exports.get("private"))}
    if public_exports & private_exports:
        failures.append("module exports contain public/private overlap")
    if exports.get("hidden_import_access_policy") != "reject":
        failures.append("hidden import access policy must reject")

    hidden_diagnostic = section.get("expected_hidden_diagnostic")
    for access_case in [_as_object(entry) for entry in _as_list(contract.get("visibility_access_cases"))]:
        allowed = access_case.get("allowed") is True
        if not allowed and access_case.get("diagnostic") != hidden_diagnostic:
            failures.append(f"hidden access case must fail closed for {access_case.get('symbol')}")
    hidden_symbols = {
        str(entry.get("symbol"))
        for entry in _as_list(contract.get("visibility_access_cases"))
        if isinstance(entry, dict) and entry.get("allowed") is False
    }
    if not {"FNPrivateBridgeShim", "FNBridgeScratchBuffer"} <= hidden_symbols:
        failures.append("hidden/private symbol rejection cases are incomplete")

    rebuild = _as_object(contract.get("incremental_rebuild"))
    if rebuild.get("deterministic") is not True:
        failures.append("incremental rebuild key must be deterministic")
    if _as_str_set(rebuild.get("identity_inputs")) < _as_str_set(section.get("required_rebuild_inputs")):
        failures.append("incremental rebuild identity inputs are incomplete")
    if _as_str_set(rebuild.get("invalidation_conditions")) < _as_str_set(section.get("required_invalidation_conditions")):
        failures.append("incremental rebuild invalidation conditions are incomplete")
    if rebuild.get("stale_metadata_diagnostic") != section.get("expected_stale_metadata_diagnostic"):
        failures.append("stale metadata diagnostic drifted")
    if rebuild.get("abi_mismatch_diagnostic") != section.get("expected_abi_mismatch_diagnostic"):
        failures.append("ABI mismatch diagnostic drifted")
    replay_key = rebuild.get("replay_key")
    if not isinstance(replay_key, str) or _sha256_text(replay_key) != section.get("rebuild_key_sha256"):
        failures.append("deterministic rebuild key digest drifted")
    for token in ("bridge_metadata_digest=", "mixed_image_loader_metadata_digest=", "imports=[", "foreign=["):
        if not isinstance(replay_key, str) or token not in replay_key:
            failures.append(f"deterministic rebuild key lost {token.rstrip('=')}")
    for case in [_as_object(entry) for entry in _as_list(rebuild.get("invalidation_cases"))]:
        if case.get("fail_closed") is not True or case.get("deterministic") is not True:
            failures.append(f"rebuild invalidation case is not deterministic fail-closed: {case.get('condition')}")

    return {
        "module_name": identity.get("module_name"),
        "import_count": len(imports),
        "public_reexport_count": len(public_reexports),
        "private_import_count": len(private_imports),
        "hidden_rejection_count": len(hidden_symbols),
    }


def _validate_interop_metadata(payload: dict[str, Any], failures: list[str]) -> dict[str, Any]:
    section = _as_object(payload.get("interop_metadata"))
    runtime_surface = _load_contract_path(section, "runtime_bridge_surface_path", failures)
    package_metadata = _load_contract_path(section, "package_loader_metadata_path", failures)
    stress_manifest = _load_contract_path(section, "stress_manifest_path", failures)
    required_languages = _as_str_set(section.get("required_languages"))

    language_surfaces: set[str] = set()
    runtime_sections_present = 0
    for section_name in _as_list(section.get("required_runtime_metadata_sections")):
        if not isinstance(section_name, str):
            continue
        runtime_section = _as_object(runtime_surface.get(section_name))
        if not runtime_section:
            failures.append(f"runtime import metadata section is missing: {section_name}")
            continue
        runtime_sections_present += 1
        if runtime_section.get("deterministic") is not True:
            failures.append(f"runtime import metadata section is not deterministic: {section_name}")
        if section_name == "objc_interop_header_module_and_bridge_generation":
            if runtime_section.get("runtime_generation_ready") is not True:
                failures.append(f"runtime bridge generation section is not generation-ready: {section_name}")
            if runtime_section.get("cross_module_packaging_ready") is not True:
                failures.append(f"runtime bridge generation section lost cross-module packaging readiness: {section_name}")
        else:
            if runtime_section.get("runtime_import_artifact_ready") is not True:
                failures.append(f"runtime import metadata section is not artifact-ready: {section_name}")
            if runtime_section.get("separate_compilation_preservation_ready") is not True:
                failures.append(f"runtime import metadata section lost separate-compilation preservation: {section_name}")

    bridge_generation = _as_object(runtime_surface.get("objc_interop_header_module_and_bridge_generation"))
    bridge_surfaces = [_as_object(entry) for entry in _as_list(bridge_generation.get("bridge_surfaces"))]
    for bridge_surface in bridge_surfaces:
        language = str(bridge_surface.get("language"))
        language_surfaces.add(language)
        if bridge_surface.get("fail_closed") is not True:
            failures.append(f"runtime bridge surface is not fail-closed: {language}")
        source_fixture = bridge_surface.get("source_fixture")
        _reject_bad_path(source_fixture, f"{language} source fixture", failures)
        if isinstance(source_fixture, str) and not _repo_path(source_fixture).is_file():
            failures.append(f"runtime bridge surface source fixture missing: {source_fixture}")
        abi = _as_object(bridge_surface.get("abi"))
        if not all(abi.get(field) for field in ("calling_convention", "ownership", "error_model", "async_model", "object_identity")):
            failures.append(f"runtime bridge surface lost ABI metadata: {language}")
        linkage = _as_object(bridge_surface.get("linkage"))
        if linkage.get("visibility") not in {"public", "private"}:
            failures.append(f"runtime bridge surface has invalid visibility: {language}")
    if language_surfaces != required_languages:
        failures.append("runtime bridge surfaces do not preserve every required foreign language")

    derived_count = len(bridge_surfaces)
    if bridge_generation.get("local_foreign_callable_count") != derived_count:
        failures.append("runtime bridge callable count is not derived from bridge surfaces")
    unsupported_topologies = {
        str(entry.get("topology_id")): _as_object(entry)
        for entry in _as_list(bridge_generation.get("unsupported_topologies"))
        if isinstance(entry, dict)
    }
    missing_topologies = sorted(_as_str_set(section.get("required_unsupported_topologies")) - set(unsupported_topologies))
    if missing_topologies:
        failures.append("runtime unsupported topology rejections are incomplete: " + ", ".join(missing_topologies))
    for topology_id, topology in unsupported_topologies.items():
        if topology.get("public_state") != "rejected" or topology.get("fallback_allowed") is not False:
            failures.append(f"runtime unsupported topology must fail closed: {topology_id}")

    module_contract = _as_object(payload.get("module_semantics"))
    module_payload = _load_contract_path(module_contract, "contract_path", failures)
    module_interop = _as_object(module_payload.get("interop"))
    module_foreign = [_as_object(entry) for entry in _as_list(module_interop.get("foreign_surfaces"))]
    supported_languages = {
        str(entry.get("language"))
        for entry in module_foreign
        if entry.get("support_state") == "supported" and entry.get("supported") is True
    }
    reserved_languages = {
        str(entry.get("language"))
        for entry in module_foreign
        if entry.get("support_state") == "reserved" and entry.get("supported") is False
    }
    if supported_languages != _as_str_set(section.get("required_supported_languages")):
        failures.append("module interop supported-language boundary drifted")
    if reserved_languages != _as_str_set(section.get("required_reserved_languages")):
        failures.append("module interop reserved-language boundary drifted")
    for entry in module_foreign:
        if entry.get("fail_closed") is not True:
            failures.append(f"module foreign surface is not fail-closed: {entry.get('language')}")
        if entry.get("support_state") == "reserved" and not entry.get("reservation_diagnostic"):
            failures.append(f"reserved foreign surface lost diagnostic: {entry.get('language')}")

    packages = [_as_object(entry) for entry in _as_list(package_metadata.get("packages"))]
    package_unsupported = {str(value) for value in _as_list(package_metadata.get("unsupported_surfaces"))}
    missing_package_unsupported = sorted(
        _as_str_set(section.get("required_package_unsupported_surfaces")) - package_unsupported
    )
    if missing_package_unsupported:
        failures.append("package loader unsupported surfaces are incomplete: " + ", ".join(missing_package_unsupported))
    package_bridge_count = 0
    mixed_image_count = 0
    for package in packages:
        if _as_object(package.get("tamper_rejection")).get("diagnostic_code") != section.get("tamper_diagnostic"):
            failures.append(f"package loader tamper diagnostic drifted for {package.get('package_id')}")
        package_bridge_count += len(_as_list(package.get("bridge_surfaces")))
        mixed_image_count += len(_as_list(package.get("mixed_images")))
        for negative in [_as_object(entry) for entry in _as_list(package.get("negative_fixtures"))]:
            if not negative.get("diagnostic_code") or not negative.get("rejection_kind"):
                failures.append(f"package negative fixture lost fail-closed metadata for {package.get('package_id')}")
    if package_bridge_count < 4 or mixed_image_count < 3:
        failures.append("package loader metadata lost mixed-image bridge coverage")

    stress_surfaces = _as_object(stress_manifest.get("surface_contracts"))
    for surface_name, expected_contract in _as_object(section.get("required_stress_surfaces")).items():
        if stress_surfaces.get(surface_name) != expected_contract:
            failures.append(f"stress manifest surface drifted: {surface_name}")

    return {
        "runtime_metadata_sections": runtime_sections_present,
        "runtime_bridge_surface_count": len(bridge_surfaces),
        "package_bridge_surface_count": package_bridge_count,
        "package_mixed_image_count": mixed_image_count,
    }


def _validate_abi_governance(payload: dict[str, Any], failures: list[str]) -> dict[str, Any]:
    section = _as_object(payload.get("abi_governance"))
    manifest = _load_contract_path(section, "manifest_path", failures)
    release = _load_contract_path(section, "release_governance_path", failures)
    diagnostic = section.get("diagnostic")
    if diagnostic != "O3ABI8173":
        failures.append("ABI governance diagnostic drifted")
    if "#8173" not in _as_str_set(manifest.get("issue_refs")):
        failures.append("ABI governance manifest is not tied to #8173")
    posture = _as_object(manifest.get("governance_posture"))
    if posture.get("failure_mode") != "fail-closed":
        failures.append("ABI governance failure mode must stay fail-closed")
    if posture.get("compatibility_shims_supported") is not False:
        failures.append("ABI governance must reject compatibility shims")
    if posture.get("fallback_routes_supported") is not False:
        failures.append("ABI governance must reject fallback routes")
    missing_claims = sorted(_as_str_set(section.get("required_unsupported_claims")) - _as_str_set(posture.get("unsupported_stability_claims")))
    if missing_claims:
        failures.append("ABI governance unsupported-claim boundaries drifted: " + ", ".join(missing_claims))

    required_blocked = _as_str_set(section.get("required_release_blocked_transitions"))
    manifest_blocked = _as_str_set(manifest.get("release_blocked_transitions"))
    if manifest_blocked < required_blocked:
        failures.append("ABI governance manifest lost blocked transitions")
    release_policy = _as_object(release.get("allowed_transition_policy"))
    release_blocked = _as_str_set(release_policy.get("release_blocked_transitions"))
    if release_blocked < required_blocked:
        failures.append("release ABI/API governance lost blocked transitions")
    if "#8173" not in _as_str_set(release.get("release_blocker_issue_refs")):
        failures.append("release ABI/API governance is not blocked by #8173")
    if release.get("gate_action") != "check-release-abi-api-drift":
        failures.append("release ABI/API governance gate action drifted")

    known_surfaces = {
        str(surface.get("surface_id"))
        for surface in _as_list(manifest.get("governed_surfaces"))
        if isinstance(surface, dict)
    }
    known_surfaces.update(
        str(extractor.get("extractor_id"))
        for extractor in _as_list(manifest.get("surface_extractors"))
        if isinstance(extractor, dict)
    )
    cases = [_as_object(entry) for entry in _as_list(_as_object(manifest.get("compatibility_evidence")).get("cases"))]
    observed_transitions = {str(case.get("transition")) for case in cases}
    if observed_transitions < _as_str_set(section.get("required_compatibility_transitions")):
        failures.append("ABI governance compatibility cases lost required transitions")
    for case in cases:
        case_id = case.get("case_id")
        if case.get("surface_id") not in known_surfaces:
            failures.append(f"ABI governance compatibility case references unknown surface: {case_id}")
        if case.get("transition") not in manifest_blocked:
            failures.append(f"ABI governance compatibility case is not release-blocked: {case_id}")
        if case.get("expected_decision") != "block-release" or case.get("release_blocker_issue_ref") != "#8173":
            failures.append(f"ABI governance compatibility case must block release under #8173: {case_id}")

    policy = _as_object(manifest.get("compatibility_policy"))
    if policy.get("shim_policy") != "no-compatibility-shims":
        failures.append("ABI compatibility policy must reject shims")
    if policy.get("fallback_route_policy") != "no-fallback-routes":
        failures.append("ABI compatibility policy must reject fallback routes")

    return {
        "governed_surface_count": len(_as_list(manifest.get("governed_surfaces"))),
        "surface_extractor_count": len(_as_list(manifest.get("surface_extractors"))),
        "compatibility_case_count": len(cases),
        "release_blocked_transition_count": len(manifest_blocked),
    }


def validate_contract_payload(payload: dict[str, Any]) -> tuple[list[str], dict[str, Any]]:
    failures: list[str] = []
    if payload.get("contract_id") != "objc3c.module_abi_interop.completion_contract.v1":
        failures.append("contract id drifted")
    if REQUIRED_ISSUES - _as_str_set(payload.get("issue_refs")):
        failures.append("contract must cover #8163, #8165, and #8173")
    if payload.get("source_authority") != "checked-source-anchors":
        failures.append("source authority must be checked source anchors")
    if payload.get("evidence_log_allowed") is not False:
        failures.append("evidence logs cannot be source authority")
    if payload.get("public_command") != "npm run objc3c -- validate-post-cutover-issue-evidence":
        failures.append("public command drifted")

    source_anchor_status = _validate_source_anchors(payload, failures)
    module_summary = _validate_module_semantics(payload, failures)
    interop_summary = _validate_interop_metadata(payload, failures)
    abi_summary = _validate_abi_governance(payload, failures)
    details = {
        "source_anchor_count": len(source_anchor_status),
        "source_anchors": source_anchor_status,
        "module": module_summary,
        "interop": interop_summary,
        "abi_governance": abi_summary,
    }
    return failures, details


def build_summary(path: Path = CONTRACT_PATH) -> dict[str, Any]:
    payload = _load_json(path)
    failures, details = validate_contract_payload(payload)
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS" if not failures else "FAIL",
        "contract": _repo_rel(path) if path.resolve().is_relative_to(ROOT.resolve()) else path.as_posix(),
        "issues": payload.get("issue_refs", []),
        "diagnostic_codes": ["O3MOD8163", "O3MOD8165", "O3MOD8166", "O3PKG8054", "O3ABI8173"],
        "failure_count": len(failures),
        "failures": failures,
        **details,
    }


def _parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("contract", nargs="?", type=Path, default=CONTRACT_PATH)
    parser.add_argument("--summary", type=Path, default=SUMMARY_PATH)
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = _parser().parse_args(argv)
    summary = build_summary(args.contract)
    _write_json(args.summary, summary)
    display_summary = (
        _repo_rel(args.summary)
        if args.summary.resolve().is_relative_to(ROOT.resolve())
        else args.summary.as_posix()
    )
    print(f"summary_path: {display_summary}")
    if summary["status"] != "PASS":
        print(f"objc3c-module-abi-interop-completion-contract: FAIL ({summary['failure_count']} failures)")
        for failure in summary["failures"]:
            print(f"- {failure}")
        return 1
    print("objc3c-module-abi-interop-completion-contract: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
