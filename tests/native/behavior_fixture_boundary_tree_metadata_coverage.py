from behavior_fixture_boundary_support import (
    EXPECTED_BOUNDARY_BY_KIND,
    NATIVE_ROOT,
    PHASE_ORDER,
    REQUIRED_TREE,
    RETIRED_POSITIVE_SURFACE_TERMS,
    STRICT_KINDS,
    load_behavior_fixture_catalog,
)


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
