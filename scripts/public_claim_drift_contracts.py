"""Public claim drift contract shaping."""

from __future__ import annotations

import re
from dataclasses import dataclass
from typing import Any, Mapping, Sequence

from objc3c_tooling.reports import markdown_table


JsonObject = dict[str, Any]
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


@dataclass(frozen=True)
class PublicClaimDriftInputs:
    support_summary: Mapping[str, Any]
    support_summary_path: str
    support_summary_sha256: str
    surface_sha256: Mapping[str, str]
    surface_lines: Mapping[str, Sequence[str]]


def public_claim_drift_owner_contract(
    *,
    public_surfaces: set[str],
    scan_paths: list[str],
) -> JsonObject:
    return {
        "owner_id": PUBLIC_CLAIM_DRIFT_OWNER,
        "owner_surface": PUBLIC_CLAIM_DRIFT_OWNER_SURFACE,
        "public_claim_surface_count": len(public_surfaces),
        "scan_path_count": len(scan_paths),
        "public_claim_surfaces": sorted(public_surfaces),
        "scan_paths": list(scan_paths),
        "blocker_metadata": dict(PUBLIC_CLAIM_DRIFT_BLOCKER_METADATA),
    }


def public_claim_scan_paths(support_summary: Mapping[str, Any]) -> list[str]:
    classifications = support_summary["classifications"]
    public_surfaces = set(support_summary["public_claim_surfaces"])
    return sorted(
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


def clipped(text: str, limit: int = 220) -> str:
    stripped = text.strip()
    if len(stripped) <= limit:
        return stripped
    return stripped[: limit - 3] + "..."


def context_window(lines: Sequence[str], index: int) -> str:
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


def scan_surface_lines(
    *,
    relative_path: str,
    lines: Sequence[str],
    rows_by_path: Mapping[str, list[Mapping[str, Any]]],
    envelope_row: Mapping[str, Any] | None,
    public_surfaces: set[str],
) -> tuple[list[JsonObject], list[JsonObject]]:
    claim_mappings: list[JsonObject] = []
    findings: list[JsonObject] = []
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


def build_public_claim_drift_summary(inputs: PublicClaimDriftInputs) -> JsonObject:
    support_summary = inputs.support_summary
    classifications = support_summary["classifications"]
    public_surfaces = set(support_summary["public_claim_surfaces"])
    rows_by_path: dict[str, list[Mapping[str, Any]]] = {}
    envelope_row: Mapping[str, Any] | None = None
    for row in classifications:
        if row.get("surface") == "envelope-level-public-claims":
            envelope_row = row
        for surface in row["checked_in_surfaces"]:
            rows_by_path.setdefault(surface, []).append(row)

    scan_paths = public_claim_scan_paths(support_summary)
    tmp_source_truth_paths = [path for path in scan_paths if path.startswith("tmp/")]
    owner_contract = public_claim_drift_owner_contract(
        public_surfaces=public_surfaces,
        scan_paths=scan_paths,
    )

    claim_mappings: list[JsonObject] = []
    findings: list[JsonObject] = []
    for relative_path in scan_paths:
        mappings, surface_findings = scan_surface_lines(
            relative_path=relative_path,
            lines=inputs.surface_lines[relative_path],
            rows_by_path=rows_by_path,
            envelope_row=envelope_row,
            public_surfaces=public_surfaces,
        )
        claim_mappings.extend(mappings)
        findings.extend(surface_findings)

    checks = {
        "support_classification_passed": support_summary.get("status") == "PASS",
        "source_truth_excludes_tmp": not tmp_source_truth_paths,
        "public_claim_surfaces_exist": all(path in inputs.surface_lines for path in public_surfaces),
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
        "support_summary_path": inputs.support_summary_path,
        "support_summary_contract_id": support_summary.get("contract_id"),
        "support_summary_sha256": inputs.support_summary_sha256,
        "owner_contract": owner_contract,
        "blocker_metadata": owner_contract["blocker_metadata"],
        "scanned_surface_count": len(scan_paths),
        "public_claim_surface_count": len(public_surfaces),
        "claim_mapping_count": len(claim_mappings),
        "finding_count": len(findings),
        "scan_paths": scan_paths,
        "surface_sha256": dict(inputs.surface_sha256),
        "evidence_by_surface": evidence_by_surface,
        "tmp_source_truth_paths": tmp_source_truth_paths,
        "claim_mappings": claim_mappings,
        "findings": findings,
        "checks": checks,
    }


def render_markdown(summary: Mapping[str, Any]) -> str:
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


def status_line(summary: Mapping[str, Any], report_path: str) -> str:
    return (
        "public claim drift: {status} (mapped={mapped}, findings={findings}, report={report})"
    ).format(
        status=summary["status"],
        mapped=summary["claim_mapping_count"],
        findings=summary["finding_count"],
        report=report_path,
    )


__all__ = [
    "PublicClaimDriftInputs",
    "build_public_claim_drift_summary",
    "public_claim_scan_paths",
    "render_markdown",
    "status_line",
]
