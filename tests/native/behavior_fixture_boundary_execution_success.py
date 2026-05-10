def assert_successful_fixture_result(return_code: int, output: str) -> None:
    assert return_code == 0, output
