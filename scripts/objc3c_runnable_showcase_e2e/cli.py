from __future__ import annotations

from datetime import datetime
from typing import Sequence

from .constants import REPORT_PATH, ROOT
from .execution import run_showcase_examples
from .manifest import load_showcase_package_surface
from .packaging import run_package_command
from .report import build_summary_payload, write_summary


def main(argv: Sequence[str] | None = None) -> int:
    del argv

    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-showcase-e2e" / run_id
    manifest_path = (
        package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
    )
    artifacts_root = package_root / "tmp" / "artifacts" / "showcase-e2e"

    package_result = run_package_command(package_root=package_root)
    surface = load_showcase_package_surface(
        package_root=package_root,
        manifest_path=manifest_path,
    )
    execution_result = run_showcase_examples(
        package_root=package_root,
        artifacts_root=artifacts_root,
        surface=surface,
    )

    payload = build_summary_payload(
        package_result=package_result,
        package_root=package_root,
        manifest_path=manifest_path,
        surface=surface,
        execution_result=execution_result,
    )
    write_summary(REPORT_PATH, payload)
    return 0


__all__ = ["main"]
