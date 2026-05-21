#!/usr/bin/env python3
"""Validate the public macro/metaprogramming surface contract."""

from __future__ import annotations

import re
from pathlib import Path
from typing import Any

from objc3c_macro_expansion_artifact_ownership import (
    validate_macro_expansion_artifact_ownership,
)
from objc3c_tooling.json_io import (
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
    / "metaprogramming_public_surface"
    / "macro_metaprogramming_public_surface_contract.json"
)
MANIFEST_PATH = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"
CATALOG_PATH = ROOT / "tests" / "conformance" / "support_claim_runnable_evidence_catalog.json"
TRUST_REGISTRY_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "security_hardening"
    / "macro_supply_chain_trust_registry.json"
)
PUBLIC_CONFORMANCE_SCRIPT = ROOT / "scripts" / "check_objc3c_runnable_metaprogramming_conformance.py"
DERIVE_HELPER_PATH = (
    ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes_protocol_metaprogramming_helpers.inc"
)
DERIVE_NEGATIVE_CASES_PATH = (
    ROOT
    / "scripts"
    / "objc3c_runtime_acceptance"
    / "domains"
    / "metaprogramming_derive_property_negative_cases.py"
)
PROPERTY_NEGATIVE_CASES_PATH = DERIVE_NEGATIVE_CASES_PATH
MACRO_NEGATIVE_CASES_PATH = (
    ROOT
    / "scripts"
    / "objc3c_runtime_acceptance"
    / "domains"
    / "metaprogramming_macro_safety_negative_cases.py"
)
SEMA_CONTRACT_PATH = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract_metaprogramming_surfaces.h"
REPORT_PATH = ROOT / "tmp" / "reports" / "metaprogramming-public-surface" / "summary.json"
SUMMARY_CONTRACT_ID = "objc3c.metaprogramming.public.macro.surface.summary.v1"

ATTR_RE = re.compile(
    r"objc_(macro(?:_(?:package|provenance|cache_key|sandbox))?)\(named\(\"([^\"]+)\"\)\)"
)


def _record(
    checks: dict[str, bool],
    failures: list[str],
    name: str,
    condition: bool,
    message: str,
) -> None:
    checks[name] = bool(condition)
    if not condition:
        failures.append(message)


def _display_path(path: Path) -> str:
    try:
        return repo_rel(path)
    except ValueError:
        return str(path)


def _as_dict(value: Any) -> dict[str, Any]:
    return value if isinstance(value, dict) else {}


def _as_list(value: Any) -> list[Any]:
    return value if isinstance(value, list) else []


def _string_set(value: Any) -> set[str]:
    return {str(item) for item in _as_list(value)}


def _repo_path(relative_path: str) -> Path:
    return ROOT / relative_path


def _read_text(relative_path: str) -> str:
    return _repo_path(relative_path).read_text(encoding="utf-8")


def _all_repo_paths_exist(paths: list[str]) -> bool:
    return all(_repo_path(path).is_file() for path in paths)


def _validate_contract_schema(
    contract: dict[str, Any],
    contract_path: Path,
    checks: dict[str, bool],
    failures: list[str],
) -> None:
    schema_path = str(contract.get("schema_path", ""))
    try:
        schema = load_json_object(ROOT / schema_path) if schema_path else {}
        validate_json_schema(contract, schema, label=_display_path(contract_path))
        checks["contract_schema"] = True
    except (JsonSchemaValidationError, RuntimeError) as exc:
        checks["contract_schema"] = False
        failures.append(f"{_display_path(contract_path)}: {exc}")


def _catalog_rows(catalog: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return {
        str(row.get("support_claim")): row
        for row in catalog.get("rows", [])
        if isinstance(row, dict) and row.get("support_claim") is not None
    }


def _manifest_claims(manifest: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return {
        str(claim.get("claim_id")): claim
        for claim in manifest.get("support_claims", [])
        if isinstance(claim, dict) and claim.get("claim_id") is not None
    }


def _manifest_fixtures(manifest: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return {
        str(fixture.get("path")): fixture
        for fixture in manifest.get("fixtures", [])
        if isinstance(fixture, dict) and fixture.get("path") is not None
    }


def _require_subset(actual: Any, expected: list[str]) -> bool:
    return set(expected) <= set(actual if isinstance(actual, list) else [])


def _extract_macro_attrs(path: Path) -> dict[str, str]:
    attrs: dict[str, str] = {}
    for match in ATTR_RE.finditer(path.read_text(encoding="utf-8")):
        attrs[match.group(1)] = match.group(2)
    return attrs


def _artifact_key(payload: dict[str, Any]) -> tuple[Any, Any, Any, Any]:
    return (
        payload.get("package"),
        payload.get("macro"),
        payload.get("provenance"),
        payload.get("cache_key"),
    )


def _validate_support_claims(
    contract: dict[str, Any],
    manifest: dict[str, Any],
    catalog: dict[str, Any],
    checks: dict[str, bool],
    failures: list[str],
) -> list[str]:
    claims = _manifest_claims(manifest)
    fixtures = _manifest_fixtures(manifest)
    rows = _catalog_rows(catalog)
    claim_ids: list[str] = []
    for claim in contract["support_claims"]:
        claim_id = str(claim["claim_id"])
        claim_ids.append(claim_id)
        manifest_claim = claims.get(claim_id, {})
        catalog_row = rows.get(claim_id, {})
        fixture = fixtures.get(str(claim["behavior_fixture"]), {})

        _record(
            checks,
            failures,
            f"{claim_id}:manifest_claim",
            bool(manifest_claim),
            f"missing canonical manifest support claim {claim_id}",
        )
        _record(
            checks,
            failures,
            f"{claim_id}:catalog_row",
            bool(catalog_row),
            f"missing runnable evidence catalog row {claim_id}",
        )
        _record(
            checks,
            failures,
            f"{claim_id}:manifest_fixture",
            bool(fixture),
            f"missing canonical manifest behavior fixture for {claim_id}",
        )
        _record(
            checks,
            failures,
            f"{claim_id}:owner_phase",
            manifest_claim.get("owner_phase") == claim["owner_phase"]
            and catalog_row.get("owner_phase") == claim["owner_phase"],
            f"owner phase drift for {claim_id}",
        )
        _record(
            checks,
            failures,
            f"{claim_id}:capability",
            catalog_row.get("capability_id") == claim["capability_id"],
            f"capability id drift for {claim_id}",
        )
        _record(
            checks,
            failures,
            f"{claim_id}:behavior_fixture",
            manifest_claim.get("behavior_fixture") == claim["behavior_fixture"]
            and claim["behavior_fixture"] in catalog_row.get("positive_evidence", []),
            f"behavior fixture drift for {claim_id}",
        )
        _record(
            checks,
            failures,
            f"{claim_id}:runnable_command",
            manifest_claim.get("executable_command") == claim["runnable_command"]
            and catalog_row.get("runnable_command") == claim["runnable_command"],
            f"runnable command drift for {claim_id}",
        )
        _record(
            checks,
            failures,
            f"{claim_id}:runtime_case",
            catalog_row.get("runtime_acceptance_case") == claim["runtime_acceptance_case"],
            f"runtime acceptance case drift for {claim_id}",
        )
        _record(
            checks,
            failures,
            f"{claim_id}:positive_evidence",
            _require_subset(
                catalog_row.get("positive_evidence"),
                claim["required_positive_evidence"],
            ),
            f"positive evidence incomplete for {claim_id}",
        )
        _record(
            checks,
            failures,
            f"{claim_id}:negative_evidence",
            _require_subset(
                catalog_row.get("negative_evidence"),
                claim["required_negative_evidence"],
            ),
            f"negative evidence incomplete for {claim_id}",
        )
        _record(
            checks,
            failures,
            f"{claim_id}:diagnostic_codes",
            _require_subset(
                catalog_row.get("required_diagnostic_codes"),
                claim["required_diagnostic_codes"],
            ),
            f"diagnostic code coverage incomplete for {claim_id}",
        )
        _record(
            checks,
            failures,
            f"{claim_id}:no_tmp_evidence",
            all(
                not str(path).startswith("tmp/")
                for path in [
                    *catalog_row.get("positive_evidence", []),
                    *catalog_row.get("negative_evidence", []),
                ]
            ),
            f"catalog row for {claim_id} uses tmp evidence as source truth",
        )
    return claim_ids


def _validate_derive_surface(
    contract: dict[str, Any],
    checks: dict[str, bool],
    failures: list[str],
) -> None:
    derive_surface = contract["derive_surface"]
    positive_text = _read_text(derive_surface["positive_fixture"])
    helper_text = DERIVE_HELPER_PATH.read_text(encoding="utf-8")
    negative_case_text = DERIVE_NEGATIVE_CASES_PATH.read_text(encoding="utf-8")
    sema_contract_text = SEMA_CONTRACT_PATH.read_text(encoding="utf-8")

    for form in derive_surface["supported_forms"]:
        expected_spelling = f'objc_derive(named("{form["name"]}"))'
        alias_spellings = [
            f'objc_derive(named("{alias}"))' for alias in form.get("aliases", [])
        ]
        _record(
            checks,
            failures,
            f"derive:{form['name']}:fixture",
            expected_spelling in positive_text
            or any(alias in positive_text for alias in alias_spellings),
            f"derive fixture does not publish {form['name']}",
        )
        _record(
            checks,
            failures,
            f"derive:{form['name']}:helper",
            f'return "{form["name"]}"' in helper_text
            and f'return "{form["selector"]}"' in helper_text,
            f"derive helper does not bind {form['name']} to {form['selector']}",
        )

    for negative in derive_surface["expected_negative_cases"]:
        fixture_path = _repo_path(negative["fixture"])
        _record(
            checks,
            failures,
            f"derive-negative:{negative['key']}:fixture",
            fixture_path.is_file(),
            f"missing derive negative fixture {negative['fixture']}",
        )
        _record(
            checks,
            failures,
            f"derive-negative:{negative['key']}:acceptance",
            negative["key"] in negative_case_text
            and negative["code"] in negative_case_text
            and negative["message"] in negative_case_text,
            f"derive negative case {negative['key']} is not wired into runtime acceptance",
        )
        _record(
            checks,
            failures,
            f"derive-negative:{negative['key']}:sema",
            negative["code"] in sema_contract_text or negative["code"] in helper_text or negative["code"] in negative_case_text,
            f"derive negative case {negative['key']} does not carry {negative['code']}",
        )


def _validate_property_surface(
    contract: dict[str, Any],
    checks: dict[str, bool],
    failures: list[str],
) -> None:
    surface = contract["property_behavior_surface"]
    positive_text = _read_text(surface["positive_fixture"])
    helper_text = DERIVE_HELPER_PATH.read_text(encoding="utf-8")
    negative_case_text = PROPERTY_NEGATIVE_CASES_PATH.read_text(encoding="utf-8")
    sema_contract_text = SEMA_CONTRACT_PATH.read_text(encoding="utf-8")

    for behavior in surface["supported_behaviors"]:
        _record(
            checks,
            failures,
            f"property:{behavior}:fixture",
            f"behavior={behavior}" in positive_text,
            f"property behavior fixture does not publish {behavior}",
        )
        _record(
            checks,
            failures,
            f"property:{behavior}:helper",
            f'return "{behavior}"' in helper_text,
            f"property helper does not support {behavior}",
        )

    for negative in surface["expected_negative_cases"]:
        _record(
            checks,
            failures,
            f"property-negative:{negative['key']}:fixture",
            _repo_path(negative["fixture"]).is_file(),
            f"missing property negative fixture {negative['fixture']}",
        )
        _record(
            checks,
            failures,
            f"property-negative:{negative['key']}:acceptance",
            negative["key"] in negative_case_text
            and negative["code"] in negative_case_text
            and negative["message"] in negative_case_text,
            f"property negative case {negative['key']} is not wired into runtime acceptance",
        )
        _record(
            checks,
            failures,
            f"property-negative:{negative['key']}:contract",
            negative["code"] in sema_contract_text or negative["code"] in negative_case_text,
            f"property negative case {negative['key']} does not carry {negative['code']}",
        )


def _validate_macro_surface(
    contract: dict[str, Any],
    registry: dict[str, Any],
    checks: dict[str, bool],
    failures: list[str],
) -> None:
    surface = contract["macro_safety_surface"]
    packages = {
        package.get("package"): package
        for package in registry.get("packages", [])
        if isinstance(package, dict)
    }
    signed_artifacts = {
        _artifact_key(artifact): artifact
        for artifact in registry.get("signed_artifacts", [])
        if isinstance(artifact, dict)
    }
    expected_denial_ids = {
        str(case.get("case_id"))
        for case in registry.get("expected_denials", [])
        if isinstance(case, dict)
    }

    for fixture in surface["positive_fixture_paths"]:
        attrs = _extract_macro_attrs(_repo_path(fixture))
        candidate = {
            "package": attrs.get("macro_package"),
            "macro": attrs.get("macro"),
            "provenance": attrs.get("macro_provenance"),
            "cache_key": attrs.get("macro_cache_key"),
            "sandbox_policy": attrs.get("macro_sandbox"),
        }
        artifact = signed_artifacts.get(_artifact_key(candidate))
        _record(
            checks,
            failures,
            f"macro:{fixture}:attributes",
            all(
                attr.replace("objc_", "") in attrs
                if attr == "objc_macro"
                else attr.removeprefix("objc_") in attrs
                for attr in surface["required_attributes"]
            ),
            f"macro fixture {fixture} does not publish all required attributes",
        )
        _record(
            checks,
            failures,
            f"macro:{fixture}:signed",
            isinstance(artifact, dict)
            and artifact.get("sandbox_policy") in surface["allowed_sandbox_policies"]
            and artifact.get("replay_metadata", {}).get("deterministic") is True,
            f"macro fixture {fixture} is not signed with deterministic replay metadata",
        )

    for package_name in surface["trusted_packages"]:
        package = packages.get(package_name)
        _record(
            checks,
            failures,
            f"macro-package:{package_name}:trusted",
            isinstance(package, dict)
            and package.get("trust_state") == "trusted"
            and package.get("default_sandbox") == "deny"
            and package.get("network_allowed") is False
            and package.get("filesystem_write_allowed") is False,
            f"macro package {package_name} is not fail-closed trusted",
        )

    _record(
        checks,
        failures,
        "macro:expected_denials",
        set(surface["expected_denial_case_ids"]) <= expected_denial_ids,
        "macro trust registry is missing expected fail-closed denial cases",
    )


def _validate_supported_surface_policy(
    contract: dict[str, Any],
    checks: dict[str, bool],
    failures: list[str],
) -> None:
    supported = _as_dict(contract.get("supported_surface"))
    macro_model = _as_dict(supported.get("macro_declaration_model"))
    derive_model = _as_dict(supported.get("derive_model"))
    property_model = _as_dict(supported.get("property_behavior_model"))
    host_boundary = _as_dict(supported.get("host_cache_boundary"))
    rejection_model = _as_dict(supported.get("rejection_behavior_model"))
    macro_surface = _as_dict(contract.get("macro_safety_surface"))
    derive_surface = _as_dict(contract.get("derive_surface"))
    property_surface = _as_dict(contract.get("property_behavior_surface"))
    fail_closed_validation = _as_dict(contract.get("fail_closed_validation"))

    supported_derive_forms: set[str] = set()
    for form in _as_list(derive_surface.get("supported_forms")):
        if not isinstance(form, dict):
            continue
        supported_derive_forms.add(str(form.get("name", "")))
        supported_derive_forms.update(_string_set(form.get("aliases")))
    supported_derive_forms.discard("")

    _record(
        checks,
        failures,
        "supported-surface:macro-names",
        _string_set(macro_model.get("supported_macro_names"))
        == _string_set(macro_surface.get("allowed_macro_names"))
        == {"Trace"},
        "public macro surface must be limited to the checked Trace macro",
    )
    _record(
        checks,
        failures,
        "supported-surface:macro-attributes",
        _string_set(macro_model.get("supported_attribute_names"))
        == _string_set(macro_surface.get("required_attributes")),
        "public macro surface attributes drifted from required macro safety attributes",
    )
    _record(
        checks,
        failures,
        "supported-surface:macro-topology",
        "pure-body-backed-free-function"
        in _string_set(macro_model.get("admitted_callable_topologies"))
        and "methods" in _string_set(macro_model.get("reserved_callable_topologies"))
        and macro_model.get("unsafe_host_execution_allowed") is False,
        "public macro surface must admit only pure free-function macro markers and reserve methods/unsafe host execution",
    )
    _record(
        checks,
        failures,
        "supported-surface:derive-forms",
        _string_set(derive_model.get("supported_forms")) == supported_derive_forms,
        "supported derive model drifted from derive surface forms and aliases",
    )
    _record(
        checks,
        failures,
        "supported-surface:property-behaviors",
        _string_set(property_model.get("supported_behaviors"))
        == _string_set(property_surface.get("supported_behaviors")),
        "supported property behavior model drifted from property behavior surface",
    )
    _record(
        checks,
        failures,
        "supported-surface:host-cache-boundary",
        host_boundary.get("artifact_contract")
        == "objc3c.metaprogramming.macro.expansion.artifact.ownership.v1"
        and host_boundary.get("generated_artifacts_are_support_authority") is False,
        "host-cache boundary must point at artifact ownership and deny support authority",
    )
    _record(
        checks,
        failures,
        "supported-surface:rejection-topology",
        _string_set(rejection_model.get("unsupported_topologies"))
        == _string_set(macro_model.get("reserved_callable_topologies"))
        and rejection_model.get("fail_closed_before_expansion") is True,
        "macro rejection behavior must mirror reserved callable topologies and fail before expansion",
    )
    _record(
        checks,
        failures,
        "supported-surface:rejection-diagnostics",
        _string_set(rejection_model.get("required_negative_case_ids"))
        == _string_set(fail_closed_validation.get("required_negative_case_ids"))
        and _string_set(rejection_model.get("required_diagnostic_codes"))
        == _string_set(fail_closed_validation.get("required_diagnostic_codes"))
        and _string_set(rejection_model.get("denial_case_ids"))
        == _string_set(macro_surface.get("expected_denial_case_ids")),
        "macro rejection behavior drifted from fail-closed cases, diagnostics, or denial ids",
    )


def _validate_expansion_metadata_model(
    contract: dict[str, Any],
    registry: dict[str, Any],
    checks: dict[str, bool],
    failures: list[str],
) -> None:
    supported = _as_dict(contract.get("supported_surface"))
    metadata_model = _as_dict(supported.get("expansion_metadata_model"))
    required_fields = _string_set(metadata_model.get("required_metadata_fields"))
    sources = [
        str(path)
        for path in _as_list(metadata_model.get("replay_visible_metadata_sources"))
    ]
    signed_artifacts = [
        artifact
        for artifact in _as_list(registry.get("signed_artifacts"))
        if isinstance(artifact, dict)
    ]

    def artifact_has_required_fields(artifact: dict[str, Any]) -> bool:
        for field in required_fields:
            if field == "deterministic":
                if (
                    _as_dict(artifact.get("replay_metadata")).get("deterministic")
                    is not True
                ):
                    return False
                continue
            if field not in artifact:
                return False
        return True

    _record(
        checks,
        failures,
        "expansion-metadata:authority",
        metadata_model.get("generated_artifact_authority") is False
        and metadata_model.get("deterministic_replay_required") is True
        and "trust registry" in str(metadata_model.get("metadata_authority", "")),
        "expansion metadata must be source-authoritative, deterministic, and not generated-artifact authoritative",
    )
    _record(
        checks,
        failures,
        "expansion-metadata:sources",
        all(path and _repo_path(path).is_file() for path in sources)
        and all(not path.startswith("tmp/") for path in sources)
        and str(metadata_model.get("status")) == "supported",
        "expansion metadata replay sources must be checked-in non-tmp source paths",
    )
    _record(
        checks,
        failures,
        "expansion-metadata:registry-fields",
        bool(signed_artifacts)
        and all(artifact_has_required_fields(artifact) for artifact in signed_artifacts),
        "macro trust registry signed artifacts do not satisfy the public expansion metadata fields",
    )


def _validate_reserved_surface_policy(
    contract: dict[str, Any],
    checks: dict[str, bool],
    failures: list[str],
) -> None:
    reserved = _as_dict(contract.get("reserved_surface"))
    expected_statuses = {
        "arbitrary_compile_time_execution": {"rejected"},
        "unsafe_plugin_execution": {"rejected"},
        "network_access": {"rejected"},
        "filesystem_write_access": {"rejected"},
        "third_party_macro_ecosystem_compatibility": {"reserved"},
        "runtime_expanded_body_materialization": {"deferred"},
    }
    for key, allowed_statuses in expected_statuses.items():
        entry = _as_dict(reserved.get(key))
        _record(
            checks,
            failures,
            f"reserved-surface:{key}",
            entry.get("status") in allowed_statuses
            and bool(str(entry.get("reason", "")).strip()),
            f"reserved surface {key} must stay {sorted(allowed_statuses)} with a reason",
        )

    non_claims = set(str(claim) for claim in _as_list(contract.get("non_claims")))
    _record(
        checks,
        failures,
        "reserved-surface:non-claims",
        {
            "no arbitrary host process execution",
            "no network access from macro expansion",
            "no source compatibility with third-party macro ecosystems",
        }
        <= non_claims,
        "public surface non-claims must reject arbitrary execution, network access, and third-party ecosystem compatibility",
    )


def _validate_expansion_security_policy(
    contract: dict[str, Any],
    registry: dict[str, Any],
    checks: dict[str, bool],
    failures: list[str],
) -> None:
    policy = _as_dict(contract.get("expansion_security_policy"))
    fail_closed_requirements = _as_dict(registry.get("fail_closed_requirements"))
    expected_denial_ids = {
        str(case.get("case_id"))
        for case in _as_list(registry.get("expected_denials"))
        if isinstance(case, dict)
    }
    macro_surface = _as_dict(contract.get("macro_safety_surface"))

    _record(
        checks,
        failures,
        "security-policy:deny-by-default",
        policy.get("default_sandbox") == "deny"
        and policy.get("network_allowed") is False
        and policy.get("filesystem_write_allowed") is False
        and policy.get("arbitrary_host_process_execution_allowed") is False
        and policy.get("third_party_plugin_loading_allowed") is False,
        "macro expansion security policy must deny network/filesystem/unsafe host/plugin execution",
    )
    _record(
        checks,
        failures,
        "security-policy:allowed-sandbox",
        _string_set(policy.get("allowed_sandbox_policies"))
        == _string_set(macro_surface.get("allowed_sandbox_policies"))
        == {"deterministic"},
        "macro expansion security policy must be limited to deterministic sandbox policy",
    )
    _record(
        checks,
        failures,
        "security-policy:registry-path",
        _repo_path(str(policy.get("trust_registry_path", ""))) == TRUST_REGISTRY_PATH
        and TRUST_REGISTRY_PATH.is_file(),
        "macro expansion security policy trust registry path drifted",
    )
    _record(
        checks,
        failures,
        "security-policy:required-trust-checks",
        all(
            fail_closed_requirements.get(requirement) is True
            for requirement in _string_set(policy.get("fail_closed_requirements"))
        )
        and policy.get("require_signature") is True
        and policy.get("require_replay_metadata") is True
        and policy.get("require_cache_key_binding") is True
        and policy.get("require_deterministic_replay") is True,
        "macro expansion security policy is not backed by fail-closed trust registry requirements",
    )
    _record(
        checks,
        failures,
        "security-policy:expected-denials",
        _string_set(policy.get("expected_denial_case_ids")) <= expected_denial_ids
        and _string_set(policy.get("expected_denial_case_ids"))
        == _string_set(macro_surface.get("expected_denial_case_ids")),
        "macro expansion security policy denial cases drifted from trust registry or macro safety surface",
    )


def _validate_deterministic_fixture_contracts(
    contract: dict[str, Any],
    checks: dict[str, bool],
    failures: list[str],
) -> None:
    fixture_contracts = _as_dict(contract.get("deterministic_fixture_contracts"))
    macro_surface = _as_dict(contract.get("macro_safety_surface"))
    derive_surface = _as_dict(contract.get("derive_surface"))
    property_surface = _as_dict(contract.get("property_behavior_surface"))
    replay_sources = [
        str(path) for path in _as_list(fixture_contracts.get("replay_metadata_sources"))
    ]

    fixture_paths = [
        *[
            str(path)
            for path in _as_list(fixture_contracts.get("positive_macro_fixtures"))
        ],
        str(fixture_contracts.get("derive_positive_fixture", "")),
        str(fixture_contracts.get("property_behavior_positive_fixture", "")),
        str(fixture_contracts.get("generated_artifact_boundary_contract", "")),
        *replay_sources,
    ]
    _record(
        checks,
        failures,
        "deterministic-fixtures:macro-positive-set",
        _string_set(fixture_contracts.get("positive_macro_fixtures"))
        == _string_set(macro_surface.get("positive_fixture_paths")),
        "deterministic macro fixture set drifted from macro safety surface",
    )
    _record(
        checks,
        failures,
        "deterministic-fixtures:derive-positive",
        fixture_contracts.get("derive_positive_fixture")
        == derive_surface.get("positive_fixture"),
        "deterministic derive fixture drifted from derive surface",
    )
    _record(
        checks,
        failures,
        "deterministic-fixtures:property-positive",
        fixture_contracts.get("property_behavior_positive_fixture")
        == property_surface.get("positive_fixture"),
        "deterministic property behavior fixture drifted from property behavior surface",
    )
    _record(
        checks,
        failures,
        "deterministic-fixtures:paths-exist",
        all(path and _repo_path(path).is_file() for path in fixture_paths)
        and all(not path.startswith("tmp/") for path in fixture_paths),
        "deterministic fixture contract must use checked-in non-tmp source paths",
    )
    _record(
        checks,
        failures,
        "deterministic-fixtures:tmp-not-source-truth",
        fixture_contracts.get("tmp_report_source_truth_allowed") is False,
        "deterministic fixture contract must deny tmp report source truth",
    )


def _validate_fail_closed_validation_policy(
    contract: dict[str, Any],
    checks: dict[str, bool],
    failures: list[str],
) -> None:
    validation = _as_dict(contract.get("fail_closed_validation"))
    macro_surface = _as_dict(contract.get("macro_safety_surface"))
    macro_claims = [
        claim
        for claim in _as_list(contract.get("support_claims"))
        if isinstance(claim, dict)
        and claim.get("claim_id")
        == "objc3c.behavior.language.metaprogramming.macro-safety-sandbox-determinism"
    ]
    macro_claim = macro_claims[0] if macro_claims else {}
    negative_case_text = MACRO_NEGATIVE_CASES_PATH.read_text(encoding="utf-8")
    required_negative_case_ids = _string_set(validation.get("required_negative_case_ids"))
    required_diagnostic_codes = _string_set(validation.get("required_diagnostic_codes"))
    source_truth_paths = [
        str(path) for path in _as_list(validation.get("source_truth_paths"))
    ]

    _record(
        checks,
        failures,
        "fail-closed-validation:runner-schema",
        validation.get("validation_runner")
        == "scripts/check_objc3c_metaprogramming_public_surface.py"
        and validation.get("schema_path") == contract.get("schema_path")
        and _repo_path(str(validation.get("schema_path", ""))).is_file(),
        "fail-closed validation must point at the public surface runner and schema",
    )
    _record(
        checks,
        failures,
        "fail-closed-validation:negative-cases",
        all(case_id in negative_case_text for case_id in required_negative_case_ids)
        and "method_topology" in required_negative_case_ids
        and _string_set(macro_surface.get("expected_denial_case_ids")),
        "fail-closed validation must enumerate wired macro-safety negative cases",
    )
    _record(
        checks,
        failures,
        "fail-closed-validation:diagnostic-codes",
        required_diagnostic_codes
        <= _string_set(macro_claim.get("required_diagnostic_codes")),
        "fail-closed validation diagnostic codes drifted from macro safety support claim",
    )
    _record(
        checks,
        failures,
        "fail-closed-validation:source-truth-paths",
        all(path and _repo_path(path).is_file() for path in source_truth_paths)
        and all(not path.startswith("tmp/") for path in source_truth_paths),
        "fail-closed validation source truth paths must be checked-in non-tmp files",
    )
    _record(
        checks,
        failures,
        "fail-closed-validation:tmp-not-committable",
        validation.get("tmp_report_committable") is False,
        "fail-closed validation must not make tmp reports committable source truth",
    )


def build_summary() -> dict[str, Any]:
    contract = load_json_object(CONTRACT_PATH)
    manifest = load_json_object(MANIFEST_PATH)
    catalog = load_json_object(CATALOG_PATH)
    registry = load_json_object(TRUST_REGISTRY_PATH)

    checks: dict[str, bool] = {}
    failures: list[str] = []

    _validate_contract_schema(contract, CONTRACT_PATH, checks, failures)
    _record(
        checks,
        failures,
        "contract_id",
        contract.get("contract_id")
        == "objc3c.metaprogramming.public.macro.surface.contract.v1",
        "public metaprogramming surface contract id drifted",
    )
    _record(
        checks,
        failures,
        "issue_ref",
        contract.get("issue_ref") == 8168 and 8168 in catalog.get("issue_refs", []),
        "issue 8168 is not carried by the public surface contract and catalog",
    )
    _record(
        checks,
        failures,
        "public_command",
        contract.get("public_command")
        == "npm run objc3c -- validate-metaprogramming-conformance",
        "public metaprogramming command drifted",
    )
    _record(
        checks,
        failures,
        "source_contract_paths",
        _all_repo_paths_exist(contract["source_contract_paths"]),
        "one or more public metaprogramming source contract paths are missing",
    )
    _record(
        checks,
        failures,
        "validate_command_wiring",
        Path(contract["runner_path"]).name
        in PUBLIC_CONFORMANCE_SCRIPT.read_text(encoding="utf-8"),
        "validate-metaprogramming-conformance does not run the public surface checker",
    )
    _record(
        checks,
        failures,
        "non_claims_fail_closed",
        any("no arbitrary host process execution" == claim for claim in contract["non_claims"])
        and any("no network access from macro expansion" == claim for claim in contract["non_claims"])
        and any("no compatibility shim" in claim for claim in contract["non_claims"]),
        "public surface non-claims do not preserve the hard-cutover boundary",
    )

    claim_ids = _validate_support_claims(contract, manifest, catalog, checks, failures)
    _validate_derive_surface(contract, checks, failures)
    _validate_property_surface(contract, checks, failures)
    _validate_macro_surface(contract, registry, checks, failures)
    _validate_supported_surface_policy(contract, checks, failures)
    _validate_expansion_metadata_model(contract, registry, checks, failures)
    _validate_reserved_surface_policy(contract, checks, failures)
    _validate_expansion_security_policy(contract, registry, checks, failures)
    _validate_deterministic_fixture_contracts(contract, checks, failures)
    _validate_fail_closed_validation_policy(contract, checks, failures)
    artifact_ownership_result = validate_macro_expansion_artifact_ownership()
    _record(
        checks,
        failures,
        "macro_expansion_artifact_ownership",
        artifact_ownership_result.passed,
        "macro expansion artifact ownership contract validation failed",
    )
    failures.extend(
        f"macro expansion artifact ownership: {failure}"
        for failure in artifact_ownership_result.failures
    )

    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "source_contract_id": contract["contract_id"],
        "schema_path": contract.get("schema_path"),
        "artifact_ownership_contract_id": artifact_ownership_result.payload[
            "source_contract_id"
        ],
        "status": "PASS" if not failures else "FAIL",
        "issue_ref": contract["issue_ref"],
        "public_command": contract["public_command"],
        "runner_path": "scripts/check_objc3c_metaprogramming_public_surface.py",
        "support_claim_ids": claim_ids,
        "lead_owned_support_rows": contract["lead_owned_support_rows"],
        "supported_surface": contract.get("supported_surface", {}),
        "reserved_surface": contract.get("reserved_surface", {}),
        "expansion_security_policy": contract.get("expansion_security_policy", {}),
        "deterministic_fixture_contracts": contract.get(
            "deterministic_fixture_contracts", {}
        ),
        "fail_closed_validation": contract.get("fail_closed_validation", {}),
        "surface_counts": {
            "support_claims": len(contract["support_claims"]),
            "derive_forms": len(contract["derive_surface"]["supported_forms"]),
            "derive_negative_cases": len(
                contract["derive_surface"]["expected_negative_cases"]
            ),
            "property_behaviors": len(
                contract["property_behavior_surface"]["supported_behaviors"]
            ),
            "macro_positive_fixtures": len(
                contract["macro_safety_surface"]["positive_fixture_paths"]
            ),
            "artifact_ownership_required_fields": artifact_ownership_result.payload[
                "required_field_count"
            ],
            "artifact_ownership_fail_closed_cases": artifact_ownership_result.payload[
                "fail_closed_case_count"
            ],
            "reserved_surface_entries": len(contract.get("reserved_surface", {})),
            "fail_closed_validation_cases": len(
                _as_list(
                    contract.get("fail_closed_validation", {}).get(
                        "required_negative_case_ids"
                    )
                )
            ),
        },
        "checks": checks,
        "failures": failures,
    }


def main() -> int:
    payload = build_summary()
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, payload, sort_keys=True)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
