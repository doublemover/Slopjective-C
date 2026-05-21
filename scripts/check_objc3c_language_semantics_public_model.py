#!/usr/bin/env python3
"""Validate the Objective-C 3.0 public language semantics model."""

from __future__ import annotations

from objc3c_language_semantics_public_model import REPORT_PATH
from objc3c_language_semantics_public_model import validate_language_semantics_public_model
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel


def main() -> int:
    result = validate_language_semantics_public_model()
    write_json_file(REPORT_PATH, result.payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    print(f"objc3c-language-semantics-public-model: {result.payload['status']}")
    for failure in result.failures:
        print(f"failure: {failure}")
    return 0 if result.passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
