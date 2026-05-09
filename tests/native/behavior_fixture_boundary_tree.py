from behavior_fixture_boundary_support import (
    EXPECTED_BOUNDARY_BY_KIND,
    HARD_CUTOVER_CONTRACTS,
    NATIVE_ROOT,
    PHASE_ORDER,
    PHASE_OWNER_CONTRACTS,
    REQUIRED_TREE,
    RETIRED_POSITIVE_SURFACE_TERMS,
    ROOT,
    STRICT_KINDS,
    load_behavior_fixture_catalog,
    load_json,
    load_manifest_fixture_entries,
)


def test_required_behavior_tree_boundaries_exist() -> None:
    assert tuple(REQUIRED_TREE) == PHASE_ORDER

    for phase, families in REQUIRED_TREE.items():
        phase_root = NATIVE_ROOT / phase
        assert phase_root.is_dir(), f"missing native phase root: {phase_root.relative_to(ROOT)}"
        for family in families:
            family_root = phase_root / family
            assert family_root.is_dir(), f"missing native behavior family: {family_root.relative_to(ROOT)}"

    assert (ROOT / "tests" / "fixtures" / "canonical").is_dir()
    assert (ROOT / "tests" / "fixtures" / "generated").is_dir()


def test_native_fixture_metadata_records_phase_and_diagnostics() -> None:
    fixtures = load_behavior_fixture_catalog().fixtures
    assert fixtures, "native behavior fixtures must carry metadata"

    for fixture in fixtures:
        metadata = fixture.metadata
        fixture_path = fixture.source_path
        assert metadata["schema_version"] == 1
        assert metadata["fixture"] == fixture_path.name
        assert metadata["origin"] == "hand-authored"

        owner_phase = metadata["owner_phase"]
        behavior_family = metadata["behavior_family"]
        assert owner_phase in REQUIRED_TREE
        assert behavior_family in REQUIRED_TREE[owner_phase]
        assert fixture.phase_family == (owner_phase, behavior_family)

        relative_parts = fixture_path.relative_to(NATIVE_ROOT).parts
        assert relative_parts[0] == owner_phase
        assert relative_parts[1] == behavior_family

        fixture_kind = metadata["fixture_kind"]
        boundary = metadata["boundary"]
        assert boundary["canonical_behavior_source"] is True
        assert boundary["behavior_contract"] == EXPECTED_BOUNDARY_BY_KIND[fixture_kind]
        assert boundary["retired_positive_surface"] == bool(fixture.retired_surface_tags)

        if fixture_kind in STRICT_KINDS:
            expected = metadata["expected"]
            assert expected["stage"] in {"parse", "compile", "link", "run"}
            assert expected["diagnostic_code"]
            assert expected["required_tokens"]
        else:
            source_text = fixture_path.read_text(encoding="utf-8").lower()
            assert not fixture.retired_surface_tags
            for term in RETIRED_POSITIVE_SURFACE_TERMS:
                assert term not in source_text


def test_behavior_matrix_has_representative_phase_coverage() -> None:
    catalog = load_behavior_fixture_catalog()

    assert catalog.covered_phases == set(PHASE_ORDER)
    for phase in PHASE_ORDER:
        assert catalog.phase(phase), f"missing representative behavior fixtures for {phase}"


def test_behavior_matrix_has_representative_family_coverage() -> None:
    catalog = load_behavior_fixture_catalog()

    for phase, families in REQUIRED_TREE.items():
        for family in families:
            if (phase, family) == ("parser", "snapshots"):
                snapshots = tuple((NATIVE_ROOT / phase / family).glob("*.diagnostics.txt"))
                assert snapshots, "parser snapshots must remain diagnostics-only evidence"
                continue
            assert catalog.family(phase, family), f"missing fixture coverage for {phase}/{family}"


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
