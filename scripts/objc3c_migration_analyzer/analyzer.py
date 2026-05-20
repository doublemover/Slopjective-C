from __future__ import annotations

from dataclasses import dataclass
from datetime import datetime, timezone
import hashlib
import json
import re
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_object, write_json_file, write_text_file
from objc3c_tooling.paths import ROOT, repo_rel, resolve_repo_path_inside

try:
    from format_objc3c_source import FormatterDiagnostic, scan_source
except ModuleNotFoundError:
    from scripts.format_objc3c_source import FormatterDiagnostic, scan_source


CONTRACT_ID = "objc3c.migration_analyzer.contract.v1"
INPUT_CONTRACT_ID = "objc3c.migration_analyzer.input.v1"
ANALYSIS_REPORT_CONTRACT_ID = "objc3c.migration_analyzer.report.v1"
REWRITE_REPORT_CONTRACT_ID = "objc3c.migration.rewrite.report.v1"
REWRITE_PLAN_CONTRACT_ID = "objc3c.migration.rewrite.plan.v1"
CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "adoption_legibility"
    / "migration_analyzer_contract.json"
)
REPORT_ROOT = ROOT / "tmp" / "reports" / "migration-analyzer"
ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "migration-analyzer"
REQUIRED_SURFACES = (
    "header-import-export",
    "abi-alignment",
    "foreign-type-diagnostics",
    "mixed-image-loading",
    "packaged-execution",
)
SOURCE_LANGUAGES = {"objective-c-2", "swift", "c++", "mixed"}
TARGET_PROFILES = {"objc3-canonical"}
FOREIGN_LANGUAGES = {"swift", "c++", "c", "objective-c-2"}
IDENTIFIER_RE = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")
LINE_IMPORT_RULES = (
    (
        "objc2-foundation-import",
        re.compile(r'^(\s*)#import\s+<Foundation/Foundation\.h>\s*$'),
        r"\1import Foundation;",
    ),
    (
        "objc2-quoted-header-import",
        re.compile(r'^(\s*)#import\s+"([^"]+)"\s*$'),
        r'\1import foreign "\2";',
    ),
    (
        "cpp-quoted-header-import",
        re.compile(r'^(\s*)#include\s+"([^"]+)"\s*$'),
        r'\1import cxx "\2";',
    ),
)
TOKEN_REPLACEMENTS = {
    "BOOL": ("objc2-bool-type", "Bool"),
    "YES": ("objc2-bool-literal", "true"),
    "NO": ("objc2-bool-literal", "false"),
    "NULL": ("objc2-null-literal", "nil"),
}


@dataclass(frozen=True)
class MigrationDiagnostic:
    code: str
    severity: str
    message: str
    line: int = 1
    column: int = 1
    surface: str = "migration-input"

    def as_payload(self) -> dict[str, object]:
        return {
            "code": self.code,
            "severity": self.severity,
            "message": self.message,
            "surface": self.surface,
            "range": {
                "start": {"line": self.line, "column": self.column},
                "end": {"line": self.line, "column": self.column},
            },
        }


@dataclass(frozen=True)
class RewriteEdit:
    rule_id: str
    original: str
    replacement: str
    start_offset: int
    end_offset: int
    line: int
    column: int

    def as_payload(self) -> dict[str, object]:
        return {
            "rule_id": self.rule_id,
            "original": self.original,
            "replacement": self.replacement,
            "start_offset": self.start_offset,
            "end_offset": self.end_offset,
            "range": {
                "start": {"line": self.line, "column": self.column},
                "end": {
                    "line": self.line,
                    "column": self.column + max(len(self.original), 1),
                },
            },
        }


@dataclass(frozen=True)
class MigrationReport:
    payload: dict[str, Any]
    source_text: str
    source_path: Path | None

    @property
    def ok(self) -> bool:
        return self.payload["status"] == "PASS"


def sha256_text(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def stable_digest(payload: dict[str, Any]) -> str:
    stable_payload = {
        key: value
        for key, value in payload.items()
        if key not in {"generated_at_utc", "deterministic_digest"}
    }
    return sha256_text(json.dumps(stable_payload, sort_keys=True, separators=(",", ":")))


def slugify(path_text: str) -> str:
    digest = hashlib.sha256(path_text.encode("utf-8")).hexdigest()[:12]
    stem = Path(path_text).stem.lower()
    safe = "".join(ch if ch.isalnum() else "-" for ch in stem).strip("-")
    return f"{safe or 'migration-source'}-{digest}"


def load_contract(path: Path = CONTRACT_PATH) -> dict[str, Any]:
    contract = load_json_object(path)
    if contract.get("contract_id") != CONTRACT_ID:
        raise ValueError(f"migration analyzer contract_id drifted: {path}")
    return contract


def load_migration_input(path: Path | str) -> dict[str, Any]:
    return load_json_object(resolve_repo_path_inside(path))


def default_analysis_report_path(input_path: Path | str) -> Path:
    return REPORT_ROOT / slugify(repo_rel(resolve_repo_path_inside(input_path))) / "analysis-report.json"


def default_rewrite_report_path(input_path: Path | str) -> Path:
    return REPORT_ROOT / slugify(repo_rel(resolve_repo_path_inside(input_path))) / "rewrite-report.json"


def diagnostic_from_formatter(diagnostic: FormatterDiagnostic) -> MigrationDiagnostic:
    return MigrationDiagnostic(
        "O3M007",
        "error",
        f"malformed migration source: {diagnostic.message}",
        diagnostic.line,
        diagnostic.column,
        "source-parse",
    )


def expect_input_shape(payload: dict[str, Any]) -> list[MigrationDiagnostic]:
    diagnostics: list[MigrationDiagnostic] = []
    if payload.get("contract_id") != INPUT_CONTRACT_ID:
        diagnostics.append(
            MigrationDiagnostic(
                "O3M002",
                "error",
                "migration input contract_id is missing or unsupported",
            )
        )
    if not isinstance(payload.get("source_path"), str) or not payload.get("source_path"):
        diagnostics.append(MigrationDiagnostic("O3M003", "error", "source_path must name a checked-in source file"))
    if payload.get("source_language") not in SOURCE_LANGUAGES:
        diagnostics.append(MigrationDiagnostic("O3M005", "error", "source_language is not a supported migration language"))
    if payload.get("target_profile") not in TARGET_PROFILES:
        diagnostics.append(MigrationDiagnostic("O3M006", "error", "target_profile must be objc3-canonical"))
    requested = payload.get("requested_surfaces")
    if not isinstance(requested, list) or not all(isinstance(item, str) for item in requested):
        diagnostics.append(MigrationDiagnostic("O3M008", "error", "requested_surfaces must be a string array"))
    else:
        unknown = sorted(set(requested) - set(REQUIRED_SURFACES))
        missing = [surface for surface in REQUIRED_SURFACES if surface not in requested]
        if unknown:
            diagnostics.append(MigrationDiagnostic("O3M009", "error", f"unknown migration surfaces: {', '.join(unknown)}"))
        if missing:
            diagnostics.append(MigrationDiagnostic("O3M010", "error", f"missing required migration surfaces: {', '.join(missing)}"))
    if not IDENTIFIER_RE.fullmatch(str(payload.get("module_name", ""))):
        diagnostics.append(MigrationDiagnostic("O3M011", "error", "module_name must be a valid identifier"))

    foreign_interfaces = payload.get("foreign_interfaces")
    if not isinstance(foreign_interfaces, list) or not foreign_interfaces:
        diagnostics.append(MigrationDiagnostic("O3M012", "error", "foreign_interfaces must declare Swift and C++ boundaries"))
    else:
        languages: set[str] = set()
        for index, interface in enumerate(foreign_interfaces, start=1):
            if not isinstance(interface, dict):
                diagnostics.append(MigrationDiagnostic("O3M013", "error", f"foreign interface {index} must be an object"))
                continue
            language = interface.get("language")
            if language not in FOREIGN_LANGUAGES:
                diagnostics.append(MigrationDiagnostic("O3M014", "error", f"foreign interface {index} has unsupported language"))
            else:
                languages.add(str(language))
            if not isinstance(interface.get("module"), str) or not interface.get("module"):
                diagnostics.append(MigrationDiagnostic("O3M015", "error", f"foreign interface {index} missing module"))
            symbols = interface.get("symbols")
            if (
                not isinstance(symbols, list)
                or not symbols
                or not all(isinstance(symbol, str) and symbol for symbol in symbols)
            ):
                diagnostics.append(MigrationDiagnostic("O3M016", "error", f"foreign interface {index} symbols must be non-empty strings"))
        for required_language in ("swift", "c++"):
            if required_language not in languages:
                diagnostics.append(MigrationDiagnostic("O3M017", "error", f"missing {required_language} foreign interface"))

    packaged_execution = payload.get("packaged_execution")
    if not isinstance(packaged_execution, dict) or not isinstance(packaged_execution.get("manifest"), str):
        diagnostics.append(MigrationDiagnostic("O3M018", "error", "packaged_execution.manifest must name checked-in package evidence"))
    return diagnostics


def resolve_input_paths(payload: dict[str, Any]) -> tuple[Path | None, Path | None, list[MigrationDiagnostic]]:
    diagnostics: list[MigrationDiagnostic] = []
    source_path: Path | None = None
    manifest_path: Path | None = None
    raw_source = payload.get("source_path")
    if isinstance(raw_source, str) and raw_source:
        try:
            source_path = resolve_repo_path_inside(raw_source)
            if not source_path.is_file():
                diagnostics.append(MigrationDiagnostic("O3M019", "error", f"source_path does not exist: {raw_source}"))
        except ValueError as exc:
            diagnostics.append(MigrationDiagnostic("O3M020", "error", str(exc)))
    packaged_execution = payload.get("packaged_execution")
    raw_manifest = packaged_execution.get("manifest") if isinstance(packaged_execution, dict) else None
    if isinstance(raw_manifest, str) and raw_manifest:
        try:
            manifest_path = resolve_repo_path_inside(raw_manifest)
            if not manifest_path.is_file():
                diagnostics.append(MigrationDiagnostic("O3M021", "error", f"packaged execution manifest does not exist: {raw_manifest}"))
        except ValueError as exc:
            diagnostics.append(MigrationDiagnostic("O3M022", "error", str(exc)))
    return source_path, manifest_path, diagnostics


def observed_surfaces(source_text: str, manifest_path: Path | None, payload: dict[str, Any]) -> dict[str, bool]:
    foreign_interfaces = payload.get("foreign_interfaces", [])
    foreign_languages = {
        str(interface.get("language"))
        for interface in foreign_interfaces
        if isinstance(interface, dict)
    }
    header_evidence = any(marker in source_text for marker in ("#import", "#include", "import "))
    abi_evidence = any(marker in source_text for marker in ("NSInteger", "NSUInteger", "BOOL", "NSString", "extern \"C\""))
    foreign_evidence = any(marker in source_text for marker in ("Swift", "Cpp", "std::", "@objc", "extern \"C\""))
    mixed_image_evidence = (
        ("Swift" in source_text and ("Cpp" in source_text or "#include" in source_text))
        or {"swift", "c++"}.issubset(foreign_languages)
    )
    return {
        "header-import-export": header_evidence,
        "abi-alignment": abi_evidence,
        "foreign-type-diagnostics": foreign_evidence,
        "mixed-image-loading": mixed_image_evidence,
        "packaged-execution": bool(manifest_path and manifest_path.is_file()),
    }


def surface_diagnostics(surfaces: dict[str, bool], source_text: str) -> list[MigrationDiagnostic]:
    diagnostics: list[MigrationDiagnostic] = []
    codes = {
        "header-import-export": "O3M101",
        "abi-alignment": "O3M102",
        "foreign-type-diagnostics": "O3M103",
        "mixed-image-loading": "O3M104",
        "packaged-execution": "O3M105",
    }
    for surface, observed in surfaces.items():
        if not observed:
            diagnostics.append(MigrationDiagnostic(codes[surface], "error", f"missing {surface} migration evidence", surface=surface))
    unsafe_markers = ("dlopen(", "NSClassFromString(", "reinterpret_cast<")
    for marker in unsafe_markers:
        offset = source_text.find(marker)
        if offset >= 0:
            line = source_text.count("\n", 0, offset) + 1
            column = offset - source_text.rfind("\n", 0, offset)
            diagnostics.append(
                MigrationDiagnostic(
                    "O3M210",
                    "error",
                    f"unsafe mixed-image loading marker is not rewriteable: {marker}",
                    line,
                    column,
                    "mixed-image-loading",
                )
            )
    return diagnostics


def line_offsets(text: str) -> list[int]:
    offsets = [0]
    for index, ch in enumerate(text):
        if ch == "\n":
            offsets.append(index + 1)
    return offsets


def collect_line_import_edits(source_text: str) -> list[RewriteEdit]:
    edits: list[RewriteEdit] = []
    cursor = 0
    for line_number, line in enumerate(source_text.splitlines(keepends=True), start=1):
        line_without_newline = line.rstrip("\r\n")
        for rule_id, pattern, replacement in LINE_IMPORT_RULES:
            if not pattern.match(line_without_newline):
                continue
            replaced = pattern.sub(replacement, line_without_newline)
            edits.append(
                RewriteEdit(
                    rule_id=rule_id,
                    original=line_without_newline,
                    replacement=replaced,
                    start_offset=cursor,
                    end_offset=cursor + len(line_without_newline),
                    line=line_number,
                    column=1,
                )
            )
            break
        cursor += len(line)
    return edits


def collect_token_edits(source_text: str) -> list[RewriteEdit]:
    edits: list[RewriteEdit] = []
    i = 0
    line = 1
    column = 1
    quote: str | None = None
    escaped = False
    in_line_comment = False
    in_block_comment = False
    while i < len(source_text):
        ch = source_text[i]
        nxt = source_text[i + 1] if i + 1 < len(source_text) else ""
        if ch == "\n":
            line += 1
            column = 1
            in_line_comment = False
            escaped = False if quote is None else escaped
            i += 1
            continue
        if in_line_comment:
            i += 1
            column += 1
            continue
        if in_block_comment:
            if ch == "*" and nxt == "/":
                in_block_comment = False
                i += 2
                column += 2
                continue
            i += 1
            column += 1
            continue
        if quote is not None:
            if escaped:
                escaped = False
            elif ch == "\\":
                escaped = True
            elif ch == quote:
                quote = None
            i += 1
            column += 1
            continue
        if ch == "/" and nxt == "/":
            in_line_comment = True
            i += 2
            column += 2
            continue
        if ch == "/" and nxt == "*":
            in_block_comment = True
            i += 2
            column += 2
            continue
        if ch in {'"', "'"}:
            quote = ch
            i += 1
            column += 1
            continue
        match = IDENTIFIER_RE.match(source_text, i)
        if match:
            token = match.group(0)
            replacement = TOKEN_REPLACEMENTS.get(token)
            if replacement:
                rule_id, replacement_text = replacement
                edits.append(
                    RewriteEdit(
                        rule_id=rule_id,
                        original=token,
                        replacement=replacement_text,
                        start_offset=i,
                        end_offset=match.end(),
                        line=line,
                        column=column,
                    )
                )
            token_length = match.end() - i
            i = match.end()
            column += token_length
            continue
        i += 1
        column += 1
    return edits


def collect_rewrite_edits(source_text: str) -> list[RewriteEdit]:
    edits = [*collect_line_import_edits(source_text), *collect_token_edits(source_text)]
    return sorted(edits, key=lambda edit: edit.start_offset)


def build_manual_steps(source_text: str) -> list[dict[str, object]]:
    steps: list[dict[str, object]] = []
    manual_rules = (
        ("objc2-interface-shape", "@interface", "rewrite interface declarations into canonical Objective-C 3 class or protocol declarations"),
        ("objc2-implementation-shape", "@implementation", "rewrite implementation blocks into canonical Objective-C 3 method bodies"),
        ("objc2-message-send-shape", "[", "rewrite bracket message sends into checked Objective-C 3 dispatch syntax after selector ownership is known"),
        ("foreign-type-ownership", "Cpp", "bind C++ foreign types through explicit imported-module owner contracts before publishing ABI claims"),
        ("swift-bridge-ownership", "Swift", "bind Swift bridge symbols through explicit imported-module owner contracts before publishing mixed-image claims"),
    )
    for rule_id, marker, description in manual_rules:
        if marker in source_text:
            steps.append(
                {
                    "rule_id": rule_id,
                    "marker": marker,
                    "description": description,
                    "automatic": False,
                }
            )
    return steps


def build_rewrite_plan(source_text: str, diagnostics: list[MigrationDiagnostic]) -> dict[str, Any]:
    edits = [] if diagnostics else collect_rewrite_edits(source_text)
    manual_steps = build_manual_steps(source_text)
    return {
        "contract_id": REWRITE_PLAN_CONTRACT_ID,
        "safe_to_apply": not diagnostics,
        "automatic_edit_count": len(edits),
        "manual_step_count": len(manual_steps),
        "edits": [edit.as_payload() for edit in edits],
        "manual_steps": manual_steps,
        "claim_boundary": (
            "automatic edits are token-boundary and import-line rewrites only; "
            "manual steps are planning surfaces and do not claim retired-source acceptance"
        ),
    }


def build_report_payload(
    *,
    input_path: Path,
    source_path: Path | None,
    manifest_path: Path | None,
    source_text: str,
    payload: dict[str, Any],
    contract: dict[str, Any],
    diagnostics: list[MigrationDiagnostic],
    surfaces: dict[str, bool],
) -> dict[str, Any]:
    report: dict[str, Any] = {
        "contract_id": ANALYSIS_REPORT_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
        "status": "PASS" if not diagnostics else "FAIL",
        "issue_ids": [8077, 8078],
        "input_path": repo_rel(input_path),
        "source_path": repo_rel(source_path) if source_path else None,
        "contract_path": repo_rel(CONTRACT_PATH),
        "source_language": payload.get("source_language"),
        "target_profile": payload.get("target_profile"),
        "module_name": payload.get("module_name"),
        "requested_surfaces": payload.get("requested_surfaces", []),
        "observed_surfaces": surfaces,
        "packaged_execution_manifest": repo_rel(manifest_path) if manifest_path else None,
        "diagnostics": [diagnostic.as_payload() for diagnostic in diagnostics],
        "source_sha256": sha256_text(source_text),
        "source_contracts": contract.get("source_contracts", []),
        "public_actions": contract.get("public_actions", []),
        "rewrite_plan": build_rewrite_plan(source_text, diagnostics),
        "support_boundary": contract.get("support_boundary"),
        "fail_closed": bool(diagnostics),
    }
    report["deterministic_digest"] = stable_digest(report)
    return report


def analyze_migration_input(input_path: Path | str, *, contract_path: Path = CONTRACT_PATH) -> MigrationReport:
    resolved_input_path = resolve_repo_path_inside(input_path)
    contract = load_contract(contract_path)
    payload = load_json_object(resolved_input_path)
    diagnostics = expect_input_shape(payload)
    source_path, manifest_path, path_diagnostics = resolve_input_paths(payload)
    diagnostics.extend(path_diagnostics)
    source_text = ""
    if source_path and source_path.is_file():
        source_text = source_path.read_text(encoding="utf-8")
        scan = scan_source(source_text)
        diagnostics.extend(diagnostic_from_formatter(item) for item in scan.diagnostics)
    surfaces = observed_surfaces(source_text, manifest_path, payload)
    diagnostics.extend(surface_diagnostics(surfaces, source_text))
    report = build_report_payload(
        input_path=resolved_input_path,
        source_path=source_path,
        manifest_path=manifest_path,
        source_text=source_text,
        payload=payload,
        contract=contract,
        diagnostics=diagnostics,
        surfaces=surfaces,
    )
    return MigrationReport(payload=report, source_text=source_text, source_path=source_path)


def edit_from_payload(payload: dict[str, Any]) -> RewriteEdit:
    start = payload["range"]["start"]
    return RewriteEdit(
        rule_id=str(payload["rule_id"]),
        original=str(payload["original"]),
        replacement=str(payload["replacement"]),
        start_offset=int(payload["start_offset"]),
        end_offset=int(payload["end_offset"]),
        line=int(start["line"]),
        column=int(start["column"]),
    )


def apply_rewrite_plan(source_text: str, rewrite_plan: dict[str, Any]) -> str:
    edits = [edit_from_payload(edit) for edit in rewrite_plan.get("edits", []) if isinstance(edit, dict)]
    chunks: list[str] = []
    cursor = 0
    for edit in sorted(edits, key=lambda item: item.start_offset):
        if edit.start_offset < cursor:
            raise ValueError("overlapping migration rewrite edits are not safe to apply")
        if source_text[edit.start_offset : edit.end_offset] != edit.original:
            raise ValueError(f"migration rewrite edit drifted for {edit.rule_id}")
        chunks.append(source_text[cursor : edit.start_offset])
        chunks.append(edit.replacement)
        cursor = edit.end_offset
    chunks.append(source_text[cursor:])
    rewritten = "".join(chunks)
    return rewritten if rewritten.endswith("\n") else rewritten + "\n"


def build_rewrite_workflow_report(
    analysis: MigrationReport,
    *,
    input_path: Path | str,
    rewritten_output_path: Path | None,
    rewritten_text: str | None,
    analysis_report_path: Path | None,
) -> dict[str, Any]:
    diagnostics = analysis.payload.get("diagnostics", [])
    report = {
        "contract_id": REWRITE_REPORT_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
        "status": "PASS" if analysis.ok and rewritten_text is not None else "FAIL",
        "issue_ids": [8077, 8078],
        "input_path": repo_rel(resolve_repo_path_inside(input_path)),
        "analysis_report_path": repo_rel(analysis_report_path) if analysis_report_path else None,
        "source_path": analysis.payload.get("source_path"),
        "rewritten_output_path": repo_rel(rewritten_output_path) if rewritten_output_path else None,
        "applied_edit_count": analysis.payload["rewrite_plan"]["automatic_edit_count"] if analysis.ok else 0,
        "manual_step_count": analysis.payload["rewrite_plan"]["manual_step_count"],
        "changed": bool(rewritten_text is not None and rewritten_text != analysis.source_text),
        "diagnostics": diagnostics,
        "source_sha256": sha256_text(analysis.source_text),
        "rewritten_sha256": sha256_text(rewritten_text) if rewritten_text is not None else None,
        "rewrite_plan": analysis.payload["rewrite_plan"],
        "support_boundary": analysis.payload.get("support_boundary"),
    }
    report["deterministic_digest"] = stable_digest(report)
    return report


def write_analysis_report(report: MigrationReport, path: Path) -> None:
    write_json_file(path, report.payload)


def write_rewrite_report(report: dict[str, Any], path: Path) -> None:
    write_json_file(path, report)


def write_rewritten_source(path: Path, rewritten_text: str) -> None:
    write_text_file(path, rewritten_text)
