from __future__ import annotations

import unittest


def assert_success_output(
    case: unittest.TestCase,
    result: tuple[int, str, str],
    expected_output: str,
) -> None:
    code, output, stderr = result
    case.assertEqual(code, 0)
    case.assertEqual(stderr, "")
    case.assertEqual(output, expected_output)
    case.assertNotIn("\r", output)


def assert_empty_success(
    case: unittest.TestCase,
    result: tuple[int, str, str],
) -> None:
    code, output, stderr = result
    case.assertEqual(code, 0)
    case.assertEqual(output, "")
    case.assertEqual(stderr, "")


def assert_system_exit(
    case: unittest.TestCase,
    context,
    *,
    expected_code: int,
    stdout: str,
    stderr: str,
    expected_stderr: str,
) -> None:
    case.assertEqual(context.exception.code, expected_code)
    case.assertEqual(stdout, "")
    case.assertIn(expected_stderr, stderr)


def assert_subprocess_success_output(
    case: unittest.TestCase,
    *,
    returncode: int,
    stdout: bytes,
    stderr: bytes,
    expected_stdout: bytes,
) -> None:
    case.assertEqual(
        returncode,
        0,
        stderr.decode("utf-8", errors="replace"),
    )
    case.assertEqual(stdout, expected_stdout)
    case.assertNotIn(b"\r", stdout)
