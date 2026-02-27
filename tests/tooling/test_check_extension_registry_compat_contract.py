from __future__ import annotations

import importlib.util
import io
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2]
    / "scripts"
    / "check_extension_registry_compat_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_extension_registry_compat_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_extension_registry_compat_contract.py")
module = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = module
SPEC.loader.exec_module(module)

FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "extension_registry_compat_contract"
HAPPY_ROOT = FIXTURE_ROOT / "happy"
DRIFT_README_PATH = FIXTURE_ROOT / "drift_missing_validator_ordering" / "readme.md"
W2_DRIFT_ROOT = FIXTURE_ROOT / "w2_drift_parity_taxonomy_gating"
W2_INVALID_UTF8_ROOT = FIXTURE_ROOT / "w2_hard_fail_invalid_utf8"


def run_main(args: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = module.main(args)
    return code, stdout.getvalue(), stderr.getvalue()


def test_happy_path_is_deterministic() -> None:
    args = [
        "--package",
        str(HAPPY_ROOT / "package.md"),
        "--schema",
        str(HAPPY_ROOT / "schema.json"),
        "--readme",
        str(HAPPY_ROOT / "readme.md"),
    ]
    first_code, first_stdout, first_stderr = run_main(args)
    second_code, second_stdout, second_stderr = run_main(args)

    assert first_code == 0
    assert second_code == 0
    assert first_stderr == ""
    assert second_stderr == ""
    assert first_stdout == second_stdout
    assert first_stdout == (
        "extension-registry-compat-contract: OK\n"
        "- mode=extension-registry-compat-contract-v2\n"
        "- package=tests/tooling/fixtures/extension_registry_compat_contract/happy/package.md\n"
        "- schema=tests/tooling/fixtures/extension_registry_compat_contract/happy/schema.json\n"
        "- readme=tests/tooling/fixtures/extension_registry_compat_contract/happy/readme.md\n"
        "- checks_passed=62\n"
        "- fail_closed=true\n"
    )


def test_drift_path_returns_exit_1_with_stable_diagnostics() -> None:
    code, stdout, stderr = run_main(
        [
            "--package",
            str(HAPPY_ROOT / "package.md"),
            "--schema",
            str(HAPPY_ROOT / "schema.json"),
            "--readme",
            str(DRIFT_README_PATH),
        ]
    )

    assert code == 1
    assert stdout == ""
    assert stderr == (
        "extension-registry-compat-contract: contract drift detected (1 failed check(s)).\n"
        "drift findings:\n"
        "- readme:RDM-05\n"
        "  expected snippet: Validator ordering is fixed (`VAL-RC-01`..`VAL-RC-06`). Any "
        "non-zero exit code is a blocking failure.\n"
        "remediation:\n"
        "1. Restore missing snippet anchors, schema invariants, or W2 "
        "parity/taxonomy/publication-gating controls in the listed artifact(s).\n"
        "2. Re-run validator:\n"
        "python scripts/check_extension_registry_compat_contract.py --package "
        "tests/tooling/fixtures/extension_registry_compat_contract/happy/package.md --schema "
        "tests/tooling/fixtures/extension_registry_compat_contract/happy/schema.json --readme "
        "tests/tooling/fixtures/extension_registry_compat_contract/"
        "drift_missing_validator_ordering/readme.md\n"
    )


def test_w2_drift_path_orders_multi_finding_diagnostics_deterministically() -> None:
    args = [
        "--package",
        str(W2_DRIFT_ROOT / "package.md"),
        "--schema",
        str(W2_DRIFT_ROOT / "schema.json"),
        "--readme",
        str(W2_DRIFT_ROOT / "readme.md"),
    ]
    first_code, first_stdout, first_stderr = run_main(args)
    second_code, second_stdout, second_stderr = run_main(args)

    assert first_code == 1
    assert second_code == 1
    assert first_stdout == ""
    assert second_stdout == ""
    assert first_stderr == second_stderr
    assert first_stderr == (
        "extension-registry-compat-contract: contract drift detected (7 failed check(s)).\n"
        "drift findings:\n"
        "- package:PKG-05\n"
        "  expected snippet: | `VAL-RC-06` | `rg -n \"AC-V013-GOV-02|VAL-RC-|ESC-RC-\" "
        "tests/governance/registry_compat/README.md` | Exit `0`; all required governance "
        "identifiers are present. | Treat test/readme contract as incomplete and block "
        "publish. |\n"
        "- package:PKG-19\n"
        "  expected snippet: | `FTX-RC-M16-07` | `Hard` | Missing evidence anchors, missing "
        "exit-code reporting, or publication-gate ambiguity. | `REJECT` | `ESC-RC-03` "
        "(`E3`) | Not waiverable |\n"
        "- package:PKG-26\n"
        "  strict validator parity: package validator row set must be [\"VAL-RC-01\", "
        "\"VAL-RC-02\", \"VAL-RC-03\", \"VAL-RC-04\", \"VAL-RC-05\", "
        "\"VAL-RC-06\"] (found [\"VAL-RC-01\", \"VAL-RC-02\", \"VAL-RC-03\", "
        "\"VAL-RC-04\", \"VAL-RC-05\"])\n"
        "- package:PKG-27\n"
        "  deterministic failure taxonomy: expected row set [\"FTX-RC-M16-01\", "
        "\"FTX-RC-M16-02\", \"FTX-RC-M16-03\", \"FTX-RC-M16-04\", "
        "\"FTX-RC-M16-05\", \"FTX-RC-M16-06\", \"FTX-RC-M16-07\"] (found "
        "[\"FTX-RC-M16-01\", \"FTX-RC-M16-02\", \"FTX-RC-M16-03\", "
        "\"FTX-RC-M16-04\", \"FTX-RC-M16-05\", \"FTX-RC-M16-06\"])\n"
        "- package:PKG-28\n"
        "  deterministic publication gating: expected gate row set [\"PUB-RC-M16-01\", "
        "\"PUB-RC-M16-02\", \"PUB-RC-M16-03\", \"PUB-RC-M16-04\"] (found "
        "[\"PUB-RC-M16-01\", \"PUB-RC-M16-02\", \"PUB-RC-M16-03\"])\n"
        "- schema:SCH-19\n"
        "  strict validator parity mismatch: package validator rows [\"VAL-RC-01\", "
        "\"VAL-RC-02\", \"VAL-RC-03\", \"VAL-RC-04\", \"VAL-RC-05\"] != "
        "schema validator enum [\"VAL-RC-01\", \"VAL-RC-02\", \"VAL-RC-03\", "
        "\"VAL-RC-04\", \"VAL-RC-05\", \"VAL-RC-06\"]\n"
        "- readme:RDM-15\n"
        "  strict validator parity mismatch: package validator rows [\"VAL-RC-01\", "
        "\"VAL-RC-02\", \"VAL-RC-03\", \"VAL-RC-04\", \"VAL-RC-05\"] != "
        "readme validator rows [\"VAL-RC-01\", \"VAL-RC-02\", \"VAL-RC-03\", "
        "\"VAL-RC-04\", \"VAL-RC-05\", \"VAL-RC-06\"]\n"
        "remediation:\n"
        "1. Restore missing snippet anchors, schema invariants, or W2 "
        "parity/taxonomy/publication-gating controls in the listed artifact(s).\n"
        "2. Re-run validator:\n"
        "python scripts/check_extension_registry_compat_contract.py --package "
        "tests/tooling/fixtures/extension_registry_compat_contract/"
        "w2_drift_parity_taxonomy_gating/package.md --schema "
        "tests/tooling/fixtures/extension_registry_compat_contract/"
        "w2_drift_parity_taxonomy_gating/schema.json --readme "
        "tests/tooling/fixtures/extension_registry_compat_contract/"
        "w2_drift_parity_taxonomy_gating/readme.md\n"
    )


def test_hard_fail_returns_exit_2_for_missing_file() -> None:
    code, stdout, stderr = run_main(
        [
            "--package",
            str(HAPPY_ROOT / "package.md"),
            "--schema",
            str(HAPPY_ROOT / "schema.json"),
            "--readme",
            str(FIXTURE_ROOT / "drift_missing_validator_ordering" / "missing_readme.md"),
        ]
    )

    assert code == 2
    assert stdout == ""
    assert stderr == (
        "extension-registry-compat-contract: error: readme file does not exist: "
        "tests/tooling/fixtures/extension_registry_compat_contract/"
        "drift_missing_validator_ordering/missing_readme.md\n"
    )


def test_w2_hard_fail_returns_exit_2_for_invalid_utf8_readme() -> None:
    code, stdout, stderr = run_main(
        [
            "--package",
            str(W2_INVALID_UTF8_ROOT / "package.md"),
            "--schema",
            str(W2_INVALID_UTF8_ROOT / "schema.json"),
            "--readme",
            str(W2_INVALID_UTF8_ROOT / "readme.md"),
        ]
    )

    assert code == 2
    assert stdout == ""
    assert stderr == (
        "extension-registry-compat-contract: error: readme file is not valid UTF-8: "
        "tests/tooling/fixtures/extension_registry_compat_contract/"
        "w2_hard_fail_invalid_utf8/readme.md\n"
    )
