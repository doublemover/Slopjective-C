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

SEED_ROWS = [
    ("V013-SPEC-01", "FAM-SPEC", "WL-SPEC", "[v0.13][spec] Seed 01", "artifact-spec-01", (), "small", "AC-V013-SPEC-01", 80, 5, 0, "Tier-0"),
    ("V013-SPEC-02", "FAM-SPEC", "WL-SPEC", "[v0.13][spec] Seed 02", "artifact-spec-02", ("V013-SPEC-01",), "small", "AC-V013-SPEC-02", 78, 5, 0, "Tier-0"),
    ("V013-SPEC-03", "FAM-SPEC", "WL-SPEC", "[v0.13][spec] Seed 03", "artifact-spec-03", (), "small", "AC-V013-SPEC-03", 76, 4, 0, "Tier-0"),
    ("V013-SPEC-04", "FAM-SPEC", "WL-SPEC", "[v0.13][spec] Seed 04", "artifact-spec-04", (), "small", "AC-V013-SPEC-04", 74, 4, 0, "Tier-0"),
    ("V013-TOOL-01", "FAM-TOOL", "WL-TOOL", "[v0.13][tooling] Seed 01", "artifact-tool-01", ("V013-SPEC-02",), "medium", "AC-V013-TOOL-01", 72, 4, 1, "Tier-1"),
    ("V013-TOOL-02", "FAM-TOOL", "WL-TOOL", "[v0.13][tooling] Seed 02", "artifact-tool-02", ("V013-SPEC-02",), "medium", "AC-V013-TOOL-02", 70, 4, 1, "Tier-1"),
    ("V013-TOOL-03", "FAM-TOOL", "WL-TOOL", "[v0.13][tooling] Seed 03", "artifact-tool-03", ("V013-SPEC-02",), "medium", "AC-V013-TOOL-03", 68, 4, 1, "Tier-1"),
    ("V013-TOOL-04", "FAM-TOOL", "WL-TOOL", "[v0.13][tooling] Seed 04", "artifact-tool-04", ("V013-SPEC-03",), "medium", "AC-V013-TOOL-04", 66, 3, 1, "Tier-1"),
    ("V013-GOV-01", "FAM-GOV", "WL-GOV", "[v0.13][governance] Seed 01", "artifact-gov-01", (), "small", "AC-V013-GOV-01", 64, 3, 0, "Tier-1"),
    ("V013-GOV-02", "FAM-GOV", "WL-GOV", "[v0.13][governance] Seed 02", "artifact-gov-02", ("V013-GOV-01",), "small", "AC-V013-GOV-02", 62, 3, 0, "Tier-1"),
    ("V013-GOV-03", "FAM-GOV", "WL-GOV", "[v0.13][governance] Seed 03", "artifact-gov-03", (), "small", "AC-V013-GOV-03", 60, 3, 0, "Tier-1"),
    ("V013-GOV-04", "FAM-GOV", "WL-GOV", "[v0.13][governance] Seed 04", "artifact-gov-04", (), "small", "AC-V013-GOV-04", 58, 3, 0, "Tier-1"),
    ("V013-CONF-01", "FAM-CONF", "WL-CONF", "[v0.13][conformance] Seed 01", "artifact-conf-01", ("V013-TOOL-01", "V013-TOOL-02"), "medium", "AC-V013-CONF-01", 56, 3, 1, "Tier-2"),
    ("V013-CONF-02", "FAM-CONF", "WL-CONF", "[v0.13][conformance] Seed 02", "artifact-conf-02", ("V013-TOOL-03",), "medium", "AC-V013-CONF-02", 54, 3, 1, "Tier-2"),
    ("V013-CONF-03", "FAM-CONF", "WL-CONF", "[v0.13][conformance] Seed 03", "artifact-conf-03", ("V013-TOOL-04",), "medium", "AC-V013-CONF-03", 52, 3, 1, "Tier-2"),
    ("V013-CONF-04", "FAM-CONF", "WL-CONF", "[v0.13][conformance] Seed 04", "artifact-conf-04", ("V013-SPEC-04",), "medium", "AC-V013-CONF-04", 50, 3, 1, "Tier-2"),
    ("V013-REL-01", "FAM-REL", "WL-REL", "[v0.13][release] Seed 01", "artifact-rel-01", ("V013-GOV-02", "V013-CONF-01"), "large", "AC-V013-REL-01", 48, 2, 2, "Tier-2"),
    ("V013-REL-02", "FAM-REL", "WL-REL", "[v0.13][release] Seed 02", "artifact-rel-02", ("V013-CONF-02", "V013-REL-01"), "large", "AC-V013-REL-02", 46, 2, 2, "Tier-2"),
    ("V013-REL-03", "FAM-REL", "WL-REL", "[v0.13][release] Seed 03", "artifact-rel-03", ("V013-GOV-03", "V013-CONF-03", "V013-REL-02"), "large", "AC-V013-REL-03", 44, 2, 2, "Tier-2"),
    ("V013-REL-04", "FAM-REL", "WL-REL", "[v0.13][release] Seed 04", "artifact-rel-04", ("V013-CONF-04", "V013-GOV-04", "V013-REL-03"), "large", "AC-V013-REL-04", 42, 2, 2, "Tier-2"),
]

EDGE_ROWS = [
    ("EDGE-V013-001", "V013-SPEC-01", "V013-SPEC-02", "hard", "spec-01 unlocks spec-02"),
    ("EDGE-V013-002", "V013-GOV-01", "V013-GOV-02", "hard", "gov-01 unlocks gov-02"),
    ("EDGE-V013-003", "V013-SPEC-02", "V013-TOOL-01", "hard", "spec-02 unlocks tool-01"),
    ("EDGE-V013-004", "V013-SPEC-02", "V013-TOOL-02", "hard", "spec-02 unlocks tool-02"),
    ("EDGE-V013-005", "V013-SPEC-02", "V013-TOOL-03", "hard", "spec-02 unlocks tool-03"),
    ("EDGE-V013-006", "V013-TOOL-01", "V013-CONF-01", "hard", "tool-01 unlocks conf-01"),
    ("EDGE-V013-007", "V013-TOOL-02", "V013-CONF-01", "hard", "tool-02 unlocks conf-01"),
    ("EDGE-V013-008", "V013-TOOL-03", "V013-CONF-02", "hard", "tool-03 unlocks conf-02"),
    ("EDGE-V013-009", "V013-GOV-02", "V013-REL-01", "hard", "gov-02 unlocks rel-01"),
    ("EDGE-V013-010", "V013-CONF-01", "V013-REL-01", "hard", "conf-01 unlocks rel-01"),
    ("EDGE-V013-011", "V013-REL-01", "V013-REL-02", "hard", "rel-01 unlocks rel-02"),
    ("EDGE-V013-012", "V013-CONF-02", "V013-REL-02", "hard", "conf-02 unlocks rel-02"),
    ("EDGE-V013-013", "V013-GOV-03", "V013-REL-03", "hard", "gov-03 unlocks rel-03"),
    ("EDGE-V013-014", "V013-REL-02", "V013-REL-03", "hard", "rel-02 unlocks rel-03"),
    ("EDGE-V013-015", "V013-SPEC-03", "V013-TOOL-04", "hard", "spec-03 unlocks tool-04"),
    ("EDGE-V013-016", "V013-TOOL-04", "V013-CONF-03", "hard", "tool-04 unlocks conf-03"),
    ("EDGE-V013-017", "V013-CONF-03", "V013-REL-03", "hard", "conf-03 unlocks rel-03"),
    ("EDGE-V013-018", "V013-SPEC-04", "V013-CONF-04", "hard", "spec-04 unlocks conf-04"),
    ("EDGE-V013-019", "V013-CONF-04", "V013-REL-04", "hard", "conf-04 unlocks rel-04"),
    ("EDGE-V013-020", "V013-GOV-04", "V013-REL-04", "hard", "gov-04 unlocks rel-04"),
    ("EDGE-V013-021", "V013-REL-03", "V013-REL-04", "hard", "rel-03 unlocks rel-04"),
]

WAVE_ROWS = {
    "W0": ("V013-GOV-01", "V013-GOV-03", "V013-GOV-04", "V013-SPEC-01", "V013-SPEC-03", "V013-SPEC-04"),
    "W1": ("V013-CONF-04", "V013-GOV-02", "V013-SPEC-02", "V013-TOOL-04"),
    "W2": ("V013-CONF-03", "V013-TOOL-01", "V013-TOOL-02", "V013-TOOL-03"),
    "W3": ("V013-CONF-01", "V013-CONF-02"),
    "W4": ("V013-REL-01",),
    "W5": ("V013-REL-02",),
    "W6": ("V013-REL-03",),
    "W7": ("V013-REL-04",),
}

BATCH_ROWS = [
    ("BATCH-V013-S-01", "small", WAVE_ROWS["W0"], "none", "wave-0-ready"),
    ("BATCH-V013-S-02", "small", WAVE_ROWS["W1"], "wave-0-ready", "wave-1-ready"),
    ("BATCH-V013-M-03", "medium", WAVE_ROWS["W2"], "wave-1-ready", "wave-2-ready"),
    ("BATCH-V013-M-04", "medium", WAVE_ROWS["W3"], "wave-2-ready", "wave-3-ready"),
    ("BATCH-V013-L-05", "large", WAVE_ROWS["W4"], "wave-3-ready", "wave-4-ready"),
    ("BATCH-V013-L-06", "large", WAVE_ROWS["W5"], "wave-4-ready", "wave-5-ready"),
    ("BATCH-V013-L-07", "large", WAVE_ROWS["W6"], "wave-5-ready", "wave-6-ready"),
    ("BATCH-V013-L-08", "large", WAVE_ROWS["W7"], "wave-6-ready", "wave-7-ready"),
]


def run_main(args: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = generate_seed_batches.main(args)
    return code, stdout.getvalue(), stderr.getvalue()


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def build_matrix_text() -> str:
    lines = [
        "# Deterministic Seed Matrix",
        "",
        "Snapshot date: 2026-02-23",
        "",
        "| Seed ID | Family | Worklane | Proposed GH issue title | Primary artifact targets | Depends on | Shard class | Acceptance gate ID |",
        "| --- | --- | --- | --- | --- | --- | --- | --- |",
    ]
    for seed_id, family, worklane, title, artifact, depends_on, shard_class, gate_id, *_ in SEED_ROWS:
        deps = ", ".join(depends_on) if depends_on else "none"
        lines.append(
            f"| `{seed_id}` | `{family}` | `{worklane}` | {title} | `{artifact}` | `{deps}` | `{shard_class}` | `{gate_id}` |"
        )
    lines.extend(
        [
            "",
            "| Edge ID | Predecessor | Successor | Type | Rationale |",
            "| --- | --- | --- | --- | --- |",
        ]
    )
    for edge_id, predecessor, successor, edge_type, rationale in EDGE_ROWS:
        lines.append(
            f"| `{edge_id}` | `{predecessor}` | `{successor}` | `{edge_type}` | {rationale} |"
        )
    lines.extend(
        [
            "",
            "| Wave | Seeds eligible for execution (all hard predecessors satisfied) |",
            "| --- | --- |",
        ]
    )
    for wave_id, seed_ids in WAVE_ROWS.items():
        lines.append(f"| `{wave_id}` | `{', '.join(seed_ids)}` |")
    lines.extend(
        [
            "",
            "| Batch ID | Class | Included seed IDs | Entry prerequisites | Exit signal |",
            "| --- | --- | --- | --- | --- |",
        ]
    )
    for batch_id, batch_class, seed_ids, entry_prerequisites, exit_signal in BATCH_ROWS:
        lines.append(
            f"| `{batch_id}` | `{batch_class}` | `{', '.join(seed_ids)}` | {entry_prerequisites} | {exit_signal} |"
        )
    lines.extend(
        [
            "",
            "| Seed ID | CPI | DUV | RBV | ERC | ECP | DC | Priority score | Tier |",
            "| --- | --- | --- | --- | --- | --- | --- | --- | --- |",
        ]
    )
    for seed_id, *_prefix, priority_score, duv, dc, tier in SEED_ROWS:
        lines.append(
            f"| `{seed_id}` | 5 | {duv} | 4 | 3 | 2 | {dc} | {priority_score} | `{tier}` |"
        )
    return "\n".join(lines) + "\n"


def write_matrix_fixture(path: Path) -> None:
    path.write_text(build_matrix_text(), encoding="utf-8")


def write_owner_map_fixture(path: Path, *, matrix_path: Path) -> None:
    payload = {
        "contract_id": generate_seed_batches.OWNER_MAP_CONTRACT_ID,
        "seed_id": generate_seed_batches.OWNER_MAP_SEED_ID,
        "snapshot_date": "2026-02-23",
        "source_matrix_path": generate_seed_batches.display_path(matrix_path),
        "owner_registry": [
            {
                "seed_id": seed_id,
                "owner_primary": f"owner.primary.{index:02d}",
                "owner_backup": f"owner.backup.{index:02d}",
            }
            for index, (seed_id, *_rest) in enumerate(sorted(SEED_ROWS), start=1)
        ],
    }
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def build_repo_fixture_paths(tmp_path: Path) -> tuple[Path, Path]:
    fixture_root = ROOT / "tmp" / "test_seed_generation" / tmp_path.name
    fixture_root.mkdir(parents=True, exist_ok=True)
    return fixture_root / "seed_matrix.md", fixture_root / "owner_map.json"


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
    matrix_path, owner_map_path = build_repo_fixture_paths(tmp_path)
    output_without_owner_map = tmp_path / "without_owner_map.json"
    output_with_owner_map = tmp_path / "with_owner_map.json"
    write_matrix_fixture(matrix_path)
    write_owner_map_fixture(owner_map_path, matrix_path=matrix_path)

    code_without, stdout_without, stderr_without = run_main(
        [
            "--matrix",
            str(matrix_path),
            "--output",
            str(output_without_owner_map),
        ]
    )
    code_with, stdout_with, stderr_with = run_main(
        [
            "--matrix",
            str(matrix_path),
            "--output",
            str(output_with_owner_map),
            "--owner-map-json",
            str(owner_map_path),
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
    expected_owners = read_owner_registry(owner_map_path)

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
        assert owners == ("unassigned", "unassigned")
        assert templates_with[seed_id] == expected_owners[seed_id]


def test_owner_map_fails_when_seed_assignment_is_missing(tmp_path: Path) -> None:
    matrix_path, owner_map_path = build_repo_fixture_paths(tmp_path)
    write_matrix_fixture(matrix_path)
    write_owner_map_fixture(owner_map_path, matrix_path=matrix_path)
    payload = read_json(owner_map_path)
    payload["owner_registry"] = payload["owner_registry"][:-1]

    broken_owner_map_path = tmp_path / "owner_map_missing_seed.json"
    output_path = tmp_path / "out.json"
    broken_owner_map_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    code, stdout, stderr = run_main(
        [
            "--matrix",
            str(matrix_path),
            "--output",
            str(output_path),
            "--owner-map-json",
            str(broken_owner_map_path),
        ]
    )

    assert code == 1
    assert stdout == ""
    assert "owner map missing seed id(s):" in stderr


def test_owner_map_fails_when_owner_value_is_placeholder(tmp_path: Path) -> None:
    matrix_path, owner_map_path = build_repo_fixture_paths(tmp_path)
    write_matrix_fixture(matrix_path)
    write_owner_map_fixture(owner_map_path, matrix_path=matrix_path)
    payload = read_json(owner_map_path)
    payload["owner_registry"][0]["owner_primary"] = "TBD"

    broken_owner_map_path = tmp_path / "owner_map_invalid_owner.json"
    output_path = tmp_path / "out.json"
    broken_owner_map_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    code, stdout, stderr = run_main(
        [
            "--matrix",
            str(matrix_path),
            "--output",
            str(output_path),
            "--owner-map-json",
            str(broken_owner_map_path),
        ]
    )

    assert code == 1
    assert stdout == ""
    assert "owner_primary" in stderr
    assert "placeholder value" in stderr


def test_owner_map_fails_when_snapshot_date_mismatches_matrix(tmp_path: Path) -> None:
    matrix_path, owner_map_path = build_repo_fixture_paths(tmp_path)
    write_matrix_fixture(matrix_path)
    write_owner_map_fixture(owner_map_path, matrix_path=matrix_path)
    payload = read_json(owner_map_path)
    payload["snapshot_date"] = "2099-12-31"

    broken_owner_map_path = tmp_path / "owner_map_snapshot_mismatch.json"
    output_path = tmp_path / "out.json"
    broken_owner_map_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    code, stdout, stderr = run_main(
        [
            "--matrix",
            str(matrix_path),
            "--output",
            str(output_path),
            "--owner-map-json",
            str(broken_owner_map_path),
        ]
    )

    assert code == 1
    assert stdout == ""
    assert "owner map snapshot_date mismatch;" in stderr


def test_fails_when_seed_table_dependencies_do_not_match_edge_table(
    tmp_path: Path,
) -> None:
    matrix_text = build_matrix_text()
    lines = matrix_text.splitlines()
    for index, line in enumerate(lines):
        candidate = line.strip()
        if not candidate.startswith("| `V013-TOOL-03`"):
            continue
        cells = [cell.strip() for cell in candidate.strip("|").split("|")]
        assert len(cells) == 8
        assert cells[5] == "`V013-SPEC-02`"
        cells[5] = "`none`"
        lines[index] = "| " + " | ".join(cells) + " |"
        break
    else:
        raise AssertionError("expected seed-table row for V013-TOOL-03")

    modified = "\n".join(lines)
    if matrix_text.endswith("\n"):
        modified += "\n"

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
