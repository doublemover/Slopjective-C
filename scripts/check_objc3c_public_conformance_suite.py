#!/usr/bin/env python3
"""Validate, stage, and verify the stable public conformance suite package."""

from __future__ import annotations

import argparse
import importlib.util
import sys
from pathlib import Path

from objc3c_public_conformance_suite.contracts import (
    DEFAULT_PACKAGE_ROOT,
    DEFAULT_REPORT_PATH,
    MANIFEST_CHECKER_PATH,
)
from objc3c_public_conformance_suite.package import build_package, build_summary, verify_package
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel


def run_manifest_checker() -> None:
    spec = importlib.util.spec_from_file_location(
        "check_objc3c_public_conformance_suite_manifest",
        MANIFEST_CHECKER_PATH,
    )
    if spec is None or spec.loader is None:
        raise RuntimeError("unable to load public conformance suite manifest checker")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    result = module.main()
    if result != 0:
        raise RuntimeError("public conformance suite manifest checker failed")


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--package-root", type=Path, default=DEFAULT_PACKAGE_ROOT)
    parser.add_argument("--report-path", type=Path, default=DEFAULT_REPORT_PATH)
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(sys.argv[1:] if argv is None else argv)
    try:
        run_manifest_checker()
        package_manifest = build_package(package_root=args.package_root)
        verification = verify_package(args.package_root)
        summary = build_summary(package_manifest, verification)
        args.report_path.parent.mkdir(parents=True, exist_ok=True)
        write_json_file(args.report_path, summary)
    except Exception as exc:
        print(f"objc3c-public-conformance-suite: FAIL\n- {exc}", file=sys.stderr)
        return 1

    print(f"summary_path: {repo_rel(args.report_path)}")
    print("objc3c-public-conformance-suite: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
