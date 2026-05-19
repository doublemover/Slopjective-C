#!/usr/bin/env python3
from __future__ import annotations

import argparse
import hashlib
import json
import sys
from pathlib import Path
from typing import Any, Sequence

from objc3c_tooling.paths import repo_rel
from objc3c_tooling.reports import expected_json_report
from objc3c_tooling.reports import write_report_outputs

try:
    from public_claim_drift_contracts import (
        PUBLIC_CLAIM_DRIFT_OWNER,
        PublicClaimDriftInputs,
        build_public_claim_drift_summary,
        public_claim_drift_owner_contract,
        public_claim_scan_paths,
        render_markdown,
        status_line,
    )
except ModuleNotFoundError:
    from scripts.public_claim_drift_contracts import (
        PUBLIC_CLAIM_DRIFT_OWNER,
        PublicClaimDriftInputs,
        build_public_claim_drift_summary,
        public_claim_drift_owner_contract,
        public_claim_scan_paths,
        render_markdown,
        status_line,
    )

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_SUPPORT_SUMMARY = (
    ROOT
    / "reports"
    / "claimability"
    / "support-classification"
    / "support_classification_summary.json"
)
DEFAULT_JSON_OUT = (
    ROOT
    / "reports"
    / "claimability"
    / "public-claim-drift"
    / "public_claim_drift_summary.json"
)
DEFAULT_MD_OUT = (
    ROOT
    / "reports"
    / "claimability"
    / "public-claim-drift"
    / "public_claim_drift_summary.md"
)

class ClaimDriftError(RuntimeError):
    pass



def resolve_path(root: Path, raw_path: str) -> Path:
    path = Path(raw_path)
    return path if path.is_absolute() else root / path


def read_json(path: Path) -> dict[str, Any]:
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise ClaimDriftError(f"missing JSON file `{repo_rel(path)}`") from exc
    except json.JSONDecodeError as exc:
        raise ClaimDriftError(f"invalid JSON at `{repo_rel(path)}`: {exc}") from exc
    if not isinstance(payload, dict):
        raise ClaimDriftError(f"JSON object expected at `{repo_rel(path)}`")
    return payload


def stable_digest(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def require_list(payload: dict[str, Any], key: str) -> list[Any]:
    value = payload.get(key)
    if not isinstance(value, list):
        raise ClaimDriftError(f"support summary field `{key}` must be a list")
    return value


def normalize_path(raw_path: Any, label: str) -> str:
    if not isinstance(raw_path, str) or not raw_path:
        raise ClaimDriftError(f"{label} must be a non-empty string")
    path = raw_path.replace("\\", "/")
    if path.startswith("tmp/"):
        raise ClaimDriftError(f"{label} must not use tmp as source truth")
    return path


def require_row_list(row: dict[str, Any], key: str, label: str) -> list[Any]:
    value = row.get(key)
    if not isinstance(value, list):
        raise ClaimDriftError(f"{label}.{key} must be a list")
    return value


def require_string(raw_value: Any, label: str) -> str:
    if not isinstance(raw_value, str) or not raw_value:
        raise ClaimDriftError(f"{label} must be a non-empty string")
    return raw_value


def load_support_summary(path: Path, root: Path) -> dict[str, Any]:
    summary = read_json(path)
    if summary.get("status") != "PASS":
        raise ClaimDriftError("support classification summary must be PASS")
    classifications = require_list(summary, "classifications")
    public_surfaces = [
        normalize_path(surface, f"public_claim_surfaces[{index}]")
        for index, surface in enumerate(require_list(summary, "public_claim_surfaces"))
    ]
    for index, row in enumerate(classifications):
        if not isinstance(row, dict):
            raise ClaimDriftError(f"classifications[{index}] must be an object")
        normalize_path(row.get("surface"), f"classifications[{index}].surface")
        current_class = row.get("current_class")
        if current_class not in {"supported", "experimental", "unsupported", "release-blocking"}:
            raise ClaimDriftError(
                f"classifications[{index}].current_class must be a canonical support class"
            )
        checked_in_surfaces = [
            normalize_path(surface, f"classifications[{index}].checked_in_surfaces[{surface_index}]")
            for surface_index, surface in enumerate(
                require_row_list(row, "checked_in_surfaces", f"classifications[{index}]")
            )
        ]
        if not checked_in_surfaces:
            raise ClaimDriftError(f"classifications[{index}] must name checked_in_surfaces")
        for surface in checked_in_surfaces:
            if not resolve_path(root, surface).exists():
                raise ClaimDriftError(f"missing checked-in surface `{surface}`")
        for family_index, family in enumerate(
            require_row_list(row, "required_evidence_families", f"classifications[{index}]")
        ):
            require_string(
                family,
                f"classifications[{index}].required_evidence_families[{family_index}]",
            )
    for surface in public_surfaces:
        if not resolve_path(root, surface).exists():
            raise ClaimDriftError(f"missing public claim surface `{surface}`")
    return summary


def read_lines(path: Path) -> list[str]:
    return path.read_text(encoding="utf-8").splitlines()


def build_summary(support_summary_path: Path, root: Path) -> dict[str, Any]:
    support_summary = load_support_summary(support_summary_path, root)
    scan_paths = public_claim_scan_paths(support_summary)
    tmp_source_truth_paths = [path for path in scan_paths if path.startswith("tmp/")]
    if tmp_source_truth_paths:
        raise ClaimDriftError(
            "public claim drift source truth must not use tmp: "
            + ", ".join(tmp_source_truth_paths)
        )
    surface_digests: dict[str, str] = {}
    surface_lines: dict[str, list[str]] = {}
    for relative_path in scan_paths:
        resolved = resolve_path(root, relative_path)
        surface_digests[relative_path] = stable_digest(resolved)
        surface_lines[relative_path] = read_lines(resolved)
    return build_public_claim_drift_summary(
        PublicClaimDriftInputs(
            support_summary=support_summary,
            support_summary_path=repo_rel(support_summary_path, root=root),
            support_summary_sha256=stable_digest(support_summary_path),
            surface_sha256=surface_digests,
            surface_lines=surface_lines,
        )
    )


def write_outputs(summary: dict[str, Any], json_out: Path, md_out: Path) -> None:
    write_report_outputs(
        summary=summary,
        json_path=json_out,
        markdown_path=md_out,
        markdown=render_markdown(summary),
        sort_keys=False,
    )


def build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument("--support-summary", type=Path, default=DEFAULT_SUPPORT_SUMMARY)
    parser.add_argument("--summary-json", type=Path, default=DEFAULT_JSON_OUT)
    parser.add_argument("--summary-md", type=Path, default=DEFAULT_MD_OUT)
    parser.add_argument(
        "--check",
        action="store_true",
        help="fail if the existing drift reports differ from generated output",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_arg_parser().parse_args(argv)
    root = args.root.resolve()
    support_summary_path = (
        args.support_summary
        if args.support_summary.is_absolute()
        else root / args.support_summary
    )
    json_out = args.summary_json if args.summary_json.is_absolute() else root / args.summary_json
    md_out = args.summary_md if args.summary_md.is_absolute() else root / args.summary_md
    try:
        summary = build_summary(support_summary_path, root)
    except ClaimDriftError as exc:
        print(f"public claim drift error: {exc}", file=sys.stderr)
        return 1

    next_json = expected_json_report(summary, sort_keys=False)
    next_md = render_markdown(summary)
    if args.check:
        mismatches = []
        if not json_out.is_file() or json_out.read_text(encoding="utf-8") != next_json:
            mismatches.append(repo_rel(json_out, root=root))
        if not md_out.is_file() or md_out.read_text(encoding="utf-8") != next_md:
            mismatches.append(repo_rel(md_out, root=root))
        if mismatches:
            print("public claim drift output drift: " + ", ".join(mismatches), file=sys.stderr)
            return 1
    else:
        write_outputs(summary, json_out, md_out)

    print(
        status_line(summary, repo_rel(json_out, root=root))
    )
    return 0 if summary["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
