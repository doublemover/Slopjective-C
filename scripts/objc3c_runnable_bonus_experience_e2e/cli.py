from __future__ import annotations

from datetime import datetime

from .capability_probe import run_capability_probe
from .constants import REPORT_PATH, ROOT
from .examples import materialize_and_run_examples
from .manifest import load_bonus_package_surface
from .packaging import run_package_command
from .summary import build_summary_payload, write_summary


def main() -> int:
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-bonus-experience-e2e" / run_id
    manifest_path = (
        package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
    )
    artifacts_root = package_root / "tmp" / "artifacts" / "bonus-experience-e2e"
    capability_report = (
        package_root
        / "tmp"
        / "reports"
        / "bonus-experience-e2e"
        / "capability-explorer.json"
    )

    package_result = run_package_command(package_root=package_root)
    surface = load_bonus_package_surface(
        package_root=package_root,
        manifest_path=manifest_path,
    )
    capability_probe_result = run_capability_probe(
        package_root=package_root,
        capability_probe_script=surface.capability_probe_script,
        capability_report=capability_report,
    )
    example_results = materialize_and_run_examples(
        package_root=package_root,
        artifacts_root=artifacts_root,
        surface=surface,
    )

    payload = build_summary_payload(
        package_result=package_result,
        capability_probe_result=capability_probe_result,
        package_root=package_root,
        manifest_path=manifest_path,
        surface=surface,
        capability_report=capability_report,
        example_results=example_results,
    )
    write_summary(REPORT_PATH, payload)
    return 0
