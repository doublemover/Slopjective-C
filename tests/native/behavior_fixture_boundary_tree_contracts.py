from behavior_fixture_boundary_support import (
    EXPECTED_BOUNDARY_BY_KIND,
    HARD_CUTOVER_CONTRACTS,
    NATIVE_ROOT,
    PHASE_ORDER,
    PHASE_OWNER_CONTRACTS,
    REQUIRED_TREE,
    ROOT,
    STRICT_KINDS,
    load_behavior_fixture_catalog,
    load_json,
    load_manifest_fixture_entries,
)


def test_hard_cutover_contract_fixtures_are_canonicalized() -> None:
    catalog_by_path = load_behavior_fixture_catalog().by_relative_source()
    manifest_by_path = {
        entry["path"]: entry
        for entry in load_manifest_fixture_entries(ROOT / "tests" / "fixtures" / "canonical" / "manifest.json")
    }

    for relative_source, (fixture_kind, diagnostic_code) in HARD_CUTOVER_CONTRACTS.items():
        fixture = catalog_by_path[relative_source]
        manifest_entry = manifest_by_path[relative_source]
        boundary = fixture.metadata["boundary"]

        assert fixture.fixture_kind == fixture_kind
        assert manifest_entry["fixture_kind"] == fixture_kind
        assert fixture.expected_diagnostic_code == diagnostic_code
        assert manifest_entry["expected_diagnostic_code"] == diagnostic_code
        assert boundary["canonical_behavior_source"] is True
        assert boundary["behavior_contract"] == EXPECTED_BOUNDARY_BY_KIND[fixture_kind]


def test_hard_cutover_catalog_links_live_native_behavior_fixtures() -> None:
    hard_cutover_catalog = load_json(ROOT / "tests" / "conformance" / "hard_cutover_catalog.json")
    behavior_paths = set(load_behavior_fixture_catalog().by_relative_source())
    canonical_manifest_path = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"

    assert hard_cutover_catalog["policy"]["native_behavior_manifest"] == (
        canonical_manifest_path.relative_to(ROOT).as_posix()
    )

    for boundary in hard_cutover_catalog["boundaries"]:
        manifest_path = ROOT / boundary["manifest"]
        owned_root = ROOT / boundary["owned_fixture_root"]
        assert manifest_path.exists(), boundary["manifest"]
        assert owned_root.exists(), boundary["owned_fixture_root"]
        for fixture_path in boundary.get("native_behavior_fixtures", []):
            assert fixture_path in behavior_paths


def test_phase_owner_contracts_match_required_fixture_topology() -> None:
    phase_contracts = load_json(PHASE_OWNER_CONTRACTS)
    behavior_by_path = load_behavior_fixture_catalog().by_relative_source()
    generated_paths = {
        entry["path"]
        for entry in load_manifest_fixture_entries(ROOT / "tests" / "fixtures" / "generated" / "manifest.json")
    }
    support_claims = {
        claim["owner_phase"]: claim
        for claim in load_json(ROOT / "tests" / "fixtures" / "canonical" / "manifest.json")["support_claims"]
    }

    assert phase_contracts["phase_order"] == list(PHASE_ORDER)
    assert [entry["phase"] for entry in phase_contracts["phase_contracts"]] == list(PHASE_ORDER)
    assert phase_contracts["generated_fixture_boundary"]["phase_support_claim_authority"] is False

    for contract in phase_contracts["phase_contracts"]:
        phase = contract["phase"]
        fixture_root = ROOT / contract["fixture_root"]
        support_claim = support_claims[phase]

        assert fixture_root == NATIVE_ROOT / phase
        assert contract["required_families"] == list(REQUIRED_TREE[phase])
        assert contract["support_claim"] == support_claim["claim_id"]
        assert support_claim["behavior_fixture"] in contract["canonical_positive_evidence"] + contract[
            "retired_surface_evidence"
        ]
        assert support_claim["executable_command"] == "npm run objc3c -- test-behavior-matrix"
        assert contract["generated_fixture_authority"] is False
        assert set(contract["canonical_positive_evidence"]).isdisjoint(generated_paths)
        assert set(contract["retired_surface_evidence"]).isdisjoint(generated_paths)

        for relative_path in contract["canonical_positive_evidence"]:
            fixture = behavior_by_path[relative_path]
            assert fixture.owner_phase == phase
            assert fixture.fixture_kind == "positive"
            assert not fixture.retired_surface_tags

        for relative_path in contract["retired_surface_evidence"]:
            fixture = behavior_by_path[relative_path]
            assert fixture.owner_phase == phase
            assert fixture.fixture_kind in STRICT_KINDS
            assert fixture.expected_diagnostic_code
