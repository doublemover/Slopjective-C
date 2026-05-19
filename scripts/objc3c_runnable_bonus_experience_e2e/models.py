from __future__ import annotations

from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Any


@dataclass(frozen=True)
class BonusPackageSurface:
    manifest: dict[str, Any]
    compile_wrapper: Path
    runtime_library: Path
    showcase_portfolio: Path
    guided_walkthrough_manifest: Path
    repo_superclean_surface: Path
    capability_probe_script: Path
    showcase_examples: list[Any]


@dataclass(frozen=True)
class ExampleRunResult:
    example_id: str
    packaged_source: str
    template_root: str
    template_source: str
    template_readme: str
    workspace_manifest: str
    compile_dir: str
    executable: str
    link_log: str
    run_log: str
    expected_exit_code: int
    actual_exit_code: int

    def payload(self) -> dict[str, Any]:
        return asdict(self)
