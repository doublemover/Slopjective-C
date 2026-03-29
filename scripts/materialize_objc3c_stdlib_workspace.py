#!/usr/bin/env python3
"""Materialize the checked-in stdlib workspace into a machine-owned artifact root."""

from __future__ import annotations

import argparse
import json
import shutil
from datetime import datetime, timezone
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
WORKSPACE_PATH = ROOT / "stdlib" / "workspace.json"
DEFAULT_OUTPUT_ROOT = ROOT / "tmp" / "artifacts" / "stdlib" / "workspace"
SUMMARY_CONTRACT_ID = "objc3c.stdlib.materialized.workspace.v1"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Copy the checked-in stdlib workspace to a machine-owned artifact root."
    )
    parser.add_argument(
        "--out-dir",
        default="",
        help="Target output directory. Defaults to tmp/artifacts/stdlib/workspace/<timestamp_pid>.",
    )
    return parser.parse_args()


def load_json(path: Path) -> dict[str, object]:
    return json.loads(path.read_text(encoding="utf-8"))


def resolve_out_dir(raw_out_dir: str) -> Path:
    if raw_out_dir:
        candidate = Path(raw_out_dir)
        if not candidate.is_absolute():
            candidate = ROOT / candidate
        return candidate.resolve()
    run_id = datetime.now(timezone.utc).strftime("%Y%m%d_%H%M%S_%f")
    return (DEFAULT_OUTPUT_ROOT / run_id).resolve()


def main() -> int:
    workspace = load_json(WORKSPACE_PATH)
    inventory = load_json(ROOT / str(workspace["module_inventory"]))
    stability_policy = load_json(ROOT / str(workspace["stability_policy"]))
    package_surface = load_json(ROOT / str(workspace["package_surface"]))
    output_root = resolve_out_dir(parse_args().out_dir)
    output_root.mkdir(parents=True, exist_ok=True)

    copied_paths: list[str] = []
    root_files = [
        Path("stdlib/README.md"),
        Path("stdlib/workspace.json"),
        Path("stdlib/module_inventory.json"),
        Path("stdlib/stability_policy.json"),
        Path("stdlib/package_surface.json"),
        Path("stdlib/core_architecture.json"),
        Path("docs/runbooks/objc3c_stdlib_foundation.md"),
        Path("docs/runbooks/objc3c_stdlib_core.md"),
        Path("spec/STANDARD_LIBRARY_CONTRACT.md"),
    ]
    for relative in root_files:
        destination = output_root / relative
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(ROOT / relative, destination)
        copied_paths.append(relative.as_posix())

    modules: list[dict[str, object]] = []
    for entry in inventory["canonical_modules"]:
        module_entry = dict(entry)
        workspace_root = ROOT / str(module_entry["workspace_root"])
        destination_root = output_root / str(module_entry["workspace_root"])
        destination_root.parent.mkdir(parents=True, exist_ok=True)
        if destination_root.exists():
            shutil.rmtree(destination_root)
        shutil.copytree(workspace_root, destination_root)
        copied_paths.extend(
            [
                str(module_entry["manifest"]),
                str(module_entry["source"]),
                str(module_entry["smoke_source"]),
            ]
        )
        modules.append(
            {
                "canonical_module": module_entry["module"],
                "implementation_module": module_entry["implementation_module"],
                "workspace_root": str(module_entry["workspace_root"]),
                "manifest": str(module_entry["manifest"]),
                "source": str(module_entry["source"]),
                "smoke_source": str(module_entry["smoke_source"]),
            }
        )

    summary_path = output_root / "stdlib.materialized.workspace.json"
    summary_path.write_text(
        json.dumps(
            {
                "contract_id": SUMMARY_CONTRACT_ID,
                "schema_version": 1,
                "generated_at_utc": datetime.now(timezone.utc).isoformat(),
                "output_root": repo_rel(output_root),
                "workspace_contract": repo_rel(WORKSPACE_PATH),
                "module_inventory": repo_rel(ROOT / str(workspace["module_inventory"])),
                "stability_policy": repo_rel(ROOT / str(workspace["stability_policy"])),
                "package_surface": repo_rel(ROOT / str(workspace["package_surface"])),
                "modules": modules,
                "copied_paths": sorted(dict.fromkeys(copied_paths)),
                "copied_file_count": len(dict.fromkeys(copied_paths)),
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    print(f"workspace_root: {repo_rel(output_root)}")
    print(f"summary_path: {repo_rel(summary_path)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
