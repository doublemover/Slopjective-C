from __future__ import annotations

import subprocess
from dataclasses import asdict, dataclass
from pathlib import Path

from .owners import (
    SOURCE_HYGIENE_GENERATED_REPORT_OWNER,
    SOURCE_HYGIENE_GENERATED_REPORT_OWNER_SURFACE,
)


@dataclass(frozen=True)
class GeneratedTruthBoundary:
    output_path: str
    source_paths: tuple[str, ...]
    generator_path: str
    owner_id: str = SOURCE_HYGIENE_GENERATED_REPORT_OWNER
    owner_surface: str = SOURCE_HYGIENE_GENERATED_REPORT_OWNER_SURFACE


GENERATED_TRUTH_BOUNDARIES: tuple[GeneratedTruthBoundary, ...] = (
    GeneratedTruthBoundary(
        output_path="site/index.md",
        source_paths=(
            "site/src/index.body.md",
            "site/src/index.contract.json",
        ),
        generator_path="scripts/build_site_index.py",
    ),
    GeneratedTruthBoundary(
        output_path="docs/objc3c-native.md",
        source_paths=(
            "docs/objc3c-native/src/README.md",
            "docs/objc3c-native/src/10-cli.md",
            "docs/objc3c-native/src/20-grammar.md",
            "docs/objc3c-native/src/30-semantics.md",
            "docs/objc3c-native/src/35-runtime-architecture.md",
            "docs/objc3c-native/src/40-diagnostics.md",
            "docs/objc3c-native/src/50-artifacts.md",
            "docs/objc3c-native/src/60-tests.md",
            "docs/objc3c-native/src/library-api.md",
        ),
        generator_path="scripts/build_objc3c_native_docs.py",
    ),
    GeneratedTruthBoundary(
        output_path="docs/runbooks/objc3c_public_command_surface.md",
        source_paths=(
            "scripts/objc3c_workflow/action_catalog.py",
            "schemas/objc3c-public-command-contract-v1.schema.json",
        ),
        generator_path="scripts/render_objc3c_public_command_surface.py",
    ),
)


def _git_ls_files(root: Path, pathspecs: tuple[str, ...]) -> list[str]:
    if not pathspecs:
        return []
    try:
        result = subprocess.run(
            ["git", "ls-files", *pathspecs],
            cwd=root,
            check=False,
            text=True,
            capture_output=True,
        )
    except OSError:
        return []
    if result.returncode != 0:
        return []
    return sorted(
        line.strip().replace("\\", "/")
        for line in result.stdout.splitlines()
        if line.strip()
    )


def tracked_generated_reports(root: Path) -> list[str]:
    return _git_ls_files(root, ("reports",))


def generated_truth_boundary_report(root: Path) -> dict[str, list[dict[str, object]]]:
    tracked_outputs = set(
        _git_ls_files(
            root,
            tuple(boundary.output_path for boundary in GENERATED_TRUTH_BOUNDARIES),
        )
    )
    boundaries: list[dict[str, object]] = []
    findings: list[dict[str, object]] = []

    for boundary in GENERATED_TRUTH_BOUNDARIES:
        output_file = root / boundary.output_path
        output_exists = output_file.is_file()
        output_tracked = boundary.output_path in tracked_outputs
        missing_sources = [
            source_path
            for source_path in boundary.source_paths
            if not (root / source_path).exists()
        ]
        generator_exists = (root / boundary.generator_path).is_file()
        boundary_payload = {
            **asdict(boundary),
            "output_exists": output_exists,
            "output_tracked": output_tracked,
            "missing_source_paths": missing_sources,
            "generator_exists": generator_exists,
        }
        boundaries.append(boundary_payload)
        if not output_exists:
            continue
        if not output_tracked:
            findings.append(
                {
                    "path": boundary.output_path,
                    "description": "generated truth output exists but is not tracked",
                }
            )
        if missing_sources:
            findings.append(
                {
                    "path": boundary.output_path,
                    "description": "generated truth output is missing canonical source paths",
                    "missing_source_paths": missing_sources,
                }
            )
        if not generator_exists:
            findings.append(
                {
                    "path": boundary.output_path,
                    "description": "generated truth output is missing its generator script",
                    "generator_path": boundary.generator_path,
                }
            )

    return {"boundaries": boundaries, "findings": findings}
