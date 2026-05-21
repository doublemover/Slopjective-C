#!/usr/bin/env python3
"""Validate semantic-preserving Objective-C 3.0 optimization pipeline truth."""

from __future__ import annotations

from objc3c_semantic_optimization_pipeline import REPORT_PATH, validate_pipeline
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel


def main() -> int:
    result = validate_pipeline()
    write_json_file(REPORT_PATH, result.payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    print(f"objc3c-semantic-optimization-pipeline: {result.payload['status']}")
    if result.failures:
        for failure in result.failures:
            print(f"failure: {failure}")
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
