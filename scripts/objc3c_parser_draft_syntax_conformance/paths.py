from __future__ import annotations

from pathlib import Path

from objc3c_tooling.artifact_identity import current_host_artifact_identity

ROOT = Path(__file__).resolve().parents[2]
ARTIFACT_IDENTITY = current_host_artifact_identity()
CONFORMANCE_MANIFEST = ROOT / "tests" / "conformance" / "parser" / "draft_syntax_surface_conformance.json"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "dispatch" / "parser_draft_syntax_surfaces.objc3"
PARSER = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
README = ROOT / "tests" / "conformance" / "parser" / "README.md"
COMPILER = ROOT / ARTIFACT_IDENTITY.native_executable_relative_path
REPORT_DIR = ROOT / "reports" / "claimability" / "parser-draft-syntax-conformance"
JSON_OUT = REPORT_DIR / "parser_draft_syntax_conformance_summary.json"
MD_OUT = REPORT_DIR / "parser_draft_syntax_conformance_summary.md"
TMP_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-native" / "parser-draft-syntax-conformance"

CONTRACT_ID = "objc3c.parser.draft-syntax-conformance.v1"


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8-sig")
