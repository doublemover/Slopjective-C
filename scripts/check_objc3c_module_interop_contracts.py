#!/usr/bin/env python3
"""Validate focused module identity, visibility, rebuild, and interop contracts."""

from __future__ import annotations

import hashlib
import re
import sys
from pathlib import Path
from typing import Any

from objc3c_shared.json_io import (
    JsonSchemaValidationError,
    load_json_object,
    validate_json_schema,
    write_json_file,
)
from objc3c_tooling.paths import repo_rel


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "module_interop_contracts"
    / "foundation_next_visibility_bridge_contract.json"
)
SCHEMA_PATH = ROOT / "schemas" / "objc3c-module-interop-contract-v1.schema.json"
SUMMARY_PATH = (
    ROOT
    / "tmp"
    / "reports"
    / "module-interop-contracts"
    / "foundation-next-visibility-bridge-summary.json"
)

PUBLIC_COMMAND = "npm run objc3c -- validate-module-interop-contracts"
REQUIRED_ISSUES = {8163, 8165}
REQUIRED_LANGUAGES = {"c", "objc2", "swift", "cpp"}
REQUIRED_REBUILD_INPUTS = {
    "module_identity",
    "import_graph",
    "visibility",
    "foreign_surfaces",
    "package_lock",
}
REQUIRED_INVALIDATION_CONDITIONS = {
    "source-digest-drift",
    "imported-module-abi-identity-drift",
    "package-lock-module-identity-drift",
    "visibility-surface-drift",
    "bridge-metadata-digest-drift",
}
REQUIRED_INVALIDATION_CASES = {
    "source-digest-drift": {
        "diagnostic": "O3MOD8163",
        "rebuild_affects": {"semantic", "abi", "link", "package-lock"},
    },
    "imported-module-abi-identity-drift": {
        "diagnostic": "O3MOD8166",
        "rebuild_affects": {"semantic", "abi", "link", "package-lock"},
    },
    "package-lock-module-identity-drift": {
        "diagnostic": "O3MOD8163",
        "rebuild_affects": {"package-lock"},
    },
    "visibility-surface-drift": {
        "diagnostic": "O3MOD8165",
        "rebuild_affects": {"semantic"},
    },
    "bridge-metadata-digest-drift": {
        "diagnostic": "O3MOD8163",
        "rebuild_affects": {"semantic", "abi", "link"},
    },
}
REQUIRED_UNSUPPORTED_SURFACES = {
    "objc2-retired-source-syntax",
    "swift-unstable-abi-shape",
    "cpp-template-instantiation-import",
}
REQUIRED_MODULE_DIAGNOSTICS = {
    "missing-module": "O3MOD8161",
    "import-cycle": "O3MOD8162",
    "stale-metadata": "O3MOD8163",
    "duplicate-export": "O3MOD8164",
    "hidden-declaration": "O3MOD8165",
    "abi-mismatch": "O3MOD8166",
}
REQUIRED_CHECKED_SOURCE_PROOFS = {
    "missing-module": "O3MOD8161",
    "import-cycle": "O3MOD8162",
    "duplicate-export": "O3MOD8164",
    "hidden-declaration": "O3MOD8165",
    "private-reexport": "O3MOD8165",
    "stale-metadata": "O3MOD8163",
    "abi-mismatch": "O3MOD8166",
    "bridge-digest-drift": "O3MOD8163",
}
FORBIDDEN_SOURCE_PREFIXES = (
    "docs/support/",
    "docs/runbooks/objc3c_release",
    "schemas/objc3c-release",
)
NATIVE_ANCHOR_FRAGMENTS = {
    "native/objc3c/src/pipeline/objc3_runtime_import_surface.h": (
        "interop_header_module_bridge_cross_module_packaging_ready",
        "interop_bridge_module_artifact_relative_path",
        "interop_local_swift_name_annotation_count",
        "interop_local_cpp_name_annotation_count",
    ),
    "native/objc3c/src/pipeline/objc3_module_interop_contract_surface.h": (
        "Objc3ModuleImportVisibility",
        "Objc3ModuleImportEdgeContract",
        "Objc3ModuleVisibilityAccessContract",
        "Objc3ModuleInteropLaneState",
        "Objc3ModuleInteropForeignLaneContract",
        "Objc3ModuleInteropContractSurface",
        "import_edges_source_order",
        "public_exports_source_order",
        "package_imported_module_identities_source_order",
        "visibility_access_source_order",
        "module_identity_rebuild_key",
        "module_source_digest",
        "bridge_modulemap_relative_path",
        "bridge_metadata_digest",
        "mixed_image_loader_metadata_digest",
        "stale_metadata_diagnostic_code",
        "abi_mismatch_diagnostic_code",
        "abi_alignment",
        "c_foreign_type_contract_count",
        "objc2_bridge_metadata_only_retired_syntax_rejected",
        "swift_callable_metadata_count",
        "cpp_callable_metadata_count",
        "supported_foreign_lane_count",
        "reserved_foreign_lane_count",
        "reserved_lanes_have_stable_diagnostics",
        "bridge_metadata_digest_participates_in_rebuild_key",
    ),
    "native/objc3c/src/pipeline/objc3_module_interop_contract_surface.cpp": (
        "BuildObjc3ModuleInteropRebuildKey",
        "ValidateObjc3ModuleInteropContractSurface",
        "module interop rebuild key must be deterministic",
        "reexported import edge must be public",
        "package imported module identities must match import graph",
        "module interop rebuild digests must be stable sha256 hex values",
        "module visibility access allowance drifted",
        "hidden declaration access must fail closed with a stable",
        "reserved foreign lane must publish a stable diagnostic",
    ),
}
SHA256_RE = re.compile(r"^[0-9a-fA-F]{64}$")


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def _as_list(value: object) -> list[Any]:
    return value if isinstance(value, list) else []


def _as_object(value: object) -> dict[str, Any]:
    return value if isinstance(value, dict) else {}


def _is_power_of_two(value: int) -> bool:
    return value > 0 and value & (value - 1) == 0


def _is_portable_relative_path(value: object) -> bool:
    if not isinstance(value, str) or not value:
        return False
    path = Path(value)
    if path.is_absolute() or "\\" in value:
        return False
    return all(part not in {"", ".", ".."} for part in path.parts)


def _is_sha256_digest(value: object) -> bool:
    return isinstance(value, str) and SHA256_RE.fullmatch(value) is not None


def _file_sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _import_identity(entry: dict[str, Any]) -> str:
    return (
        f"{entry.get('module_name')}@{entry.get('metadata_version')}:"
        f"{entry.get('abi_identity')}"
    )


def replay_key_for_contract(payload: dict[str, Any]) -> str:
    module_identity = _as_object(payload.get("module_identity"))
    imports = sorted(
        (
            _as_object(entry)
            for entry in _as_list(payload.get("imports"))
            if isinstance(entry, dict)
        ),
        key=lambda entry: str(entry.get("module_name", "")),
    )
    import_part = ",".join(
        f"{entry.get('module_name')}@{entry.get('metadata_version')}:"
        f"{entry.get('abi_identity')}:{entry.get('visibility')}:"
        f"{'reexport' if entry.get('reexport') is True else 'import'}"
        for entry in imports
    )
    public_exports = sorted(str(symbol) for symbol in _as_list(_as_object(payload.get("exports")).get("public")))
    foreign_surfaces = sorted(
        (
            _as_object(entry)
            for entry in _as_list(_as_object(payload.get("interop")).get("foreign_surfaces"))
            if isinstance(entry, dict)
        ),
        key=lambda entry: str(entry.get("language", "")),
    )
    foreign_part = ",".join(
        f"{entry.get('language')}:{entry.get('surface_id')}:"
        f"{entry.get('symbol_owner')}:{entry.get('abi_alignment')}:"
        f"{entry.get('support_state')}"
        for entry in foreign_surfaces
    )
    return (
        f"module={module_identity.get('module_name')};"
        f"metadata={module_identity.get('metadata_version')};"
        f"abi={module_identity.get('abi_identity')};"
        f"source_digest={module_identity.get('source_digest')};"
        f"package={module_identity.get('package_lock_identity')};"
        f"imports=[{import_part}];"
        f"exports=[{','.join(public_exports)}];"
        f"bridge_metadata_digest={_as_object(payload.get('interop')).get('bridge_metadata_digest')};"
        f"mixed_image_loader_metadata_digest={_as_object(payload.get('package_metadata')).get('mixed_image_loader_metadata_digest')};"
        f"foreign=[{foreign_part}]"
    )


def _validate_schema(payload: dict[str, Any], failures: list[str]) -> None:
    try:
        validate_json_schema(payload, load_json_object(SCHEMA_PATH), label=repo_rel(CONTRACT_PATH))
    except JsonSchemaValidationError as exc:
        failures.append(str(exc))


def _validate_source_paths(payload: dict[str, Any], failures: list[str]) -> dict[str, bool]:
    checks: dict[str, bool] = {}
    source_paths = [
        *[str(path) for path in _as_list(payload.get("native_anchors"))],
        *[str(path) for path in _as_list(payload.get("fixture_anchors"))],
    ]
    for raw_path in source_paths:
        portable = _is_portable_relative_path(raw_path)
        allowed = not raw_path.replace("\\", "/").startswith(FORBIDDEN_SOURCE_PREFIXES)
        exists = (ROOT / raw_path).is_file()
        checks[raw_path] = portable and allowed and exists
        expect(portable, f"source path is not portable-relative: {raw_path}", failures)
        expect(allowed, f"contract slice touched forbidden support/release surface: {raw_path}", failures)
        expect(exists, f"contract references missing source path: {raw_path}", failures)

    for raw_path, fragments in NATIVE_ANCHOR_FRAGMENTS.items():
        path = ROOT / raw_path
        if not path.is_file():
            checks[raw_path] = False
            continue
        text = path.read_text(encoding="utf-8")
        fragments_present = all(fragment in text for fragment in fragments)
        checks[raw_path] = checks.get(raw_path, True) and fragments_present
        expect(fragments_present, f"native anchor missing module/interop fragments: {raw_path}", failures)
    return checks


def _validate_checked_source_proofs(payload: dict[str, Any], failures: list[str]) -> None:
    proofs = [
        _as_object(entry)
        for entry in _as_list(payload.get("checked_source_proofs"))
        if isinstance(entry, dict)
    ]
    proofs_by_case: dict[str, dict[str, Any]] = {}
    for proof in proofs:
        case = str(proof.get("case", ""))
        expect(case not in proofs_by_case, f"checked source proof duplicated: {case}", failures)
        proofs_by_case[case] = proof

        expected_diagnostic = REQUIRED_CHECKED_SOURCE_PROOFS.get(case)
        expect(
            expected_diagnostic is not None,
            f"checked source proof case is unsupported: {case}",
            failures,
        )
        if expected_diagnostic is not None:
            expect(
                proof.get("diagnostic") == expected_diagnostic,
                f"checked source proof diagnostic drifted for {case}",
                failures,
            )
        expect(proof.get("fail_closed") is True, f"checked source proof is not fail-closed: {case}", failures)

        raw_path = proof.get("source_anchor")
        portable = _is_portable_relative_path(raw_path)
        allowed = isinstance(raw_path, str) and not raw_path.replace("\\", "/").startswith(FORBIDDEN_SOURCE_PREFIXES)
        expect(portable, f"checked source proof anchor is not portable-relative: {case}: {raw_path}", failures)
        expect(allowed, f"checked source proof touched forbidden support/release surface: {case}: {raw_path}", failures)
        if not isinstance(raw_path, str):
            continue
        path = ROOT / raw_path
        exists = path.is_file()
        expect(exists, f"checked source proof anchor is missing: {case}: {raw_path}", failures)
        if not exists:
            continue
        expected_sha = proof.get("source_sha256")
        if _is_sha256_digest(expected_sha):
            expect(
                str(expected_sha).lower() == _file_sha256(path),
                f"checked source proof digest drifted for {case}: {raw_path}",
                failures,
            )
        fragments = [str(fragment) for fragment in _as_list(proof.get("fragments"))]
        expect(fragments != [], f"checked source proof has no fragments: {case}", failures)
        if fragments:
            text = path.read_text(encoding="utf-8")
            missing_fragments = [fragment for fragment in fragments if fragment not in text]
            expect(
                missing_fragments == [],
                f"checked source proof fragments missing for {case}: {raw_path}: {', '.join(missing_fragments)}",
                failures,
            )

    missing_cases = sorted(set(REQUIRED_CHECKED_SOURCE_PROOFS) - set(proofs_by_case))
    expect(
        missing_cases == [],
        "checked source proofs are incomplete: " + ", ".join(missing_cases),
        failures,
    )

    graph_diagnostics = {
        str(entry.get("case")): str(entry.get("diagnostic"))
        for entry in _as_list(_as_object(payload.get("dependency_graph")).get("diagnostics"))
        if isinstance(entry, dict)
    }
    for case in REQUIRED_MODULE_DIAGNOSTICS:
        proof = proofs_by_case.get(case)
        if proof is None:
            continue
        expect(
            graph_diagnostics.get(case) == proof.get("diagnostic"),
            f"checked source proof diagnostic is not wired to dependency graph: {case}",
            failures,
        )

    rebuild = _as_object(payload.get("incremental_rebuild"))
    stale_proof = proofs_by_case.get("stale-metadata")
    if stale_proof is not None:
        expect(
            rebuild.get("stale_metadata_diagnostic") == stale_proof.get("diagnostic"),
            "checked source stale metadata proof is not wired to rebuild diagnostics",
            failures,
        )
    abi_proof = proofs_by_case.get("abi-mismatch")
    if abi_proof is not None:
        expect(
            rebuild.get("abi_mismatch_diagnostic") == abi_proof.get("diagnostic"),
            "checked source ABI mismatch proof is not wired to rebuild diagnostics",
            failures,
        )
    bridge_proof = proofs_by_case.get("bridge-digest-drift")
    if bridge_proof is not None:
        invalidation_cases = {
            str(entry.get("condition")): _as_object(entry)
            for entry in _as_list(rebuild.get("invalidation_cases"))
            if isinstance(entry, dict)
        }
        bridge_case = invalidation_cases.get("bridge-metadata-digest-drift")
        expect(
            bridge_case is not None and bridge_case.get("diagnostic") == bridge_proof.get("diagnostic"),
            "checked source bridge digest proof is not wired to bridge metadata invalidation",
            failures,
        )
        if bridge_case is not None:
            expect(
                bridge_case.get("fail_closed") is True and bridge_case.get("deterministic") is True,
                "checked source bridge digest proof must be deterministic fail-closed",
                failures,
            )
        replay_key = rebuild.get("replay_key")
        bridge_digest = _as_object(payload.get("interop")).get("bridge_metadata_digest")
        expect(
            isinstance(replay_key, str)
            and "bridge_metadata_digest=" in replay_key
            and isinstance(bridge_digest, str)
            and bridge_digest in replay_key,
            "checked source bridge digest proof lost replay-key binding",
            failures,
        )

    duplicate_proof = proofs_by_case.get("duplicate-export")
    if duplicate_proof is not None:
        expect(
            _as_object(payload.get("exports")).get("duplicate_policy") == "fail-closed",
            "checked source duplicate export proof lost fail-closed policy",
            failures,
        )
    hidden_proof = proofs_by_case.get("hidden-declaration")
    if hidden_proof is not None:
        hidden_access_cases = [
            _as_object(entry)
            for entry in _as_list(payload.get("visibility_access_cases"))
            if isinstance(entry, dict)
            and entry.get("allowed") is False
            and entry.get("diagnostic") == hidden_proof.get("diagnostic")
        ]
        expect(
            hidden_access_cases != [],
            "checked source hidden declaration proof has no fail-closed access case",
            failures,
        )
    private_reexport_proof = proofs_by_case.get("private-reexport")
    if private_reexport_proof is not None:
        imports = [
            _as_object(entry)
            for entry in _as_list(payload.get("imports"))
            if isinstance(entry, dict)
        ]
        private_imports = {
            str(entry.get("module_name"))
            for entry in imports
            if entry.get("visibility") != "public"
        }
        private_reexports = sorted(
            str(entry.get("module_name"))
            for entry in imports
            if entry.get("visibility") != "public" and entry.get("reexport") is True
        )
        expect(
            private_reexports == [],
            "checked source private reexport proof found private reexports: " + ", ".join(private_reexports),
            failures,
        )
        graph_reexports = {
            str(value)
            for value in _as_list(_as_object(payload.get("dependency_graph")).get("reexported_modules"))
        }
        graph_private_reexports = sorted(private_imports & graph_reexports)
        expect(
            graph_private_reexports == [],
            "checked source private reexport proof found graph private reexports: "
            + ", ".join(graph_private_reexports),
            failures,
        )
        private_hidden_cases = [
            _as_object(entry)
            for entry in _as_list(payload.get("visibility_access_cases"))
            if isinstance(entry, dict)
            and entry.get("provided_by") in private_imports
            and entry.get("allowed") is False
            and entry.get("diagnostic") == private_reexport_proof.get("diagnostic")
        ]
        expect(
            private_hidden_cases != [],
            "checked source private reexport proof has no hidden/private import rejection case",
            failures,
        )


def _validate_module_identity(payload: dict[str, Any], failures: list[str]) -> None:
    module_identity = _as_object(payload.get("module_identity"))
    package_metadata = _as_object(payload.get("package_metadata"))
    source_identity = module_identity.get("source_identity_key")
    package_identity = module_identity.get("package_lock_identity")

    expect(
        _is_sha256_digest(module_identity.get("source_digest")),
        "module source digest must be a stable sha256 hex value",
        failures,
    )
    expect(source_identity == package_identity, "source and package module identities drifted", failures)
    expect(
        package_metadata.get("module_identity") == package_identity,
        "package metadata module identity does not match module identity",
        failures,
    )
    expect(
        package_metadata.get("package_lock_module_identity") == package_identity,
        "package lock module identity does not match module identity",
        failures,
    )


def _validate_imports_and_exports(payload: dict[str, Any], failures: list[str]) -> None:
    module_name = str(_as_object(payload.get("module_identity")).get("module_name", ""))
    imports = [
        _as_object(entry)
        for entry in _as_list(payload.get("imports"))
        if isinstance(entry, dict)
    ]
    import_names = [str(entry.get("module_name")) for entry in imports]
    expect(import_names == sorted(import_names), "imports must be sorted by module name", failures)
    expect(len(import_names) == len(set(import_names)), "imported module names must be unique", failures)
    expect(module_name not in import_names, "module cannot import itself", failures)
    for entry in imports:
        if entry.get("reexport") is True:
            expect(entry.get("visibility") == "public", "reexported imports must be public", failures)
        expect(entry.get("rebuild_affects") != [], f"import has no rebuild effect: {entry.get('module_name')}", failures)

    exports = _as_object(payload.get("exports"))
    public_exports = {str(symbol) for symbol in _as_list(exports.get("public"))}
    private_exports = {str(symbol) for symbol in _as_list(exports.get("private"))}
    expect(public_exports.isdisjoint(private_exports), "public and private exports overlap", failures)
    expect(exports.get("duplicate_policy") == "fail-closed", "duplicate exports must fail closed", failures)
    expect(exports.get("hidden_import_access_policy") == "reject", "hidden import access must reject", failures)

    all_imported_exports: list[str] = []
    for entry in imports:
        all_imported_exports.extend(str(symbol) for symbol in _as_list(entry.get("exported_symbols")))
    all_exports = list(public_exports | private_exports) + all_imported_exports
    expect(len(all_exports) == len(set(all_exports)), "exported symbols must be unique across module and imports", failures)

    package_metadata = _as_object(payload.get("package_metadata"))
    package_imports = set(str(identity) for identity in _as_list(package_metadata.get("imported_module_identities")))
    expected_imports = {_import_identity(entry) for entry in imports}
    expect(package_imports == expected_imports, "package imported module identities do not match import graph", failures)


def _validate_dependency_graph(payload: dict[str, Any], failures: list[str]) -> None:
    module_name = str(_as_object(payload.get("module_identity")).get("module_name", ""))
    imports = {
        str(entry.get("module_name")): _as_object(entry)
        for entry in _as_list(payload.get("imports"))
        if isinstance(entry, dict)
    }
    graph = _as_object(payload.get("dependency_graph"))
    edges = [
        _as_object(entry)
        for entry in _as_list(graph.get("edges"))
        if isinstance(entry, dict)
    ]
    expect(len(edges) == len(imports), "dependency graph edge count must match imports", failures)
    for edge in edges:
        target = str(edge.get("to"))
        import_entry = imports.get(target)
        expect(edge.get("from") == module_name, f"dependency edge source drifted for {target}", failures)
        expect(import_entry is not None, f"dependency graph references unknown import {target}", failures)
        if import_entry is None:
            continue
        expect(edge.get("visibility") == import_entry.get("visibility"), f"dependency edge visibility drifted for {target}", failures)
        expect(edge.get("reexport") == import_entry.get("reexport"), f"dependency edge reexport flag drifted for {target}", failures)

    expected_reexports = {
        str(entry.get("module_name"))
        for entry in imports.values()
        if entry.get("reexport") is True
    }
    actual_reexports = {str(value) for value in _as_list(graph.get("reexported_modules"))}
    expect(actual_reexports == expected_reexports, "dependency graph reexports do not match public reexport imports", failures)

    diagnostics = {
        str(entry.get("case")): str(entry.get("diagnostic"))
        for entry in _as_list(graph.get("diagnostics"))
        if isinstance(entry, dict)
    }
    expect(
        diagnostics == REQUIRED_MODULE_DIAGNOSTICS,
        "dependency graph diagnostics must cover missing modules, cycles, stale metadata, duplicate exports, hidden declarations, and ABI mismatch",
        failures,
    )


def _validate_visibility_access(payload: dict[str, Any], failures: list[str]) -> None:
    module_name = str(_as_object(payload.get("module_identity")).get("module_name", ""))
    imports = [
        _as_object(entry)
        for entry in _as_list(payload.get("imports"))
        if isinstance(entry, dict)
    ]
    public_exports = {str(symbol) for symbol in _as_list(_as_object(payload.get("exports")).get("public"))}
    private_exports = {str(symbol) for symbol in _as_list(_as_object(payload.get("exports")).get("private"))}
    import_by_symbol = {
        str(symbol): entry
        for entry in imports
        for symbol in _as_list(entry.get("exported_symbols"))
    }

    access_cases = [
        _as_object(entry)
        for entry in _as_list(payload.get("visibility_access_cases"))
        if isinstance(entry, dict)
    ]
    expect(access_cases != [], "visibility access cases are missing", failures)
    for case in access_cases:
        symbol = str(case.get("symbol"))
        provider = str(case.get("provided_by"))
        allowed = case.get("allowed") is True
        diagnostic = case.get("diagnostic")
        if provider == module_name:
            public = symbol in public_exports
            hidden = symbol in private_exports
        else:
            import_entry = import_by_symbol.get(symbol)
            public = bool(import_entry and import_entry.get("visibility") == "public")
            hidden = bool(import_entry and import_entry.get("visibility") != "public")
        expect(allowed == public, f"visibility access allowance drifted for {symbol}", failures)
        if hidden or not allowed:
            expect(
                isinstance(diagnostic, str) and diagnostic == REQUIRED_MODULE_DIAGNOSTICS["hidden-declaration"],
                f"hidden access case must fail closed with O3MOD8165 for {symbol}",
                failures,
            )


def _validate_rebuild(payload: dict[str, Any], failures: list[str]) -> None:
    rebuild = _as_object(payload.get("incremental_rebuild"))
    invalidation_conditions = {
        str(value) for value in _as_list(rebuild.get("invalidation_conditions"))
    }
    expect(rebuild.get("deterministic") is True, "incremental rebuild identity must be deterministic", failures)
    expect(
        rebuild.get("stale_metadata_diagnostic") == REQUIRED_MODULE_DIAGNOSTICS["stale-metadata"],
        "incremental rebuild stale metadata diagnostic drifted",
        failures,
    )
    expect(
        rebuild.get("abi_mismatch_diagnostic") == REQUIRED_MODULE_DIAGNOSTICS["abi-mismatch"],
        "incremental rebuild ABI mismatch diagnostic drifted",
        failures,
    )
    expect(
        REQUIRED_REBUILD_INPUTS <= {str(value) for value in _as_list(rebuild.get("identity_inputs"))},
        "incremental rebuild identity inputs are incomplete",
        failures,
    )
    expect(
        REQUIRED_INVALIDATION_CONDITIONS <= invalidation_conditions,
        "incremental rebuild invalidation conditions are incomplete",
        failures,
    )
    invalidation_cases = [
        _as_object(entry)
        for entry in _as_list(rebuild.get("invalidation_cases"))
        if isinstance(entry, dict)
    ]
    cases_by_condition: dict[str, dict[str, Any]] = {}
    for entry in invalidation_cases:
        condition = str(entry.get("condition"))
        expect(
            condition not in cases_by_condition,
            f"incremental rebuild invalidation case is duplicated: {condition}",
            failures,
        )
        cases_by_condition[condition] = entry

    expect(
        set(REQUIRED_INVALIDATION_CASES) <= set(cases_by_condition),
        "incremental rebuild invalidation cases are incomplete",
        failures,
    )
    expect(
        set(cases_by_condition) <= invalidation_conditions,
        "incremental rebuild invalidation cases reference unknown conditions",
        failures,
    )
    for condition, expected in REQUIRED_INVALIDATION_CASES.items():
        entry = cases_by_condition.get(condition)
        if entry is None:
            continue
        rebuild_affects = {str(value) for value in _as_list(entry.get("rebuild_affects"))}
        expect(
            entry.get("diagnostic") == expected["diagnostic"],
            f"incremental rebuild invalidation diagnostic drifted for {condition}",
            failures,
        )
        expect(
            rebuild_affects == expected["rebuild_affects"],
            f"incremental rebuild invalidation scope drifted for {condition}",
            failures,
        )
        expect(
            entry.get("fail_closed") is True and entry.get("deterministic") is True,
            f"incremental rebuild invalidation must fail closed deterministically for {condition}",
            failures,
        )
        expect(
            isinstance(entry.get("mutated_surface"), str) and entry["mutated_surface"],
            f"incremental rebuild invalidation must name mutated surface for {condition}",
            failures,
        )
    expect(
        rebuild.get("replay_key") == replay_key_for_contract(payload),
        "deterministic rebuild replay key drifted",
        failures,
    )


def _validate_interop(payload: dict[str, Any], failures: list[str]) -> None:
    module_name = str(_as_object(payload.get("module_identity")).get("module_name", ""))
    interop = _as_object(payload.get("interop"))
    bridge_header = interop.get("bridge_header")
    modulemap = interop.get("modulemap")
    bridge_metadata = interop.get("bridge_metadata")
    bridge_metadata_digest = interop.get("bridge_metadata_digest")
    for label, value in (
        ("bridge header", bridge_header),
        ("modulemap", modulemap),
        ("bridge metadata", bridge_metadata),
    ):
        expect(_is_portable_relative_path(value), f"{label} path is not portable-relative", failures)
    expect(
        _is_sha256_digest(bridge_metadata_digest),
        "bridge metadata digest must be a stable sha256 hex value",
        failures,
    )

    foreign_surfaces = [
        _as_object(entry)
        for entry in _as_list(interop.get("foreign_surfaces"))
        if isinstance(entry, dict)
    ]
    languages = {str(entry.get("language")) for entry in foreign_surfaces}
    expect(languages == REQUIRED_LANGUAGES, "foreign surfaces must cover C, ObjC2, Swift, and C++ exactly", failures)

    surface_alignments: dict[str, int] = {}
    supported_languages: set[str] = set()
    reserved_languages: set[str] = set()
    for entry in foreign_surfaces:
        language = str(entry.get("language"))
        alignment = int(entry.get("abi_alignment", 0))
        surface_alignments[language] = alignment
        support_state = str(entry.get("support_state"))
        supported = entry.get("supported") is True
        expect(
            support_state in {"supported", "reserved"},
            f"{language} surface support_state must be supported or reserved",
            failures,
        )
        expect(
            supported == (support_state == "supported"),
            f"{language} supported flag must match support_state",
            failures,
        )
        if support_state == "supported":
            supported_languages.add(language)
        if support_state == "reserved":
            reserved_languages.add(language)
            expect(
                isinstance(entry.get("reservation_reason"), str) and entry.get("reservation_reason"),
                f"{language} reserved surface must explain why executable proof is absent",
                failures,
            )
            diagnostic = entry.get("reservation_diagnostic")
            expect(
                isinstance(diagnostic, str) and diagnostic.startswith("O3"),
                f"{language} reserved surface must publish a stable diagnostic",
                failures,
            )
        expect(entry.get("fail_closed") is True, f"{language} surface must fail closed", failures)
        expect(entry.get("symbol_owner") == module_name, f"{language} symbol owner drifted", failures)
        expect(_is_power_of_two(alignment), f"{language} ABI alignment must be a power of two", failures)
        expect(entry.get("bridge_header") == bridge_header, f"{language} bridge header drifted", failures)
        expect(entry.get("modulemap") == modulemap, f"{language} modulemap drifted", failures)
        expect(_as_list(entry.get("callable_metadata")) != [], f"{language} callable metadata is missing", failures)
        evidence_anchors = _as_list(entry.get("evidence_anchors"))
        if support_state == "supported":
            expect(evidence_anchors != [], f"{language} supported surface is missing executable evidence anchors", failures)
            for raw_path in evidence_anchors:
                expect(_is_portable_relative_path(raw_path), f"{language} evidence anchor is not portable-relative: {raw_path}", failures)
                expect((ROOT / str(raw_path)).is_file(), f"{language} evidence anchor is missing: {raw_path}", failures)
        safety = _as_object(entry.get("safety_policies"))
        for policy_name in ("ownership", "errors", "async", "object_identity"):
            policy_value = safety.get(policy_name)
            expect(
                isinstance(policy_value, str) and policy_value and policy_value != "implicit",
                f"{language} {policy_name} policy must be explicit",
                failures,
            )
        if language == "objc2":
            expect(
                entry.get("source_syntax_policy") == "metadata-only-retired-syntax-rejected",
                "ObjC2 bridge must be metadata-only and reject retired syntax",
                failures,
            )
    expect(supported_languages == {"c", "objc2"}, "only C and ObjC2 interop lanes may be supported by current evidence", failures)
    expect(reserved_languages == {"swift", "cpp"}, "Swift and C++ interop lanes must remain reserved until executable proof lands", failures)

    foreign_type_contracts = [
        _as_object(entry)
        for entry in _as_list(interop.get("foreign_type_contracts"))
        if isinstance(entry, dict)
    ]
    type_languages = {str(entry.get("source_language")) for entry in foreign_type_contracts}
    expect(REQUIRED_LANGUAGES <= type_languages, "foreign type contracts must cover every interop language", failures)
    type_names = [str(entry.get("type_name")) for entry in foreign_type_contracts]
    expect(len(type_names) == len(set(type_names)), "foreign type contract names must be unique", failures)
    for entry in foreign_type_contracts:
        language = str(entry.get("source_language"))
        alignment = int(entry.get("abi_alignment", 0))
        expect(entry.get("symbol_owner") == module_name, f"{language} foreign type owner drifted", failures)
        expect(alignment == surface_alignments.get(language), f"{language} foreign type ABI alignment drifted", failures)

    package_metadata = _as_object(payload.get("package_metadata"))
    expect(package_metadata.get("bridge_surface_count") == len(foreign_surfaces), "package bridge surface count drifted", failures)
    expect(package_metadata.get("supported_bridge_surface_count") == len(supported_languages), "package supported bridge surface count drifted", failures)
    expect(package_metadata.get("reserved_bridge_surface_count") == len(reserved_languages), "package reserved bridge surface count drifted", failures)
    expect(package_metadata.get("swift_bridge_surface_count") == 0, "package Swift supported bridge surface count drifted", failures)
    expect(package_metadata.get("cpp_bridge_surface_count") == 0, "package C++ supported bridge surface count drifted", failures)
    mixed_image_metadata = package_metadata.get("mixed_image_loader_metadata")
    expect(_is_portable_relative_path(mixed_image_metadata), "mixed image metadata path is not portable-relative", failures)
    if isinstance(mixed_image_metadata, str):
        metadata_path = ROOT / mixed_image_metadata
        expect(metadata_path.is_file(), "mixed image loader metadata fixture is missing", failures)
        if metadata_path.is_file():
            expect(
                package_metadata.get("mixed_image_loader_metadata_digest")
                == _file_sha256(metadata_path),
                "mixed image loader metadata digest drifted",
                failures,
            )


def _validate_unsupported_surfaces(payload: dict[str, Any], failures: list[str]) -> None:
    unsupported = [
        _as_object(entry)
        for entry in _as_list(payload.get("unsupported_surfaces"))
        if isinstance(entry, dict)
    ]
    surfaces = {str(entry.get("surface")) for entry in unsupported}
    expect(REQUIRED_UNSUPPORTED_SURFACES <= surfaces, "unsupported interop surfaces are incomplete", failures)
    for entry in unsupported:
        expect(entry.get("fail_closed") is True, f"unsupported surface is not fail-closed: {entry.get('surface')}", failures)
        diagnostic = entry.get("diagnostic")
        expect(isinstance(diagnostic, str) and diagnostic.startswith("O3"), "unsupported surface diagnostic must be stable", failures)


def validate_contract_payload(payload: dict[str, Any]) -> tuple[list[str], dict[str, bool]]:
    failures: list[str] = []
    _validate_schema(payload, failures)
    expect(
        REQUIRED_ISSUES <= {int(issue) for issue in _as_list(payload.get("issue_refs")) if isinstance(issue, int)},
        "contract must cover issues 8163 and 8165",
        failures,
    )
    expect(payload.get("public_command") == PUBLIC_COMMAND, "public command drifted", failures)
    expect(payload.get("evidence_log_allowed") is False, "evidence logs cannot be source authority", failures)
    native_checks = _validate_source_paths(payload, failures)
    _validate_checked_source_proofs(payload, failures)
    _validate_module_identity(payload, failures)
    _validate_imports_and_exports(payload, failures)
    _validate_dependency_graph(payload, failures)
    _validate_visibility_access(payload, failures)
    _validate_rebuild(payload, failures)
    _validate_interop(payload, failures)
    _validate_unsupported_surfaces(payload, failures)
    return failures, native_checks


def build_summary(path: Path = CONTRACT_PATH) -> dict[str, Any]:
    payload = load_json_object(path)
    failures, native_checks = validate_contract_payload(payload)
    interop = _as_object(payload.get("interop"))
    package_metadata = _as_object(payload.get("package_metadata"))
    foreign_surfaces = [
        _as_object(entry)
        for entry in _as_list(interop.get("foreign_surfaces"))
        if isinstance(entry, dict)
    ]
    return {
        "contract_id": "objc3c.module_interop.import_visibility_bridge_contract.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "contract": repo_rel(path),
        "schema": repo_rel(SCHEMA_PATH),
        "issues": payload.get("issue_refs", []),
        "public_command": payload.get("public_command"),
        "source_authority": payload.get("source_authority"),
        "module_name": _as_object(payload.get("module_identity")).get("module_name"),
        "foreign_languages": sorted(str(entry.get("language")) for entry in foreign_surfaces),
        "bridge_surface_count": package_metadata.get("bridge_surface_count"),
        "supported_bridge_surface_count": package_metadata.get("supported_bridge_surface_count"),
        "reserved_bridge_surface_count": package_metadata.get("reserved_bridge_surface_count"),
        "checked_source_proof_count": len(_as_list(payload.get("checked_source_proofs"))),
        "invalidation_case_count": len(
            _as_list(_as_object(payload.get("incremental_rebuild")).get("invalidation_cases"))
        ),
        "deterministic_replay_key": replay_key_for_contract(payload),
        "native_checks": native_checks,
        "failures": failures,
    }


def main(argv: list[str] | None = None) -> int:
    paths = [Path(arg) for arg in (argv or [])] or [CONTRACT_PATH]
    summaries = [build_summary(path) for path in paths]
    if len(summaries) == 1:
        summary: dict[str, Any] = summaries[0]
    else:
        failures = [
            f"{summary['contract']}: {failure}"
            for summary in summaries
            for failure in summary["failures"]
        ]
        summary = {
            "contract_id": "objc3c.module_interop.import_visibility_bridge_contract.batch_summary.v1",
            "status": "PASS" if not failures else "FAIL",
            "contracts": [summary["contract"] for summary in summaries],
            "failures": failures,
        }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, summary, sort_keys=True)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    if summary["status"] != "PASS":
        print("objc3c-module-interop-contracts: FAIL")
        for failure in summary["failures"]:
            print(f"- {failure}")
        return 1
    print("objc3c-module-interop-contracts: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
