"""CLI orchestration for the release manifest builder."""

from __future__ import annotations

from datetime import datetime, timezone

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.subprocesses import python_script_command

from .commands import git_output, run
from .model import build_release_manifest_payload
from .package import package_once
from .paths import (
    EVIDENCE_INDEX_PATH,
    PAYLOAD_POLICY,
    RELEASE_EVIDENCE_PY,
    REPRO_POLICY,
    ROOT,
    SOURCE_SURFACE,
    SUMMARY_PATH,
)
from .render import write_release_manifest_artifacts
from .validate import validate_release_inputs


def main() -> int:
    load_json(SOURCE_SURFACE)
    payload_policy = load_json(PAYLOAD_POLICY)
    reproducibility_policy = load_json(REPRO_POLICY)

    run(python_script_command(RELEASE_EVIDENCE_PY))
    if not EVIDENCE_INDEX_PATH.is_file():
        raise RuntimeError(f"missing release evidence index {repo_rel(EVIDENCE_INDEX_PATH)}")

    run_root = (
        ROOT
        / "tmp"
        / "pkg"
        / "objc3c-release-foundation"
        / datetime.now(timezone.utc).strftime("%Y%m%d_%H%M%S_%f")
    )
    manifest_relative_path = "artifacts/package/objc3c-runnable-toolchain-package.json"
    first = package_once(run_root / "run-1", manifest_relative_path)
    second = package_once(run_root / "run-2", manifest_relative_path)
    validation = validate_release_inputs(
        first=first,
        second=second,
        payload_policy=payload_policy,
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

