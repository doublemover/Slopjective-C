from test_behavior_strict_error_fixture_naming_support import (
    CURRENT_STRICT_ERROR_NAME_DEBT,
    ROOT,
    STRICT_ERROR_SUFFIXES,
    load_json,
    strict_error_sidecars,
)


def strict_error_fixture_names_are_explicit_or_tracked_debt() -> None:
    nonconforming_names = {
        sidecar_path.with_name(load_json(sidecar_path)["fixture"]).relative_to(ROOT).as_posix()
        for sidecar_path in strict_error_sidecars()
        if not load_json(sidecar_path)["fixture"].endswith(STRICT_ERROR_SUFFIXES)
    }

    assert nonconforming_names == CURRENT_STRICT_ERROR_NAME_DEBT
