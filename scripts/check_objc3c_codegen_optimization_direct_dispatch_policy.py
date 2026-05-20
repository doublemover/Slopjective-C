#!/usr/bin/env python3
"""Validate codegen optimization and direct-dispatch policy source truth."""

from __future__ import annotations

from objc3c_codegen_optimization_policy import REPORT_PATH, validate_policy
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel


def main() -> int:
    result = validate_policy()
    write_json_file(REPORT_PATH, result.payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    print(f"objc3c-codegen-optimization-direct-dispatch-policy: {result.payload['status']}")
    if result.failures:
        for failure in result.failures:
            print(f"failure: {failure}")
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
