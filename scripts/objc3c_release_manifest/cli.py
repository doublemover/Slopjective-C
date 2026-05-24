"""CLI orchestration for the release manifest builder."""

from __future__ import annotations

import argparse
from collections.abc import Sequence
from datetime import datetime, timezone

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.paths import repo_rel, resolve_repo_path_inside
from objc3c_tooling.subprocesses import python_script_command

from .commands import git_output, run
from .model import build_release_manifest_payload
from .package import load_package_assembly, package_once
from .paths import (
    ABI_API_DRIFT_PY,
    ABI_API_DRIFT_SUMMARY_PATH,
    EVIDENCE_INDEX_PATH,
    PACKAGE_MANIFEST_RELATIVE_PATH,
    PACKAGE_STAGE_ROOT,
    PAYLOAD_POLICY,
    RELEASE_EVIDENCE_PY,
    REPRO_POLICY,
    SOURCE_SURFACE,
    SUMMARY_PATH,
)
from .render import write_release_manifest_artifacts
from .validate import validate_release_inputs


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--reuse-primary-package-root",
        default=None,
        help=(
            "Use an already built runnable package as the first release-foundation "
            "assembly after manifest/layout validation, then build a fresh second "
            "assembly to preserve reproducibility proof."
        ),
    )
    return parser.parse_args(argv)


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    load_json(SOURCE_SURFACE)
    payload_policy = load_json(PAYLOAD_POLICY)
    reproducibility_policy = load_json(REPRO_POLICY)

    run(python_script_command(ABI_API_DRIFT_PY))
    run(python_script_command(RELEASE_EVIDENCE_PY))
    if not EVIDENCE_INDEX_PATH.is_file():
        raise RuntimeError(f"missing release evidence index {repo_rel(EVIDENCE_INDEX_PATH)}")

    run_root = (
        PACKAGE_STAGE_ROOT
        / "release-foundation"
        / datetime.now(timezone.utc).strftime("%Y%m%d_%H%M%S_%f")
    )
    required_manifest_fields = tuple(payload_policy["required_manifest_fields"])
    if args.reuse_primary_package_root is None:
        first = package_once(
            run_root / "run-1",
            PACKAGE_MANIFEST_RELATIVE_PATH,
            required_manifest_fields,
        )
    else:
        first = load_package_assembly(
            package_root=resolve_repo_path_inside(args.reuse_primary_package_root),
            manifest_relative_path=PACKAGE_MANIFEST_RELATIVE_PATH,
            required_manifest_fields=required_manifest_fields,
        )
    second = package_once(
        run_root / "run-2",
        PACKAGE_MANIFEST_RELATIVE_PATH,
        required_manifest_fields,
    )
    validation = validate_release_inputs(
        first=first,
        second=second,
        payload_policy=payload_policy,
        abi_api_drift_summary_path=ABI_API_DRIFT_SUMMARY_PATH,
    )

    git_commit = git_output("rev-parse", "HEAD")
    git_tree_dirty = bool(git_output("status", "--porcelain"))

    payload = build_release_manifest_payload(
        first=first,
        second=second,
        source_surface=SOURCE_SURFACE,
        reproducibility_policy=reproducibility_policy,
        evidence_index_path=EVIDENCE_INDEX_PATH,
        validation=validation,
        git_commit=git_commit,
        git_tree_dirty=git_tree_dirty,
    )
    write_release_manifest_artifacts(
        payload=payload,
        first=first,
        validation=validation,
    )

    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-release-manifest: PASS")
    return 0
