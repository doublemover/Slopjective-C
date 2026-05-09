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
        if fixture_kind in STRICT_KINDS:
            expected = metadata["expected"]
            assert expected["stage"] in {"parse", "compile", "link", "run"}
            assert expected["diagnostic_code"]
            assert expected["required_tokens"]


def test_behavior_matrix_has_representative_phase_coverage() -> None:
    catalog = load_behavior_fixture_catalog()

    assert catalog.covered_phases == set(PHASE_ORDER)
    for phase in PHASE_ORDER:
        assert catalog.phase(phase), f"missing representative behavior fixtures for {phase}"


def test_canonical_and_generated_fixture_ownership_are_disjoint() -> None:
    canonical_manifest = _load_json(FIXTURE_ROOT / "canonical" / "manifest.json")
    generated_manifest = _load_json(FIXTURE_ROOT / "generated" / "manifest.json")
    canonical_entries = load_manifest_fixture_entries(FIXTURE_ROOT / "canonical" / "manifest.json")
    generated_entries = load_manifest_fixture_entries(FIXTURE_ROOT / "generated" / "manifest.json")

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
    assert generated_manifest["fixtures"] == generated_entries

    for entry in generated_entries:
        path = ROOT / entry["path"]
        assert path.exists(), entry["path"]
        assert entry["origin"] == "generated"
        assert entry["generator"]
        assert entry["provenance"]
        assert not path.is_relative_to(NATIVE_ROOT)


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


def test_legacy_literal_aliases_are_rejection_coverage_not_positive_recovery() -> None:
    recovery_positive = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "positive"

    assert not (recovery_positive / "objc_literal_aliases_globals.objc3").exists()
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
