from __future__ import annotations

import json
from pathlib import Path

from objc3c_library_cli_parity_support import parity


def write_text_fixture(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def write_json_fixture(path: Path, payload: object) -> None:
    write_text_fixture(path, json.dumps(payload, indent=2) + "\n")


def write_source_mode_artifacts(
    out_dir: Path,
    *,
    emit_prefix: str,
    marker: str,
) -> None:
    out_dir.mkdir(parents=True, exist_ok=True)
    write_text_fixture(
        out_dir / f"{emit_prefix}.diagnostics.json",
        f'{{"marker":"{marker}"}}\n',
    )
    write_text_fixture(
        out_dir / f"{emit_prefix}.manifest.json",
        f'{{"module":"{marker}"}}\n',
    )
    write_text_fixture(
        out_dir / f"{emit_prefix}.ll",
        "define i32 @main() { ret i32 0 }\n",
    )
    (out_dir / f"{emit_prefix}.obj").write_bytes(b"\x00OBJ")


def write_synthetic_ll(path: Path) -> None:
    write_text_fixture(
        path,
        "\n".join(
            [
                f"; {parity.SYNTHETIC_FIXTURE_LABEL}",
                f"; artifact_family_id: {parity.SYNTHETIC_FIXTURE_FAMILY_ID}",
                "; provenance_class: synthetic_fixture",
                "; provenance_mode: fixture_curated",
                f"; fixture_family_id: {parity.SYNTHETIC_FIXTURE_FAMILY_ID}",
                f"; explicit_fixture_label: {parity.SYNTHETIC_FIXTURE_LABEL}",
                "define i32 @main() {",
                "entry:",
                "  ret i32 0",
                "}",
                "",
            ]
        ),
    )


def write_synthetic_manifest(path: Path) -> None:
    write_json_fixture(
        path,
        {
            "entrypoint": "main",
            "module": "fixture_library_cli_parity",
            "source": "fixtures/native/library_cli_parity",
            "artifact_authenticity": parity.synthetic_fixture_manifest_envelope(),
        },
    )


def write_capability_summary(
    path: Path,
    *,
    clang_found: bool = True,
    llc_found: bool = True,
    llc_supports_filetype_obj: bool = True,
    parity_ready: bool = True,
    blockers: list[str] | None = None,
) -> None:
    payload = {
        "mode": "objc3c-llvm-capabilities-v2",
        "clang": {
            "path": "clang",
            "found": clang_found,
        },
        "llc": {
            "path": "llc",
            "found": llc_found,
        },
        "llc_features": {
            "supports_filetype_obj": llc_supports_filetype_obj,
        },
        "sema_type_system_parity": {
            "parity_ready": parity_ready,
            "blockers": blockers or [],
        },
    }
    write_text_fixture(path, json.dumps(payload, indent=2) + "\n")


def write_source_and_bins(tmp_path: Path) -> tuple[Path, Path, Path]:
    source = tmp_path / "sample.objc3"
    source.write_text("fn main() -> i32 { return 0; }\n", encoding="utf-8")
    cli_bin = tmp_path / "objc3c-native.exe"
    c_api_bin = tmp_path / "objc3c-frontend-c-api-runner.exe"
    cli_bin.write_text("stub\n", encoding="utf-8")
    c_api_bin.write_text("stub\n", encoding="utf-8")
    return source, cli_bin, c_api_bin
