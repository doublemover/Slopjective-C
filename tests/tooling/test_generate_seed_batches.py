import importlib.util
import io
import json
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path
from typing import Any

SCRIPT_PATH = Path(__file__).resolve().parents[2] / "scripts" / "generate_seed_batches.py"
SPEC = importlib.util.spec_from_file_location("generate_seed_batches", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/generate_seed_batches.py for tests.")
generate_seed_batches = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = generate_seed_batches
SPEC.loader.exec_module(generate_seed_batches)

ROOT = Path(__file__).resolve().parents[2]
MATRIX_PATH = ROOT / "spec/planning/v013_future_work_seed_matrix.md"
OWNER_MAP_PATH = ROOT / "spec/planning/v013_seed_owner_registry.json"


def run_main(args: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = generate_seed_batches.main(args)
    return code, stdout.getvalue(), stderr.getvalue()


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def read_owner_registry(path: Path) -> dict[str, tuple[str, str]]:
    payload = read_json(path)
    registry: dict[str, tuple[str, str]] = {}
    for row in payload["owner_registry"]:
        registry[row["seed_id"]] = (row["owner_primary"], row["owner_backup"])
    return registry


def write_matrix(path: Path, text: str) -> None:
    path.write_text(text, encoding="utf-8")


def test_owner_map_populates_templates_and_preserves_deterministic_ordering(
    tmp_path: Path,
) -> None:
    output_without_owner_map = tmp_path / "without_owner_map.json"
    output_with_owner_map = tmp_path / "with_owner_map.json"

    code_without, stdout_without, stderr_without = run_main(
        [
            "--matrix",
            str(MATRIX_PATH),
            "--output",
            str(output_without_owner_map),
        ]
    )
    code_with, stdout_with, stderr_with = run_main(
        [
            "--matrix",
            str(MATRIX_PATH),
            "--output",
            str(output_with_owner_map),
            "--owner-map-json",
            str(OWNER_MAP_PATH),
        ]
    )

    assert code_without == 0
    assert code_with == 0
    assert stderr_without == ""
    assert stderr_with == ""
    assert "seed-dag: OK" in stdout_without
    assert "seed-dag: OK" in stdout_with

    payload_without = read_json(output_without_owner_map)
    payload_with = read_json(output_with_owner_map)
    expected_owners = read_owner_registry(OWNER_MAP_PATH)

    assert payload_without["execution_order"] == payload_with["execution_order"]
    assert payload_without["wave_eligibility"] == payload_with["wave_eligibility"]

    batches_without = payload_without["batch_skeletons"]["batches"]
    batches_with = payload_with["batch_skeletons"]["batches"]
    assert [row["batch_id"] for row in batches_without] == [
        row["batch_id"] for row in batches_with
    ]

    templates_without: dict[str, tuple[str, str]] = {}
    templates_with: dict[str, tuple[str, str]] = {}
    for batch_without, batch_with in zip(batches_without, batches_with, strict=True):
        assert batch_without["ordered_seed_ids"] == batch_with["ordered_seed_ids"]
        assert [template["seed_id"] for template in batch_without["issue_templates"]] == [
            template["seed_id"] for template in batch_with["issue_templates"]
        ]

        for template in batch_without["issue_templates"]:
            body_fields = template["body_fields"]
            templates_without[template["seed_id"]] = (
                body_fields["owner_primary"],
                body_fields["owner_backup"],
            )

        for template in batch_with["issue_templates"]:
            body_fields = template["body_fields"]
            templates_with[template["seed_id"]] = (
                body_fields["owner_primary"],
                body_fields["owner_backup"],
            )

    assert set(templates_with.keys()) == set(expected_owners.keys())

    for seed_id, owners in templates_without.items():
        assert owners == ("TBD", "TBD")
        assert templates_with[seed_id] == expected_owners[seed_id]


def test_owner_map_fails_when_seed_assignment_is_missing(tmp_path: Path) -> None:
    payload = read_json(OWNER_MAP_PATH)
    payload["owner_registry"] = payload["owner_registry"][:-1]

    owner_map_path = tmp_path / "owner_map_missing_seed.json"
    output_path = tmp_path / "out.json"
    owner_map_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    code, stdout, stderr = run_main(
        [
            "--matrix",
            str(MATRIX_PATH),
            "--output",
            str(output_path),
            "--owner-map-json",
            str(owner_map_path),
        ]
    )

    assert code == 1
    assert stdout == ""
    assert "owner map missing seed id(s):" in stderr


def test_owner_map_fails_when_owner_value_is_placeholder(tmp_path: Path) -> None:
    payload = read_json(OWNER_MAP_PATH)
    payload["owner_registry"][0]["owner_primary"] = "TBD"

    owner_map_path = tmp_path / "owner_map_invalid_owner.json"
    output_path = tmp_path / "out.json"
    owner_map_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    code, stdout, stderr = run_main(
        [
            "--matrix",
            str(MATRIX_PATH),
            "--output",
            str(output_path),
            "--owner-map-json",
            str(owner_map_path),
        ]
    )

    assert code == 1
    assert stdout == ""
    assert "owner_primary" in stderr
    assert "placeholder value" in stderr


def test_owner_map_fails_when_snapshot_date_mismatches_matrix(tmp_path: Path) -> None:
    payload = read_json(OWNER_MAP_PATH)
    payload["snapshot_date"] = "2099-12-31"

    owner_map_path = tmp_path / "owner_map_snapshot_mismatch.json"
    output_path = tmp_path / "out.json"
    owner_map_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    code, stdout, stderr = run_main(
        [
            "--matrix",
            str(MATRIX_PATH),
            "--output",
            str(output_path),
            "--owner-map-json",
            str(owner_map_path),
        ]
    )

    assert code == 1
    assert stdout == ""
    assert "owner map snapshot_date mismatch;" in stderr


def test_fails_when_seed_table_dependencies_do_not_match_edge_table(
    tmp_path: Path,
) -> None:
    matrix_text = MATRIX_PATH.read_text(encoding="utf-8")
    original_row = (
        "| `V013-TOOL-03` | `FAM-TOOL` | `WL-TOOL` | "
        "`[v0.13][tooling] Generate seed DAG and batch skeletons from matrix` | "
        "`scripts/generate_seed_batches.py`, `spec/planning/v013_seed_dependency_graph.json` | "
        "`V013-SPEC-02` | `medium` | `AC-V013-TOOL-03` |"
    )
    mutated_row = (
        "| `V013-TOOL-03` | `FAM-TOOL` | `WL-TOOL` | "
        "`[v0.13][tooling] Generate seed DAG and batch skeletons from matrix` | "
        "`scripts/generate_seed_batches.py`, `spec/planning/v013_seed_dependency_graph.json` | "
        "`none` | `medium` | `AC-V013-TOOL-03` |"
    )
    assert original_row in matrix_text
    modified = matrix_text.replace(original_row, mutated_row, 1)

    matrix_path = tmp_path / "dependency_mismatch_matrix.md"
    output_path = tmp_path / "out.json"
    write_matrix(matrix_path, modified)

    code, stdout, stderr = run_main(
        [
            "--matrix",
            str(matrix_path),
            "--output",
            str(output_path),
        ]
    )

    assert code == 1
    assert stdout == ""
    assert "seed V013-TOOL-03 dependency mismatch between seed table and edge table;" in stderr
