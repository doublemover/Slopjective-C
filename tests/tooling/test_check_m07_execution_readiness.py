import importlib.util
import io
import sys
import tempfile
import unittest
from contextlib import redirect_stdout
from pathlib import Path

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2] / "scripts" / "check_m07_execution_readiness.py"
)
SPEC = importlib.util.spec_from_file_location("check_m07_execution_readiness", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_m07_execution_readiness.py")
check_m07_execution_readiness = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = check_m07_execution_readiness
SPEC.loader.exec_module(check_m07_execution_readiness)


class CheckM07ExecutionReadinessTests(unittest.TestCase):
    def run_main(self, argv: list[str]) -> tuple[int, str]:
        stdout = io.StringIO()
        with redirect_stdout(stdout):
            code = check_m07_execution_readiness.main(argv)
        return code, stdout.getvalue()

    def test_fails_when_required_files_are_missing(self) -> None:
        with tempfile.TemporaryDirectory() as tmp_dir:
            code, output = self.run_main(["--m07-dir", tmp_dir])

        self.assertEqual(code, 1)
        self.assertIn("status: FAIL", output)
        self.assertIn("missing:", output)

    def test_passes_when_all_required_files_exist(self) -> None:
        with tempfile.TemporaryDirectory() as tmp_dir:
            root = Path(tmp_dir)
            for relative in check_m07_execution_readiness.REQUIRED_FILES:
                path = root / relative
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_text("# ok\n", encoding="utf-8")

            code, output = self.run_main(["--m07-dir", tmp_dir])

        self.assertEqual(code, 0)
        self.assertIn("status: PASS", output)


if __name__ == "__main__":
    unittest.main()
