import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_ROOT = ROOT / "scripts"
if str(SCRIPT_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPT_ROOT))

from objc3c_tooling.behavior_fixtures import (
    FIXTURE_ROOT,
    NATIVE_ROOT,
    REQUIRED_TREE,
    STRICT_KINDS,
    load_behavior_fixtures,
)

NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RETIRED_SURFACE_TAGS = {"old-mode", "runtime-adapter", "fallback", "runtime-dispatch"}


def _load_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def test_required_behavior_tree_boundaries_exist() -> None:
    for phase, families in REQUIRED_TREE.items():
        phase_root = NATIVE_ROOT / phase
        assert phase_root.is_dir(), f"missing native phase root: {phase_root.relative_to(ROOT)}"
        for family in families:
            family_root = phase_root / family
            assert family_root.is_dir(), f"missing native behavior family: {family_root.relative_to(ROOT)}"

    assert (FIXTURE_ROOT / "canonical").is_dir()
    assert (FIXTURE_ROOT / "generated").is_dir()


def test_native_fixture_metadata_records_phase_and_diagnostics() -> None:
    fixtures = load_behavior_fixtures()
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
    fixtures = load_behavior_fixtures()
    covered_phases = {fixture.owner_phase for fixture in fixtures}

    assert covered_phases == set(REQUIRED_TREE)


def test_canonical_and_generated_fixture_ownership_are_disjoint() -> None:
    canonical_manifest = _load_json(FIXTURE_ROOT / "canonical" / "manifest.json")
    generated_manifest = _load_json(FIXTURE_ROOT / "generated" / "manifest.json")

    canonical_paths = {entry["path"] for entry in canonical_manifest["fixtures"]}
    generated_paths = {entry["path"] for entry in generated_manifest["fixtures"]}

    assert canonical_paths
    assert generated_paths
    assert canonical_paths.isdisjoint(generated_paths)

    for entry in canonical_manifest["fixtures"]:
        path = ROOT / entry["path"]
        assert path.exists(), entry["path"]
        assert entry["origin"] == "hand-authored"
        assert entry["owner_phase"] in REQUIRED_TREE
        assert path.is_relative_to(NATIVE_ROOT)

    behavior_paths = {fixture.relative_source for fixture in load_behavior_fixtures()}
    assert behavior_paths.issubset(canonical_paths)

    for entry in generated_manifest["fixtures"]:
        path = ROOT / entry["path"]
        assert path.exists(), entry["path"]
        assert entry["origin"] == "generated"
        assert entry["generator"]
        assert entry["provenance"]
        assert not path.is_relative_to(NATIVE_ROOT)


def test_old_mode_and_runtime_strict_error_cases_are_not_positive_canonical_fixtures() -> None:
    canonical_manifest = _load_json(FIXTURE_ROOT / "canonical" / "manifest.json")
    fixtures_by_path = {fixture.relative_source: fixture for fixture in load_behavior_fixtures()}

    retired_surface_entries = []
    for entry in canonical_manifest["fixtures"]:
        fixture = fixtures_by_path[entry["path"]]
        manifest_tags = set(entry.get("retired_surface_tags", ()))
        metadata_tags = set(fixture.metadata.get("retired_surface_tags", ()))
        if (manifest_tags | metadata_tags) & RETIRED_SURFACE_TAGS:
            retired_surface_entries.append((entry, fixture, manifest_tags, metadata_tags))
    assert retired_surface_entries

    for entry, fixture, manifest_tags, metadata_tags in retired_surface_entries:
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


def test_support_claims_link_to_executable_behavior_fixtures() -> None:
    canonical_manifest = _load_json(FIXTURE_ROOT / "canonical" / "manifest.json")
    behavior_paths = {fixture.relative_source for fixture in load_behavior_fixtures()}
    claims = canonical_manifest["support_claims"]

    assert {claim["owner_phase"] for claim in claims} == set(REQUIRED_TREE)
    for claim in claims:
        assert claim["behavior_fixture"] in behavior_paths
        assert claim["executable_command"] == "npm run objc3c -- test-behavior-matrix"


def _compile_fixture(source_path: Path, out_dir: Path) -> tuple[int, str]:
    completed = subprocess.run(
        [
            str(NATIVE_EXE),
            str(source_path),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
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

    positive_source = NATIVE_ROOT / "e2e" / "smoke" / "basic_i32_return_main.objc3"
    positive_code, positive_output = _compile_fixture(positive_source, tmp_path / "positive")
    assert positive_code == 0, positive_output

    negative_cases = (
        (
            NATIVE_ROOT / "parser" / "negative" / "legacy_null_literal_alias_rejected.objc3",
            "O3C002",
            "legacy literal alias 'NULL' is rejected",
        ),
        (
            NATIVE_ROOT / "sema" / "negative" / "return_void_with_value_rejected.objc3",
            "O3S211",
            "must use 'return;'",
        ),
    )
    for index, (source_path, code, token) in enumerate(negative_cases):
        return_code, output = _compile_fixture(source_path, tmp_path / f"negative-{index}")
        assert return_code != 0
        assert code in output
        assert token in output
