#!/usr/bin/env python3
from __future__ import annotations

import argparse
import hashlib
import json
import sys
from pathlib import Path
from typing import Any, Sequence

from objc3c_tooling.json_io import load_json_object
from objc3c_tooling.paths import display_path as repo_rel, resolve_repo_path
from objc3c_tooling.reports import write_report_outputs

try:
    from support_classification_contracts import (
        ContractError,
        SupportClassificationSource,
        build_support_classification_report,
    )
except ModuleNotFoundError:
    from scripts.support_classification_contracts import (
        ContractError,
        SupportClassificationSource,
        build_support_classification_report,
    )

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_CONTRACT = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "full_envelope_claimability"
    / "support_matrix_claim_taxonomy.json"
)
DEFAULT_JSON_OUT = (
    ROOT
    / "reports"
    / "claimability"
    / "support-classification"
    / "support_classification_summary.json"
)
DEFAULT_MD_OUT = (
    ROOT
    / "reports"
    / "claimability"
    / "support-classification"
    / "support_classification_summary.md"
)


def load_json(path: Path) -> dict[str, Any]:
    try:
        payload = load_json_object(path)
    except json.JSONDecodeError as exc:
        raise ContractError(f"invalid JSON at {repo_rel(path)}: {exc}") from exc
    if not isinstance(payload, dict):
        raise ContractError(f"JSON object expected at {repo_rel(path)}")
    return payload


def stable_digest(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def path_exists(path_text: str) -> bool:
    return resolve_repo_path(path_text).exists()


def write_outputs(
    summary: dict[str, Any],
    markdown: str,
    json_out: Path,
    md_out: Path,
) -> None:
    write_report_outputs(
        summary=summary,
        json_path=json_out,
        markdown_path=md_out,
        markdown=markdown,
        sort_keys=False,
    )


def build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--contract", type=Path, default=DEFAULT_CONTRACT)
    parser.add_argument("--summary-json", type=Path, default=DEFAULT_JSON_OUT)
    parser.add_argument("--summary-md", type=Path, default=DEFAULT_MD_OUT)
    parser.add_argument(
        "--check",
        action="store_true",
        help="fail if the existing outputs differ from the generated summary",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_arg_parser().parse_args(argv)
    contract_path = args.contract if args.contract.is_absolute() else ROOT / args.contract
    json_out = (
        args.summary_json
        if args.summary_json.is_absolute()
        else ROOT / args.summary_json
    )
    md_out = args.summary_md if args.summary_md.is_absolute() else ROOT / args.summary_md
    try:
        source = SupportClassificationSource(
            contract=load_json(contract_path),
            contract_path=repo_rel(contract_path),
            contract_sha256=stable_digest(contract_path),
        )
        report = build_support_classification_report(source, path_exists=path_exists)
    except ContractError as exc:
        print(f"support classification contract error: {exc}", file=sys.stderr)
        return 1

    if args.check:
        mismatches = []
        if (
            not json_out.is_file()
            or json_out.read_text(encoding="utf-8") != report.json_report
        ):
            mismatches.append(repo_rel(json_out))
        if (
            not md_out.is_file()
            or md_out.read_text(encoding="utf-8") != report.markdown
        ):
            mismatches.append(repo_rel(md_out))
        if mismatches:
            print(
                "support classification output drift: " + ", ".join(mismatches),
                file=sys.stderr,
            )
            return 1
    else:
        write_outputs(report.summary, report.markdown, json_out, md_out)

    print(report.console_json)
    return 0 if report.status == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
