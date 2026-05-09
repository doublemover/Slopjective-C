import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_ROOT = ROOT / "scripts"
if str(SCRIPT_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPT_ROOT))

from objc3c_tooling.behavior_fixtures import (
    PHASE_ORDER,
    FIXTURE_ROOT,
    NATIVE_ROOT,
    REQUIRED_TREE,
    RETIRED_SURFACE_TAGS,
    BehaviorFixture,
    STRICT_KINDS,
    load_behavior_fixture_catalog,
    load_manifest_fixture_entries,
)

NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
EXPECTED_BOUNDARY_BY_KIND = {
    "positive": "canonical-positive",
    "negative": "canonical-rejection",
    "rejection": "canonical-rejection",
    "strict-error": "canonical-strict-error",
}
RETIRED_POSITIVE_SURFACE_TERMS = (
    "old-mode",
    "shim",
    "fallback",
    "compat",
    "migration-lane",
)
RETIRED_SURFACE_CONTRACT_INDEX = (
    ROOT / "tests" / "conformance" / "hard_cutover_retired_surface_fixture_contracts.json"
)
POSITIVE_RESIDUE_AUDIT = (
    ROOT / "tests" / "conformance" / "hard_cutover_positive_residue_audit.json"
)
FIXTURE_BOUNDARY_CONTRACTS = (
    ROOT / "tests" / "conformance" / "hard_cutover_fixture_boundary_contracts.json"
)
STRICT_REJECTION_NAME_SUFFIXES = (
    "_rejected.objc3",
    "_strict_error.objc3",
    "_contract.objc3",
)
EXPECTED_FIXTURE_FAMILY_INDEX = {
    "canonical_native_manifest": ("behavior_manifest", "canonical_behavior_fixtures"),
    "retired_surface_matrix": ("retired_surface_index", "canonical_behavior_fixtures"),
    "parser_behavior": ("phase_fixture_family", "parser_lexer_ast"),
    "sema_behavior": ("phase_fixture_family", "semantic_diagnostics"),
    "lowering_behavior": ("phase_fixture_family", "lowering_and_ir"),
    "ir_behavior": ("phase_fixture_family", "lowering_and_ir"),
    "runtime_behavior": ("phase_fixture_family", "runtime_dispatch_registration"),
    "e2e_behavior": ("phase_fixture_family", "canonical_behavior_fixtures"),
    "generated_boundary": ("provenance_only_fixture_family", "pipeline_artifacts_config_json"),
    "tooling_native_execution_metadata": ("metadata_fixture_family", "runtime_dispatch_registration"),
}
HARD_CUTOVER_CONTRACTS = {
    "tests/native/parser/negative/legacy_yes_literal_alias_rejected.objc3": (
        "rejection",
        "O3C002",
    ),
    "tests/native/parser/negative/legacy_no_literal_alias_rejected.objc3": (
        "rejection",
        "O3C002",
    ),
    "tests/native/parser/negative/removed_compatibility_mode_flag_rejected.objc3": (
        "rejection",
        "OBJC3-E-REMOVED-COMPATIBILITY-MODE",
    ),
    "tests/native/parser/negative/removed_parser_fallback_flag_rejected.objc3": (
        "rejection",
        "OBJC3-E-REMOVED-FALLBACK-FLAG",
    ),
    "tests/native/sema/errors/removed_compatibility_shim_gate_rejected.objc3": (
        "negative",
        "OBJC3-E-REMOVED-COMPATIBILITY-SHIM",
    ),
    "tests/native/sema/errors/unsupported_arc_ownership_qualifier_rejected.objc3": (
        "negative",
        "O3S221",
    ),
    "tests/native/sema/concurrency/throws_feature_claim_rejected.objc3": (
        "negative",
        "O3S221",
    ),
    "tests/native/lowering/objc_runtime/numeric_zero_receiver_requires_runtime_dispatch.objc3": (
        "strict-error",
        "link.unresolved_symbol",
    ),
    "tests/native/lowering/errors/removed_runtime_dispatch_fallback_flag_rejected.objc3": (
        "strict-error",
        "OBJC3-E-REMOVED-RUNTIME-FALLBACK",
    ),
    "tests/native/ir/runtime_calls/non_nil_receiver_runtime_call_contract.objc3": (
        "strict-error",
        "link.unresolved_symbol",
    ),
    "tests/native/runtime/dispatch/nonzero_constant_receiver_dispatch_strict_error.objc3": (
        "strict-error",
        "O3RT002",
    ),
    "tests/native/e2e/negative_execution/runtime_dispatch_unknown_receiver_strict_error.objc3": (
        "strict-error",
        "O3RT002",
    ),
}


def _load_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def _canonical_manifest_entries() -> tuple[dict, list[dict]]:
    manifest = _load_json(FIXTURE_ROOT / "canonical" / "manifest.json")
    entries = load_manifest_fixture_entries(FIXTURE_ROOT / "canonical" / "manifest.json")
    return manifest, entries


def _generated_manifest_entries() -> tuple[dict, list[dict]]:
    manifest = _load_json(FIXTURE_ROOT / "generated" / "manifest.json")
    entries = load_manifest_fixture_entries(FIXTURE_ROOT / "generated" / "manifest.json")
    return manifest, entries


def test_required_behavior_tree_boundaries_exist() -> None:
    assert tuple(REQUIRED_TREE) == PHASE_ORDER

    for phase, families in REQUIRED_TREE.items():
        phase_root = NATIVE_ROOT / phase
        assert phase_root.is_dir(), f"missing native phase root: {phase_root.relative_to(ROOT)}"
        for family in families:
            family_root = phase_root / family
            assert family_root.is_dir(), f"missing native behavior family: {family_root.relative_to(ROOT)}"

    assert (FIXTURE_ROOT / "canonical").is_dir()
    assert (FIXTURE_ROOT / "generated").is_dir()


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
        for entry in load_manifest_fixture_entries(FIXTURE_ROOT / "canonical" / "manifest.json")
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
    hard_cutover_catalog = _load_json(ROOT / "tests" / "conformance" / "hard_cutover_catalog.json")
    behavior_paths = set(load_behavior_fixture_catalog().by_relative_source())
    canonical_manifest_path = FIXTURE_ROOT / "canonical" / "manifest.json"

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


def test_canonical_fixture_manifest_matches_native_behavior_catalog() -> None:
    canonical_manifest, canonical_entries = _canonical_manifest_entries()
    canonical_boundary = canonical_manifest["boundary"]
    canonical_by_path = {entry["path"]: entry for entry in canonical_entries}

    assert canonical_by_path
    behavior_by_path = load_behavior_fixture_catalog().by_relative_source()
    assert set(behavior_by_path) == set(canonical_by_path)

    for relative_source, fixture in behavior_by_path.items():
        entry = canonical_by_path[relative_source]
        path = ROOT / relative_source
        assert path.exists(), entry["path"]
        assert path.is_relative_to(NATIVE_ROOT)
        assert entry == fixture.canonical_manifest_entry()

    assert canonical_manifest["fixtures"] == canonical_entries
    assert canonical_boundary["kind"] == "hand-authored-native-behavior"
    assert canonical_boundary["source_of_truth"] == "tests/native"
    assert canonical_boundary["phase_order"] == list(PHASE_ORDER)
    assert canonical_boundary["positive_fixture_policy"] == (
        "positive fixtures cover canonical behavior only"
    )
    assert canonical_boundary["retired_surface_policy"] == (
        "old-mode, shim, fallback, compatibility, migration-lane, unsupported feature, and runtime-dispatch residues must be rejection, strict-error, or absent-support metadata"
    )
    assert canonical_boundary["boundary_contract_index"] == (
        "tests/conformance/hard_cutover_fixture_boundary_contracts.json"
    )


def test_canonical_and_generated_fixture_paths_are_disjoint() -> None:
    _, canonical_entries = _canonical_manifest_entries()
    _, generated_entries = _generated_manifest_entries()
    canonical_paths = {entry["path"] for entry in canonical_entries}
    generated_paths = {entry["path"] for entry in generated_entries}

    assert canonical_paths
    assert generated_paths
    assert canonical_paths.isdisjoint(generated_paths)


def test_generated_fixture_manifest_is_provenance_only() -> None:
    generated_manifest, generated_entries = _generated_manifest_entries()
    generated_boundary = generated_manifest["boundary"]

    assert generated_manifest["fixtures"] == generated_entries
    assert generated_boundary["kind"] == "generated-contract-artifacts"
    assert generated_boundary["source_of_truth"] == "generator-output"
    assert generated_boundary["canonical_behavior_source"] is False
    assert generated_boundary["allowed_path_roots"] == ["tests/tooling/fixtures/objc3c"]
    assert "not canonical behavior expectations" in generated_boundary["hand_edit_policy"]
    assert generated_boundary["positive_fixture_policy"] == (
        "generated artifacts never define positive native behavior"
    )
    assert generated_boundary["boundary_contract_index"] == (
        "tests/conformance/hard_cutover_fixture_boundary_contracts.json"
    )
    generated_allowed_roots = tuple(ROOT / root for root in generated_boundary["allowed_path_roots"])

    for entry in generated_entries:
        path = ROOT / entry["path"]
        assert path.exists(), entry["path"]
        assert entry["origin"] == "generated"
        assert entry["generator"]
        assert entry["provenance"]
        assert entry["boundary"] == "generated-contract-artifact"
        assert entry["behavior_boundary"] == "generated-provenance-only"
        assert entry["canonical_behavior_source"] is False
        assert not path.is_relative_to(NATIVE_ROOT)
        assert any(path.is_relative_to(root) for root in generated_allowed_roots)


def test_positive_residue_audit_policy_is_hard_cutover_only() -> None:
    audit = _load_json(POSITIVE_RESIDUE_AUDIT)

    assert audit["result"].startswith("no remaining old-mode")
    assert audit["validation"] == "not run"
    assert set(audit["conversion_policy"]) == {
        "true_retired_positive",
        "lexical_false_positive",
        "negative_fixture",
    }


def test_positive_residue_confirmed_rejections_are_strict() -> None:
    audit = _load_json(POSITIVE_RESIDUE_AUDIT)
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
    audit = _load_json(POSITIVE_RESIDUE_AUDIT)

    for relative_path in audit["absent_retired_positive_paths"]:
        assert not (ROOT / relative_path).exists(), relative_path


def test_positive_residue_documents_lexical_false_positive_surfaces() -> None:
    audit = _load_json(POSITIVE_RESIDUE_AUDIT)

    for hit in audit["documented_lexical_positive_hits"]:
        path = ROOT / hit["path"]
        assert path.is_file(), hit["path"]
        text = path.read_text(encoding="utf-8")
        assert hit["token"] in text
        assert hit["classification"]
        assert "not " in hit["disposition"]


def test_positive_fixture_lexical_residue_hits_are_documented() -> None:
    audit = _load_json(POSITIVE_RESIDUE_AUDIT)
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
    audit = _load_json(POSITIVE_RESIDUE_AUDIT)
    outcome_index = _load_json(
        ROOT / "tests" / "conformance" / "hard_cutover_behavior_outcome_owner_index.json"
    )
    documented_paths = {hit["path"] for hit in audit["documented_lexical_positive_hits"]}

    residue_outcome = next(
        entry
        for entry in outcome_index["outcomes"]
        if entry["outcome"] == "positive_residue_false_positive"
    )
    assert set(residue_outcome["evidence"]) == documented_paths


def test_generated_manifest_cannot_reference_native_behavior_or_retired_support() -> None:
    generated_manifest = _load_json(FIXTURE_ROOT / "generated" / "manifest.json")
    forbidden_support_tokens = (
        "old-mode",
        "compatibility",
        "migration",
        "fallback",
        "shim",
        "runtime_dispatch",
        "runtime-dispatch",
    )

    assert generated_manifest["boundary"]["canonical_behavior_source"] is False
    assert generated_manifest["boundary"]["positive_fixture_policy"] == (
        "generated artifacts never define positive native behavior"
    )
    for entry in generated_manifest["fixtures"]:
        path = ROOT / entry["path"]
        serialized = json.dumps(entry, sort_keys=True).lower()
        assert path.is_file(), entry["path"]
        assert not path.is_relative_to(NATIVE_ROOT)
        assert entry["behavior_boundary"] == "generated-provenance-only"
        assert entry["canonical_behavior_source"] is False
        for token in forbidden_support_tokens:
            assert token not in serialized


def test_canonical_manifest_is_phase_ordered_and_behavior_first() -> None:
    entries = load_manifest_fixture_entries(FIXTURE_ROOT / "canonical" / "manifest.json")
    phase_index = {phase: index for index, phase in enumerate(PHASE_ORDER)}
    indexed_phases = [phase_index[entry["owner_phase"]] for entry in entries]

    assert indexed_phases == sorted(indexed_phases)
    for entry in entries:
        path = ROOT / entry["path"]
        relative_parts = path.relative_to(NATIVE_ROOT).parts
        assert relative_parts[0] == entry["owner_phase"]
        assert relative_parts[1] == entry["behavior_family"]


def test_old_mode_and_runtime_strict_error_cases_are_not_positive_canonical_fixtures() -> None:
    canonical_by_path = {
        entry["path"]: entry
        for entry in load_manifest_fixture_entries(FIXTURE_ROOT / "canonical" / "manifest.json")
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
    matrix = _load_json(NATIVE_ROOT / "retired_surface_matrix.json")
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
    contract_index = _load_json(RETIRED_SURFACE_CONTRACT_INDEX)
    outcome_index = _load_json(
        ROOT / "tests" / "conformance" / "hard_cutover_behavior_outcome_owner_index.json"
    )
    diagnostic_index = _load_json(
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
    contract_index = _load_json(RETIRED_SURFACE_CONTRACT_INDEX)
    behavior_by_path = load_behavior_fixture_catalog().by_relative_source()

    for family in contract_index["surface_families"]:
        for entry in family["fixtures"]:
            fixture_path = ROOT / entry["path"]
            sidecar_path = ROOT / entry["sidecar"]
            metadata = _load_json(sidecar_path)
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
    contract_index = _load_json(RETIRED_SURFACE_CONTRACT_INDEX)
    outcome_index = _load_json(
        ROOT / "tests" / "conformance" / "hard_cutover_behavior_outcome_owner_index.json"
    )
    diagnostic_index = _load_json(
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
    contract_index = _load_json(RETIRED_SURFACE_CONTRACT_INDEX)
    retired_matrix = _load_json(NATIVE_ROOT / "retired_surface_matrix.json")
    matrix_paths = {entry["fixture_path"] for entry in retired_matrix["entries"]}
    indexed_paths = {
        entry["path"]
        for family in contract_index["surface_families"]
        for entry in family["fixtures"]
    }

    assert matrix_paths <= indexed_paths


def test_fixture_family_owner_index_keeps_phase_boundaries_hard_cutover() -> None:
    family_index = _load_json(
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
    outcome_index = _load_json(
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


def test_fixture_boundary_contract_index_links_all_boundary_families() -> None:
    contracts = _load_json(FIXTURE_BOUNDARY_CONTRACTS)
    catalog = _load_json(ROOT / "tests" / "conformance" / "hard_cutover_catalog.json")
    behavior_by_path = load_behavior_fixture_catalog().by_relative_source()
    generated_paths = {
        entry["path"] for entry in load_manifest_fixture_entries(FIXTURE_ROOT / "generated" / "manifest.json")
    }
    families = {entry["family"]: entry for entry in contracts["contract_families"]}

    assert contracts["catalog"] == "objc3-hard-cutover-fixture-boundary-contracts"
    assert catalog["policy"]["fixture_boundary_contracts"] == (
        FIXTURE_BOUNDARY_CONTRACTS.relative_to(ROOT).as_posix()
    )
    assert contracts["manifests"]["canonical_behavior"]["path"] == (
        FIXTURE_ROOT / "canonical" / "manifest.json"
    ).relative_to(ROOT).as_posix()
    assert contracts["manifests"]["canonical_behavior"]["positive_support"] is True
    assert contracts["manifests"]["generated_provenance"]["path"] == (
        FIXTURE_ROOT / "generated" / "manifest.json"
    ).relative_to(ROOT).as_posix()
    assert contracts["manifests"]["generated_provenance"]["positive_support"] is False

    canonical = families["canonical_positive_behavior"]
    assert canonical["positive_support"] is True
    for relative_path in canonical["evidence"]:
        fixture = behavior_by_path[relative_path]
        assert fixture.fixture_kind == "positive"
        assert not fixture.retired_surface_tags

    retired = families["retired_surface_rejection_and_strict_error"]
    assert retired["positive_support"] is False
    for relative_path in retired["evidence"]:
        fixture = behavior_by_path[relative_path]
        assert fixture.fixture_kind in STRICT_KINDS
        assert fixture.expected_diagnostic_code

    generated = families["generated_provenance_only"]
    assert generated["positive_support"] is False
    assert set(generated["evidence"]) == generated_paths
    for relative_path in generated["evidence"]:
        path = ROOT / relative_path
        assert path.is_file(), relative_path
        assert not path.is_relative_to(NATIVE_ROOT)

    lexical = families["tooling_positive_lexical_residue_audit"]
    audit_paths = {
        hit["path"] for hit in _load_json(POSITIVE_RESIDUE_AUDIT)["documented_lexical_positive_hits"]
    }
    assert set(lexical["evidence"]).issubset(audit_paths)


def test_fixture_boundary_contract_reference_anchors_are_provenance_only() -> None:
    contracts = _load_json(FIXTURE_BOUNDARY_CONTRACTS)
    families = {entry["family"]: entry for entry in contracts["contract_families"]}
    reference_family = families["conformance_reference_anchors"]
    anchor_token = reference_family["anchor_token"]
    canonical_paths = {
        entry["path"] for entry in load_manifest_fixture_entries(FIXTURE_ROOT / "canonical" / "manifest.json")
    }
    roots = [ROOT / root for root in reference_family["roots"]]

    assert reference_family["positive_support"] is False
    assert reference_family["contract"] == "reference-provenance-only"
    assert anchor_token == "docs/reference/legacy_spec_anchor_index.md"

    anchored_files = {
        path
        for root in roots
        for path in root.rglob("*.json")
        if anchor_token in path.read_text(encoding="utf-8")
    }
    assert anchored_files
    for path in anchored_files:
        relative_path = path.relative_to(ROOT).as_posix()
        assert relative_path not in canonical_paths
        assert path.is_relative_to(ROOT / "tests" / "conformance")


def test_legacy_runtime_dispatch_execution_residues_are_negative() -> None:
    negative_root = ROOT / "tests" / "tooling" / "fixtures" / "native" / "execution" / "negative"
    strict_runtime_cases = (
        "message_send_runtime_dispatch_strict_error.objc3",
        "message_send_six_args_custom_cap.objc3",
    )

    for fixture_name in strict_runtime_cases:
        source_path = negative_root / fixture_name
        first_line = source_path.read_text(encoding="utf-8").splitlines()[0]
        meta = _load_json(source_path.with_name(f"{source_path.stem}.meta.json"))
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
        meta = _load_json(fixture_path.with_name(f"{fixture_path.stem}.meta.json"))
        assert meta["fixture_kind"] == "rejection"
        assert meta["expected"]["stage"] == "compile"
        assert meta["expected"]["diagnostic_code"] == "O3C002"


def test_support_claims_link_to_executable_behavior_fixtures() -> None:
    canonical_manifest = _load_json(FIXTURE_ROOT / "canonical" / "manifest.json")
    behavior_paths = set(load_behavior_fixture_catalog().by_relative_source())
    claims = canonical_manifest["support_claims"]

    assert {claim["owner_phase"] for claim in claims} == set(REQUIRED_TREE)
    for claim in claims:
        assert claim["behavior_fixture"] in behavior_paths
        assert claim["executable_command"] == "npm run objc3c -- test-behavior-matrix"


def _compile_fixture(fixture: BehaviorFixture, out_dir: Path) -> tuple[int, str]:
    completed = subprocess.run(
        [
            str(NATIVE_EXE),
            str(fixture.source_path),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
            *fixture.native_compile_args,
        ],
        capture_output=True,
        text=True,
        check=False,
    )
    diagnostics_path = out_dir / "module.diagnostics.txt"
    diagnostics = diagnostics_path.read_text(encoding="utf-8") if diagnostics_path.exists() else ""
    return completed.returncode, diagnostics + completed.stdout + completed.stderr


def test_behavior_fixture_slice_executes_compile_and_strict_error_contracts(tmp_path: Path) -> None:
    assert NATIVE_EXE.exists(), "native compiler binary must exist before running behavior fixtures"

    driver_fixtures = load_behavior_fixture_catalog().compiler_driver_fixtures()
    assert driver_fixtures
    assert {fixture.owner_phase for fixture in driver_fixtures} >= {
        "parser",
        "sema",
        "lowering",
        "ir",
        "e2e",
    }

    for index, fixture in enumerate(driver_fixtures):
        return_code, output = _compile_fixture(fixture, tmp_path / f"driver-{index}")
        if fixture.is_strict:
            assert return_code != 0
            assert fixture.expected_diagnostic_code in output
            for token in fixture.required_tokens:
                assert token in output
        else:
            assert return_code == 0, output
