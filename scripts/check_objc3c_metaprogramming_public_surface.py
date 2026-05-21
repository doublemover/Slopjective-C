#!/usr/bin/env python3
"""Validate the public macro/metaprogramming surface contract."""

from __future__ import annotations

import re
from pathlib import Path
from typing import Any

from objc3c_macro_expansion_artifact_ownership import (
    validate_macro_expansion_artifact_ownership,
)
from objc3c_tooling.json_io import load_json_object, write_json_file
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


def _repo_path(relative_path: str) -> Path:
    return ROOT / relative_path


def _read_text(relative_path: str) -> str:
    return _repo_path(relative_path).read_text(encoding="utf-8")


def _all_repo_paths_exist(paths: list[str]) -> bool:
    return all(_repo_path(path).is_file() for path in paths)


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


def build_summary() -> dict[str, Any]:
    contract = load_json_object(CONTRACT_PATH)
    manifest = load_json_object(MANIFEST_PATH)
    catalog = load_json_object(CATALOG_PATH)
    registry = load_json_object(TRUST_REGISTRY_PATH)

    checks: dict[str, bool] = {}
    failures: list[str] = []

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
        "artifact_ownership_contract_id": artifact_ownership_result.payload[
            "source_contract_id"
        ],
        "status": "PASS" if not failures else "FAIL",
        "issue_ref": contract["issue_ref"],
        "public_command": contract["public_command"],
        "runner_path": "scripts/check_objc3c_metaprogramming_public_surface.py",
        "support_claim_ids": claim_ids,
        "lead_owned_support_rows": contract["lead_owned_support_rows"],
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
