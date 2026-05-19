#!/usr/bin/env python3
from __future__ import annotations

from objc3c_tooling.paths import repo_rel
from platform_hardening_contracts import (
    SUPPORT_MATRIX_ARTIFACT_PATH,
    SUPPORT_MATRIX_SUMMARY_PATH,
    build_support_matrix_payload,
    write_support_matrix,
)


def main() -> int:
    write_support_matrix(build_support_matrix_payload())
    print(f"summary_path: {repo_rel(SUPPORT_MATRIX_SUMMARY_PATH)}")
    print(f"artifact_path: {repo_rel(SUPPORT_MATRIX_ARTIFACT_PATH)}")
    print("objc3c-platform-support-matrix: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
