from __future__ import annotations

from pathlib import Path

from objc3c_tooling.artifact_identity import current_host_artifact_identity

ROOT = Path(__file__).resolve().parents[2]
ARTIFACT_IDENTITY = current_host_artifact_identity()
REPORT_DIR = ROOT / "reports" / "claimability" / "cross-module-semantic-contracts-diagnostics"
JSON_OUT = REPORT_DIR / "cross_module_semantic_contracts_diagnostics_summary.json"
MD_OUT = REPORT_DIR / "cross_module_semantic_contracts_diagnostics_summary.md"
TMP_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-native" / "cross-module-semantic-contracts-diagnostics"

COMPILER = ROOT / ARTIFACT_IDENTITY.native_executable_relative_path


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8-sig")
