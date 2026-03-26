from __future__ import annotations

import json
from pathlib import Path
import re
import sys


REPO_ROOT = Path(__file__).resolve().parents[1]
NATIVE_ROOT = REPO_ROOT / "native" / "objc3c" / "src"
EXPECTED_PATH = (
    REPO_ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m315"
    / "m315_b005_comment_constexpr_and_contract_string_decontamination_sweep_edge_case_and_compatibility_completion_result.json"
)
SUMMARY_PATH = (
    REPO_ROOT
    / "tmp"
    / "reports"
    / "m315"
    / "M315-B005"
    / "comment_constexpr_contract_string_decontamination_summary.json"
)
TOKEN_PATTERN = re.compile(r"m\d{3}|M\d{3}")


def classify(rel_path: str, line: str) -> str | None:
    if rel_path == "native/objc3c/src/ast/objc3_ast.h" and (
        "tests/tooling/fixtures/" in line or "tests/tooling/runtime/" in line
    ):
        return "legacy_fixture_path_reference"
    if (
        "m248_integration_closeout_signoff" in line
        or "M248IntegrationCloseoutSignoff" in line
        or "m248-integration-closeout-signoff" in line
    ):
        return "legacy_m248_surface_identifier"
    if rel_path == "native/objc3c/src/pipeline/objc3_frontend_types.h" and (
        "portability_dependency_issue_ids" in line
        or "M264-E002" in line
        or "M265-E002" in line
        or "M266-E002" in line
        or "M267-E002" in line
        or "M268-E002" in line
        or "M269-E002" in line
        or "M270-E002" in line
        or "M271-E002" in line
        or "M272-E002" in line
        or "M273-E002" in line
        or "M274-E002" in line
    ):
        return "dependency_issue_array"
    if "next_issue" in line or "kObjc3RuntimeLiveDispatchGateNextIssue" in line:
        return "next_issue_schema_field"
    if line.strip() == '"M255-E002";':
        return "next_issue_schema_field"
    if "_issue=" in line:
        return "issue_key_schema_field"
    if rel_path in (
        "native/objc3c/src/lower/objc3_lowering_contract.h",
        "native/objc3c/src/lower/objc3_lowering_contract.cpp",
    ):
        return "transitional_source_model"
    return None


def collect_summary() -> dict:
    excluded_paths = {
        str(
            (NATIVE_ROOT / "io" / "objc3_toolchain_runtime_ga_operations_core_feature_surface.h")
            .resolve()
        ),
    }
    residuals: dict[str, list[dict[str, object]]] = {}
    disallowed: list[dict[str, object]] = []

    for path in sorted(NATIVE_ROOT.rglob("*")):
        if path.is_dir() or path.suffix == ".md":
            continue
        if str(path.resolve()) in excluded_paths:
            continue
        rel_path = path.relative_to(REPO_ROOT).as_posix()
        text = path.read_text(encoding="utf-8")
        for lineno, line in enumerate(text.splitlines(), start=1):
            if not TOKEN_PATTERN.search(line):
                continue
            classification = classify(rel_path, line)
            record = {
                "file": rel_path,
                "line": lineno,
                "text": line.strip(),
            }
            if classification is None:
                disallowed.append(record)
            else:
                residuals.setdefault(classification, []).append(record)

    total = sum(len(entries) for entries in residuals.values()) + len(disallowed)
    return {
        "match_count": total,
        "residual_class_counts": {
            key: len(value) for key, value in sorted(residuals.items())
        },
        "residual_examples": {
            key: value[:5] for key, value in sorted(residuals.items())
        },
        "disallowed_count": len(disallowed),
        "disallowed_examples": disallowed[:20],
    }


def main() -> int:
    expected = json.loads(EXPECTED_PATH.read_text(encoding="utf-8"))
    summary = collect_summary()
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    errors: list[str] = []
    if summary["match_count"] != expected["post_sweep_milestone_token_lines"]:
        errors.append(
            f"expected {expected['post_sweep_milestone_token_lines']} milestone-token lines, found {summary['match_count']}"
        )
    if summary["disallowed_count"] != expected["disallowed_residual_lines"]:
        errors.append(
            f"expected {expected['disallowed_residual_lines']} disallowed residual lines, found {summary['disallowed_count']}"
        )

    expected_counts = {
        key: value["count"]
        for key, value in expected["allowed_residual_classes"].items()
    }
    if summary["residual_class_counts"] != expected_counts:
        errors.append(
            "residual class counts drifted: "
            + json.dumps(summary["residual_class_counts"], sort_keys=True)
        )

    if errors:
        for error in errors:
            print(f"ERROR: {error}", file=sys.stderr)
        print(f"Summary written to {SUMMARY_PATH}", file=sys.stderr)
        return 1

    print("M315-B005 native-source edge sweep summary:")
    print(json.dumps(summary["residual_class_counts"], indent=2, sort_keys=True))
    print(f"Summary written to {SUMMARY_PATH}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
