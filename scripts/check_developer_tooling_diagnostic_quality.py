#!/usr/bin/env python3
"""Validate developer-facing diagnostic taxonomy and fix-it quality."""

from __future__ import annotations

import hashlib
import json
import re
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import display_path


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/developer_tooling/diagnostic_quality_contract.json"
OUT_DIR = ROOT / "tmp/reports/developer-tooling/diagnostic-quality"
JSON_OUT = OUT_DIR / "diagnostic_quality_summary.json"
MD_OUT = OUT_DIR / "diagnostic_quality_summary.md"
DOC_CODE_RE = re.compile(r"^OBJC3-D-[A-Za-z0-9_]+(?:-[A-Za-z0-9_]+)*$")
NATIVE_CODE_RE = re.compile(r"^O3[CLPSREAT][0-9]{3}$")
ALLOWED_SEVERITIES = {"note", "warning", "error", "fatal"}


def read_json_object(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object at {path}")
    return payload


def stable_json(payload: object) -> str:
    return json.dumps(payload, sort_keys=True, separators=(",", ":"), ensure_ascii=True)


def diagnostic_code_family(code: str) -> str:
    if code.startswith("OBJC3-D-"):
        parts = code.split("-")
        return "-".join(parts[:3]) if len(parts) >= 3 else code
    if NATIVE_CODE_RE.fullmatch(code):
        return code[:3]
    return "unknown"


def valid_code(code: object) -> bool:
    return isinstance(code, str) and (
        DOC_CODE_RE.fullmatch(code) is not None or NATIVE_CODE_RE.fullmatch(code) is not None
    )


def valid_position(position: object) -> bool:
    return (
        isinstance(position, dict)
        and isinstance(position.get("line"), int)
        and isinstance(position.get("column"), int)
        and int(position["line"]) > 0
        and int(position["column"]) > 0
    )


def valid_range(range_payload: object) -> bool:
    if not isinstance(range_payload, dict):
        return False
    start = range_payload.get("start")
    end = range_payload.get("end")
    if not valid_position(start) or not valid_position(end):
        return False
    start_pos = (int(start["line"]), int(start["column"]))  # type: ignore[index]
    end_pos = (int(end["line"]), int(end["column"]))  # type: ignore[index]
    return end_pos >= start_pos


def valid_fixit(fixit: object) -> bool:
    return (
        isinstance(fixit, dict)
        and valid_range(fixit.get("range"))
        and isinstance(fixit.get("replacement"), str)
    )


def manifest_files(manifest: dict[str, Any]) -> list[str]:
    files: list[str] = []
    seen: set[str] = set()
    for group in manifest.get("groups", []):
        if not isinstance(group, dict):
            continue
        for file_name in group.get("files", []):
            if not isinstance(file_name, str) or file_name in seen:
                continue
            seen.add(file_name)
            files.append(file_name)
    return files


def iter_expected_diagnostics(case_payload: dict[str, Any]) -> list[dict[str, Any]]:
    expect = case_payload.get("expect", {})
    if not isinstance(expect, dict):
        return []
    diagnostics = expect.get("diagnostics", [])
    return [diagnostic for diagnostic in diagnostics if isinstance(diagnostic, dict)]


def build_diagnostic_quality_summary(contract: dict[str, Any]) -> dict[str, Any]:
    manifest_path = ROOT / str(contract["manifest_path"])
    manifest = read_json_object(manifest_path)
    diagnostic_root = manifest_path.parent
    files = manifest_files(manifest)

    failures: list[str] = []
    entries: list[dict[str, Any]] = []
    code_families: dict[str, int] = {}
    fixit_count = 0
    machine_applicable_fixit_count = 0
    non_diagnostic_case_count = 0
    missing_files: list[str] = []
    required_fixit_codes = set(str(code) for code in contract["required_fixit_codes"])
    observed_fixit_codes: set[str] = set()

    for file_name in files:
        case_path = diagnostic_root / file_name
        if not case_path.is_file():
            missing_files.append(file_name)
            continue
        case_payload = read_json_object(case_path)
        case_id = str(case_payload.get("id", Path(file_name).stem))
        diagnostics = iter_expected_diagnostics(case_payload)
        if not diagnostics:
            non_diagnostic_case_count += 1
            continue
        for index, diagnostic in enumerate(diagnostics):
            code = diagnostic.get("code")
            severity = diagnostic.get("severity")
            message = diagnostic.get("message")
            span = diagnostic.get("span")
            fixits = diagnostic.get("fixits", [])
            valid_fixits = [fixit for fixit in fixits if valid_fixit(fixit)] if isinstance(fixits, list) else []
            entry_ok = (
                valid_code(code)
                and isinstance(severity, str)
                and severity in ALLOWED_SEVERITIES
                and isinstance(message, str)
                and bool(message.strip())
                and valid_range(span)
                and isinstance(fixits, list)
                and len(valid_fixits) == len(fixits)
            )
            code_text = str(code) if isinstance(code, str) else ""
            family = diagnostic_code_family(code_text)
            code_families[family] = code_families.get(family, 0) + 1
            fixit_count += len(fixits) if isinstance(fixits, list) else 0
            machine_applicable_fixit_count += len(valid_fixits)
            if valid_fixits:
                observed_fixit_codes.add(code_text)
            if not entry_ok:
                failures.append(f"{file_name}#{index}: malformed diagnostic payload")
            entries.append(
                {
                    "case_id": case_id,
                    "file": file_name,
                    "diagnostic_index": index,
                    "code": code_text,
                    "severity": severity,
                    "family": family,
                    "fixit_count": len(fixits) if isinstance(fixits, list) else 0,
                    "machine_applicable_fixit_count": len(valid_fixits),
                    "ok": entry_ok,
                }
            )

    missing_required_fixit_codes = sorted(required_fixit_codes - observed_fixit_codes)
    if missing_files:
        failures.extend(f"missing diagnostic fixture: {file_name}" for file_name in missing_files)
    if missing_required_fixit_codes:
        failures.append(
            "missing required machine-applicable fix-it code(s): "
            + ", ".join(missing_required_fixit_codes)
        )

    min_diagnostic_count = int(contract["minimum_diagnostic_count"])
    min_fixit_count = int(contract["minimum_machine_applicable_fixit_count"])
    if len(entries) < min_diagnostic_count:
        failures.append(
            f"diagnostic entry count {len(entries)} below required {min_diagnostic_count}"
        )
    if machine_applicable_fixit_count < min_fixit_count:
        failures.append(
            "machine-applicable fix-it count "
            f"{machine_applicable_fixit_count} below required {min_fixit_count}"
        )

    digest_input = {
        "files": files,
        "entries": entries,
        "code_families": dict(sorted(code_families.items())),
    }
    deterministic_digest = hashlib.sha256(stable_json(digest_input).encode("utf-8")).hexdigest()
    checks = {
        "manifest_exists": manifest_path.is_file(),
        "all_manifest_files_exist": not missing_files,
        "all_diagnostic_payloads_structured": not any(not entry["ok"] for entry in entries),
        "minimum_diagnostic_count_met": len(entries) >= min_diagnostic_count,
        "minimum_machine_applicable_fixit_count_met": machine_applicable_fixit_count >= min_fixit_count,
        "required_fixit_codes_present": not missing_required_fixit_codes,
        "deterministic_digest_ready": bool(deterministic_digest),
    }
    return {
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "manifest_path": display_path(manifest_path),
        "diagnostic_file_count": len(files),
        "non_diagnostic_case_count": non_diagnostic_case_count,
        "diagnostic_entry_count": len(entries),
        "code_family_count": len(code_families),
        "code_families": dict(sorted(code_families.items())),
        "fixit_count": fixit_count,
        "machine_applicable_fixit_count": machine_applicable_fixit_count,
        "required_fixit_codes": sorted(required_fixit_codes),
        "missing_required_fixit_codes": missing_required_fixit_codes,
        "deterministic_digest": deterministic_digest,
        "checks": checks,
        "failures": failures,
        "status": "PASS" if not failures and all(checks.values()) else "FAIL",
        "ok": not failures and all(checks.values()),
    }


def main() -> int:
    contract = read_json_object(CONTRACT_PATH)
    summary = build_diagnostic_quality_summary(contract)
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    write_json_file(JSON_OUT, summary)
    MD_OUT.write_text(
        "# Developer Tooling Diagnostic Quality Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Diagnostic files: `{summary['diagnostic_file_count']}`\n"
        f"- Diagnostic entries: `{summary['diagnostic_entry_count']}`\n"
        f"- Code families: `{summary['code_family_count']}`\n"
        f"- Machine-applicable fix-its: `{summary['machine_applicable_fixit_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(f"summary_path: {display_path(JSON_OUT)}")
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
