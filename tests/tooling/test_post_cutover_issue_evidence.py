from __future__ import annotations

from copy import deepcopy
from pathlib import Path
import sys


ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from scripts import check_objc3c_post_cutover_issue_evidence as checker  # noqa: E402


def load_markdown() -> str:
    return checker.ISSUE_EVIDENCE_PATH.read_text(encoding="utf-8")


def load_matrix() -> dict[str, object]:
    return checker.load_json_object(checker.CAPABILITY_MATRIX_PATH)


def load_evidence_map() -> dict[str, object]:
    return checker.load_json_object(checker.EVIDENCE_MAP_PATH)


def validate(markdown: str) -> dict[str, object]:
    return checker.validate_markdown_text(
        markdown,
        matrix=load_matrix(),
        evidence_map=load_evidence_map(),
        registered_actions=set(checker.public_workflow_action_names()),
    )


def test_post_cutover_issue_evidence_covers_every_issue_and_owned_capability() -> None:
    summary = validate(load_markdown())

    assert summary["ok"] is True
    issue_evidence = summary["issue_evidence"]
    assert issue_evidence["issue_count"] == len(checker.REQUIRED_ISSUES)
    assert issue_evidence["source_path_count"] >= len(checker.REQUIRED_ISSUES)
    capability_rows = summary["capability_rows"]
    assert capability_rows["owned_capability_count"] >= len(checker.REQUIRED_ISSUES)
    assert capability_rows["implemented_owned_capability_count"] > 0


def test_post_cutover_issue_evidence_rejects_missing_issue_row() -> None:
    markdown = "\n".join(
        line
        for line in load_markdown().splitlines()
        if not line.startswith("| #8179 ")
    )

    summary = validate(markdown)

    assert summary["ok"] is False
    assert any("#8179: expected exactly one evidence row" in failure for failure in summary["failures"])


def test_post_cutover_issue_evidence_rejects_unregistered_public_command() -> None:
    markdown = load_markdown().replace(
        "npm run objc3c -- validate-release-operations",
        "npm run objc3c -- validate-nonexistent-release-action",
        1,
    )

    summary = validate(markdown)

    assert summary["ok"] is False
    assert any("public action is not registered" in failure for failure in summary["failures"])


def test_post_cutover_issue_evidence_rejects_generated_source_truth() -> None:
    markdown = load_markdown().replace(
        "docs/support/capability_matrix.json",
        "tmp/reports/generated-capability-matrix.json",
        1,
    )

    summary = validate(markdown)

    assert summary["ok"] is False
    assert any("generated output cannot be source truth" in failure for failure in summary["failures"])


def test_post_cutover_issue_evidence_rejects_owned_capability_without_matrix_row() -> None:
    markdown = (
        load_markdown()
        + "\n- `language.nonexistent.unsupported-contract-for-test`\n"
    )

    summary = validate(markdown)

    assert summary["ok"] is False
    assert any("owned capability is not present" in failure for failure in summary["failures"])


def test_post_cutover_issue_evidence_rejects_owned_support_claim_missing_evidence() -> None:
    markdown = load_markdown()
    matrix = deepcopy(load_matrix())
    rows = matrix["capabilities"]
    assert isinstance(rows, list)
    for row in rows:
        assert isinstance(row, dict)
        if row.get("id") == "runtime.public-api.reflection":
            row["support_claims"] = ["objc3c.behavior.runtime.public-api.synthetic-missing"]
            break
    else:
        raise AssertionError("runtime.public-api.reflection row missing")

    summary = checker.validate_markdown_text(
        markdown,
        matrix=matrix,
        evidence_map=load_evidence_map(),
        registered_actions=set(checker.public_workflow_action_names()),
    )

    assert summary["ok"] is False
    assert any("support claim lacks evidence-map row" in failure for failure in summary["failures"])
