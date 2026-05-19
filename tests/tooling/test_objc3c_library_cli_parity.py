from __future__ import annotations

from pathlib import Path

import pytest

from objc3c_library_cli_parity_assertions import (
    assert_failure_contains,
    assert_parity_pass_summary,
    assert_sha256_proxy_summary,
    assert_synthetic_fixture_authenticity_summary,
    load_summary,
)
from objc3c_library_cli_parity_fixtures import (
    write_json_fixture,
    write_synthetic_ll,
    write_synthetic_manifest,
    write_text_fixture,
)
from objc3c_library_cli_parity_support import FIXTURE_ROOT, parity


def test_parity_pass(tmp_path: Path) -> None:
    library_dir = tmp_path / "library"
    cli_dir = tmp_path / "cli"

    write_text_fixture(library_dir / "module.diagnostics.json", '{"ok":true}\n')
    write_text_fixture(cli_dir / "module.diagnostics.json", '{"ok":true}\n')
    write_text_fixture(library_dir / "module.manifest.json", '{"module":"m"}\n')
    write_text_fixture(cli_dir / "module.manifest.json", '{"module":"m"}\n')

    summary_out = tmp_path / "summary.json"
    exit_code = parity.run(
        [
            "--library-dir",
            str(library_dir),
            "--cli-dir",
            str(cli_dir),
            "--artifacts",
            "module.diagnostics.json",
            "module.manifest.json",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 0
    assert_parity_pass_summary(load_summary(summary_out))


def test_parity_fail_on_digest_mismatch(tmp_path: Path) -> None:
    library_dir = tmp_path / "library"
    cli_dir = tmp_path / "cli"

    write_text_fixture(library_dir / "module.ll", "define i32 @main() { ret i32 1 }\n")
    write_text_fixture(cli_dir / "module.ll", "define i32 @main() { ret i32 2 }\n")

    summary_out = tmp_path / "summary.json"
    exit_code = parity.run(
        [
            "--library-dir",
            str(library_dir),
            "--cli-dir",
            str(cli_dir),
            "--artifacts",
            "module.ll",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    assert_failure_contains(load_summary(summary_out), "digest mismatch")


def test_parity_supports_sha256_proxy_for_missing_object_artifact(tmp_path: Path) -> None:
    library_dir = tmp_path / "library"
    cli_dir = tmp_path / "cli"

    digest = "3" * 64
    write_text_fixture(library_dir / "module.o.sha256", digest + "\n")
    write_text_fixture(cli_dir / "module.o.sha256", digest + "\n")

    summary_out = tmp_path / "summary.json"
    exit_code = parity.run(
        [
            "--library-dir",
            str(library_dir),
            "--cli-dir",
            str(cli_dir),
            "--artifacts",
            "module.o",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 0
    assert_sha256_proxy_summary(load_summary(summary_out))


def test_parity_check_golden_detects_drift(tmp_path: Path) -> None:
    library_dir = tmp_path / "library"
    cli_dir = tmp_path / "cli"

    write_text_fixture(library_dir / "module.manifest.json", '{"module":"m"}\n')
    write_text_fixture(cli_dir / "module.manifest.json", '{"module":"m"}\n')

    summary_out = tmp_path / "summary.json"
    golden = tmp_path / "golden.json"
    write_code = parity.run(
        [
            "--library-dir",
            str(library_dir),
            "--cli-dir",
            str(cli_dir),
            "--artifacts",
            "module.manifest.json",
            "--summary-out",
            str(summary_out),
            "--golden-summary",
            str(golden),
            "--write-golden",
        ]
    )
    assert write_code == 0
    assert golden.is_file()

    write_text_fixture(cli_dir / "module.manifest.json", '{"module":"m2"}\n')
    check_code = parity.run(
        [
            "--library-dir",
            str(library_dir),
            "--cli-dir",
            str(cli_dir),
            "--artifacts",
            "module.manifest.json",
            "--summary-out",
            str(summary_out),
            "--golden-summary",
            str(golden),
            "--check-golden",
        ]
    )

    assert check_code == 1
    assert_failure_contains(load_summary(summary_out), "golden summary drift detected")


def test_parity_applies_synthetic_fixture_authenticity_contract(tmp_path: Path) -> None:
    library_dir = tmp_path / "library"
    cli_dir = tmp_path / "cli"

    write_synthetic_ll(library_dir / "module.ll")
    write_synthetic_ll(cli_dir / "module.ll")
    write_synthetic_manifest(library_dir / "module.manifest.json")
    write_synthetic_manifest(cli_dir / "module.manifest.json")

    summary_out = tmp_path / "summary.json"
    exit_code = parity.run(
        [
            "--library-dir",
            str(library_dir),
            "--cli-dir",
            str(cli_dir),
            "--artifacts",
            "module.manifest.json",
            "module.ll",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 0
    assert_synthetic_fixture_authenticity_summary(load_summary(summary_out))


def test_parity_fail_closes_on_partial_synthetic_fixture_labeling(tmp_path: Path) -> None:
    library_dir = tmp_path / "library"
    cli_dir = tmp_path / "cli"

    write_synthetic_ll(library_dir / "module.ll")
    write_synthetic_ll(cli_dir / "module.ll")
    write_synthetic_manifest(library_dir / "module.manifest.json")
    write_json_fixture(
        cli_dir / "module.manifest.json",
        {
            "entrypoint": "main",
            "module": "fixture_library_cli_parity",
            "source": "fixtures/native/library_cli_parity",
        },
    )

    summary_out = tmp_path / "summary.json"
    exit_code = parity.run(
        [
            "--library-dir",
            str(library_dir),
            "--cli-dir",
            str(cli_dir),
            "--artifacts",
            "module.manifest.json",
            "module.ll",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    assert_failure_contains(
        load_summary(summary_out),
        "synthetic fixture authenticity mismatch",
    )


def test_parity_fixture_contract_matches_golden_and_is_replay_deterministic(
    tmp_path: Path,
) -> None:
    summary_a = tmp_path / "fixture_summary_a.json"
    summary_b = tmp_path / "fixture_summary_b.json"
    golden = FIXTURE_ROOT / "golden_summary.json"

    argv = [
        "--library-dir",
        str(FIXTURE_ROOT / "library"),
        "--cli-dir",
        str(FIXTURE_ROOT / "cli"),
        "--summary-out",
        str(summary_a),
        "--golden-summary",
        str(golden),
        "--check-golden",
    ]
    first_code = parity.run(argv)
    assert first_code == 0
    first_payload = load_summary(summary_a)
    assert first_payload == load_summary(golden)

    second_code = parity.run(
        [
            "--library-dir",
            str(FIXTURE_ROOT / "library"),
            "--cli-dir",
            str(FIXTURE_ROOT / "cli"),
            "--summary-out",
            str(summary_b),
            "--golden-summary",
            str(golden),
            "--check-golden",
        ]
    )
    assert second_code == 0
    assert first_payload == load_summary(summary_b)


def test_parity_reports_source_kind_mismatch(tmp_path: Path) -> None:
    library_dir = tmp_path / "library"
    cli_dir = tmp_path / "cli"

    (library_dir / "module.o").parent.mkdir(parents=True, exist_ok=True)
    (library_dir / "module.o").write_bytes(b"\x00OBJ")
    write_text_fixture(cli_dir / "module.o.sha256", "1" * 64 + "\n")

    summary_out = tmp_path / "summary.json"
    exit_code = parity.run(
        [
            "--library-dir",
            str(library_dir),
            "--cli-dir",
            str(cli_dir),
            "--artifacts",
            "module.o",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    assert_failure_contains(load_summary(summary_out), "source-kind mismatch for module.o")


def test_parity_reports_invalid_proxy_digest_failure(tmp_path: Path) -> None:
    library_dir = tmp_path / "library"
    cli_dir = tmp_path / "cli"

    write_text_fixture(library_dir / "module.o.sha256", "NOT-A-DIGEST\n")
    write_text_fixture(cli_dir / "module.o.sha256", "3" * 64 + "\n")

    summary_out = tmp_path / "summary.json"
    exit_code = parity.run(
        [
            "--library-dir",
            str(library_dir),
            "--cli-dir",
            str(cli_dir),
            "--artifacts",
            "module.o",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    assert_failure_contains(
        load_summary(summary_out),
        "library module.o: invalid sha256 proxy digest",
    )


def test_parity_reports_missing_cli_artifact_or_proxy(tmp_path: Path) -> None:
    library_dir = tmp_path / "library"
    cli_dir = tmp_path / "cli"

    write_text_fixture(library_dir / "module.manifest.json", '{"module":"present"}\n')
    cli_dir.mkdir(parents=True, exist_ok=True)

    summary_out = tmp_path / "summary.json"
    exit_code = parity.run(
        [
            "--library-dir",
            str(library_dir),
            "--cli-dir",
            str(cli_dir),
            "--artifacts",
            "module.manifest.json",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    assert_failure_contains(
        load_summary(summary_out),
        "cli module.manifest.json: missing artifact and proxy digest",
    )


@pytest.mark.parametrize(
    ("dimension_map", "message"),
    [
        ("diagnostics", "DIMENSION=ARTIFACT format"),
        ("unknown=module.ll", "unsupported dimension"),
        ("ir=", "artifact path must be non-empty"),
        ("ir=subdir/module.ll", "filename only"),
    ],
)
def test_parity_rejects_invalid_dimension_map_entries(
    tmp_path: Path,
    dimension_map: str,
    message: str,
) -> None:
    library_dir = tmp_path / "library"
    cli_dir = tmp_path / "cli"
    write_text_fixture(library_dir / "module.ll", "define i32 @main() { ret i32 0 }\n")
    write_text_fixture(cli_dir / "module.ll", "define i32 @main() { ret i32 0 }\n")

    with pytest.raises(ValueError, match=message):
        parity.run(
            [
                "--library-dir",
                str(library_dir),
                "--cli-dir",
                str(cli_dir),
                "--artifacts",
                "module.ll",
                "--dimension-map",
                dimension_map,
                "--summary-out",
                str(tmp_path / "summary.json"),
            ]
        )


def test_parity_rejects_artifacts_entries_with_path_segments(tmp_path: Path) -> None:
    library_dir = tmp_path / "library"
    cli_dir = tmp_path / "cli"
    write_text_fixture(library_dir / "module.ll", "define i32 @main() { ret i32 0 }\n")
    write_text_fixture(cli_dir / "module.ll", "define i32 @main() { ret i32 0 }\n")

    with pytest.raises(ValueError, match="filename only"):
        parity.run(
            [
                "--library-dir",
                str(library_dir),
                "--cli-dir",
                str(cli_dir),
                "--artifacts",
                "sub/module.ll",
                "--summary-out",
                str(tmp_path / "summary.json"),
            ]
        )


def test_parity_check_golden_reports_invalid_json(tmp_path: Path) -> None:
    library_dir = tmp_path / "library"
    cli_dir = tmp_path / "cli"
    write_text_fixture(library_dir / "module.manifest.json", '{"module":"m"}\n')
    write_text_fixture(cli_dir / "module.manifest.json", '{"module":"m"}\n')
    golden = tmp_path / "golden.json"
    write_text_fixture(golden, "{invalid-json}\n")

    summary_out = tmp_path / "summary.json"
    exit_code = parity.run(
        [
            "--library-dir",
            str(library_dir),
            "--cli-dir",
            str(cli_dir),
            "--artifacts",
            "module.manifest.json",
            "--summary-out",
            str(summary_out),
            "--golden-summary",
            str(golden),
            "--check-golden",
        ]
    )

    assert exit_code == 1
    assert_failure_contains(load_summary(summary_out), "golden summary parse error")
