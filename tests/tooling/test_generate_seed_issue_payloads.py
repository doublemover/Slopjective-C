import importlib.util
import io
import json
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path
from typing import Any

SCRIPT_PATH = Path(__file__).resolve().parents[2] / "scripts" / "generate_seed_issue_payloads.py"
SPEC = importlib.util.spec_from_file_location("generate_seed_issue_payloads", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/generate_seed_issue_payloads.py for tests.")
generate_seed_issue_payloads = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = generate_seed_issue_payloads
SPEC.loader.exec_module(generate_seed_issue_payloads)

ROOT = Path(__file__).resolve().parents[2]
GRAPH_PATH = ROOT / "spec/planning/v013_seed_dependency_graph.json"


def run_main(args: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = generate_seed_issue_payloads.main(args)
    return code, stdout.getvalue(), stderr.getvalue()


def build_minimal_graph_payload() -> dict[str, Any]:
    validation_commands = [
        "python scripts/spec_lint.py",
        "python scripts/check_issue_checkbox_drift.py",
    ]
    return {
        "contract_id": "V013-TOOL-03-SEED-DAG-v1",
        "seed_id": "V013-TOOL-03",
        "graph": {
            "seeds": [
                {
                    "seed_id": "V013-SPEC-01",
                    "proposed_issue_title": "[v0.13][spec] Example seed one",
                    "wave_id": "W0",
                    "depends_on": [],
                    "shard_class": "small",
                    "acceptance_gate_id": "AC-V013-SPEC-01",
                    "priority": {
                        "priority_score": 50,
                        "duv": 4,
                        "dc": 1,
                        "tier": "Tier-0",
                    },
                },
                {
                    "seed_id": "V013-TOOL-02",
                    "proposed_issue_title": "[v0.13][tooling] Example seed two",
                    "wave_id": "W1",
                    "depends_on": ["V013-SPEC-01"],
                    "shard_class": "medium",
                    "acceptance_gate_id": "AC-V013-TOOL-02",
                    "priority": {
                        "priority_score": 40,
                        "duv": 3,
                        "dc": 2,
                        "tier": "Tier-1",
                    },
                },
            ]
        },
        "batch_skeletons": {
            "batches": [
                {
                    "batch_id": "BATCH-V013-S-01",
                    "issue_templates": [
                        {
                            "seed_id": "V013-SPEC-01",
                            "labels": [
                                "phase:v013",
                                "family:spec",
                                "lane:spec",
                                "seed:V013-SPEC-01",
                                "shard:small",
                                "priority:tier-0",
                            ],
                            "body_fields": {
                                "validation_commands": validation_commands,
                            },
                        },
                        {
                            "seed_id": "V013-TOOL-02",
                            "labels": [
                                "phase:v013",
                                "family:tooling",
                                "lane:tooling",
                                "seed:V013-TOOL-02",
                                "shard:medium",
                                "priority:tier-1",
                            ],
                            "body_fields": {
                                "validation_commands": validation_commands,
                            },
                        },
                    ],
                }
            ]
        },
    }


def write_graph(path: Path, payload: dict[str, Any]) -> None:
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def write_issues(path: Path, payload: Any) -> None:
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def test_repository_graph_output_is_deterministic_and_contains_required_fields(
    tmp_path: Path,
) -> None:
    output_a = tmp_path / "payloads_a.json"
    output_b = tmp_path / "payloads_b.json"

    code_a, stdout_a, stderr_a = run_main(
        ["--graph", str(GRAPH_PATH), "--output", str(output_a)]
    )
    code_b, stdout_b, stderr_b = run_main(
        ["--graph", str(GRAPH_PATH), "--output", str(output_b)]
    )

    assert code_a == 0
    assert code_b == 0
    assert stderr_a == ""
    assert stderr_b == ""
    assert "seed-issue-payloads: OK" in stdout_a
    assert "seed-issue-payloads: OK" in stdout_b
    assert output_a.read_bytes() == output_b.read_bytes()

    payload = json.loads(output_a.read_text(encoding="utf-8"))
    records = payload["records"]
    assert payload["record_count"] == 20
    assert len(records) == 20
    assert records[0]["seed_id"] == "V013-SPEC-02"
    assert records[-1]["seed_id"] == "V013-REL-03"

    required_fields = {
        "seed_id",
        "labels",
        "milestone",
        "dependencies",
        "shard_class",
        "acceptance_gate_id",
        "validation_commands",
    }
    for record in records:
        assert required_fields.issubset(record.keys())
        assert isinstance(record["labels"], list)
        assert isinstance(record["validation_commands"], list)
        assert "issue_number" not in record
        assert "issue_url" not in record
        assert "issue_state" not in record
        assert "closed_at" not in record
        assert "completion_status" not in record

    tool_03 = next(record for record in records if record["seed_id"] == "V013-TOOL-03")
    assert tool_03["milestone"] == "v0.13 Seed Wave W1"
    assert tool_03["dependencies"] == ["V013-SPEC-02"]
    assert tool_03["shard_class"] == "medium"
    assert tool_03["acceptance_gate_id"] == "AC-V013-TOOL-03"
    assert tool_03["validation_commands"] == [
        "python scripts/spec_lint.py",
        "python scripts/check_issue_checkbox_drift.py",
    ]
    assert "seed:V013-TOOL-03" in tool_03["labels"]


def test_fail_fast_when_seed_metadata_is_missing(tmp_path: Path) -> None:
    payload = build_minimal_graph_payload()
    del payload["graph"]["seeds"][0]["acceptance_gate_id"]

    graph_path = tmp_path / "missing_seed_field.json"
    output_path = tmp_path / "out.json"
    write_graph(graph_path, payload)

    code, stdout, stderr = run_main(
        ["--graph", str(graph_path), "--output", str(output_path)]
    )

    assert code == 1
    assert stdout == ""
    assert "missing required key(s): acceptance_gate_id" in stderr


def test_fail_fast_when_dependency_references_unknown_seed(tmp_path: Path) -> None:
    payload = build_minimal_graph_payload()
    payload["graph"]["seeds"][1]["depends_on"] = ["V013-GOV-99"]

    graph_path = tmp_path / "unknown_dependency.json"
    output_path = tmp_path / "out.json"
    write_graph(graph_path, payload)

    code, stdout, stderr = run_main(
        ["--graph", str(graph_path), "--output", str(output_path)]
    )

    assert code == 1
    assert stdout == ""
    assert "depends on unknown seed id: V013-GOV-99" in stderr


def test_fail_fast_when_issue_template_metadata_is_missing(tmp_path: Path) -> None:
    payload = build_minimal_graph_payload()
    payload["batch_skeletons"]["batches"][0]["issue_templates"] = [
        payload["batch_skeletons"]["batches"][0]["issue_templates"][0]
    ]

    graph_path = tmp_path / "missing_template.json"
    output_path = tmp_path / "out.json"
    write_graph(graph_path, payload)

    code, stdout, stderr = run_main(
        ["--graph", str(graph_path), "--output", str(output_path)]
    )

    assert code == 1
    assert stdout == ""
    assert "missing issue template metadata for seed id(s): V013-TOOL-02" in stderr


def test_fail_fast_when_source_contract_id_is_not_canonical(tmp_path: Path) -> None:
    payload = build_minimal_graph_payload()
    payload["contract_id"] = "V013-TOOL-03-SEED-DAG-v999"

    graph_path = tmp_path / "bad_contract_id.json"
    output_path = tmp_path / "out.json"
    write_graph(graph_path, payload)

    code, stdout, stderr = run_main(
        ["--graph", str(graph_path), "--output", str(output_path)]
    )

    assert code == 1
    assert stdout == ""
    assert "root.contract_id must be V013-TOOL-03-SEED-DAG-v1;" in stderr


def test_fail_fast_when_source_seed_id_is_not_canonical(tmp_path: Path) -> None:
    payload = build_minimal_graph_payload()
    payload["seed_id"] = "V013-TOOL-99"

    graph_path = tmp_path / "bad_seed_id.json"
    output_path = tmp_path / "out.json"
    write_graph(graph_path, payload)

    code, stdout, stderr = run_main(
        ["--graph", str(graph_path), "--output", str(output_path)]
    )

    assert code == 1
    assert stdout == ""
    assert "root.seed_id must be V013-TOOL-03;" in stderr


def test_overlay_success_adds_completion_metadata_for_unambiguous_matches(
    tmp_path: Path,
) -> None:
    payload = build_minimal_graph_payload()

    graph_path = tmp_path / "graph.json"
    issues_path = tmp_path / "issues.json"
    output_path = tmp_path / "out.json"

    write_graph(graph_path, payload)
    write_issues(
        issues_path,
        [
            {
                "number": 1201,
                "title": "[W0][V013-SPEC-01] Closeout",
                "labels": [{"name": "seed:V013-SPEC-01"}],
                "state": "closed",
                "closed_at": "2026-02-23T16:00:00Z",
                "html_url": "https://example.test/issues/1201",
            },
            {
                "number": 1202,
                "title": "[W1][V013-TOOL-02] In progress",
                "labels": [{"name": "lane:B"}],
                "state": "open",
                "url": "https://example.test/issues/1202",
            },
        ],
    )

    code, stdout, stderr = run_main(
        [
            "--graph",
            str(graph_path),
            "--issues-json",
            str(issues_path),
            "--output",
            str(output_path),
        ]
    )

    assert code == 0
    assert stderr == ""
    assert "seed-issue-payloads: OK" in stdout

    generated = json.loads(output_path.read_text(encoding="utf-8"))
    records_by_seed = {record["seed_id"]: record for record in generated["records"]}

    spec_01 = records_by_seed["V013-SPEC-01"]
    assert spec_01["issue_number"] == 1201
    assert spec_01["issue_url"] == "https://example.test/issues/1201"
    assert spec_01["issue_state"] == "closed"
    assert spec_01["closed_at"] == "2026-02-23T16:00:00Z"
    assert spec_01["completion_status"] == "completed"

    tool_02 = records_by_seed["V013-TOOL-02"]
    assert tool_02["issue_number"] == 1202
    assert tool_02["issue_url"] == "https://example.test/issues/1202"
    assert tool_02["issue_state"] == "open"
    assert tool_02["closed_at"] is None
    assert tool_02["completion_status"] == "incomplete"


def test_malformed_overlay_fails_fast(tmp_path: Path) -> None:
    payload = build_minimal_graph_payload()

    graph_path = tmp_path / "graph.json"
    issues_path = tmp_path / "issues.json"
    output_path = tmp_path / "out.json"

    write_graph(graph_path, payload)
    write_issues(issues_path, {"items": []})

    code, stdout, stderr = run_main(
        [
            "--graph",
            str(graph_path),
            "--issues-json",
            str(issues_path),
            "--output",
            str(output_path),
        ]
    )

    assert code == 1
    assert stdout == ""
    assert "must be an array" in stderr


def test_overlay_ambiguous_and_missing_matches_are_left_unresolved(
    tmp_path: Path,
) -> None:
    payload = build_minimal_graph_payload()

    graph_path = tmp_path / "graph.json"
    issues_path = tmp_path / "issues.json"
    output_path = tmp_path / "out.json"

    write_graph(graph_path, payload)
    write_issues(
        issues_path,
        [
            {
                "number": 1301,
                "title": "[W0][V013-SPEC-01] Candidate one",
                "labels": [],
                "state": "open",
            },
            {
                "number": 1302,
                "title": "[W0][V013-SPEC-01] Candidate two",
                "labels": [],
                "state": "open",
            },
        ],
    )

    code, stdout, stderr = run_main(
        [
            "--graph",
            str(graph_path),
            "--issues-json",
            str(issues_path),
            "--output",
            str(output_path),
        ]
    )

    assert code == 0
    assert stderr == ""
    assert "seed-issue-payloads: OK" in stdout

    generated = json.loads(output_path.read_text(encoding="utf-8"))
    records_by_seed = {record["seed_id"]: record for record in generated["records"]}

    # V013-SPEC-01 has two candidates (ambiguous), V013-TOOL-02 has no candidates (missing).
    for seed_id in ("V013-SPEC-01", "V013-TOOL-02"):
        record = records_by_seed[seed_id]
        assert "issue_number" not in record
        assert "issue_url" not in record
        assert "issue_state" not in record
        assert "closed_at" not in record
        assert "completion_status" not in record
