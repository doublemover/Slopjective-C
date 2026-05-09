from behavior_fixture_boundary_support import (
    EXPECTED_FIXTURE_FAMILY_INDEX,
    NATIVE_ROOT,
    PHASE_OWNER_CONTRACTS,
    POSITIVE_RESIDUE_AUDIT,
    REQUIRED_TREE,
    RETIRED_SURFACE_CONTRACT_INDEX,
    RETIRED_SURFACE_TAGS,
    ROOT,
    STRICT_KINDS,
    STRICT_REJECTION_NAME_SUFFIXES,
    load_behavior_fixture_catalog,
    load_json,
)


def test_positive_residue_audit_policy_is_hard_cutover_only() -> None:
    audit = load_json(POSITIVE_RESIDUE_AUDIT)

    assert audit["result"].startswith("no remaining old-mode")
    assert audit["validation"] == "not run"
    assert set(audit["conversion_policy"]) == {
        "true_retired_positive",
        "lexical_false_positive",
        "negative_fixture",
    }


def test_positive_residue_confirmed_rejections_are_strict() -> None:
    audit = load_json(POSITIVE_RESIDUE_AUDIT)
    behavior_by_path = load_behavior_fixture_catalog().by_relative_source()

    for relative_path in audit["confirmed_rejections"]:
        path = ROOT / relative_path
        assert path.is_file(), relative_path
        if relative_path in behavior_by_path:
            fixture = behavior_by_path[relative_path]
            assert fixture.fixture_kind in STRICT_KINDS
        else:
            assert any(marker in path.stem.lower() for marker in ("negative", "rejected"))


def test_positive_residue_absent_retired_positive_paths_remain_absent() -> None:
    audit = load_json(POSITIVE_RESIDUE_AUDIT)

    for relative_path in audit["absent_retired_positive_paths"]:
        assert not (ROOT / relative_path).exists(), relative_path


def test_positive_residue_documents_lexical_false_positive_surfaces() -> None:
    audit = load_json(POSITIVE_RESIDUE_AUDIT)

    for hit in audit["documented_lexical_positive_hits"]:
        path = ROOT / hit["path"]
        assert path.is_file(), hit["path"]
        text = path.read_text(encoding="utf-8")
        assert hit["token"] in text
        assert hit["classification"]
        assert "not " in hit["disposition"]


def test_positive_fixture_lexical_residue_hits_are_documented() -> None:
    audit = load_json(POSITIVE_RESIDUE_AUDIT)
    documented_paths = {hit["path"] for hit in audit["documented_lexical_positive_hits"]}
    tooling_root = ROOT / "tests" / "tooling" / "fixtures" / "native"
    positive_fixture_paths = {
        *tooling_root.glob("*positive*.objc3"),
        *(tooling_root / "execution" / "positive").glob("*.objc3"),
        *(tooling_root / "recovery" / "positive").glob("*.objc3"),
    }
    retired_terms = (
        "old-mode",
        "old_mode",
        "shim",
        "fallback",
        "compatibility",
        "migration",
        "legacy",
    )
    lexical_hit_paths = {
        path.relative_to(ROOT).as_posix()
        for path in positive_fixture_paths
        if any(term in path.read_text(encoding="utf-8").lower() for term in retired_terms)
    }

    assert lexical_hit_paths <= documented_paths


def test_positive_residue_outcome_index_uses_documented_false_positive_paths() -> None:
    audit = load_json(POSITIVE_RESIDUE_AUDIT)
    outcome_index = load_json(
        ROOT / "tests" / "conformance" / "hard_cutover_behavior_outcome_owner_index.json"
    )
    documented_paths = {hit["path"] for hit in audit["documented_lexical_positive_hits"]}

    residue_outcome = next(
        entry
        for entry in outcome_index["outcomes"]
        if entry["outcome"] == "positive_residue_false_positive"
    )
    assert set(residue_outcome["evidence"]) == documented_paths


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
        "shim",
        "fallback",
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
        "removed-compatibility-mode-flag",
        "removed-parser-fallback-flag",
        "removed-compatibility-shim-gate",
        "removed-runtime-dispatch-fallback-flag",
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
        "shim",
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


def test_support_claims_link_to_executable_behavior_fixtures() -> None:
    canonical_manifest = load_json(ROOT / "tests" / "fixtures" / "canonical" / "manifest.json")
    behavior_paths = set(load_behavior_fixture_catalog().by_relative_source())
    claims = canonical_manifest["support_claims"]

    assert {claim["owner_phase"] for claim in claims} == set(REQUIRED_TREE)
    for claim in claims:
        assert claim["behavior_fixture"] in behavior_paths
        assert claim["executable_command"] == "npm run objc3c -- test-behavior-matrix"
