from behavior_fixture_boundary_support import (
    NATIVE_ROOT,
    PHASE_ORDER,
    REQUIRED_TREE,
    ROOT,
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
