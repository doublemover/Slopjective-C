#!/usr/bin/env python3
from __future__ import annotations

import argparse
import hashlib
import json
import subprocess
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
FRONTEND_RUNNER = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
DEFAULT_SOURCE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
REPORT_ROOT = ROOT / "tmp" / "reports" / "developer-tooling" / "editor-surface"
ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "developer-tooling" / "editor-surface"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", nargs="?", default=DEFAULT_SOURCE.relative_to(ROOT).as_posix())
    return parser.parse_args()


def repo_rel(path: Path) -> str:
    path = path.resolve()
    try:
        return path.relative_to(ROOT).as_posix()
    except ValueError:
        return path.as_posix()


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


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


def extract_symbols(manifest: dict[str, Any]) -> list[dict[str, Any]]:
    symbol_groups = [
        ("globals", "global"),
        ("functions", "function"),
        ("interfaces", "interface"),
        ("implementations", "implementation"),
        ("protocols", "protocol"),
        ("categories", "category"),
    ]
    symbols: list[dict[str, Any]] = []
    for key, kind in symbol_groups:
        entries = manifest.get(key, [])
        if not isinstance(entries, list):
            continue
        for entry in entries:
            if not isinstance(entry, dict):
                continue
            name = entry.get("name")
            if key == "implementations":
                name = entry.get("class_name")
            if key == "categories":
                class_name = entry.get("class_name", "")
                category_name = entry.get("category_name", "")
                if class_name or category_name:
                    name = f"{class_name}({category_name})"
            if not isinstance(name, str) or not name:
                continue
            line = int(entry.get("line", 0) or 0)
            column = int(entry.get("column", 0) or 0)
            symbols.append(
                {
                    "name": name,
                    "kind": kind,
                    "line": line,
                    "column": column,
                    "end_line": line,
                    "end_column": column + len(name),
                }
            )
    symbols.sort(key=lambda symbol: (symbol["line"], symbol["column"], symbol["kind"], symbol["name"]))
    return symbols


def build_language_server_payload(
    summary: dict[str, Any],
    manifest_path_text: str | None,
    symbols: list[dict[str, Any]],
) -> dict[str, Any]:
    manifest_available = bool(manifest_path_text)
    supported_capabilities = [
        "publishDiagnostics",
        "documentSymbol" if manifest_available else None,
        "workspaceSymbol" if manifest_available else None,
        "definition" if manifest_available and symbols else None,
    ]
    supported_capabilities = [capability for capability in supported_capabilities if capability is not None]
    fallback_capabilities = [
        "references",
        "rename",
        "semanticTokens",
        "codeAction",
        "statementLevelStepping",
    ]
    capability_statuses = {
        "publishDiagnostics": {
            "supported": True,
            "support_class": "authoritative",
            "evidence": "diagnostics-json",
        },
        "documentSymbol": {
            "supported": manifest_available,
            "support_class": "manifest-backed" if manifest_available else "fail-closed",
            "fallback_behavior": "" if manifest_available else "disabled until compile emits manifest declarations",
        },
        "workspaceSymbol": {
            "supported": manifest_available,
            "support_class": "manifest-backed" if manifest_available else "fail-closed",
            "fallback_behavior": "" if manifest_available else "disabled until compile emits manifest declarations",
        },
        "definition": {
            "supported": manifest_available and bool(symbols),
            "support_class": "manifest-backed" if manifest_available and symbols else "fail-closed",
            "fallback_behavior": "" if manifest_available and symbols else "disabled until compile emits declaration coordinates",
        },
        "references": {
            "supported": False,
            "support_class": "fallback-only",
            "fallback_behavior": "not published; use documentSymbol/workspaceSymbol and definition on compile-owned declarations",
        },
        "rename": {
            "supported": False,
            "support_class": "fallback-only",
            "fallback_behavior": "not published; canonical compile graph has no rename contract yet",
        },
        "semanticTokens": {
            "supported": False,
            "support_class": "fallback-only",
            "fallback_behavior": "not published; no semantic token contract is emitted on the canonical toolchain path",
        },
        "codeAction": {
            "supported": False,
            "support_class": "fallback-only",
            "fallback_behavior": "not published; diagnostics remain actionable only through compile output and operator guidance",
        },
        "statementLevelStepping": {
            "supported": False,
            "support_class": "fallback-only",
            "fallback_behavior": "not published; statement stepping remains fail-closed pending line-table evidence",
        },
    }
    return {
        "contract_id": "objc3c.developer.tooling.language.server.capability.surface.v1",
        "summary_status_name": summary.get("observability", {}).get("status_name", ""),
        "manifest_backed_navigation": manifest_available,
        "supported_capability_ids": supported_capabilities,
        "fallback_only_capability_ids": fallback_capabilities,
        "capability_statuses": capability_statuses,
    }


def build_navigation_payload(source_display: str, manifest_path_text: str | None, symbols: list[dict[str, Any]]) -> dict[str, Any]:
    return {
        "contract_id": "objc3c.developer.tooling.navigation.index.v1",
        "source_path": source_display,
        "available": bool(manifest_path_text),
        "manifest_path": manifest_path_text,
        "symbol_count": len(symbols),
        "supported_symbol_kinds": sorted({symbol["kind"] for symbol in symbols}),
        "symbols": symbols,
        "fallback_reason": "" if manifest_path_text else "compile produced no manifest-backed declaration surface",
    }


def main() -> int:
    args = parse_args()
    source_path, source_display = resolve_source(args.source)
    slug = slugify(source_display)
    report_dir = REPORT_ROOT / slug
    artifact_dir = ARTIFACT_ROOT / slug
    report_dir.mkdir(parents=True, exist_ok=True)
    artifact_dir.mkdir(parents=True, exist_ok=True)

    summary_path = report_dir / "compile-summary.json"
    diagnostics_report_path = report_dir / "editor-surface.json"
    capabilities_path = report_dir / "language-server-capabilities.json"
    navigation_path = report_dir / "navigation-index.json"
    formatter_path = report_dir / "formatter-output.json"
    debug_path = report_dir / "debug-map.json"

    compile = subprocess.run(
        [
            str(FRONTEND_RUNNER),
            source_display,
            "--out-dir",
            repo_rel(artifact_dir),
            "--emit-prefix",
            "module",
            "--summary-out",
            repo_rel(summary_path),
        ],
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if compile.stdout:
        sys.stdout.write(compile.stdout)
    if compile.stderr:
        sys.stderr.write(compile.stderr)
    if not summary_path.is_file():
        return compile.returncode if compile.returncode != 0 else 1

    summary = load_json(summary_path)
    diagnostics_path_text = str(summary.get("paths", {}).get("diagnostics", ""))
    manifest_path_text = str(summary.get("paths", {}).get("manifest", "")) or None
    object_path_text = str(summary.get("paths", {}).get("object", "")) or None

    diagnostics_payload = load_json(ROOT / diagnostics_path_text) if diagnostics_path_text else {"diagnostics": []}
    manifest_payload = load_json(ROOT / manifest_path_text) if manifest_path_text else {}
    symbols = extract_symbols(manifest_payload)

    language_server = build_language_server_payload(summary, manifest_path_text, symbols)
    navigation = build_navigation_payload(source_display, manifest_path_text, symbols)
    formatter = {
        "contract_id": "objc3c.developer.tooling.formatter.surface.v1",
        "supported": False,
        "support_class": "deferred",
        "fallback_reason": "formatter implementation lands in M325-C003",
        "formatted_output_path": None,
    }
    debug = {
        "contract_id": "objc3c.developer.tooling.debug.map.surface.v1",
        "supported": False,
        "support_class": "deferred",
        "object_artifact_present": bool(object_path_text),
        "declaration_breakpoint_anchor_count": len(symbols),
        "fallback_reason": "debug-map and stepping implementation lands in M325-C003",
    }

    editor_surface = {
        "contract_id": "objc3c.developer.tooling.editor.surface.v1",
        "source_path": source_display,
        "summary_path": repo_rel(summary_path),
        "manifest_path": manifest_path_text,
        "diagnostics_path": diagnostics_path_text,
        "diagnostics": {
            "status_name": summary.get("observability", {}).get("status_name", ""),
            "total": len(diagnostics_payload.get("diagnostics", [])),
            "entries": diagnostics_payload.get("diagnostics", []),
        },
        "language_server": language_server,
        "navigation": navigation,
        "formatter": formatter,
        "debug": debug,
    }

    diagnostics_report_path.write_text(json.dumps(editor_surface, indent=2) + "\n", encoding="utf-8")
    capabilities_path.write_text(json.dumps(language_server, indent=2) + "\n", encoding="utf-8")
    navigation_path.write_text(json.dumps(navigation, indent=2) + "\n", encoding="utf-8")
    formatter_path.write_text(json.dumps(formatter, indent=2) + "\n", encoding="utf-8")
    debug_path.write_text(json.dumps(debug, indent=2) + "\n", encoding="utf-8")

    print(f"summary_path: {repo_rel(summary_path)}")
    print(f"dump_path: {repo_rel(diagnostics_report_path)}")
    print(f"capabilities_path: {repo_rel(capabilities_path)}")
    print(f"navigation_path: {repo_rel(navigation_path)}")
    print(f"formatter_path: {repo_rel(formatter_path)}")
    print(f"debug_path: {repo_rel(debug_path)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
