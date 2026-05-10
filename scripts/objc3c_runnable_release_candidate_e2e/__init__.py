"""Runnable release-candidate end-to-end checker helpers."""

from __future__ import annotations

from .artifacts import assert_compile_artifacts
from .artifacts import assert_validate_artifacts
from .artifacts import collect_validate_artifacts
from .artifacts import compile_artifact_paths
from .assertions import expect
from .catalog import COMPILE_ARTIFACT_FILES
from .catalog import EXPECTED_VALIDATE_ARTIFACTS
from .catalog import RELEASE_CANDIDATE_PROBES
from .catalog import REQUIRED_MANIFEST_KEYS
from .catalog import ReleaseCandidateProbeCase
from .cli import ReleaseCandidateCliOptions
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
from .manifest import validate_package_manifest
from .paths import PACKAGE_CONTRACT_ID
from .paths import PACKAGE_PS1
from .paths import PWSH
from .paths import REPORT_PATH
from .paths import ROOT
from .paths import RUNNER_PATH
from .paths import SUMMARY_CONTRACT_ID
from .report import build_summary_payload
from .report import write_summary_report
from .results import assert_claim_probe_payload
from .results import assert_evidence_probe_payload
from .results import parse_claim_probe_payload
from .results import parse_evidence_probe_payload
from .runner import main

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
