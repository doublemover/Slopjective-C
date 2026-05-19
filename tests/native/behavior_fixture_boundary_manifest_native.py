from behavior_fixture_boundary_support import (
    FIXTURE_ROOT,
    NATIVE_ROOT,
    PHASE_ORDER,
    ROOT,
    canonical_manifest_entries,
    generated_manifest_entries,
    load_behavior_fixture_catalog,
    load_manifest_fixture_entries,
)


def test_canonical_fixture_manifest_matches_native_behavior_catalog() -> None:
    canonical_manifest, canonical_entries = canonical_manifest_entries()
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
    retired_surface_policy = canonical_boundary["retired_surface_policy"]
    for retired_surface in (
        "retired modes",
        "retired adapters",
        "alternate acceptance paths",
        "retired-source lanes",
        "unsupported features",
        "runtime-dispatch residues",
    ):
        assert retired_surface in retired_surface_policy
    assert retired_surface_policy.endswith(
        "must be rejection, strict-error, or absent-support metadata"
    )
    assert canonical_boundary["boundary_contract_index"] == (
        "tests/conformance/hard_cutover_fixture_boundary_contracts.json"
    )
    assert canonical_boundary["phase_owner_contract_index"] == (
        "tests/conformance/hard_cutover_behavior_phase_owner_contracts.json"
    )


def test_canonical_and_generated_fixture_paths_are_disjoint() -> None:
    _, canonical_entries = canonical_manifest_entries()
    _, generated_entries = generated_manifest_entries()
    canonical_paths = {entry["path"] for entry in canonical_entries}
    generated_paths = {entry["path"] for entry in generated_entries}

    assert canonical_paths
    assert generated_paths
    assert canonical_paths.isdisjoint(generated_paths)


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
