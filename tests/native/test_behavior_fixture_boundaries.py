import json
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
NATIVE_ROOT = ROOT / "tests" / "native"
FIXTURE_ROOT = ROOT / "tests" / "fixtures"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"

REQUIRED_TREE = {
    "parser": ("positive", "negative", "snapshots"),
    "sema": (
        "types",
        "ownership",
        "objc",
        "control_flow",
        "errors",
        "concurrency",
        "negative",
    ),
    "lowering": ("expressions", "statements", "objc_runtime", "ownership", "errors"),
    "ir": ("module", "function", "metadata", "runtime_calls"),
    "runtime": ("dispatch", "object_model", "storage", "arc", "blocks", "errors", "concurrency"),
    "e2e": ("smoke", "feature_matrix", "negative_execution"),
}

STRICT_KINDS = {"negative", "strict-error", "rejection"}


def _load_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def _metadata_files() -> list[Path]:
    return sorted(NATIVE_ROOT.rglob("*.meta.json"))


def _fixture_path_for_metadata(meta_path: Path) -> Path:
    return meta_path.with_name(meta_path.name.removesuffix(".meta.json") + ".objc3")


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
    metadata_paths = _metadata_files()
    assert metadata_paths, "native behavior fixtures must carry metadata"

    for meta_path in metadata_paths:
        metadata = _load_json(meta_path)
        fixture_path = _fixture_path_for_metadata(meta_path)

        assert fixture_path.exists(), f"missing fixture for {meta_path.relative_to(ROOT)}"
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

    for entry in generated_manifest["fixtures"]:
        path = ROOT / entry["path"]
        assert path.exists(), entry["path"]
        assert entry["origin"] == "generated"
        assert entry["generator"]
        assert entry["provenance"]
        assert not path.is_relative_to(NATIVE_ROOT)


def test_old_mode_and_runtime_strict_error_cases_are_not_positive_canonical_fixtures() -> None:
    canonical_manifest = _load_json(FIXTURE_ROOT / "canonical" / "manifest.json")

    retired_surface_entries = [
        entry
        for entry in canonical_manifest["fixtures"]
        if set(entry.get("retired_surface_tags", ())) & {"old-mode", "runtime-adapter", "fallback", "runtime-dispatch"}
    ]
    assert retired_surface_entries

    for entry in retired_surface_entries:
        assert entry["fixture_kind"] in STRICT_KINDS
        assert entry["expected_diagnostic_code"]


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
