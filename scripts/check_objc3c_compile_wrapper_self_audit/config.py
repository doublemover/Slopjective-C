"""Configuration for the objc3c compile-wrapper self-audit checker."""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
WRAPPER = ROOT / "scripts" / "objc3c_native_compile.ps1"
FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "synthesized_accessor_property_lowering_positive.objc3"
)
REPORT_ROOT = ROOT / "tmp" / "reports" / "objc3c-compile-wrapper-self-audit"
RUN_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-compile-wrapper-self-audit"
TRUTHFULNESS_CONTRACT_ID = "objc3c.native.compile.output.truthfulness.v1"
PROVENANCE_CONTRACT_ID = "objc3c.native.compile.output.provenance.v1"
SELF_AUDIT_CONTRACT_ID = "objc3c.native.compile.wrapper.self_audit.v1"
WRAPPER_TRUTH_OWNER = "objc3c-native-compile-wrapper-truth"
WRAPPER_RESULT_OWNER = "objc3c-native-compile-wrapper-result"
WRAPPER_ARTIFACT_OWNER = "objc3c-native-compile-wrapper-artifact"
WRAPPER_STATUS_OWNER = "objc3c-native-compile-wrapper-status"

__all__ = [
    "FIXTURE",
    "PROVENANCE_CONTRACT_ID",
    "REPORT_ROOT",
    "ROOT",
    "RUN_ROOT",
    "SELF_AUDIT_CONTRACT_ID",
    "TRUTHFULNESS_CONTRACT_ID",
    "WRAPPER",
    "WRAPPER_ARTIFACT_OWNER",
    "WRAPPER_RESULT_OWNER",
    "WRAPPER_STATUS_OWNER",
    "WRAPPER_TRUTH_OWNER",
]
