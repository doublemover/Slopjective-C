import importlib.util
import io
import os
import subprocess
import sys
import unittest
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path
from unittest import mock

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2] / "scripts" / "generate_execution_microtasks.py"
)
SPEC = importlib.util.spec_from_file_location(
    "generate_execution_microtasks", SCRIPT_PATH
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/generate_execution_microtasks.py")
generate_execution_microtasks = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = generate_execution_microtasks
SPEC.loader.exec_module(generate_execution_microtasks)

FIXTURES_DIR = Path(__file__).resolve().parent / "fixtures" / "microtasks"
STATUS_INTEGRITY_FIXTURES = (
    Path(__file__).resolve().parent / "fixtures" / "remaining_tasks_status_integrity"
)


class GenerateExecutionMicrotasksTests(unittest.TestCase):
    def run_main(self, argv: list[str]) -> tuple[int, str, str]:
        stdout = io.StringIO()
        stderr = io.StringIO()
        with redirect_stdout(stdout), redirect_stderr(stderr):
            code = generate_execution_microtasks.main(argv)
        return code, stdout.getvalue(), stderr.getvalue()

    def test_fixed_generated_on_is_deterministic_and_matches_fixture(self) -> None:
        issues_path = FIXTURES_DIR / "issues_deterministic.json"
        expected_path = FIXTURES_DIR / "expected_deterministic_2026-02-23.md"
        expected = expected_path.read_text(encoding="utf-8")
        argv = [
            "--issues-json",
            str(issues_path),
            "--closed-count",
            "10",
            "--generated-on",
            "2026-02-23",
        ]

        code1, output1, stderr1 = self.run_main(argv)
        code2, output2, stderr2 = self.run_main(argv)

        self.assertEqual(code1, 0)
        self.assertEqual(code2, 0)
        self.assertEqual(stderr1, "")
        self.assertEqual(stderr2, "")
        self.assertEqual(output1, output2)
        self.assertEqual(output1, expected)
        self.assertNotIn("\r", output1)

    def test_status_integrity_check_fails_for_missing_status_catalog(self) -> None:
        catalog_path = STATUS_INTEGRITY_FIXTURES / "catalog_missing_status.json"
        argv = ["--catalog-json", str(catalog_path)]

        stdout = io.StringIO()
        stderr = io.StringIO()
        with redirect_stdout(stdout), redirect_stderr(stderr):
            with self.assertRaises(SystemExit) as context:
                generate_execution_microtasks.main(argv)

        self.assertEqual(context.exception.code, 2)
        self.assertEqual(stdout.getvalue(), "")
        self.assertIn("missing required 'execution_status'", stderr.getvalue())
        self.assertIn("--allow-missing-status", stderr.getvalue())

    def test_status_integrity_check_succeeds_for_valid_status_catalog(self) -> None:
        catalog_path = STATUS_INTEGRITY_FIXTURES / "catalog_valid_status.json"
        code, output, stderr = self.run_main(["--catalog-json", str(catalog_path)])

        self.assertEqual(code, 0)
        self.assertEqual(output, "")
        self.assertEqual(stderr, "")

    def test_allow_missing_status_override_for_integrity_check(self) -> None:
        catalog_path = STATUS_INTEGRITY_FIXTURES / "catalog_missing_status.json"
        code, output, stderr = self.run_main(
            ["--catalog-json", str(catalog_path), "--allow-missing-status"]
        )

        self.assertEqual(code, 0)
        self.assertEqual(output, "")
        self.assertEqual(stderr, "")

    def test_generated_on_falls_back_to_source_date_epoch(self) -> None:
        issues_path = FIXTURES_DIR / "issues_deterministic.json"
        argv = [
            "--issues-json",
            str(issues_path),
            "--closed-count",
            "10",
        ]

        with mock.patch.dict(os.environ, {"SOURCE_DATE_EPOCH": "1771804800"}, clear=False):
            code, output, stderr = self.run_main(argv)

        self.assertEqual(code, 0)
        self.assertEqual(stderr, "")
        self.assertIn("_Generated on 2026-02-23 from GitHub issue snapshot._", output)

    def test_missing_generated_on_without_source_date_epoch_errors(self) -> None:
        issues_path = FIXTURES_DIR / "issues_deterministic.json"
        argv = [
            "--issues-json",
            str(issues_path),
            "--closed-count",
            "10",
        ]

        stdout = io.StringIO()
        stderr = io.StringIO()
        with mock.patch.dict(os.environ, {}, clear=True):
            with redirect_stdout(stdout), redirect_stderr(stderr):
                with self.assertRaises(SystemExit) as context:
                    generate_execution_microtasks.main(argv)

        self.assertEqual(context.exception.code, 2)
        self.assertEqual(stdout.getvalue(), "")
        self.assertIn("must provide --generated-on", stderr.getvalue())

    def test_subprocess_output_is_stable_across_hash_seeds(self) -> None:
        issues_path = FIXTURES_DIR / "issues_deterministic.json"
        expected_path = FIXTURES_DIR / "expected_deterministic_2026-02-23.md"
        expected_bytes = expected_path.read_bytes()
        cmd = [
            sys.executable,
            str(SCRIPT_PATH),
            "--issues-json",
            str(issues_path),
            "--closed-count",
            "10",
            "--generated-on",
            "2026-02-23",
        ]

        def run_with_hash_seed(seed: str) -> bytes:
            env = os.environ.copy()
            env["PYTHONHASHSEED"] = seed
            env.pop("SOURCE_DATE_EPOCH", None)
            result = subprocess.run(cmd, capture_output=True, check=False, env=env)
            self.assertEqual(
                result.returncode,
                0,
                result.stderr.decode("utf-8", errors="replace"),
            )
            return result.stdout

        output_seed_1 = run_with_hash_seed("1")
        output_seed_2 = run_with_hash_seed("2")

        self.assertEqual(output_seed_1, output_seed_2)
        self.assertEqual(output_seed_1, expected_bytes)
        self.assertNotIn(b"\r", output_seed_1)


if __name__ == "__main__":
    unittest.main()
