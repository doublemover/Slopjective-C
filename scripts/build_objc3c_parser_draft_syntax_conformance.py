from __future__ import annotations

import argparse
import re
import shutil
import subprocess
from pathlib import Path
from typing import Any

from objc3c_tooling.reports import expected_json_report
from objc3c_tooling.reports import write_report_outputs
from objc3c_tooling.json_io import load_json_any as load_json

ROOT = Path(__file__).resolve().parents[1]
CONFORMANCE_MANIFEST = ROOT / "tests" / "conformance" / "parser" / "draft_syntax_surface_conformance.json"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "dispatch" / "parser_draft_syntax_surfaces.objc3"
PARSER = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
README = ROOT / "tests" / "conformance" / "parser" / "README.md"
COMPILER = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
REPORT_DIR = ROOT / "reports" / "claimability" / "parser-draft-syntax-conformance"
JSON_OUT = REPORT_DIR / "parser_draft_syntax_conformance_summary.json"
MD_OUT = REPORT_DIR / "parser_draft_syntax_conformance_summary.md"
TMP_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-native" / "parser-draft-syntax-conformance"

CONTRACT_ID = "objc3c.parser.draft-syntax-conformance.v1"
REQUIRED_SURFACES = [
    "block_literal",
    "try_expression",
    "throw_statement",
    "do_catch",
    "throws_callable",
    "async_callable",
    "await_expression",
    "actor_interface",
    "macro_attribute",
    "macro_package",
    "macro_provenance",
    "property_behavior",
    "interop_attribute",
]
REPLAY_KEY_FIELDS = {
    "block_literal": "blocks",
    "try_expression": "try",
    "throw_statement": "throw",
    "do_catch": "do_catch",
    "throws_callable": "throws_callables",
    "async_callable": "async_callables",
    "await_expression": "await",
    "actor_interface": "actors",
    "macro_attribute": "macro_attrs",
    "macro_package": "macro_packages",
    "macro_provenance": "macro_provenance",
    "property_behavior": "property_behaviors",
    "interop_attribute": "interop_attrs",
}
HEADER_RE = re.compile(r"(?mi)^\s*//\s*Expected diagnostic code\(s\):\s*(.+?)\s*$")
CODE_RE = re.compile(r"O3[A-Z]\d{3}")


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8-sig")



def run_compiler(source: Path, out_dir: Path) -> dict[str, Any]:
    if not COMPILER.is_file():
        raise SystemExit(f"missing native compiler at {rel(COMPILER)}; run scripts/build_objc3c_native.ps1 first")
    if out_dir.exists():
        shutil.rmtree(out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    completed = subprocess.run(
        [str(COMPILER), str(source), "--out-dir", str(out_dir), "--emit-prefix", "module"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    diagnostics_path = out_dir / "module.diagnostics.json"
    manifest_path = out_dir / "module.manifest.json"
    diagnostics = []
    if diagnostics_path.is_file():
        diagnostics = load_json(diagnostics_path).get("diagnostics", [])
    manifest = None
    if manifest_path.is_file():
        manifest = load_json(manifest_path)
    return {
        "exit_code": completed.returncode,
        "stdout": completed.stdout.strip(),
        "stderr": completed.stderr.strip(),
        "diagnostics": diagnostics,
        "manifest": manifest,
    }


def find_parser_manifest(node: Any) -> dict[str, Any] | None:
    if isinstance(node, dict):
        if {
            "draft_syntax_surface_count",
            "draft_syntax_surface_fingerprint",
            "draft_syntax_surface_handoff_key",
            "draft_syntax_surface_deterministic",
        }.issubset(node.keys()):
            return node
        for value in node.values():
            found = find_parser_manifest(value)
            if found is not None:
                return found
    if isinstance(node, list):
        for value in node:
            found = find_parser_manifest(value)
            if found is not None:
                return found
    return None


def parse_replay_key(key: str) -> dict[str, int]:
    fields: dict[str, int] = {}
    for part in key.split(";"):
        if "=" not in part:
            continue
        name, value = part.split("=", 1)
        try:
            fields[name] = int(value)
        except ValueError:
            continue
    return fields


def expected_code_header(path: Path) -> list[str]:
    text = read(path)
    match = HEADER_RE.search(text)
    if not match:
        return []
    return [code.upper() for code in CODE_RE.findall(match.group(1))]


def diagnostic_matches(diagnostics: list[dict[str, Any]], expected: dict[str, Any]) -> bool:
    return any(
        diag.get("code") == expected["code"]
        and int(diag.get("line", -1)) == int(expected["line"])
        and int(diag.get("column", -1)) == int(expected["column"])
        for diag in diagnostics
    )


def build_summary() -> dict[str, Any]:
    manifest = load_json(CONFORMANCE_MANIFEST)
    positive_text = read(POSITIVE_FIXTURE)
    parser_text = read(PARSER)
    readme_text = read(README)
    surfaces = manifest["surfaces"]
    by_id = {surface["id"]: surface for surface in surfaces}
    missing_surfaces = [surface_id for surface_id in REQUIRED_SURFACES if surface_id not in by_id]
    extra_surfaces = [surface["id"] for surface in surfaces if surface["id"] not in REQUIRED_SURFACES]

    positive_run = run_compiler(POSITIVE_FIXTURE, TMP_ROOT / "positive")
    parser_manifest = find_parser_manifest(positive_run.get("manifest")) if positive_run.get("manifest") else None
    replay_fields = parse_replay_key(parser_manifest.get("draft_syntax_surface_handoff_key", "")) if parser_manifest else {}

    surface_results = []
    compiled_negative_cache: dict[str, dict[str, Any]] = {}
    for surface in surfaces:
        positive_tokens = surface.get("positive_tokens", [])
        positive_token_results = {token: token in positive_text for token in positive_tokens}
        replay_field = REPLAY_KEY_FIELDS.get(surface["id"], "")
        replay_count = replay_fields.get(replay_field, 0)
        negative_entries = [
            {
                "fixture": surface["negative_fixture"],
                "expected_diagnostic": surface["expected_diagnostic"],
            }
        ] + surface.get("additional_negative_fixtures", [])
        negative_results = []
        for entry in negative_entries:
            fixture = ROOT / entry["fixture"]
            if entry["fixture"] not in compiled_negative_cache:
                compiled_negative_cache[entry["fixture"]] = run_compiler(
                    fixture, TMP_ROOT / fixture.stem
                )
            run = compiled_negative_cache[entry["fixture"]]
            expected = entry["expected_diagnostic"]
            header_codes = expected_code_header(fixture)
            negative_results.append(
                {
                    "fixture": entry["fixture"],
                    "expected": expected,
                    "header_codes": header_codes,
                    "exit_code": run["exit_code"],
                    "observed_diagnostics": [
                        {
                            "code": diag.get("code"),
                            "line": diag.get("line"),
                            "column": diag.get("column"),
                            "message": diag.get("message"),
                        }
                        for diag in run["diagnostics"]
                    ],
                    "header_matches_expected_code": expected["code"] in header_codes,
                    "compiled_fail_closed": run["exit_code"] != 0,
                    "expected_location_observed": diagnostic_matches(run["diagnostics"], expected),
                }
            )
        surface_results.append(
            {
                "id": surface["id"],
                "summary_field": surface["summary_field"],
                "positive_tokens": positive_token_results,
                "replay_key_field": replay_field,
                "replay_key_count": replay_count,
                "positive_tokens_present": all(positive_token_results.values()),
                "positive_manifest_count_present": replay_count > 0,
                "negative_results": negative_results,
                "status": "PASS"
                if all(positive_token_results.values())
                and replay_count > 0
                and all(
                    result["header_matches_expected_code"]
                    and result["compiled_fail_closed"]
                    and result["expected_location_observed"]
                    for result in negative_results
                )
                else "FAIL",
            }
        )

    source_truth_paths = [
        CONFORMANCE_MANIFEST,
        POSITIVE_FIXTURE,
        PARSER,
        README,
        *[
            path
            for path in sorted(
                {
                    ROOT / result["fixture"]
                    for surface in surfaces
                    for result in (
                        [{"fixture": surface["negative_fixture"]}]
                        + surface.get("additional_negative_fixtures", [])
                    )
                },
                key=lambda item: item.as_posix(),
            )
        ],
    ]
    no_tmp_source_truth = all(not rel(path).startswith("tmp/") for path in source_truth_paths)
    checks = {
        "manifest_contract_matches": manifest.get("contract_id") == CONTRACT_ID,
        "all_required_surfaces_listed": not missing_surfaces,
        "no_unknown_surfaces_listed": not extra_surfaces,
        "positive_fixture_compiles": positive_run["exit_code"] == 0,
        "positive_manifest_has_parser_surface": parser_manifest is not None,
        "positive_manifest_deterministic": bool(parser_manifest and parser_manifest.get("draft_syntax_surface_deterministic")),
        "positive_manifest_total_covers_surface_counts": bool(
            parser_manifest
            and int(parser_manifest.get("draft_syntax_surface_count", 0))
            >= sum(replay_fields.get(REPLAY_KEY_FIELDS[surface_id], 0) for surface_id in REQUIRED_SURFACES)
        ),
        "parser_has_property_behavior_payload_guard": "O3P355" in parser_text,
        "readme_references_conformance_manifest": rel(CONFORMANCE_MANIFEST) in readme_text,
        "no_tmp_source_truth": no_tmp_source_truth,
        "all_surface_results_pass": all(result["status"] == "PASS" for result in surface_results),
    }
    status = "PASS" if all(checks.values()) else "FAIL"
    return {
        "contract_id": CONTRACT_ID,
        "issue": "#8012",
        "status": status,
        "checks": checks,
        "missing_surfaces": missing_surfaces,
        "extra_surfaces": extra_surfaces,
        "source_truth_paths": [rel(path) for path in source_truth_paths],
        "positive_fixture": rel(POSITIVE_FIXTURE),
        "positive_compile_exit_code": positive_run["exit_code"],
        "positive_manifest_parser_surface": parser_manifest,
        "replay_key_counts": replay_fields,
        "surface_results": surface_results,
        "validation_commands": [
            "python scripts/build_objc3c_parser_draft_syntax_conformance.py --check",
            "python -m pytest tests/tooling/test_build_objc3c_parser_draft_syntax_conformance.py",
            "npm run test:objc3c:fixture-matrix",
            "npm run test:objc3c:negative-expectations",
            "npm run test:objc3c:execution-replay-proof",
        ],
    }


def render_markdown(summary: dict[str, Any]) -> str:
    lines = [
        "# Parser Draft Syntax Conformance",
        "",
        f"- Contract: `{summary['contract_id']}`",
        f"- Status: `{summary['status']}`",
        f"- Issue: `{summary['issue']}`",
        f"- Positive fixture: `{summary['positive_fixture']}`",
        "",
        "## Checks",
    ]
    for name, passed in summary["checks"].items():
        lines.append(f"- `{name}`: `{'PASS' if passed else 'FAIL'}`")
    lines.extend(["", "## Surface Coverage"])
    for result in summary["surface_results"]:
        negative_count = len(result["negative_results"])
        lines.append(
            f"- `{result['id']}`: `{result['status']}`; "
            f"replay `{result['replay_key_field']}`=`{result['replay_key_count']}`; "
            f"negative fixtures=`{negative_count}`"
        )
    lines.extend(["", "## Validation Commands"])
    for command in summary["validation_commands"]:
        lines.append(f"- `{command}`")
    lines.append("")
    return "\n".join(lines)


def write_outputs(summary: dict[str, Any]) -> None:
    write_report_outputs(
        summary=summary,
        json_path=JSON_OUT,
        markdown_path=MD_OUT,
        markdown=render_markdown(summary),
    )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    summary = build_summary()
    expected_json = expected_json_report(summary)
    expected_md = render_markdown(summary)
    if args.check:
        if not JSON_OUT.is_file() or JSON_OUT.read_text(encoding="utf-8") != expected_json:
            raise SystemExit(f"{rel(JSON_OUT)} is stale; run this script without --check")
        if not MD_OUT.is_file() or MD_OUT.read_text(encoding="utf-8") != expected_md:
            raise SystemExit(f"{rel(MD_OUT)} is stale; run this script without --check")
        if summary["status"] != "PASS":
            raise SystemExit("parser draft syntax conformance summary failed")
        print(f"status: {summary['status']}")
        print(f"summary_path: {rel(JSON_OUT)}")
        return 0
    write_outputs(summary)
    print(f"wrote: {rel(JSON_OUT)}")
    print(f"wrote: {rel(MD_OUT)}")
    print(f"status: {summary['status']}")
    return 0 if summary["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
