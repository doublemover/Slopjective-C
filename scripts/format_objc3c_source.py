#!/usr/bin/env python3
"""Format objc3c source on the supported Objective-C 3 tooling subset."""

from __future__ import annotations

import argparse
from dataclasses import dataclass
import hashlib
import json
import re
from pathlib import Path
from objc3c_tooling.paths import display_path as repo_rel


ROOT = Path(__file__).resolve().parents[1]
REPORT_ROOT = ROOT / "tmp" / "reports" / "developer-tooling" / "formatter"
FORMATTER_SUBSET_ID = "objc3c.format.canonical-objc3-source-subset.v2"
CONTROL_KEYWORDS = {"if", "while", "for", "switch", "catch"}
PREFIX_OPERATORS = {"!", "~"}
BINARY_OPERATORS = {
    "=",
    "+",
    "-",
    "*",
    "/",
    "%",
    "==",
    "!=",
    "<",
    ">",
    "<=",
    ">=",
    "&&",
    "||",
    "&",
    "|",
    "^",
    "->",
    "?",
}
TOKEN_RE = re.compile(
    r'@?"(?:\\.|[^"\\])*"'
    r"|//.*"
    r"|/\*.*?\*/"
    r"|@?[A-Za-z_][A-Za-z0-9_]*"
    r"|\d+(?:\.\d+)?"
    r"|==|!=|<=|>=|&&|\|\||->|::"
    r"|[{}\[\]();,:+\-*/%=<>?!^.&|]"
    r"|\S"
)


@dataclass(frozen=True)
class FormatterDiagnostic:
    code: str
    message: str
    line: int
    column: int

    def as_payload(self) -> dict[str, object]:
        return {
            "code": self.code,
            "severity": "error",
            "message": self.message,
            "line": self.line,
            "column": self.column,
        }


@dataclass(frozen=True)
class SourceScan:
    ok: bool
    diagnostics: tuple[FormatterDiagnostic, ...]
    feature_ids: tuple[str, ...]
    max_delimiter_depth: int


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source")
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
    if not safe:
        safe = "source"
    return f"{safe}-{digest}"


def source_features(text: str) -> tuple[str, ...]:
    features: set[str] = set()
    marker_features = {
        "@interface": "objc-interface-declaration",
        "@implementation": "objc-implementation-declaration",
        "@protocol": "objc-protocol-declaration",
        "@end": "objc-at-end",
        "actor ": "actor-declaration",
        "await ": "await-expression",
        "macro ": "macro-declaration",
        "^": "block-or-bitwise-token",
        "[": "message-send-or-indexing",
        "@\"": "objc-string-literal",
    }
    for marker, feature in marker_features.items():
        if marker in text:
            features.add(feature)
    if "/*" in text:
        features.add("block-comment")
    if "//" in text:
        features.add("line-comment")
    return tuple(sorted(features))


def scan_source(text: str) -> SourceScan:
    diagnostics: list[FormatterDiagnostic] = []
    stack: list[tuple[str, int, int]] = []
    pairs = {"(": ")", "[": "]", "{": "}"}
    closers = {")": "(", "]": "[", "}": "{"}
    line = 1
    column = 1
    i = 0
    max_depth = 0
    quote: str | None = None
    quote_line = 1
    quote_column = 1
    block_comment_line = 1
    block_comment_column = 1
    in_line_comment = False
    in_block_comment = False
    escaped = False

    while i < len(text):
        ch = text[i]
        nxt = text[i + 1] if i + 1 < len(text) else ""

        if ch == "\n":
            line += 1
            column = 1
            in_line_comment = False
            escaped = False if quote is None else escaped
            i += 1
            continue

        if in_line_comment:
            column += 1
            i += 1
            continue

        if in_block_comment:
            if ch == "*" and nxt == "/":
                in_block_comment = False
                i += 2
                column += 2
                continue
            column += 1
            i += 1
            continue

        if quote is not None:
            if escaped:
                escaped = False
            elif ch == "\\":
                escaped = True
            elif ch == quote:
                quote = None
            column += 1
            i += 1
            continue

        if ch == "/" and nxt == "/":
            in_line_comment = True
            i += 2
            column += 2
            continue
        if ch == "/" and nxt == "*":
            in_block_comment = True
            block_comment_line = line
            block_comment_column = column
            i += 2
            column += 2
            continue
        if ch in {'"', "'"}:
            quote = ch
            quote_line = line
            quote_column = column
            column += 1
            i += 1
            continue
        if ch in pairs:
            stack.append((ch, line, column))
            max_depth = max(max_depth, len(stack))
        elif ch in closers:
            expected = closers[ch]
            if not stack or stack[-1][0] != expected:
                diagnostics.append(
                    FormatterDiagnostic(
                        "O3T101",
                        f"unbalanced closing delimiter {ch!r}",
                        line,
                        column,
                    )
                )
            else:
                stack.pop()
        column += 1
        i += 1

    if quote is not None:
        diagnostics.append(
            FormatterDiagnostic(
                "O3T102",
                "unterminated string literal",
                quote_line,
                quote_column,
            )
        )
    if in_block_comment:
        diagnostics.append(
            FormatterDiagnostic(
                "O3T103",
                "unterminated block comment",
                block_comment_line,
                block_comment_column,
            )
        )
    for opener, opener_line, opener_column in reversed(stack):
        diagnostics.append(
            FormatterDiagnostic(
                "O3T104",
                f"unclosed opening delimiter {opener!r}",
                opener_line,
                opener_column,
            )
        )
    return SourceScan(
        ok=not diagnostics,
        diagnostics=tuple(diagnostics),
        feature_ids=source_features(text),
        max_delimiter_depth=max_depth,
    )


def token_is_word_like(token: str) -> bool:
    return bool(
        re.fullmatch(r"@?[A-Za-z_][A-Za-z0-9_]*", token)
        or re.fullmatch(r"\d+(?:\.\d+)?", token)
        or token.startswith('"')
        or token.startswith('@"')
    )


def tokens_for_code(code: str) -> list[str]:
    return [match.group(0) for match in TOKEN_RE.finditer(code) if not match.group(0).isspace()]


def split_line_comment(line: str) -> tuple[str, str]:
    quote: str | None = None
    escaped = False
    in_block_comment = False
    i = 0
    while i < len(line):
        ch = line[i]
        nxt = line[i + 1] if i + 1 < len(line) else ""
        if in_block_comment:
            if ch == "*" and nxt == "/":
                in_block_comment = False
                i += 2
                continue
            i += 1
            continue
        if quote is not None:
            if escaped:
                escaped = False
            elif ch == "\\":
                escaped = True
            elif ch == quote:
                quote = None
            i += 1
            continue
        if ch == "/" and nxt == "*":
            in_block_comment = True
            i += 2
            continue
        if ch == "/" and nxt == "/":
            return line[:i], line[i:].strip()
        if ch in {'"', "'"}:
            quote = ch
        i += 1
    return line, ""


def needs_space(previous: str, token: str) -> bool:
    if not previous:
        return False
    if token in {
        ")",
        "]",
        "}",
        ";",
        ",",
        ".",
    }:
        return False
    if previous in {"(", "[", "{", ".", "->", "::"}:
        return False
    if token == "[" and previous in BINARY_OPERATORS:
        return True
    if previous == ":":
        return True
    if token in {"(", "["}:
        return previous in CONTROL_KEYWORDS
    if token == "{":
        return previous not in {"", "{"}
    if previous == ",":
        return True
    if token == ":":
        return False
    if token == "->" or previous == "->":
        return False
    if token == "::" or previous == "::":
        return False
    if token in PREFIX_OPERATORS:
        return previous not in {"(", "[", "{", ",", ":", "=", "?", "return"}
    if previous in PREFIX_OPERATORS:
        return False
    if token in BINARY_OPERATORS or previous in BINARY_OPERATORS:
        return True
    if token in {"&", "|"} or previous in {"&", "|"}:
        return True
    return token_is_word_like(previous) or token_is_word_like(token)


def normalize_code_segment(code: str) -> str:
    tokens = tokens_for_code(code.strip())
    output: list[str] = []
    previous = ""
    ternary_depth = 0
    for token in tokens:
        if token == ":" and ternary_depth > 0:
            if previous and (not output or output[-1] != " "):
                output.append(" ")
            output.append(token)
            previous = token
            ternary_depth -= 1
            continue
        if needs_space(previous, token):
            output.append(" ")
        output.append(token)
        if token == "?":
            ternary_depth += 1
        previous = token
    line = "".join(output).strip()
    line = re.sub(r"^([-+])\(", r"\1 (", line)
    line = re.sub(r"\b(fn\s+[A-Za-z_][A-Za-z0-9_]*)\s+\(", r"\1(", line)
    line = re.sub(r"^(if|while|for|switch|catch)\(", r"\1 (", line)
    line = re.sub(r"\}\s+else", "} else", line)
    line = re.sub(r" {2,}", " ", line)
    return line


def normalize_statement(line: str) -> str:
    code, comment = split_line_comment(line)
    normalized = normalize_code_segment(code)
    if comment:
        return f"{normalized} {comment}".strip() if normalized else comment
    return normalized


def opens_at_block(line: str) -> bool:
    return bool(
        re.match(r"^@(interface|implementation|protocol)\b", line)
        or (line.startswith("actor class ") and not line.endswith(";"))
    )


def count_unquoted(line: str, needle: str) -> int:
    return sum(1 for token in tokens_for_code(split_line_comment(line)[0]) if token == needle)


def format_source_text(text: str) -> tuple[str, bool, str]:
    scan = scan_source(text)
    if not scan.ok:
        reason = "; ".join(diagnostic.message for diagnostic in scan.diagnostics)
        return text if text.endswith("\n") else text + "\n", False, reason

    formatted_lines: list[str] = []
    indent = 0
    for raw_line in text.splitlines():
        stripped = raw_line.strip()
        if not stripped:
            formatted_lines.append("")
            continue
        if stripped.startswith("//"):
            formatted_lines.append(("  " * indent) + stripped)
            continue
        if stripped.startswith("}") or stripped.startswith("@end"):
            indent = max(indent - 1, 0)
        normalized = normalize_statement(stripped)
        formatted_lines.append(("  " * indent) + normalized)
        leading_close = 1 if stripped.startswith("}") else 0
        indent += max(count_unquoted(normalized, "{") - max(count_unquoted(normalized, "}") - leading_close, 0), 0)
        if opens_at_block(normalized) and not normalized.endswith("{"):
            indent += 1
    return "\n".join(formatted_lines).rstrip() + "\n", True, ""


def build_format_summary(
    source_display: str,
    formatted_output_path: str,
    changed: bool,
    supported: bool,
    reason: str,
    *,
    source_line_count: int,
    formatted_line_count: int,
) -> dict[str, object]:
    return {
        "contract_id": "objc3c.developer.tooling.formatter.surface.v1",
        "source_path": source_display,
        "supported": supported,
        "support_class": "canonical-objc3-source-formatting" if supported else "fail-closed",
        "preview_subset_id": FORMATTER_SUBSET_ID if supported else None,
        "formatted_output_path": formatted_output_path,
        "changed": changed,
        "source_line_count": source_line_count,
        "formatted_line_count": formatted_line_count,
        "retired_route_reason": reason,
    }


def build_format_summary_for_source(
    source_display: str,
    source_text: str,
    formatted_output_path: str,
) -> tuple[str, dict[str, object]]:
    formatted_text, supported, reason = format_source_text(source_text)
    scan = scan_source(source_text)
    summary = build_format_summary(
        source_display,
        formatted_output_path,
        formatted_text != source_text,
        supported,
        reason,
        source_line_count=len(source_text.splitlines()),
        formatted_line_count=len(formatted_text.splitlines()),
    )
    summary["feature_ids"] = list(scan.feature_ids)
    summary["diagnostics"] = [diagnostic.as_payload() for diagnostic in scan.diagnostics]
    summary["max_delimiter_depth"] = scan.max_delimiter_depth
    return formatted_text, summary


def main() -> int:
    args = parse_args()
    source_path, source_display = resolve_source(args.source)
    slug = slugify(source_display)
    report_dir = REPORT_ROOT / slug
    report_dir.mkdir(parents=True, exist_ok=True)
    formatted_output_path = report_dir / "formatted.objc3"
    summary_path = report_dir / "formatter-output.json"

    source_text = source_path.read_text(encoding="utf-8")
    formatted_text, summary = build_format_summary_for_source(
        source_display,
        source_text,
        repo_rel(formatted_output_path),
    )
    formatted_output_path.write_text(formatted_text, encoding="utf-8")
    summary_path.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(summary_path)}")
    print(f"formatted_output_path: {repo_rel(formatted_output_path)}")
    print(json.dumps(summary, indent=2))
    return 0 if summary["supported"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
