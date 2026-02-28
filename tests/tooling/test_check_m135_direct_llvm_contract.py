from __future__ import annotations

import importlib.util
import io
import json
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

SCRIPT_PATH = Path(__file__).resolve().parents[2] / "scripts" / "check_m135_direct_llvm_contract.py"
SPEC = importlib.util.spec_from_file_location("check_m135_direct_llvm_contract", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_m135_direct_llvm_contract.py")
module = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = module
SPEC.loader.exec_module(module)


def run_main(args: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = module.main(args)
    return code, stdout.getvalue(), stderr.getvalue()


def write_baseline_tree(root: Path) -> tuple[Path, Path, Path, Path]:
    m135_dir = root / "m135"
    m135_dir.mkdir(parents=True, exist_ok=True)
    (m135_dir / "m135_parallel_dispatch_plan_20260228.md").write_text(
        (
            "# M135 Parallel Dispatch Plan (2026-02-28)\n"
            "Final closeout sequence for M135: `A -> B -> C -> D -> E`.\n"
            "`npm run test:objc3c:lane-e`\n"
        ),
        encoding="utf-8",
    )
    (m135_dir / "m135_issue_packets_20260228.md").write_text(
        (
            "# M135 Issue Packets (2026-02-28)\n"
            "`M135-A001` -> `#4264`\n"
            "`M135-B001` -> `#4265`\n"
            "`M135-C001` -> `#4266`\n"
            "`M135-D001` -> `#4267`\n"
            "`M135-E001` -> `#4268`\n"
        ),
        encoding="utf-8",
    )
    (m135_dir / "m135_closeout_evidence_20260228.md").write_text(
        (
            "# M135 Closeout Evidence Packet (Draft, 2026-02-28)\n"
            "Gate issue: [#4268](https://github.com/doublemover/Slopjective-C/issues/4268)\n"
            "`npm run check:compiler-closeout:m135`\n"
            "Closeout remains blocked until dependency issues `#4264` through `#4267` are closed.\n"
        ),
        encoding="utf-8",
    )

    contracts_doc = root / "direct_llvm_emission_expectations.md"
    contracts_doc.write_text(
        (
            "# Direct LLVM Emission Expectations (M135)\n"
            "Contract ID: `objc3c-direct-llvm-emission/m135-v1`\n"
            "Fallback path `RunIRCompile(...clang -x ir...)` is forbidden for `.objc3` production emission.\n"
            "`python scripts/check_m135_direct_llvm_contract.py`\n"
        ),
        encoding="utf-8",
    )

    planning_lint_workflow = root / "planning-lint.yml"
    planning_lint_workflow.write_text(
        "name: Planning Lint\nrun: python scripts/check_m135_direct_llvm_contract.py\n",
        encoding="utf-8",
    )

    task_hygiene_workflow = root / "task-hygiene.yml"
    task_hygiene_workflow.write_text(
        "name: Task Hygiene\nrun: npm run check:compiler-closeout:m135\n",
        encoding="utf-8",
    )

    package_json = root / "package.json"
    package_json.write_text(
        json.dumps(
            {
                "scripts": {
                    "check:compiler-closeout:m135": (
                        "python scripts/check_m135_direct_llvm_contract.py "
                        "&& python scripts/spec_lint.py --glob \"spec/planning/compiler/m135/*.md\" "
                        "--glob \"docs/contracts/direct_llvm_emission_expectations.md\""
                    )
                }
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    return m135_dir, contracts_doc, planning_lint_workflow, task_hygiene_workflow


def test_happy_path_is_deterministic(tmp_path: Path) -> None:
    m135_dir, contracts_doc, planning_lint_workflow, task_hygiene_workflow = write_baseline_tree(
        tmp_path
    )
    package_json = tmp_path / "package.json"
    args = [
        "--m135-dir",
        str(m135_dir),
        "--direct-llvm-contract",
        str(contracts_doc),
        "--planning-lint-workflow",
        str(planning_lint_workflow),
        "--task-hygiene-workflow",
        str(task_hygiene_workflow),
        "--package-json",
        str(package_json),
    ]

    first_code, first_stdout, first_stderr = run_main(args)
    second_code, second_stdout, second_stderr = run_main(args)

    assert first_code == 0
    assert second_code == 0
    assert first_stderr == ""
    assert second_stderr == ""
    assert first_stdout == second_stdout
    assert "m135-direct-llvm-contract: OK" in first_stdout
    assert "- mode=m135-direct-llvm-contract-v1" in first_stdout
    assert "- fail_closed=true" in first_stdout


def test_drift_path_returns_exit_1_with_stable_diagnostics(tmp_path: Path) -> None:
    m135_dir, contracts_doc, planning_lint_workflow, task_hygiene_workflow = write_baseline_tree(
        tmp_path
    )
    issue_packets = m135_dir / "m135_issue_packets_20260228.md"
    issue_packets.write_text(
        (
            "# M135 Issue Packets (2026-02-28)\n"
            "`M135-A001` -> `#4264`\n"
            "`M135-B001` -> `#4265`\n"
            "`M135-C001` -> `#4266`\n"
            "`M135-D001` -> `#4267`\n"
        ),
        encoding="utf-8",
    )

    args = [
        "--m135-dir",
        str(m135_dir),
        "--direct-llvm-contract",
        str(contracts_doc),
        "--planning-lint-workflow",
        str(planning_lint_workflow),
        "--task-hygiene-workflow",
        str(task_hygiene_workflow),
        "--package-json",
        str(tmp_path / "package.json"),
    ]
    first_code, first_stdout, first_stderr = run_main(args)
    second_code, second_stdout, second_stderr = run_main(args)

    assert first_code == 1
    assert second_code == 1
    assert first_stdout == ""
    assert second_stdout == ""
    assert first_stderr == second_stderr
    assert "m135-direct-llvm-contract: contract drift detected (1 failed check(s))." in first_stderr
    assert "- issue_packets:M135-PKT-06" in first_stderr


def test_hard_fail_returns_exit_2_for_invalid_package_json(tmp_path: Path) -> None:
    m135_dir, contracts_doc, planning_lint_workflow, task_hygiene_workflow = write_baseline_tree(
        tmp_path
    )
    package_json = tmp_path / "package.json"
    package_json.write_text("{ invalid json\n", encoding="utf-8")

    code, stdout, stderr = run_main(
        [
            "--m135-dir",
            str(m135_dir),
            "--direct-llvm-contract",
            str(contracts_doc),
            "--planning-lint-workflow",
            str(planning_lint_workflow),
            "--task-hygiene-workflow",
            str(task_hygiene_workflow),
            "--package-json",
            str(package_json),
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "m135-direct-llvm-contract: error: package_json parse error" in stderr
