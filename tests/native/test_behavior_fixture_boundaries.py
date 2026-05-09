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
RETIRED_POSITIVE_SURFACE_TERMS = ("old-mode", "shim", "fallback", "compat")
RETIRED_SURFACE_CONTRACT_INDEX = (
    ROOT / "tests" / "conformance" / "hard_cutover_retired_surface_fixture_contracts.json"
)
STRICT_REJECTION_NAME_SUFFIXES = (
    "_rejected.objc3",
    "_strict_error.objc3",
    "_contract.objc3",
)
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


def test_canonical_and_generated_fixture_ownership_are_disjoint() -> None:
    canonical_manifest = _load_json(FIXTURE_ROOT / "canonical" / "manifest.json")
    generated_manifest = _load_json(FIXTURE_ROOT / "generated" / "manifest.json")
    canonical_entries = load_manifest_fixture_entries(FIXTURE_ROOT / "canonical" / "manifest.json")
    generated_entries = load_manifest_fixture_entries(FIXTURE_ROOT / "generated" / "manifest.json")
    canonical_boundary = canonical_manifest["boundary"]
    generated_boundary = generated_manifest["boundary"]

    canonical_by_path = {entry["path"]: entry for entry in canonical_entries}
    generated_paths = {entry["path"] for entry in generated_entries}

    assert canonical_by_path
    assert generated_paths
    assert set(canonical_by_path).isdisjoint(generated_paths)

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
        "old-mode, shim, fallback, and runtime adapter residues must be rejection or strict-error metadata"
    )
    assert generated_manifest["fixtures"] == generated_entries
    assert generated_boundary["kind"] == "generated-contract-artifacts"
    assert generated_boundary["source_of_truth"] == "generator-output"
    assert generated_boundary["canonical_behavior_source"] is False
    assert generated_boundary["allowed_path_roots"] == ["tests/tooling/fixtures/objc3c"]
    assert "not canonical behavior expectations" in generated_boundary["hand_edit_policy"]
    assert generated_boundary["positive_fixture_policy"] == (
        "generated artifacts never define positive native behavior"
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


def test_retired_surface_contract_index_tracks_fixture_outcomes_and_sidecars() -> None:
    contract_index = _load_json(RETIRED_SURFACE_CONTRACT_INDEX)
    outcome_index = _load_json(
        ROOT / "tests" / "conformance" / "hard_cutover_behavior_outcome_owner_index.json"
    )
    diagnostic_index = _load_json(
        ROOT / "tests" / "conformance" / "hard_cutover_diagnostic_outcome_code_index.json"
    )
    retired_matrix = _load_json(NATIVE_ROOT / "retired_surface_matrix.json")

    behavior_by_path = load_behavior_fixture_catalog().by_relative_source()
    outcomes = {entry["outcome"] for entry in outcome_index["outcomes"]}
    diagnostic_codes = {entry["code"] for entry in diagnostic_index["codes"]}
    matrix_paths = {entry["fixture_path"] for entry in retired_matrix["entries"]}
    indexed_paths: set[str] = set()

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

        for entry in family["fixtures"]:
            fixture_path = ROOT / entry["path"]
            sidecar_path = ROOT / entry["sidecar"]
            metadata = _load_json(sidecar_path)
            fixture = behavior_by_path[entry["path"]]
            indexed_paths.add(entry["path"])

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

    for absence in contract_index["absent_support_surfaces"]:
        assert absence["positive_expectation"] is False
        assert absence["behavior_outcome"] in outcomes
        assert absence["diagnostic_owner"] in diagnostic_codes
        assert absence["canonical_disposition"] == "absent-support"

    assert matrix_paths <= indexed_paths


def test_legacy_runtime_dispatch_execution_residues_are_negative() -> None:
    negative_root = ROOT / "tests" / "tooling" / "fixtures" / "native" / "execution" / "negative"
    strict_runtime_cases = (
        "message_send_runtime_dispatch.objc3",
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
        ROOT / "tests" / "tooling" / "fixtures" / "native" / "execution" / "positive",
        ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "positive",
    )
    retired_name_tokens = ("legacy", "old_mode", "old-mode", "shim", "compat", "compatibility")

    for root in positive_roots:
        for path in root.rglob("*"):
            if not path.is_file():
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
