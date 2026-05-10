from behavior_fixture_boundary_support import (
    NATIVE_ROOT,
    RETIRED_SURFACE_CONTRACT_INDEX,
    RETIRED_SURFACE_TAGS,
    ROOT,
    STRICT_KINDS,
    STRICT_REJECTION_NAME_SUFFIXES,
    load_behavior_fixture_catalog,
    load_json,
)


def test_old_mode_and_runtime_strict_error_cases_are_not_positive_canonical_fixtures() -> None:
    canonical_by_path = {
        entry["path"]: entry
        for entry in load_json(ROOT / "tests" / "fixtures" / "canonical" / "manifest.json")["fixtures"]
    }
    retired_surface_fixtures = load_behavior_fixture_catalog().retired_surface_fixtures()
    assert retired_surface_fixtures

    for fixture in retired_surface_fixtures:
        entry = canonical_by_path[fixture.relative_source]
        manifest_tags = set(entry.get("retired_surface_tags", ()))
        metadata_tags = set(fixture.retired_surface_tags)
        assert manifest_tags <= RETIRED_SURFACE_TAGS
        assert metadata_tags <= RETIRED_SURFACE_TAGS
        assert manifest_tags == metadata_tags
        assert entry["fixture_kind"] in STRICT_KINDS
        assert fixture.fixture_kind in STRICT_KINDS
        assert entry["expected_diagnostic_code"]
        assert entry["expected_diagnostic_code"] == fixture.expected_diagnostic_code


def test_retired_surface_matrix_entries_are_strict_native_fixtures() -> None:
    matrix = load_json(NATIVE_ROOT / "retired_surface_matrix.json")
    behavior_by_path = load_behavior_fixture_catalog().by_relative_source()
    seen_surfaces: set[str] = set()

    assert matrix["schema_version"] == 1
    assert matrix["source_of_truth"] == "tests/native"
    for token in (
        "old-mode",
        "gate",
        "retired-route",
        "compatibility",
        "migration-lane",
        "unsupported-feature",
        "runtime-dispatch",
    ):
        assert token in matrix["policy"]
    for entry in matrix["entries"]:
        surface = entry["surface"]
        fixture = behavior_by_path[entry["fixture_path"]]
        seen_surfaces.add(surface)

        assert fixture.fixture_kind in STRICT_KINDS
        assert entry["retired_tag"] in fixture.retired_surface_tags
        assert entry["retired_tag"] in RETIRED_SURFACE_TAGS
        assert fixture.expected_stage == entry["expected_stage"]
        assert fixture.expected_diagnostic_code == entry["diagnostic_code"]

    assert seen_surfaces == {
        "legacy-literal-aliases",
        "removed-retired-mode-flag",
        "removed-parser-retired-route-flag",
        "removed-compatibility-gate",
        "removed-runtime-dispatch-retired-route-flag",
        "non-nil-runtime-dispatch-linkage",
        "unknown-receiver-runtime-dispatch",
        "negative-execution-runtime-dispatch",
    }


def test_retired_surface_contract_index_registers_outcomes() -> None:
    contract_index = load_json(RETIRED_SURFACE_CONTRACT_INDEX)
    outcome_index = load_json(
        ROOT / "tests" / "conformance" / "hard_cutover_behavior_outcome_owner_index.json"
    )
    diagnostic_index = load_json(
        ROOT / "tests" / "conformance" / "hard_cutover_diagnostic_outcome_code_index.json"
    )
    outcomes = {entry["outcome"] for entry in outcome_index["outcomes"]}
    diagnostic_codes = {entry["code"] for entry in diagnostic_index["codes"]}

    assert contract_index["catalog"] == "objc3-hard-cutover-retired-surface-fixture-contracts"
    assert contract_index["policy"]["positive_expectation_rule"].endswith(
        "are never positive expectations"
    )

    for family in contract_index["surface_families"]:
        assert family["behavior_outcome"] in outcomes
        assert family["diagnostic_owner"] in diagnostic_codes
        assert family["canonical_disposition"] in {
            "canonical-rejection",
            "canonical-strict-error",
        }
        assert family["retired_tags"]


def test_retired_surface_contract_index_fixture_sidecars_are_strict() -> None:
    contract_index = load_json(RETIRED_SURFACE_CONTRACT_INDEX)
    behavior_by_path = load_behavior_fixture_catalog().by_relative_source()

    for family in contract_index["surface_families"]:
        for entry in family["fixtures"]:
            fixture_path = ROOT / entry["path"]
            sidecar_path = ROOT / entry["sidecar"]
            metadata = load_json(sidecar_path)
            fixture = behavior_by_path[entry["path"]]

            assert entry["positive_expectation"] is False
            assert fixture_path.is_file(), entry["path"]
            assert sidecar_path == fixture_path.with_name(f"{fixture_path.stem}.meta.json")
            assert metadata["fixture"] == fixture_path.name
            assert fixture.fixture_kind == entry["fixture_kind"]
            assert metadata["fixture_kind"] == entry["fixture_kind"]
            assert metadata["boundary"]["behavior_contract"] == family["canonical_disposition"]
            assert metadata["boundary"]["retired_positive_surface"] is True
            assert set(family["retired_tags"]).issubset(metadata["retired_surface_tags"])
            assert metadata["expected"]["stage"] == entry["expected_stage"]
            assert metadata["expected"]["diagnostic_code"] == family["diagnostic_owner"]

            if entry["strict_rejection_name"]:
                assert fixture_path.name.endswith(STRICT_REJECTION_NAME_SUFFIXES)


def test_retired_surface_contract_index_absent_support_surfaces_are_closed() -> None:
    contract_index = load_json(RETIRED_SURFACE_CONTRACT_INDEX)
    outcome_index = load_json(
        ROOT / "tests" / "conformance" / "hard_cutover_behavior_outcome_owner_index.json"
    )
    diagnostic_index = load_json(
        ROOT / "tests" / "conformance" / "hard_cutover_diagnostic_outcome_code_index.json"
    )
    outcomes = {entry["outcome"] for entry in outcome_index["outcomes"]}
    diagnostic_codes = {entry["code"] for entry in diagnostic_index["codes"]}

    for absence in contract_index["absent_support_surfaces"]:
        assert absence["positive_expectation"] is False
        assert absence["behavior_outcome"] in outcomes
        assert absence["diagnostic_owner"] in diagnostic_codes
        assert absence["canonical_disposition"] == "absent-support"


def test_retired_surface_contract_index_covers_retired_matrix_paths() -> None:
    contract_index = load_json(RETIRED_SURFACE_CONTRACT_INDEX)
    retired_matrix = load_json(NATIVE_ROOT / "retired_surface_matrix.json")
    matrix_paths = {entry["fixture_path"] for entry in retired_matrix["entries"]}
    indexed_paths = {
        entry["path"]
        for family in contract_index["surface_families"]
        for entry in family["fixtures"]
    }

    assert matrix_paths <= indexed_paths


def test_legacy_runtime_dispatch_execution_residues_are_negative() -> None:
    negative_root = ROOT / "tests" / "tooling" / "fixtures" / "native" / "execution" / "negative"
    strict_runtime_cases = (
        "message_send_runtime_dispatch_strict_error.objc3",
        "message_send_six_args_custom_cap.objc3",
    )

    for fixture_name in strict_runtime_cases:
        source_path = negative_root / fixture_name
        first_line = source_path.read_text(encoding="utf-8").splitlines()[0]
        meta = load_json(source_path.with_name(f"{source_path.stem}.meta.json"))
        tokens = meta["expect_failure"]["required_diagnostic_tokens"]

        assert first_line.startswith("// Negative execution fixture:")
        assert "Positive execution fixture" not in first_line
        assert meta["expect_failure"]["stage"] == "run"
        assert "O3RT002" in tokens


def test_legacy_migration_pair_no_longer_lives_as_tooling_root_residue() -> None:
    tooling_root = ROOT / "tests" / "tooling" / "fixtures" / "native"
    retired_paths = (
        tooling_root / "legacy_canonical_migration_positive.objc3",
        tooling_root / "legacy_canonical_migration_negative.objc3",
    )

    for path in retired_paths:
        assert not path.exists(), f"retired old-mode fixture must live in tests/native: {path.relative_to(ROOT)}"


def test_tooling_positive_fixture_names_do_not_claim_retired_surfaces() -> None:
    positive_roots = (
        ROOT / "tests" / "tooling" / "fixtures" / "native",
        ROOT / "tests" / "tooling" / "fixtures" / "native" / "execution" / "positive",
        ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "positive",
    )
    retired_name_tokens = (
        "legacy",
        "old_mode",
        "old-mode",
        "gate",
        "compat",
        "compatibility",
        "migration",
    )

    for root in positive_roots:
        for path in root.rglob("*"):
            if not path.is_file():
                continue
            if "_positive" not in path.stem.lower():
                continue
            normalized_name = path.name.lower()
            for token in retired_name_tokens:
                assert token not in normalized_name, (
                    f"retired positive fixture name must be moved to rejection coverage: "
                    f"{path.relative_to(ROOT)}"
                )


def test_legacy_literal_aliases_are_rejection_coverage_not_positive_recovery() -> None:
    recovery_positive = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "positive"
    execution_positive = ROOT / "tests" / "tooling" / "fixtures" / "native" / "execution" / "positive"

    assert not (recovery_positive / "objc_literal_aliases_globals.objc3").exists()
    assert not (execution_positive / "objc_literal_aliases_globals.objc3").exists()
    assert not (execution_positive / "objc_literal_aliases_globals.exitcode.txt").exists()
    assert (recovery_positive / "canonical_literal_globals.objc3").is_file()

    rejection_fixtures = (
        NATIVE_ROOT / "parser" / "negative" / "legacy_boolean_and_null_aliases_rejected.objc3",
        NATIVE_ROOT / "parser" / "negative" / "legacy_null_literal_alias_rejected.objc3",
        NATIVE_ROOT / "e2e" / "negative_execution" / "legacy_null_literal_alias_rejected.objc3",
    )
    for fixture_path in rejection_fixtures:
        meta = load_json(fixture_path.with_name(f"{fixture_path.stem}.meta.json"))
        assert meta["fixture_kind"] == "rejection"
        assert meta["expected"]["stage"] == "compile"
        assert meta["expected"]["diagnostic_code"] == "O3C002"
