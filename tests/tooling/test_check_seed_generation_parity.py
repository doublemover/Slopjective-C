import importlib.util
import io
import json
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path
from types import ModuleType

ROOT = Path(__file__).resolve().parents[2]
CHECK_SCRIPT_PATH = ROOT / "scripts" / "check_seed_generation_parity.py"
GENERATE_BATCHES_PATH = ROOT / "scripts" / "generate_seed_batches.py"
GENERATE_PAYLOADS_PATH = ROOT / "scripts" / "generate_seed_issue_payloads.py"
SCENARIO_MATRIX_PATH = ROOT / "tests/tooling/fixtures/seed_parity_w2/scenario_matrix.json"

MATRIX_PATH = ROOT / "spec/planning/v013_future_work_seed_matrix.md"
OWNER_MAP_PATH = ROOT / "spec/planning/v013_seed_owner_registry.json"

SCENARIO_MATRIX = json.loads(SCENARIO_MATRIX_PATH.read_text(encoding="utf-8"))
GRAPH_DRIFT_SNAPSHOT_DATE = str(SCENARIO_MATRIX["graph_semantic_drift_snapshot_date"])
OWNER_MAP_MISMATCH_SNAPSHOT_DATE = str(SCENARIO_MATRIX["owner_map_snapshot_date_mismatch"])


def load_module(path: Path, module_name: str) -> ModuleType:
    spec = importlib.util.spec_from_file_location(module_name, path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"Unable to load module from {path}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[module_name] = module
    spec.loader.exec_module(module)
    return module


check_seed_generation_parity = load_module(
    CHECK_SCRIPT_PATH,
    "check_seed_generation_parity",
)
generate_seed_batches = load_module(
    GENERATE_BATCHES_PATH,
    "generate_seed_batches_for_parity_tests",
)
generate_seed_issue_payloads = load_module(
    GENERATE_PAYLOADS_PATH,
    "generate_seed_issue_payloads_for_parity_tests",
)


def run_main(args: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = check_seed_generation_parity.main(args)
    return code, stdout.getvalue(), stderr.getvalue()


def run_script_main(module: ModuleType, args: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = module.main(args)
    return code, stdout.getvalue(), stderr.getvalue()


def build_parity_fixture(tmp_path: Path) -> tuple[Path, Path]:
    graph_path = tmp_path / "seed_dependency_graph.json"
    payload_path = tmp_path / "seed_issue_payloads.json"

    graph_code, graph_stdout, graph_stderr = run_script_main(
        generate_seed_batches,
        [
            "--matrix",
            str(MATRIX_PATH),
            "--owner-map-json",
            str(OWNER_MAP_PATH),
            "--output",
            str(graph_path),
        ],
    )
    assert graph_code == 0, graph_stdout + graph_stderr

    payload_code, payload_stdout, payload_stderr = run_script_main(
        generate_seed_issue_payloads,
        [
            "--graph",
            str(graph_path),
            "--output",
            str(payload_path),
        ],
    )
    assert payload_code == 0, payload_stdout + payload_stderr

    return graph_path, payload_path


def test_parity_passes_for_deterministically_generated_artifacts(tmp_path: Path) -> None:
    graph_path, payload_path = build_parity_fixture(tmp_path)

    code, stdout, stderr = run_main(
        [
            "--matrix",
            str(MATRIX_PATH),
            "--owner-map-json",
            str(OWNER_MAP_PATH),
            "--graph-path",
            str(graph_path),
            "--payload-path",
            str(payload_path),
        ]
    )

    assert code == 0
    assert stderr == ""
    assert "seed-generation-parity: OK" in stdout
    assert "matrix=spec/planning/v013_future_work_seed_matrix.md" in stdout


def test_parity_fails_with_semantic_drift_classification_and_remediation(tmp_path: Path) -> None:
    graph_path, payload_path = build_parity_fixture(tmp_path)
    graph_payload = json.loads(graph_path.read_text(encoding="utf-8"))
    graph_payload["snapshot_date"] = GRAPH_DRIFT_SNAPSHOT_DATE
    graph_path.write_text(json.dumps(graph_payload, indent=2) + "\n", encoding="utf-8")

    code, stdout, stderr = run_main(
        [
            "--matrix",
            str(MATRIX_PATH),
            "--owner-map-json",
            str(OWNER_MAP_PATH),
            "--graph-path",
            str(graph_path),
            "--payload-path",
            str(payload_path),
        ]
    )

    assert code == 1
    assert stdout == ""
    assert "seed-generation-parity: parity check failed (1 artifact mismatch(es))." in stderr
    assert "drift classifications:" in stderr
    assert "- graph: semantic-drift" in stderr
    assert "classification: semantic-drift" in stderr
    assert "first mismatch path: $.snapshot_date" in stderr
    assert "regeneration commands:" in stderr
    assert "python scripts/generate_seed_batches.py" in stderr
    assert "python scripts/generate_seed_issue_payloads.py" in stderr
    assert "remediation steps:" in stderr
    assert "python scripts/check_seed_generation_parity.py" in stderr


def test_parity_fails_with_serialization_drift_classification_and_remediation(
    tmp_path: Path,
) -> None:
    graph_path, payload_path = build_parity_fixture(tmp_path)
    payload = json.loads(payload_path.read_text(encoding="utf-8"))
    payload_path.write_text(
        json.dumps(payload, indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )

    code, stdout, stderr = run_main(
        [
            "--matrix",
            str(MATRIX_PATH),
            "--owner-map-json",
            str(OWNER_MAP_PATH),
            "--graph-path",
            str(graph_path),
            "--payload-path",
            str(payload_path),
        ]
    )

    assert code == 1
    assert stdout == ""
    assert "seed-generation-parity: parity check failed (1 artifact mismatch(es))." in stderr
    assert "drift classifications:" in stderr
    assert "- payload: serialization-drift" in stderr
    assert "classification: serialization-drift" in stderr
    assert "first mismatch path: n/a (parsed JSON payloads are equal)" in stderr
    assert "diff (normalized text preview):" in stderr
    assert "remediation steps:" in stderr


def test_parity_fails_with_deterministic_hard_fail_diagnostics(tmp_path: Path) -> None:
    graph_path, payload_path = build_parity_fixture(tmp_path)
    owner_map_payload = json.loads(OWNER_MAP_PATH.read_text(encoding="utf-8"))
    owner_map_payload["snapshot_date"] = OWNER_MAP_MISMATCH_SNAPSHOT_DATE
    bad_owner_map_path = tmp_path / "bad_owner_map.json"
    bad_owner_map_path.write_text(
        json.dumps(owner_map_payload, indent=2) + "\n",
        encoding="utf-8",
    )

    code, stdout, stderr = run_main(
        [
            "--matrix",
            str(MATRIX_PATH),
            "--owner-map-json",
            str(bad_owner_map_path),
            "--graph-path",
            str(graph_path),
            "--payload-path",
            str(payload_path),
        ]
    )

    assert code == 1
    assert stdout == ""
    assert "graph regeneration failed with exit code 1" in stderr
    assert (
        "invocation: python scripts/generate_seed_batches.py --matrix "
        "spec/planning/v013_future_work_seed_matrix.md"
    ) in stderr
    assert "--output <tmp>/regenerated_seed_dependency_graph.json" in stderr
    assert "owner map snapshot_date mismatch;" in stderr
    assert "remediation steps:" in stderr
    assert "python scripts/check_seed_generation_parity.py" in stderr
