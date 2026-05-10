from __future__ import annotations

import io
import os
import unittest
from contextlib import redirect_stderr, redirect_stdout
from unittest import mock

from generate_execution_microtasks_assertions import (
    assert_empty_success,
    assert_subprocess_success_output,
    assert_success_output,
    assert_system_exit,
)
from generate_execution_microtasks_sources import (
    deterministic_argv,
    deterministic_expected_markdown_path,
    generate_execution_microtasks,
    run_main,
    run_subprocess_with_hash_seed,
    source_date_epoch_argv,
    status_catalog_path,
    subprocess_determinism_command,
)


def assert_fixed_generated_on_is_deterministic_and_matches_fixture(
    case: unittest.TestCase,
) -> None:
    expected = deterministic_expected_markdown_path().read_text(encoding="utf-8")
    argv = deterministic_argv()

    result1 = run_main(argv)
    result2 = run_main(argv)

    assert_success_output(case, result1, expected)
    assert_success_output(case, result2, expected)
    case.assertEqual(result1[1], result2[1])


def assert_status_integrity_check_fails_for_missing_status_catalog(
    case: unittest.TestCase,
) -> None:
    argv = ["--catalog-json", str(status_catalog_path("catalog_missing_status.json"))]

    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        with case.assertRaises(SystemExit) as context:
            generate_execution_microtasks.main(argv)

    assert_system_exit(
        case,
        context,
        expected_code=2,
        stdout=stdout.getvalue(),
        stderr=stderr.getvalue(),
        expected_stderr="missing required 'execution_status'",
    )
    case.assertIn("--allow-missing-status", stderr.getvalue())


def assert_status_integrity_check_succeeds_for_valid_status_catalog(
    case: unittest.TestCase,
) -> None:
    result = run_main(
        ["--catalog-json", str(status_catalog_path("catalog_valid_status.json"))]
    )
    assert_empty_success(case, result)


def assert_allow_missing_status_override_for_integrity_check(
    case: unittest.TestCase,
) -> None:
    result = run_main(
        [
            "--catalog-json",
            str(status_catalog_path("catalog_missing_status.json")),
            "--allow-missing-status",
        ]
    )
    assert_empty_success(case, result)


def assert_generated_on_falls_back_to_source_date_epoch(
    case: unittest.TestCase,
) -> None:
    with mock.patch.dict(os.environ, {"SOURCE_DATE_EPOCH": "1771804800"}, clear=False):
        code, output, stderr = run_main(source_date_epoch_argv())

    case.assertEqual(code, 0)
    case.assertEqual(stderr, "")
    case.assertIn("_Generated on 2026-02-23 from GitHub issue snapshot._", output)


def assert_missing_generated_on_without_source_date_epoch_errors(
    case: unittest.TestCase,
) -> None:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with mock.patch.dict(os.environ, {}, clear=True):
        with redirect_stdout(stdout), redirect_stderr(stderr):
            with case.assertRaises(SystemExit) as context:
                generate_execution_microtasks.main(source_date_epoch_argv())

    assert_system_exit(
        case,
        context,
        expected_code=2,
        stdout=stdout.getvalue(),
        stderr=stderr.getvalue(),
        expected_stderr="must provide --generated-on",
    )


def assert_subprocess_output_is_stable_across_hash_seeds(
    case: unittest.TestCase,
) -> None:
    expected_bytes = deterministic_expected_markdown_path().read_bytes()
    command = subprocess_determinism_command()

    result_seed_1 = run_subprocess_with_hash_seed(command, "1")
    result_seed_2 = run_subprocess_with_hash_seed(command, "2")

    assert_subprocess_success_output(
        case,
        returncode=result_seed_1.returncode,
        stdout=result_seed_1.stdout,
        stderr=result_seed_1.stderr,
        expected_stdout=expected_bytes,
    )
    assert_subprocess_success_output(
        case,
        returncode=result_seed_2.returncode,
        stdout=result_seed_2.stdout,
        stderr=result_seed_2.stderr,
        expected_stdout=expected_bytes,
    )
    case.assertEqual(result_seed_1.stdout, result_seed_2.stdout)
