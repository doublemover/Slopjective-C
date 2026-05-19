from behavior_fixture_boundary_support import (
    EXPECTED_FIXTURE_FAMILY_INDEX,
    NATIVE_ROOT,
    PHASE_OWNER_CONTRACTS,
    REQUIRED_TREE,
    ROOT,
    STRICT_KINDS,
    load_behavior_fixture_catalog,
    load_json,
)


def test_fixture_family_owner_index_keeps_phase_boundaries_hard_cutover() -> None:
    family_index = load_json(
        ROOT / "tests" / "conformance" / "hard_cutover_fixture_family_owner_index.json"
    )
    behavior_by_path = load_behavior_fixture_catalog().by_relative_source()
    families = {entry["family"]: entry for entry in family_index["families"]}

    assert family_index["policy"]["canonical_positive_rule"].endswith(
        "may be positive behavior evidence"
    )
    assert family_index["policy"]["generated_fixture_rule"].endswith(
        "never define positive native behavior"
    )
    assert set(EXPECTED_FIXTURE_FAMILY_INDEX).issubset(families)

    for family_name, (kind, acceptance_area) in EXPECTED_FIXTURE_FAMILY_INDEX.items():
        family = families[family_name]
        assert family["kind"] == kind
        assert family["acceptance_area"] == acceptance_area
        assert family["owning_issues"]
        assert family["retired_disposition"]

    for family_name in (
        "parser_behavior",
        "sema_behavior",
        "lowering_behavior",
        "ir_behavior",
        "runtime_behavior",
        "e2e_behavior",
    ):
        family = families[family_name]
        for relative_path in family.get("positive_evidence", []):
            fixture = behavior_by_path[relative_path]
            assert fixture.fixture_kind == "positive"
            assert not fixture.retired_surface_tags
        for relative_path in family.get("rejection_evidence", []):
            fixture = behavior_by_path[relative_path]
            assert fixture.fixture_kind in STRICT_KINDS

    generated_family = families["generated_boundary"]
    assert generated_family["positive_evidence"] == []
    for relative_path in generated_family["provenance_evidence"]:
        path = ROOT / relative_path
        assert path.is_file(), relative_path
        assert not path.is_relative_to(NATIVE_ROOT)


def test_behavior_outcome_index_partitions_positive_rejection_and_provenance() -> None:
    outcome_index = load_json(
        ROOT / "tests" / "conformance" / "hard_cutover_behavior_outcome_owner_index.json"
    )
    behavior_by_path = load_behavior_fixture_catalog().by_relative_source()
    outcomes = {entry["outcome"]: entry for entry in outcome_index["outcomes"]}

    assert outcomes["canonical_positive_behavior"]["kind"] == "positive"
    for relative_path in outcomes["canonical_positive_behavior"]["evidence"]:
        fixture = behavior_by_path[relative_path]
        assert fixture.fixture_kind == "positive"
        assert not fixture.retired_surface_tags

    for outcome_name in (
        "parser_rejection",
        "semantic_rejection",
        "lowering_or_link_strict_error",
        "runtime_strict_error",
        "negative_execution",
    ):
        outcome = outcomes[outcome_name]
        assert outcome["kind"] != "positive"
        assert outcome["diagnostic_codes"]
        for relative_path in outcome["evidence"]:
            fixture = behavior_by_path[relative_path]
            assert fixture.fixture_kind in STRICT_KINDS

    generated = outcomes["generated_provenance_only"]
    assert generated["kind"] == "provenance-only"
    for relative_path in generated["evidence"]:
        path = ROOT / relative_path
        assert path.is_file(), relative_path
        assert not path.is_relative_to(NATIVE_ROOT)


def test_phase_owner_contracts_keep_retired_surfaces_out_of_positive_support() -> None:
    contracts = load_json(PHASE_OWNER_CONTRACTS)
    behavior_by_path = load_behavior_fixture_catalog().by_relative_source()
    generated_paths = {
        entry["path"]
        for entry in load_json(ROOT / "tests" / "fixtures" / "generated" / "manifest.json")["fixtures"]
    }

    assert contracts["policy"]["generated_boundary_rule"].endswith(
        "cannot satisfy phase support claims"
    )
    assert contracts["generated_fixture_boundary"]["positive_support"] is False
    assert contracts["generated_fixture_boundary"]["phase_support_claim_authority"] is False
    assert contracts["generated_fixture_boundary"]["boundary"] == "generated-provenance-only"

    positive_contract_paths: set[str] = set()
    retired_contract_paths: set[str] = set()
    for contract in contracts["phase_contracts"]:
        assert contract["generated_fixture_authority"] is False
        positive_contract_paths.update(contract["canonical_positive_evidence"])
        retired_contract_paths.update(contract["retired_surface_evidence"])

    assert positive_contract_paths.isdisjoint(generated_paths)
    assert retired_contract_paths.isdisjoint(generated_paths)

    for relative_path in positive_contract_paths:
        fixture = behavior_by_path[relative_path]
        assert fixture.fixture_kind == "positive"
        assert not fixture.retired_surface_tags

    for relative_path in retired_contract_paths:
        fixture = behavior_by_path[relative_path]
        assert fixture.fixture_kind in STRICT_KINDS
        assert fixture.expected_diagnostic_code

    for absence in contracts["absent_support_contracts"]:
        assert absence["disposition"] == "absent-support"
        assert absence["positive_support"] is False


def test_support_claims_link_to_executable_behavior_fixtures() -> None:
    canonical_manifest = load_json(ROOT / "tests" / "fixtures" / "canonical" / "manifest.json")
    behavior_paths = set(load_behavior_fixture_catalog().by_relative_source())
    claims = canonical_manifest["support_claims"]

    assert {claim["owner_phase"] for claim in claims} == set(REQUIRED_TREE)
    for claim in claims:
        assert claim["behavior_fixture"] in behavior_paths
        assert claim["executable_command"] == "npm run objc3c -- test-behavior-matrix"
