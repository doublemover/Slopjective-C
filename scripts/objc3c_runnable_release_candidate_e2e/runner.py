"""Runnable release-candidate end-to-end validation runner."""

from __future__ import annotations

from datetime import datetime
from typing import Sequence

from objc3c_tooling.paths import repo_rel
from objc3c_tooling.probe_compile import find_clangxx

from .catalog import RELEASE_CANDIDATE_PROBES
from .cli import parse_args
from .commands import compile_release_candidate_fixture
from .commands import compile_release_candidate_probe
from .commands import package_runnable_toolchain
from .commands import run_packaged_execution_replay
from .commands import run_packaged_execution_smoke
from .commands import run_packaged_executable
from .commands import validate_release_candidate_fixture
from .manifest import load_and_validate_manifest
from .manifest import manifest_path
from .paths import REPORT_PATH
from .paths import ROOT
from .report import build_summary_payload
from .report import write_summary_report
from .results import assert_claim_probe_payload
from .results import assert_evidence_probe_payload
from .results import parse_claim_probe_payload
from .results import parse_evidence_probe_payload


def main(argv: Sequence[str] | None = None) -> int:
    parse_args(argv)

    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-release-candidate-e2e" / run_id
    manifest_json_path = package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
    compile_out_dir = package_root / "tmp" / "artifacts" / "runnable-release-candidate-e2e" / "compile"
    validate_out_dir = package_root / "tmp" / "artifacts" / "runnable-release-candidate-e2e" / "validate"
    probe_out_dir = package_root / "tmp" / "artifacts" / "runnable-release-candidate-e2e"

    package_result = package_runnable_toolchain(package_root)
    if package_result.returncode != 0:
        raise RuntimeError("runnable toolchain package command failed")

    manifest = load_and_validate_manifest(manifest_json_path, package_root=package_root)

    native_executable = manifest_path(manifest, "native_executable", package_root=package_root)
    compile_script = manifest_path(manifest, "compile_wrapper", package_root=package_root)
    runtime_library = manifest_path(manifest, "runtime_library", package_root=package_root)
    smoke_script = manifest_path(manifest, "execution_smoke_script", package_root=package_root)
    replay_script = manifest_path(manifest, "execution_replay_script", package_root=package_root)
    release_candidate_fixture = manifest_path(manifest, "release_candidate_fixture", package_root=package_root)
    claim_probe = manifest_path(manifest, "release_candidate_claim_probe", package_root=package_root)
    evidence_probe = manifest_path(manifest, "release_candidate_evidence_probe", package_root=package_root)

    compile_result, compile_artifacts = compile_release_candidate_fixture(
        compile_script,
        release_candidate_fixture,
        compile_out_dir,
        cwd=package_root,
    )

    validation_result, validate_artifacts = validate_release_candidate_fixture(
        native_executable,
        compile_artifacts["conformance_report"],
        validate_out_dir,
        cwd=package_root,
    )

    clangxx = find_clangxx()
    probe_sources = {
        "release_candidate_claim_probe": claim_probe,
        "release_candidate_evidence_probe": evidence_probe,
    }
    probe_executables = {
        probe_case.case_id: probe_out_dir / probe_case.executable_name
        for probe_case in RELEASE_CANDIDATE_PROBES
    }
    for probe_case in RELEASE_CANDIDATE_PROBES:
        compile_release_candidate_probe(
            clangxx=clangxx,
            probe_source=probe_sources[probe_case.manifest_key],
            output_exe=probe_executables[probe_case.case_id],
            cwd=package_root,
            runtime_library=runtime_library,
        )

    claim_payload = parse_claim_probe_payload(
        run_packaged_executable(probe_executables["claim"], cwd=package_root)
    )
    assert_claim_probe_payload(claim_payload)

    evidence_payload = parse_evidence_probe_payload(
        run_packaged_executable(probe_executables["evidence"], cwd=package_root)
    )
    assert_evidence_probe_payload(evidence_payload)

    smoke_result = run_packaged_execution_smoke(smoke_script, cwd=package_root)
    if smoke_result.returncode != 0:
        raise RuntimeError("packaged execution smoke failed")

    replay_result = run_packaged_execution_replay(replay_script, cwd=package_root)
    if replay_result.returncode != 0:
        raise RuntimeError("packaged execution replay proof failed")

    payload = build_summary_payload(
        package_result=package_result,
        smoke_result=smoke_result,
        replay_result=replay_result,
        manifest_json_path=manifest_json_path,
        package_root=package_root,
        release_candidate_fixture=release_candidate_fixture,
        validate_out_dir=validate_out_dir,
        compile_artifacts=compile_artifacts,
        validate_artifacts=validate_artifacts,
        claim_payload=claim_payload,
        evidence_payload=evidence_payload,
        compile_result=compile_result,
        validation_result=validation_result,
        clangxx=clangxx,
    )

    write_summary_report(payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


__all__ = ["main"]
