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
NATIVE_CODE_TOKEN_RE = re.compile(r"\bO3[CLPSREAT][0-9]{3}\b")
EXPECTED_DIAGNOSTIC_HEADER_RE = re.compile(
    r"^\s*//\s*Expected diagnostic code\(s\):\s*(?P<codes>.+?)\s*$",
    re.MULTILINE,
)
DEFAULT_NATIVE_RECOVERY_FIXTURE_ROOT = "tests/tooling/fixtures/native/recovery/negative"
ALLOWED_SEVERITIES = {"note", "warning", "error", "fatal"}
ALLOWED_SUGGESTION_CONFIDENCE = {"high", "medium", "low"}
ALLOWED_SUGGESTION_KINDS = {"nearest-symbol"}
ALLOWED_PHASES = {
    "lex",
    "parse",
    "sema",
    "lowering",
    "post-pipeline",
    "runtime",
    "frontend-api",
    "tooling",
}


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


def valid_suggestion(
    suggestion: object,
    *,
    case_source: object,
    failures: list[str],
    failure_prefix: str,
) -> bool:
    if not isinstance(suggestion, dict):
        failures.append(f"{failure_prefix}: suggestion payload must be an object")
        return False

    ok = True
    kind = suggestion.get("kind")
    if not isinstance(kind, str) or kind not in ALLOWED_SUGGESTION_KINDS:
        failures.append(f"{failure_prefix}: suggestion.kind is invalid")
        ok = False

    confidence = suggestion.get("confidence")
    if (
        not isinstance(confidence, str)
        or confidence not in ALLOWED_SUGGESTION_CONFIDENCE
    ):
        failures.append(f"{failure_prefix}: suggestion.confidence is invalid")
        ok = False

    for field in ("symbol_kind", "target", "replacement", "explanation"):
        if not isinstance(suggestion.get(field), str) or not suggestion[field].strip():
            failures.append(f"{failure_prefix}: suggestion.{field} must be non-empty")
            ok = False

    if not valid_range(suggestion.get("range")):
        failures.append(f"{failure_prefix}: suggestion.range is invalid")
        ok = False

    target = suggestion.get("target")
    replacement = suggestion.get("replacement")
    if isinstance(target, str) and isinstance(replacement, str) and target == replacement:
        failures.append(f"{failure_prefix}: suggestion target and replacement must differ")
        ok = False

    if isinstance(case_source, str):
        if isinstance(target, str) and target not in case_source:
            failures.append(f"{failure_prefix}: suggestion target is absent from source")
            ok = False
        if isinstance(replacement, str) and replacement not in case_source:
            failures.append(f"{failure_prefix}: suggestion replacement is absent from source")
            ok = False
    else:
        failures.append(f"{failure_prefix}: suggestion case source is missing")
        ok = False

    return ok


def path_is_relative_to(path: Path, root: Path) -> bool:
    try:
        path.relative_to(root)
        return True
    except ValueError:
        return False


def normalize_recovery_fixture_source(text: str) -> str:
    retained_lines: list[str] = []
    for line in text.replace("\r\n", "\n").replace("\r", "\n").split("\n"):
        if EXPECTED_DIAGNOSTIC_HEADER_RE.match(line):
            continue
        retained_lines.append(line)
    return "\n".join(retained_lines).strip()


def expected_codes_from_native_fixture(text: str) -> set[str]:
    match = EXPECTED_DIAGNOSTIC_HEADER_RE.search(text)
    if match is None:
        return set()
    return set(NATIVE_CODE_TOKEN_RE.findall(match.group("codes")))


def valid_native_recovery_fixture(
    native_fixture: str,
    *,
    case_source: object,
    expected_native_code: object,
    failures: list[str],
    failure_prefix: str,
    native_recovery_fixture_root: Path,
) -> dict[str, bool]:
    result = {
        "within_root": False,
        "source_matches": False,
        "expected_code_matches": False,
    }
    fixture_path = (ROOT / native_fixture).resolve()
    if not path_is_relative_to(fixture_path, native_recovery_fixture_root):
        failures.append(
            f"{failure_prefix}: native_fixture must stay under "
            f"{native_recovery_fixture_root.relative_to(ROOT).as_posix()}"
        )
    else:
        result["within_root"] = True

    if not fixture_path.is_file():
        return result

    fixture_text = fixture_path.read_text(encoding="utf-8")
    expected_codes = expected_codes_from_native_fixture(fixture_text)
    if not expected_codes:
        failures.append(
            f"{failure_prefix}: native_fixture is missing expected diagnostic header"
        )
    elif isinstance(expected_native_code, str) and expected_native_code in expected_codes:
        result["expected_code_matches"] = True
    elif isinstance(expected_native_code, str):
        failures.append(
            f"{failure_prefix}: native_fixture header does not include "
            f"expected_native_code {expected_native_code}"
        )

    if not isinstance(case_source, str) or not case_source.strip():
        failures.append(f"{failure_prefix}: recovery diagnostic case source is missing")
    elif normalize_recovery_fixture_source(fixture_text) == normalize_recovery_fixture_source(
        case_source
    ):
        result["source_matches"] = True
    else:
        failures.append(f"{failure_prefix}: native_fixture source does not match diagnostic source")

    return result


def valid_recovery_payload(
    recovery: object,
    *,
    case_source: object,
    diagnostic_code: str,
    failures: list[str],
    failure_prefix: str,
    native_recovery_fixture_root: Path,
) -> bool:
    if not isinstance(recovery, dict):
        failures.append(f"{failure_prefix}: missing structured recovery payload")
        return False

    ok = True
    for field in ("strategy", "boundary"):
        if not isinstance(recovery.get(field), str) or not recovery[field].strip():
            failures.append(f"{failure_prefix}: recovery.{field} must be non-empty")
            ok = False

    if recovery.get("deterministic") is not True:
        failures.append(f"{failure_prefix}: recovery.deterministic must be true")
        ok = False
    if recovery.get("accepts_invalid_program") is not False:
        failures.append(f"{failure_prefix}: recovery must not accept invalid programs")
        ok = False
    if recovery.get("recovery_counts_as_success") is not False:
        failures.append(f"{failure_prefix}: recovery_counts_as_success must stay false")
        ok = False

    expected_native_code = recovery.get("expected_native_code")
    if expected_native_code is not None:
        if not valid_code(expected_native_code):
            failures.append(f"{failure_prefix}: expected_native_code is invalid")
            ok = False
        elif expected_native_code != diagnostic_code:
            failures.append(
                f"{failure_prefix}: expected_native_code does not match diagnostic code"
            )
            ok = False

    native_fixture = recovery.get("native_fixture")
    if native_fixture is not None:
        if not isinstance(native_fixture, str) or not native_fixture.strip():
            failures.append(f"{failure_prefix}: native_fixture must be non-empty")
            ok = False
        else:
            fixture_path = ROOT / native_fixture
            if not fixture_path.is_file():
                failures.append(f"{failure_prefix}: native_fixture does not exist")
                ok = False
            fixture_result = valid_native_recovery_fixture(
                native_fixture,
                case_source=case_source,
                expected_native_code=expected_native_code,
                failures=failures,
                failure_prefix=failure_prefix,
                native_recovery_fixture_root=native_recovery_fixture_root,
            )
            recovery["_fixture_validation"] = fixture_result
            ok = ok and all(fixture_result.values())

    return ok


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
    required_recovery_case_ids = set(
        str(case_id) for case_id in contract.get("required_recovery_case_ids", [])
    )
    required_recovery_phases = set(
        str(phase) for phase in contract.get("required_recovery_phases", [])
    )
    required_suggestion_case_ids = set(
        str(case_id) for case_id in contract.get("required_suggestion_case_ids", [])
    )
    required_suggestion_kinds = set(
        str(kind) for kind in contract.get("required_suggestion_kinds", [])
    )
    native_recovery_fixture_root = (
        ROOT / str(contract.get("native_recovery_fixture_root", DEFAULT_NATIVE_RECOVERY_FIXTURE_ROOT))
    ).resolve()
    minimum_native_recovery_fixture_count = int(
        contract.get("minimum_native_recovery_fixture_count", 0)
    )
    observed_fixit_codes: set[str] = set()
    observed_recovery_case_ids: set[str] = set()
    observed_recovery_phases: set[str] = set()
    observed_native_recovery_fixtures: set[str] = set()
    observed_suggestion_case_ids: set[str] = set()
    observed_suggestion_kinds: set[str] = set()
    recovery_diagnostic_count = 0
    suggestion_diagnostic_count = 0
    suggestion_count = 0
    native_recovery_fixture_count = 0
    native_recovery_fixture_within_root_count = 0
    native_recovery_fixture_source_match_count = 0
    native_recovery_fixture_expected_code_match_count = 0

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
            suggestions = diagnostic.get("suggestions", [])
            phase = diagnostic.get("phase")
            category = diagnostic.get("category")
            explanation = diagnostic.get("explanation")
            recovery = diagnostic.get("recovery")
            valid_fixits = [fixit for fixit in fixits if valid_fixit(fixit)] if isinstance(fixits, list) else []
            suggestions_ok = isinstance(suggestions, list)
            valid_suggestions: list[dict[str, Any]] = []
            if isinstance(suggestions, list):
                for suggestion_index, suggestion in enumerate(suggestions):
                    suggestion_prefix = (
                        f"{file_name}#{index}.suggestions[{suggestion_index}]"
                    )
                    if valid_suggestion(
                        suggestion,
                        case_source=case_payload.get("source"),
                        failures=failures,
                        failure_prefix=suggestion_prefix,
                    ):
                        valid_suggestions.append(suggestion)
                    else:
                        suggestions_ok = False
            else:
                failures.append(f"{file_name}#{index}: suggestions must be a list")
            entry_ok = (
                valid_code(code)
                and isinstance(severity, str)
                and severity in ALLOWED_SEVERITIES
                and isinstance(message, str)
                and bool(message.strip())
                and valid_range(span)
                and isinstance(fixits, list)
                and len(valid_fixits) == len(fixits)
                and suggestions_ok
            )
            code_text = str(code) if isinstance(code, str) else ""
            family = diagnostic_code_family(code_text)
            code_families[family] = code_families.get(family, 0) + 1
            fixit_count += len(fixits) if isinstance(fixits, list) else 0
            machine_applicable_fixit_count += len(valid_fixits)
            if isinstance(suggestions, list):
                suggestion_count += len(suggestions)
            if valid_fixits:
                observed_fixit_codes.add(code_text)
            requires_suggestion_payload = (
                case_id in required_suggestion_case_ids
                or bool(suggestions if isinstance(suggestions, list) else suggestions)
            )
            if requires_suggestion_payload:
                suggestion_diagnostic_count += 1
                if not isinstance(suggestions, list) or not suggestions:
                    failures.append(f"{file_name}#{index}: missing suggestion payload")
                    suggestions_ok = False
                else:
                    observed_suggestion_case_ids.add(case_id)
                    for suggestion in valid_suggestions:
                        kind = suggestion.get("kind")
                        if isinstance(kind, str):
                            observed_suggestion_kinds.add(kind)
            if not entry_ok:
                failures.append(f"{file_name}#{index}: malformed diagnostic payload")
            requires_recovery_payload = (
                case_id in required_recovery_case_ids or recovery is not None
            )
            recovery_ok = True
            if requires_recovery_payload:
                recovery_diagnostic_count += 1
                observed_recovery_case_ids.add(case_id)
                if isinstance(phase, str):
                    observed_recovery_phases.add(phase)
                recovery_prefix = f"{file_name}#{index}"
                if not isinstance(phase, str) or phase not in ALLOWED_PHASES:
                    failures.append(f"{recovery_prefix}: phase is invalid")
                    recovery_ok = False
                if not isinstance(category, str) or not category.strip():
                    failures.append(f"{recovery_prefix}: category is required")
                    recovery_ok = False
                if not isinstance(explanation, str) or not explanation.strip():
                    failures.append(f"{recovery_prefix}: explanation is required")
                    recovery_ok = False
                if not valid_recovery_payload(
                    recovery,
                    case_source=case_payload.get("source"),
                    diagnostic_code=code_text,
                    failures=failures,
                    failure_prefix=recovery_prefix,
                    native_recovery_fixture_root=native_recovery_fixture_root,
                ):
                    recovery_ok = False
                if isinstance(recovery, dict) and isinstance(
                    recovery.get("native_fixture"), str
                ):
                    native_recovery_fixture_count += 1
                    observed_native_recovery_fixtures.add(str(recovery["native_fixture"]))
                    fixture_validation = recovery.get("_fixture_validation")
                    if isinstance(fixture_validation, dict):
                        if fixture_validation.get("within_root") is True:
                            native_recovery_fixture_within_root_count += 1
                        if fixture_validation.get("source_matches") is True:
                            native_recovery_fixture_source_match_count += 1
                        if fixture_validation.get("expected_code_matches") is True:
                            native_recovery_fixture_expected_code_match_count += 1
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
                    "suggestion_count": len(suggestions) if isinstance(suggestions, list) else 0,
                    "phase": phase if isinstance(phase, str) else "",
                    "category": category if isinstance(category, str) else "",
                    "has_suggestion": bool(valid_suggestions),
                    "suggestions_ok": suggestions_ok,
                    "has_recovery": recovery is not None,
                    "recovery_ok": recovery_ok,
                    "ok": entry_ok,
                }
            )

    missing_required_fixit_codes = sorted(required_fixit_codes - observed_fixit_codes)
    missing_required_recovery_case_ids = sorted(
        required_recovery_case_ids - observed_recovery_case_ids
    )
    missing_required_recovery_phases = sorted(
        required_recovery_phases - observed_recovery_phases
    )
    missing_required_suggestion_case_ids = sorted(
        required_suggestion_case_ids - observed_suggestion_case_ids
    )
    missing_required_suggestion_kinds = sorted(
        required_suggestion_kinds - observed_suggestion_kinds
    )
    if missing_files:
        failures.extend(f"missing diagnostic fixture: {file_name}" for file_name in missing_files)
    if missing_required_fixit_codes:
        failures.append(
            "missing required machine-applicable fix-it code(s): "
            + ", ".join(missing_required_fixit_codes)
        )
    if missing_required_recovery_case_ids:
        failures.append(
            "missing required recovery diagnostic case(s): "
            + ", ".join(missing_required_recovery_case_ids)
        )
    if missing_required_recovery_phases:
        failures.append(
            "missing required recovery diagnostic phase(s): "
            + ", ".join(missing_required_recovery_phases)
        )
    if missing_required_suggestion_case_ids:
        failures.append(
            "missing required suggestion diagnostic case(s): "
            + ", ".join(missing_required_suggestion_case_ids)
        )
    if missing_required_suggestion_kinds:
        failures.append(
            "missing required suggestion kind(s): "
            + ", ".join(missing_required_suggestion_kinds)
        )

    min_diagnostic_count = int(contract["minimum_diagnostic_count"])
    min_fixit_count = int(contract["minimum_machine_applicable_fixit_count"])
    min_recovery_diagnostic_count = int(
        contract.get("minimum_recovery_diagnostic_count", 0)
    )
    min_suggestion_diagnostic_count = int(
        contract.get("minimum_suggestion_diagnostic_count", 0)
    )
    if len(entries) < min_diagnostic_count:
        failures.append(
            f"diagnostic entry count {len(entries)} below required {min_diagnostic_count}"
        )
    if machine_applicable_fixit_count < min_fixit_count:
        failures.append(
            "machine-applicable fix-it count "
            f"{machine_applicable_fixit_count} below required {min_fixit_count}"
        )
    if recovery_diagnostic_count < min_recovery_diagnostic_count:
        failures.append(
            "recovery diagnostic count "
            f"{recovery_diagnostic_count} below required {min_recovery_diagnostic_count}"
        )
    if suggestion_diagnostic_count < min_suggestion_diagnostic_count:
        failures.append(
            "suggestion diagnostic count "
            f"{suggestion_diagnostic_count} below required {min_suggestion_diagnostic_count}"
        )
    if len(observed_native_recovery_fixtures) < minimum_native_recovery_fixture_count:
        failures.append(
            "native recovery fixture count "
            f"{len(observed_native_recovery_fixtures)} below required "
            f"{minimum_native_recovery_fixture_count}"
        )

    digest_input = {
        "files": files,
        "entries": entries,
        "code_families": dict(sorted(code_families.items())),
        "native_recovery_fixtures": sorted(observed_native_recovery_fixtures),
        "suggestion_kinds": sorted(observed_suggestion_kinds),
    }
    deterministic_digest = hashlib.sha256(stable_json(digest_input).encode("utf-8")).hexdigest()
    checks = {
        "manifest_exists": manifest_path.is_file(),
        "all_manifest_files_exist": not missing_files,
        "all_diagnostic_payloads_structured": not any(not entry["ok"] for entry in entries),
        "minimum_diagnostic_count_met": len(entries) >= min_diagnostic_count,
        "minimum_machine_applicable_fixit_count_met": machine_applicable_fixit_count >= min_fixit_count,
        "required_fixit_codes_present": not missing_required_fixit_codes,
        "minimum_recovery_diagnostic_count_met": recovery_diagnostic_count >= min_recovery_diagnostic_count,
        "required_recovery_case_ids_present": not missing_required_recovery_case_ids,
        "required_recovery_phases_present": not missing_required_recovery_phases,
        "minimum_suggestion_diagnostic_count_met": (
            suggestion_diagnostic_count >= min_suggestion_diagnostic_count
        ),
        "required_suggestion_case_ids_present": (
            not missing_required_suggestion_case_ids
        ),
        "required_suggestion_kinds_present": not missing_required_suggestion_kinds,
        "structured_suggestion_payloads_valid": not any(
            entry["has_suggestion"] and not entry["suggestions_ok"] for entry in entries
        ),
        "structured_recovery_payloads_valid": not any(
            entry["has_recovery"] and not entry["recovery_ok"] for entry in entries
        ),
        "minimum_native_recovery_fixture_count_met": len(observed_native_recovery_fixtures)
        >= minimum_native_recovery_fixture_count,
        "native_recovery_fixtures_within_root": native_recovery_fixture_within_root_count
        == native_recovery_fixture_count,
        "native_recovery_fixture_sources_match": native_recovery_fixture_source_match_count
        == native_recovery_fixture_count,
        "native_recovery_fixture_expected_codes_match": native_recovery_fixture_expected_code_match_count
        == native_recovery_fixture_count,
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
        "recovery_diagnostic_count": recovery_diagnostic_count,
        "suggestion_diagnostic_count": suggestion_diagnostic_count,
        "suggestion_count": suggestion_count,
        "native_recovery_fixture_count": native_recovery_fixture_count,
        "unique_native_recovery_fixture_count": len(observed_native_recovery_fixtures),
        "native_recovery_fixture_root": display_path(native_recovery_fixture_root),
        "native_recovery_fixture_within_root_count": native_recovery_fixture_within_root_count,
        "native_recovery_fixture_source_match_count": native_recovery_fixture_source_match_count,
        "native_recovery_fixture_expected_code_match_count": native_recovery_fixture_expected_code_match_count,
        "minimum_native_recovery_fixture_count": minimum_native_recovery_fixture_count,
        "required_fixit_codes": sorted(required_fixit_codes),
        "missing_required_fixit_codes": missing_required_fixit_codes,
        "required_recovery_case_ids": sorted(required_recovery_case_ids),
        "missing_required_recovery_case_ids": missing_required_recovery_case_ids,
        "required_recovery_phases": sorted(required_recovery_phases),
        "missing_required_recovery_phases": missing_required_recovery_phases,
        "minimum_suggestion_diagnostic_count": min_suggestion_diagnostic_count,
        "required_suggestion_case_ids": sorted(required_suggestion_case_ids),
        "missing_required_suggestion_case_ids": missing_required_suggestion_case_ids,
        "required_suggestion_kinds": sorted(required_suggestion_kinds),
        "missing_required_suggestion_kinds": missing_required_suggestion_kinds,
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
        f"- Recovery diagnostics: `{summary['recovery_diagnostic_count']}`\n"
        f"- Suggestion diagnostics: `{summary['suggestion_diagnostic_count']}`\n"
        f"- Native recovery fixtures: `{summary['unique_native_recovery_fixture_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(f"summary_path: {display_path(JSON_OUT)}")
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
