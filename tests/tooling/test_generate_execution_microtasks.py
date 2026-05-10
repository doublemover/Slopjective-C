import unittest

from generate_execution_microtasks_behavior import (
    assert_allow_missing_status_override_for_integrity_check,
    assert_fixed_generated_on_is_deterministic_and_matches_fixture,
    assert_generated_on_falls_back_to_source_date_epoch,
    assert_missing_generated_on_without_source_date_epoch_errors,
    assert_status_integrity_check_fails_for_missing_status_catalog,
    assert_status_integrity_check_succeeds_for_valid_status_catalog,
    assert_subprocess_output_is_stable_across_hash_seeds,
)


class GenerateExecutionMicrotasksTests(unittest.TestCase):
    def test_fixed_generated_on_is_deterministic_and_matches_fixture(self) -> None:
        assert_fixed_generated_on_is_deterministic_and_matches_fixture(self)

    def test_status_integrity_check_fails_for_missing_status_catalog(self) -> None:
        assert_status_integrity_check_fails_for_missing_status_catalog(self)

    def test_status_integrity_check_succeeds_for_valid_status_catalog(self) -> None:
        assert_status_integrity_check_succeeds_for_valid_status_catalog(self)

    def test_allow_missing_status_override_for_integrity_check(self) -> None:
        assert_allow_missing_status_override_for_integrity_check(self)

    def test_generated_on_falls_back_to_source_date_epoch(self) -> None:
        assert_generated_on_falls_back_to_source_date_epoch(self)

    def test_missing_generated_on_without_source_date_epoch_errors(self) -> None:
        assert_missing_generated_on_without_source_date_epoch_errors(self)

    def test_subprocess_output_is_stable_across_hash_seeds(self) -> None:
        assert_subprocess_output_is_stable_across_hash_seeds(self)


if __name__ == "__main__":
    unittest.main()
