#!/usr/bin/env python3
"""Apply deterministic safe Objective-C 3 source rewrites."""

from __future__ import annotations

import argparse
from dataclasses import dataclass
import hashlib
import json
import re
from pathlib import Path

from objc3c_tooling.paths import display_path as repo_rel

from format_objc3c_source import FormatterDiagnostic, scan_source


ROOT = Path(__file__).resolve().parents[1]
REPORT_ROOT = ROOT / "tmp" / "reports" / "developer-tooling" / "source-rewrite"
SAFE_RULES = ("legacy-literal-aliases", "rename-symbol")
LEGACY_LITERAL_REPLACEMENTS = {
    "YES": "true",
    "NO": "false",
    "NULL": "nil",
}
IDENTIFIER_RE = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")


@dataclass(frozen=True)
class TextEdit:
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
            "range": {
                "start": {"line": self.line, "column": self.column},
                "end": {
                    "line": self.line,
                    "column": self.column + len(self.original),
                },
            },
            "start_offset": self.start_offset,
            "end_offset": self.end_offset,
        }


@dataclass(frozen=True)
class RewriteRuleSet:
    literal_aliases: bool
    renames: dict[str, str]

    @property
    def active_rule_ids(self) -> tuple[str, ...]:
        ids: list[str] = []
        if self.literal_aliases:
            ids.append("legacy-literal-aliases")
        if self.renames:
            ids.append("rename-symbol")
        return tuple(ids)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source")
    parser.add_argument(
        "--rule",
        choices=SAFE_RULES,
        action="append",
        help="Safe rewrite rule to apply. Defaults to legacy-literal-aliases.",
    )
    parser.add_argument(
        "--rename-symbol",
        action="append",
        default=[],
        metavar="OLD=NEW",
        help="Lexically safe identifier rename. Skips strings and comments.",
    )
    parser.add_argument("--output", help="Optional rewritten source output path.")
    return parser.parse_args()


def resolve_source(source_text: str) -> tuple[Path, str]:
    candidate = Path(source_text)
    resolved = candidate if candidate.is_absolute() else (ROOT / candidate)
    resolved = resolved.resolve()
    if not resolved.is_file():
        raise FileNotFoundError(f"source not found: {source_text}")
    return resolved, repo_rel(resolved)


def slugify(display_path: str) -> str:
    digest = hashlib.sha256(display_path.encode("utf-8")).hexdigest()[:12]
    stem = Path(display_path).stem.lower()
    safe = "".join(ch if ch.isalnum() else "-" for ch in stem).strip("-")
    return f"{safe or 'source'}-{digest}"


def parse_renames(raw_renames: list[str]) -> dict[str, str]:
    renames: dict[str, str] = {}
    for raw in raw_renames:
        if "=" not in raw:
            raise ValueError(f"rename must use OLD=NEW syntax: {raw}")
        old, new = raw.split("=", 1)
        if not IDENTIFIER_RE.fullmatch(old) or not IDENTIFIER_RE.fullmatch(new):
            raise ValueError(f"rename identifiers must be valid Objective-C 3 identifiers: {raw}")
        if old == new:
            continue
        renames[old] = new
    return renames


def build_rule_set(rules: list[str] | None, raw_renames: list[str]) -> RewriteRuleSet:
    selected = tuple(rules or ("legacy-literal-aliases",))
    renames = parse_renames(raw_renames)
    return RewriteRuleSet(
        literal_aliases="legacy-literal-aliases" in selected,
        renames=renames,
    )


def identifier_replacement(token: str, rules: RewriteRuleSet) -> tuple[str, str] | None:
    if rules.literal_aliases and token in LEGACY_LITERAL_REPLACEMENTS:
        return "legacy-literal-aliases", LEGACY_LITERAL_REPLACEMENTS[token]
    if token in rules.renames:
        return "rename-symbol", rules.renames[token]
    return None


def collect_safe_edits(source_text: str, rules: RewriteRuleSet) -> list[TextEdit]:
    edits: list[TextEdit] = []
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
                i += 2
                column += 2
                in_block_comment = False
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
        if match is not None:
            token = match.group(0)
            replacement = identifier_replacement(token, rules)
            if replacement is not None:
                rule_id, replacement_text = replacement
                edits.append(
                    TextEdit(
                        rule_id=rule_id,
                        original=token,
                        replacement=replacement_text,
                        start_offset=i,
                        end_offset=match.end(),
                        line=line,
                        column=column,
                    )
                )
            token_len = match.end() - i
            i = match.end()
            column += token_len
            continue
        i += 1
        column += 1
    return edits


def apply_edits(source_text: str, edits: list[TextEdit]) -> str:
    if not edits:
        return source_text
    chunks: list[str] = []
    cursor = 0
    for edit in sorted(edits, key=lambda item: item.start_offset):
        if edit.start_offset < cursor:
            raise ValueError("overlapping source rewrite edits are not safe to apply")
        chunks.append(source_text[cursor:edit.start_offset])
        chunks.append(edit.replacement)
        cursor = edit.end_offset
    chunks.append(source_text[cursor:])
    return "".join(chunks)


def sha256_text(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def build_rewrite_summary(
    *,
    source_display: str,
    rewritten_output_path: str,
    source_text: str,
    rewritten_text: str,
    edits: list[TextEdit],
    rules: RewriteRuleSet,
    diagnostics: tuple[FormatterDiagnostic, ...],
) -> dict[str, object]:
    status = "PASS" if not diagnostics else "FAIL"
    return {
        "contract_id": "objc3c.developer.tooling.source.rewrite.surface.v1",
        "source_path": source_display,
        "rewritten_output_path": rewritten_output_path,
        "supported": not diagnostics,
        "support_class": "deterministic-safe-rewrite" if not diagnostics else "fail-closed",
        "status": status,
        "active_rule_ids": list(rules.active_rule_ids),
        "edit_count": len(edits),
        "edits": [edit.as_payload() for edit in edits],
        "changed": source_text != rewritten_text,
        "source_sha256": sha256_text(source_text),
        "rewritten_sha256": sha256_text(rewritten_text),
        "diagnostics": [diagnostic.as_payload() for diagnostic in diagnostics],
    }


def build_rewrite_for_source(
    source_display: str,
    source_text: str,
    rewritten_output_path: str,
    rules: RewriteRuleSet,
) -> tuple[str, dict[str, object]]:
    scan = scan_source(source_text)
    edits = [] if not scan.ok else collect_safe_edits(source_text, rules)
    rewritten_text = source_text if not scan.ok else apply_edits(source_text, edits)
    summary = build_rewrite_summary(
        source_display=source_display,
        rewritten_output_path=rewritten_output_path,
        source_text=source_text,
        rewritten_text=rewritten_text,
        edits=edits,
        rules=rules,
        diagnostics=scan.diagnostics,
    )
    return rewritten_text if rewritten_text.endswith("\n") else rewritten_text + "\n", summary


def main() -> int:
    args = parse_args()
    source_path, source_display = resolve_source(args.source)
    rules = build_rule_set(args.rule, args.rename_symbol)
    slug = slugify(source_display)
    report_dir = REPORT_ROOT / slug
    report_dir.mkdir(parents=True, exist_ok=True)
    rewritten_output_path = Path(args.output) if args.output else report_dir / "rewritten.objc3"
    if not rewritten_output_path.is_absolute():
        rewritten_output_path = ROOT / rewritten_output_path
    summary_path = report_dir / "source-rewrite-summary.json"

    source_text = source_path.read_text(encoding="utf-8")
    rewritten_text, summary = build_rewrite_for_source(
        source_display,
        source_text,
        repo_rel(rewritten_output_path),
        rules,
    )
    rewritten_output_path.parent.mkdir(parents=True, exist_ok=True)
    rewritten_output_path.write_text(rewritten_text, encoding="utf-8")
    summary_path.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(summary_path)}")
    print(f"rewritten_output_path: {repo_rel(rewritten_output_path)}")
    print(json.dumps(summary, indent=2))
    return 0 if summary["supported"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
