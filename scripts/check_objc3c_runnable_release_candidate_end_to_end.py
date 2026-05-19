#!/usr/bin/env python3
"""Validate runnable release-candidate packaging and validation end to end from the staged package root."""

from __future__ import annotations

from objc3c_runnable_release_candidate_e2e import COMPILE_ARTIFACT_FILES
from objc3c_runnable_release_candidate_e2e import EXPECTED_VALIDATE_ARTIFACTS
from objc3c_runnable_release_candidate_e2e import PACKAGE_CONTRACT_ID
from objc3c_runnable_release_candidate_e2e import PACKAGE_PS1
from objc3c_runnable_release_candidate_e2e import PWSH
from objc3c_runnable_release_candidate_e2e import RELEASE_CANDIDATE_PROBES
from objc3c_runnable_release_candidate_e2e import REPORT_PATH
from objc3c_runnable_release_candidate_e2e import REQUIRED_MANIFEST_KEYS
from objc3c_runnable_release_candidate_e2e import ROOT
from objc3c_runnable_release_candidate_e2e import RUNNER_PATH
from objc3c_runnable_release_candidate_e2e import SUMMARY_CONTRACT_ID
from objc3c_runnable_release_candidate_e2e import ReleaseCandidateCliOptions
from objc3c_runnable_release_candidate_e2e import ReleaseCandidateProbeCase
from objc3c_runnable_release_candidate_e2e import assert_claim_probe_payload
from objc3c_runnable_release_candidate_e2e import assert_compile_artifacts
from objc3c_runnable_release_candidate_e2e import assert_evidence_probe_payload
from objc3c_runnable_release_candidate_e2e import assert_validate_artifacts
from objc3c_runnable_release_candidate_e2e import build_summary_payload
from objc3c_runnable_release_candidate_e2e import collect_validate_artifacts
from objc3c_runnable_release_candidate_e2e import compile_artifact_paths
from objc3c_runnable_release_candidate_e2e import compile_release_candidate_fixture
from objc3c_runnable_release_candidate_e2e import compile_release_candidate_probe
from objc3c_runnable_release_candidate_e2e import expect
from objc3c_runnable_release_candidate_e2e import load_and_validate_manifest
from objc3c_runnable_release_candidate_e2e import main
from objc3c_runnable_release_candidate_e2e import manifest_path
from objc3c_runnable_release_candidate_e2e import package_runnable_toolchain
from objc3c_runnable_release_candidate_e2e import parse_args
from objc3c_runnable_release_candidate_e2e import parse_claim_probe_payload
from objc3c_runnable_release_candidate_e2e import parse_evidence_probe_payload
from objc3c_runnable_release_candidate_e2e import run_packaged_execution_replay
from objc3c_runnable_release_candidate_e2e import run_packaged_execution_smoke
from objc3c_runnable_release_candidate_e2e import run_packaged_executable
from objc3c_runnable_release_candidate_e2e import validate_package_manifest
from objc3c_runnable_release_candidate_e2e import validate_release_candidate_fixture
from objc3c_runnable_release_candidate_e2e import write_summary_report

__all__ = [
    "COMPILE_ARTIFACT_FILES",
    "EXPECTED_VALIDATE_ARTIFACTS",
    "PACKAGE_CONTRACT_ID",
    "PACKAGE_PS1",
    "PWSH",
    "RELEASE_CANDIDATE_PROBES",
    "REPORT_PATH",
    "REQUIRED_MANIFEST_KEYS",
    "ROOT",
    "RUNNER_PATH",
    "ReleaseCandidateCliOptions",
    "ReleaseCandidateProbeCase",
    "SUMMARY_CONTRACT_ID",
    "assert_claim_probe_payload",
    "assert_compile_artifacts",
    "assert_evidence_probe_payload",
    "assert_validate_artifacts",
    "build_summary_payload",
    "collect_validate_artifacts",
    "compile_artifact_paths",
    "compile_release_candidate_fixture",
    "compile_release_candidate_probe",
    "expect",
    "load_and_validate_manifest",
    "main",
    "manifest_path",
    "package_runnable_toolchain",
    "parse_args",
    "parse_claim_probe_payload",
    "parse_evidence_probe_payload",
    "run_packaged_execution_replay",
    "run_packaged_execution_smoke",
    "run_packaged_executable",
    "validate_package_manifest",
    "validate_release_candidate_fixture",
    "write_summary_report",
]


if __name__ == "__main__":
    raise SystemExit(main())
