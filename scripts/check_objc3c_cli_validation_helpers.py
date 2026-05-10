#!/usr/bin/env python3
"""Fast coverage for small shared CLI and validation helpers."""

from __future__ import annotations

import argparse

from objc3c_tooling.cli import add_check_argument
from scripts.objc3c_workflow.public_command_api import load_public_workflow_runner
from objc3c_tooling.validation import contains_all


def test_contains_all() -> None:
    assert contains_all("alpha beta", ["alpha", "gamma"]) == {
        "alpha": True,
        "gamma": False,
    }


def test_add_check_argument() -> None:
    parser = argparse.ArgumentParser()
    add_check_argument(parser)
    assert parser.parse_args([]).check is False
    assert parser.parse_args(["--check"]).check is True


def test_public_runner_loads() -> None:
    runner = load_public_workflow_runner(module_name="objc3c_workflow_runner_helper_test")
    assert hasattr(runner, "ACTION_SPECS")
    assert callable(runner.list_actions_payload)


def main() -> int:
    test_contains_all()
    test_add_check_argument()
    test_public_runner_loads()
    print("objc3c-cli-validation-helpers: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
