from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
MANIFEST_PATH = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"
CATALOG_PATH = ROOT / "tests" / "conformance" / "support_claim_runnable_evidence_catalog.json"

PROPERTY_BEHAVIOR_CLAIM = "objc3c.behavior.language.metaprogramming.property-behavior-semantics"
HOST_CACHE_CLAIM = "objc3c.behavior.runtime.metaprogramming.host-cache-boundary"

EXPECTED_CLAIMS = {
    PROPERTY_BEHAVIOR_CLAIM: {
        "capability_id": "language.metaprogramming.property-behavior-semantics",
        "owner_phase": "sema",
        "behavior_fixture": "tests/tooling/fixtures/native/property_behavior_legality_positive.objc3",
        "required_positive": {
            "tests/tooling/fixtures/metaprogramming_interop_closure/property_behavior_runtime_materialization_policy.json",
            "tests/tooling/fixtures/metaprogramming_interop_closure/metaprogramming_runtime_semantic_model.json",
            "tests/tooling/runtime/expansion_host_runtime_boundary_probe.cpp",
        },
        "required_negative": {
            "tests/tooling/fixtures/native/property_behavior_legality_negative_unsupported.objc3",
            "tests/tooling/fixtures/native/property_behavior_legality_negative_nonobject.objc3",
            "tests/tooling/fixtures/native/property_behavior_legality_negative_protocol_observed.objc3",
            "tests/tooling/fixtures/native/property_behavior_legality_negative_projected_writable.objc3",
        },
        "required_codes": {"O3S326", "O3S327", "O3S328", "O3S330"},
    },
    HOST_CACHE_CLAIM: {
        "capability_id": "runtime.metaprogramming.host-cache-boundary",
        "owner_phase": "runtime",
        "behavior_fixture": "tests/tooling/fixtures/native/expansion_host_runtime_boundary_positive.objc3",
        "required_positive": {
            "tests/tooling/fixtures/native/macro_host_process_provider.objc3",
            "tests/tooling/fixtures/native/macro_host_process_consumer.objc3",
            "tests/tooling/runtime/expansion_host_runtime_boundary_probe.cpp",
            "tests/tooling/runtime/macro_host_process_cache_integration_probe.cpp",
        },
        "required_negative": {
            "tests/tooling/fixtures/native/macro_safety_sandbox_negative_missing_cache_key.objc3",
            "tests/tooling/fixtures/native/macro_safety_sandbox_negative_missing_sandbox_policy.objc3",
            "tests/tooling/fixtures/native/macro_safety_sandbox_negative_invalid_package.objc3",
        },
        "required_codes": {"O3S331", "O3S332", "O3S322"},
    },
}


def _load_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def _assert_repo_paths_exist(paths: set[str]) -> None:
    for relative_path in paths:
        assert not relative_path.startswith("tmp/")
        assert (ROOT / relative_path).is_file(), relative_path


def test_metaprogramming_support_claims_are_manifest_backed() -> None:
    manifest = _load_json(MANIFEST_PATH)
    claims = {claim["claim_id"]: claim for claim in manifest["support_claims"]}
    fixtures = {fixture["path"]: fixture for fixture in manifest["fixtures"]}

    for claim_id, expected in EXPECTED_CLAIMS.items():
        claim = claims[claim_id]
        assert claim["owner_phase"] == expected["owner_phase"]
        assert claim["behavior_fixture"] == expected["behavior_fixture"]
        assert claim["executable_command"] == "npm run objc3c -- test-runtime-acceptance"

        fixture = fixtures[expected["behavior_fixture"]]
        assert fixture["origin"] == "hand-authored"
        assert fixture["owner_phase"] == expected["owner_phase"]
        assert fixture["fixture_kind"] == "positive"
        assert fixture["expected_diagnostic_code"] == ""


def test_metaprogramming_runnable_evidence_catalog_rows_are_narrow() -> None:
    manifest = _load_json(MANIFEST_PATH)
    catalog = _load_json(CATALOG_PATH)
    manifest_claims = {claim["claim_id"]: claim for claim in manifest["support_claims"]}
    rows = {row["support_claim"]: row for row in catalog["rows"]}

    assert 8155 in catalog["issue_refs"]

    for claim_id, expected in EXPECTED_CLAIMS.items():
        row = rows[claim_id]
        manifest_claim = manifest_claims[claim_id]
        positive_evidence = set(row["positive_evidence"])
        negative_evidence = set(row["negative_evidence"])
        required_codes = set(row["required_diagnostic_codes"])

        assert row["capability_id"] == expected["capability_id"]
        assert row["owner_phase"] == expected["owner_phase"]
        assert row["runnable_command"] == manifest_claim["executable_command"]
        assert manifest_claim["behavior_fixture"] in positive_evidence
        assert expected["required_positive"] <= positive_evidence
        assert expected["required_negative"] <= negative_evidence
        assert expected["required_codes"] <= required_codes
        assert all(code.startswith("O3") for code in row["required_diagnostic_codes"])
        assert any("narrow" in requirement for requirement in row["source_truth_requirements"])

        _assert_repo_paths_exist(
            {
                row["conformance_fixture"],
                row["traceability_fixture"],
                *positive_evidence,
                *negative_evidence,
            }
        )
