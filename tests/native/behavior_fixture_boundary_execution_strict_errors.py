from behavior_fixture_boundary_support import BehaviorFixture


def assert_strict_error_fixture_result(
    fixture: BehaviorFixture,
    return_code: int,
    output: str,
) -> None:
    assert return_code != 0
    assert fixture.expected_diagnostic_code in output
    for token in fixture.required_tokens:
        assert token in output
