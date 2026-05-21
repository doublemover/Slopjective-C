#!/usr/bin/env python3
"""Validate macro expansion generated-artifact ownership."""

from __future__ import annotations

from objc3c_macro_expansion_artifact_ownership import (
    REPORT_PATH,
    validate_macro_expansion_artifact_ownership,
)
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel


def main() -> int:
    result = validate_macro_expansion_artifact_ownership()
    write_json_file(REPORT_PATH, result.payload, sort_keys=True)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    print(f"objc3c-macro-expansion-artifact-ownership: {result.payload['status']}")
    if result.failures:
        for failure in result.failures:
            print(f"failure: {failure}")
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
