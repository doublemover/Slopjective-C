#!/usr/bin/env python3
from __future__ import annotations

import argparse
import hashlib
import json
import re
import sys
from pathlib import Path
from typing import Any, Sequence

from objc3c_tooling.paths import repo_rel
from objc3c_tooling.reports import expected_json_report
from objc3c_tooling.reports import markdown_table
from objc3c_tooling.reports import write_report_outputs

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

SUMMARY_CONTRACT_ID = "objc3c.public.claim.drift.summary.v1"
PUBLIC_CLAIM_DRIFT_OWNER = "public-claim-drift.owner"
PUBLIC_CLAIM_DRIFT_OWNER_SURFACE = "scripts/check_objc3c_public_claim_drift.py"
PUBLIC_CLAIM_DRIFT_BLOCKER_METADATA = {
    "blocker_contract": "hard-cutover-public-claim-drift-fail-closed",
    "blocker_scope": "public-claim-surfaces-and-support-evidence-mapping",
    "blocker_owner": PUBLIC_CLAIM_DRIFT_OWNER,
    "blocker_owner_surface": PUBLIC_CLAIM_DRIFT_OWNER_SURFACE,
}

CLAIM_KEYWORD_RE = re.compile(
    r"\b("
    r"claim|claimable|support|supported|supports|validated|validation|"
    r"evidence|proof|runtime|release-ready|production-strength|stable|"
    r"complete|full|runnable|live"
    r")\b",
    re.IGNORECASE,
)
PROMOTIONAL_CLAIM_RE = re.compile(
    r"\b("
    r"supported|supports|claimable|release-ready|production-strength|"
    r"stable|complete|full|validated|runnable|live"
    r")\b",
    re.IGNORECASE,
)
GUARD_RE = re.compile(
    r"\b("
    r"incomplete|not fully|not supported|unsupported|fail-closed|outside|"
    r"non-goal|non-goals|do not|does not|must not|cannot|deferred|later|"
    r"narrower|limited to|not yet|still incomplete|no blanket claim|"
    r"no widening|no public|no hosted|no cross-platform|no manual|"
    r"do not claim|must stay|remains unsupported"
    r")\b",
    re.IGNORECASE,
)
FORBIDDEN_PATTERNS: tuple[tuple[str, re.Pattern[str]], ...] = (
    (
        "full-live-object-model-runtime",
        re.compile(r"\bfull live objective-c 3\.0 object-model runtime behavior\b", re.IGNORECASE),
    ),
    (
        "full-property-ivar-reflection-closure",
        re.compile(r"\bfull property/ivar/runtime reflection closure\b", re.IGNORECASE),
    ),
    (
        "full-escaping-block-byref-arc",
        re.compile(r"\bfull escaping block/byref (runtime behavior|and arc automation)\b", re.IGNORECASE),
    ),
    (
        "complete-runtime-backed-semantics",
        re.compile(r"\bcomplete runtime-backed (semantics|closure)\b", re.IGNORECASE),
    ),
    (
        "production-strength-completeness-stability",
        re.compile(r"\bproduction-strength completeness and stability claims\b", re.IGNORECASE),
    ),
    (
        "every-behavior-fully-complete",
        re.compile(r"\bevery\b.*\bfully complete\b", re.IGNORECASE),
    ),
    (
        "all-intended-topologies-supported",
        re.compile(r"\ball intended (future )?topolog(?:y|ies).*\bsupported\b", re.IGNORECASE),
    ),
    (
        "universal-foreign-topology-support",
        re.compile(r"\buniversal foreign topolog(?:y|ies) support\b", re.IGNORECASE),
    ),
    (
        "foreign-topologies-supported",
        re.compile(r"\bforeign topolog(?:y|ies).*\bsupported\b", re.IGNORECASE),
    ),
    (
        "full-runtime-abi",
        re.compile(r"\bfull runtime abi\b", re.IGNORECASE),
    ),
    (
        "runtime-abi-widening",
        re.compile(r"\bruntime abi\b.*\b(foreign|universal|all|widened|widening)\b", re.IGNORECASE),
    ),
    (
        "cross-platform-support",
        re.compile(r"\bcross-platform\b.*\bsupport\b", re.IGNORECASE),
    ),
    (
        "indefinite-or-cross-major-support",
        re.compile(r"\b(indefinite support|cross-major forward compatibility)\b", re.IGNORECASE),
    ),
    (
        "hosted-release-service",
        re.compile(r"\b(hosted updater|hosted update service|hosted trust service|hosted artifact registry)\b", re.IGNORECASE),
    ),
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


def public_claim_drift_owner_contract(
    *,
    public_surfaces: set[str],
    scan_paths: list[str],
) -> dict[str, Any]:
    return {
        "owner_id": PUBLIC_CLAIM_DRIFT_OWNER,
        "owner_surface": PUBLIC_CLAIM_DRIFT_OWNER_SURFACE,
        "public_claim_surface_count": len(public_surfaces),
        "scan_path_count": len(scan_paths),
        "public_claim_surfaces": sorted(public_surfaces),
        "scan_paths": list(scan_paths),
        "blocker_metadata": dict(PUBLIC_CLAIM_DRIFT_BLOCKER_METADATA),
    }


def read_lines(path: Path) -> list[str]:
    return path.read_text(encoding="utf-8").splitlines()


def clipped(text: str, limit: int = 220) -> str:
    stripped = text.strip()
    if len(stripped) <= limit:
        return stripped
    return stripped[: limit - 3] + "..."


def context_window(lines: list[str], index: int) -> str:
    context: list[str] = []
    cursor = index
    while cursor >= 0 and len(context) < 8:
        line = lines[cursor].strip()
        if line:
            context.append(line)
        cursor -= 1
    return " ".join(reversed(context))


def is_guarded(context: str) -> bool:
    return bool(GUARD_RE.search(context))


def scan_surface(
    root: Path,
    relative_path: str,
    rows_by_path: dict[str, list[dict[str, Any]]],
    envelope_row: dict[str, Any] | None,
    public_surfaces: set[str],
) -> tuple[list[dict[str, Any]], list[dict[str, Any]]]:
    path = resolve_path(root, relative_path)
    lines = read_lines(path)
    claim_mappings: list[dict[str, Any]] = []
    findings: list[dict[str, Any]] = []
    for index, line in enumerate(lines):
        context = context_window(lines, index)
        guarded = is_guarded(context)
        for pattern_id, pattern in FORBIDDEN_PATTERNS:
            if pattern.search(line) and not guarded:
                findings.append(
                    {
                        "kind": "unguarded-forbidden-public-claim",
                        "path": relative_path,
                        "line": index + 1,
                        "pattern_id": pattern_id,
                        "claim_text": clipped(line),
                    }
                )

        if not CLAIM_KEYWORD_RE.search(line):
            continue

        matched_rows = list(rows_by_path.get(relative_path, []))
        if relative_path in public_surfaces and envelope_row and envelope_row not in matched_rows:
            matched_rows.append(envelope_row)
        evidence_families = sorted(
            {
                family
                for row in matched_rows
                if not row.get("fail_closed")
                for family in row.get("required_evidence_families", [])
            }
        )
        checked_surfaces = sorted(
            {
                surface
                for row in matched_rows
                if not row.get("fail_closed")
                for surface in row.get("checked_in_surfaces", [])
            }
        )
        fail_closed_surfaces = sorted(
            row.get("surface", "")
            for row in matched_rows
            if bool(row.get("fail_closed"))
        )
        promotional = bool(PROMOTIONAL_CLAIM_RE.search(line))
        if promotional and not guarded and not evidence_families:
            findings.append(
                {
                    "kind": "unmapped-promotional-public-claim",
                    "path": relative_path,
                    "line": index + 1,
                    "claim_text": clipped(line),
                }
            )
        claim_mappings.append(
            {
                "path": relative_path,
                "line": index + 1,
                "claim_text": clipped(line),
                "guarded": guarded,
                "promotional": promotional,
                "mapped_surfaces": [row.get("surface") for row in matched_rows],
                "evidence_families": evidence_families,
                "checked_in_surface_count": len(checked_surfaces),
                "fail_closed_surfaces": fail_closed_surfaces,
            }
        )
    return claim_mappings, findings


def build_summary(support_summary_path: Path, root: Path) -> dict[str, Any]:
    support_summary = load_support_summary(support_summary_path, root)
    classifications = support_summary["classifications"]
    public_surfaces = set(support_summary["public_claim_surfaces"])
    rows_by_path: dict[str, list[dict[str, Any]]] = {}
    envelope_row: dict[str, Any] | None = None
    for row in classifications:
        if row.get("surface") == "envelope-level-public-claims":
            envelope_row = row
        for surface in row["checked_in_surfaces"]:
            rows_by_path.setdefault(surface, []).append(row)

    scan_paths = sorted(
        {
            *public_surfaces,
            *(
                surface
                for row in classifications
                if row.get("fail_closed")
                for surface in row.get("checked_in_surfaces", [])
            ),
        }
    )
    tmp_source_truth_paths = [path for path in scan_paths if path.startswith("tmp/")]
    if tmp_source_truth_paths:
        raise ClaimDriftError(
            "public claim drift source truth must not use tmp: "
            + ", ".join(tmp_source_truth_paths)
        )
    owner_contract = public_claim_drift_owner_contract(
        public_surfaces=public_surfaces,
        scan_paths=scan_paths,
    )

    claim_mappings: list[dict[str, Any]] = []
    findings: list[dict[str, Any]] = []
    surface_digests: dict[str, str] = {}
    for relative_path in scan_paths:
        resolved = resolve_path(root, relative_path)
        surface_digests[relative_path] = stable_digest(resolved)
        mappings, surface_findings = scan_surface(
            root, relative_path, rows_by_path, envelope_row, public_surfaces
        )
        claim_mappings.extend(mappings)
        findings.extend(surface_findings)

    checks = {
        "support_classification_passed": support_summary.get("status") == "PASS",
        "source_truth_excludes_tmp": not tmp_source_truth_paths,
        "public_claim_surfaces_exist": all(resolve_path(root, path).exists() for path in public_surfaces),
        "every_claim_has_mapping": all(
            mapping["mapped_surfaces"] or mapping["guarded"] for mapping in claim_mappings
        ),
        "promotional_claims_have_evidence_or_guard": not any(
            finding["kind"] == "unmapped-promotional-public-claim" for finding in findings
        ),
        "unsupported_surfaces_fail_closed": not any(
            finding["kind"] == "unguarded-forbidden-public-claim" for finding in findings
        ),
    }
    status = "PASS" if all(checks.values()) and not findings else "FAIL"
    evidence_by_surface = {
        row["surface"]: {
            "current_class": row["current_class"],
            "fail_closed": row["fail_closed"],
            "required_evidence_families": row["required_evidence_families"],
            "checked_in_surfaces": row["checked_in_surfaces"],
        }
        for row in classifications
    }
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": status,
        "support_summary_path": repo_rel(support_summary_path, root=root),
        "support_summary_contract_id": support_summary.get("contract_id"),
        "support_summary_sha256": stable_digest(support_summary_path),
        "owner_contract": owner_contract,
        "blocker_metadata": owner_contract["blocker_metadata"],
        "scanned_surface_count": len(scan_paths),
        "public_claim_surface_count": len(public_surfaces),
        "claim_mapping_count": len(claim_mappings),
        "finding_count": len(findings),
        "scan_paths": scan_paths,
        "surface_sha256": surface_digests,
        "evidence_by_surface": evidence_by_surface,
        "tmp_source_truth_paths": tmp_source_truth_paths,
        "claim_mappings": claim_mappings,
        "findings": findings,
        "checks": checks,
    }


def render_markdown(summary: dict[str, Any]) -> str:
    findings = summary["findings"]
    if findings:
        finding_rows = markdown_table(
            ["Kind", "Location", "Pattern"],
            [
                [
                    finding["kind"],
                    f"{finding['path']}:{finding['line']}",
                    finding.get("pattern_id", "n/a"),
                ]
                for finding in findings
            ],
        )
    else:
        finding_rows = markdown_table(["Kind", "Location", "Pattern"], [["none", "n/a", "n/a"]])
    finding_table = "\n".join(finding_rows)
    return (
        "# Objective-C 3.0 Public Claim Drift Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Status: `{summary['status']}`\n"
        f"- Support summary: `{summary['support_summary_path']}`\n"
        f"- Scanned surfaces: `{summary['scanned_surface_count']}`\n"
        f"- Public claim surfaces: `{summary['public_claim_surface_count']}`\n"
        f"- Mapped claim lines: `{summary['claim_mapping_count']}`\n"
        f"- Findings: `{summary['finding_count']}`\n"
        f"- Source truth excludes tmp: `{summary['checks']['source_truth_excludes_tmp']}`\n\n"
        "## Findings\n\n"
        f"{finding_table}\n"
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
        "public claim drift: {status} (mapped={mapped}, findings={findings}, report={report})".format(
            status=summary["status"],
            mapped=summary["claim_mapping_count"],
            findings=summary["finding_count"],
            report=repo_rel(json_out, root=root),
        )
    )
    return 0 if summary["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
