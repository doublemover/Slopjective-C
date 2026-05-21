from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MANIFEST_PATH = ROOT / "showcase" / "applicationFrameworkSamples" / "manifest.json"
CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "application_framework_samples"
    / "contract.json"
)
SUMMARY_PATH = ROOT / "tmp" / "reports" / "application-framework-samples" / "summary.json"
MANIFEST_CONTRACT_ID = "objc3c.application_framework_samples.v1"
WORKSPACE_CONTRACT_ID = "objc3c.application_framework_samples.workspace.v1"
CONTRACT_ID = "objc3c.application_framework_samples.contract.v1"
SUMMARY_CONTRACT_ID = "objc3c.application_framework_samples.summary.v1"
COMPILE_EMIT_PREFIX = "module"
