from test_behavior_strict_error_fixture_naming_support import (
    ROOT,
    load_json,
    strict_error_sidecars,
)


def strict_error_fixture_sidecars_match_existing_sources() -> None:
    sidecars = strict_error_sidecars()
    assert sidecars

    for sidecar_path in sidecars:
        metadata = load_json(sidecar_path)
        source_path = sidecar_path.with_name(metadata["fixture"])
        assert source_path.is_file(), source_path.relative_to(ROOT).as_posix()
        assert metadata["boundary"]["behavior_contract"] == "canonical-strict-error"
        assert metadata["expected"]["diagnostic_code"]
        assert metadata["expected"]["required_tokens"]
