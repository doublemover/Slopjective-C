"""Sanitizer runtime evidence public workflow action contracts."""

from __future__ import annotations

from dataclasses import dataclass
import sys

from ..action_spec import ActionSpec
from ..commands import run
from ..environment import ROOT


SANITIZER_RUNTIME_EVIDENCE_CHECKER_PY = (
    ROOT / "scripts" / "check_objc3c_sanitizer_runtime_evidence.py"
)
SANITIZER_RUNTIME_EVIDENCE_PROBE_PY = (
    ROOT / "scripts" / "probe_objc3c_sanitizer_runtime_evidence.py"
)
SANITIZER_RUNTIME_EVIDENCE_REPORT_ROOT = (
    "tmp/reports/sanitizer-runtime-evidence"
)
SANITIZER_RUNTIME_EVIDENCE_PACKAGE_ROOT = (
    "tmp/sre/pkg"
)
SANITIZER_RUNTIME_EVIDENCE_CONTRACT_ID = (
    "objc3c.workflow.sanitizer-runtime-evidence.action-contract.v1"
)
SANITIZER_RUNTIME_EVIDENCE_MANIFEST_RELATIVE_PATH = (
    "artifacts/package/objc3c-runnable-toolchain-package.json"
)
SANITIZER_RUNTIME_EVIDENCE_FIXTURE_GLOB = "*assignment_basic_counter.objc3"
SANITIZER_RUNTIME_EVIDENCE_PARALLELISM = "2"
ASAN_SANITIZER_RUNTIME_EVIDENCE_ACTION_ID = (
    "check-sanitizer-runtime-evidence-asan"
)
UBSAN_SANITIZER_RUNTIME_EVIDENCE_ACTION_ID = (
    "check-sanitizer-runtime-evidence-ubsan"
)
PINNED_SANITIZER_RUNTIME_EVIDENCE_ARGS = (
    "--sanitizer-variant",
    "--target-platform",
    "--report",
    "--probe-script",
    "--package-root",
    "--manifest-relative-path",
    "--fixture-glob",
    "--parallelism",
    "--run-id",
    "--summary-out",
    "--work-dir",
)


@dataclass(frozen=True)
class SanitizerRuntimeEvidenceActionContract:
    action_id: str
    issue_ref: int
    sanitizer_label: str
    sanitizer_variant_id: str
    package_variant_row_id: str
    package_channel_id: str
    package_id: str
    target_platform_ids: tuple[str, ...]
    runtime_library_manifest_path: str
    report_paths: tuple[str, ...]
    required_inputs: tuple[str, ...]
    required_outputs: tuple[str, ...]
    fail_closed_preconditions: tuple[str, ...]

    @property
    def primary_report_path(self) -> str:
        return self.report_paths[0]

    @property
    def pinned_package_root(self) -> str:
        return f"{SANITIZER_RUNTIME_EVIDENCE_PACKAGE_ROOT}/{self.action_id}"

    @property
    def pinned_run_id(self) -> str:
        return f"sre-{self.sanitizer_variant_id}"

    def checker_backend(self) -> str:
        platforms = ",".join(self.target_platform_ids)
        return (
            "python:scripts/check_objc3c_sanitizer_runtime_evidence.py "
            f"--sanitizer-variant {self.sanitizer_variant_id} "
            f"--target-platform {platforms} "
            f"--report {self.primary_report_path} "
            "--probe-script scripts/probe_objc3c_sanitizer_runtime_evidence.py "
            f"--package-root {self.pinned_package_root} "
            f"--manifest-relative-path {SANITIZER_RUNTIME_EVIDENCE_MANIFEST_RELATIVE_PATH} "
            f"--fixture-glob {SANITIZER_RUNTIME_EVIDENCE_FIXTURE_GLOB} "
            f"--parallelism {SANITIZER_RUNTIME_EVIDENCE_PARALLELISM} "
            f"--run-id {self.pinned_run_id}"
        )

    def action_spec(self) -> ActionSpec:
        return ActionSpec(
            self.action_id,
            (
                f"check {self.sanitizer_label} runtime evidence as a "
                "reserved, non-promoting workflow contract"
            ),
            self.checker_backend(),
            validation_tier="ci",
            guarantee_owner=(
                f"{self.sanitizer_label} runtime evidence for issue #{self.issue_ref} "
                f"stays tied to sanitizer variant {self.sanitizer_variant_id}, "
                f"target platform ids {', '.join(self.target_platform_ids)}, "
                f"package channel {self.package_channel_id}, report path "
                f"{self.primary_report_path}, and fail-closed preconditions; "
                "evidence reports are review inputs only and do not promote "
                "sanitizer runtime support claims"
            ),
            pass_through_args=False,
        )

    def public_contract_payload(self) -> dict[str, object]:
        return {
            "contract_id": SANITIZER_RUNTIME_EVIDENCE_CONTRACT_ID,
            "action_id": self.action_id,
            "public_command": f"npm run objc3c -- {self.action_id}",
            "action_aliases_allowed": False,
            "fallback_sanitizer_variant_allowed": False,
            "fallback_target_platform_allowed": False,
            "report_or_probe_rerouting_allowed": False,
            "package_root_rerouting_allowed": False,
            "fixture_rerouting_allowed": False,
            "public_pass_through_args_allowed": False,
            "pinned_action_options": list(PINNED_SANITIZER_RUNTIME_EVIDENCE_ARGS),
            "pinned_package_root": self.pinned_package_root,
            "pinned_manifest_relative_path": (
                SANITIZER_RUNTIME_EVIDENCE_MANIFEST_RELATIVE_PATH
            ),
            "pinned_fixture_glob": SANITIZER_RUNTIME_EVIDENCE_FIXTURE_GLOB,
            "pinned_parallelism": int(SANITIZER_RUNTIME_EVIDENCE_PARALLELISM),
            "pinned_run_id": self.pinned_run_id,
            "issue_ref": self.issue_ref,
            "support_claim_policy": "non-promoting-review-evidence-only",
            "support_claim_authority": False,
            "support_promotion_allowed": False,
            "native_execution_claim_promotion_allowed": False,
            "sanitizer_variant_id": self.sanitizer_variant_id,
            "sanitizer_label": self.sanitizer_label,
            "package_variant_row_id": self.package_variant_row_id,
            "package_channel_id": self.package_channel_id,
            "package_id": self.package_id,
            "target_platform_ids": list(self.target_platform_ids),
            "runtime_library_manifest_path": self.runtime_library_manifest_path,
            "checker_script": "scripts/check_objc3c_sanitizer_runtime_evidence.py",
            "probe_script": "scripts/probe_objc3c_sanitizer_runtime_evidence.py",
            "required_inputs": list(self.required_inputs),
            "required_outputs": list(self.required_outputs),
            "report_paths": list(self.report_paths),
            "fail_closed_preconditions": list(self.fail_closed_preconditions),
        }


def _report_paths(sanitizer_variant_id: str) -> tuple[str, str]:
    return (
        f"{SANITIZER_RUNTIME_EVIDENCE_REPORT_ROOT}/{sanitizer_variant_id}/summary.json",
        f"{SANITIZER_RUNTIME_EVIDENCE_REPORT_ROOT}/{sanitizer_variant_id}/probe.json",
    )


def _required_inputs(*, runtime_library_manifest_path: str) -> tuple[str, ...]:
    return (
        "scripts/package_objc3c_runnable_toolchain.ps1",
        "scripts/check_objc3c_native_execution_smoke.ps1",
        "scripts/check_objc3c_sanitizer_runtime_evidence.py",
        "scripts/probe_objc3c_sanitizer_runtime_evidence.py",
        "scripts/objc3c_package_channels/sanitizer_contracts.py",
        "scripts/objc3c_runnable_toolchain_package_helpers.psm1",
        "scripts/objc3c_runnable_toolchain_package_helpers",
        "scripts/objc3c_native_cmake.psm1",
        "scripts/package_objc3c_runnable_toolchain/staging_orchestration.psm1",
        "scripts/package_objc3c_runnable_toolchain/artifact_report_foundation.psm1",
        "scripts/objc3c_native_execution_smoke_helpers.psm1",
        "scripts/objc3c_native_execution_smoke_helpers",
        "scripts/objc3c_native_execution_smoke_runner.psm1",
        "scripts/objc3c_native_execution_smoke_runner",
        "scripts/objc3c_runtime_launch_contract.ps1",
        "tests/tooling/fixtures/native/execution/positive",
        "tests/tooling/fixtures/native/execution/negative",
        "schemas/objc3c-package-channels-manifest-v1.schema.json",
        "schemas/objc3c-sanitizer-runtime-library-manifest-v1.schema.json",
        "schemas/objc3c-sanitizer-execution-evidence-v1.schema.json",
        f"generated-package-root:{runtime_library_manifest_path}",
    )


def _required_outputs(sanitizer_variant_id: str) -> tuple[str, ...]:
    summary_path, probe_path = _report_paths(sanitizer_variant_id)
    return (
        summary_path,
        probe_path,
        f"{SANITIZER_RUNTIME_EVIDENCE_REPORT_ROOT}/{sanitizer_variant_id}/diagnostics.json",
    )


def _fail_closed_preconditions(
    *,
    sanitizer_variant_id: str,
    target_platform_id: str,
    package_channel_id: str,
) -> tuple[str, ...]:
    return (
        "checker and probe scripts must exist before the action can run",
        "public command aliases and sanitizer-variant fallback routing are forbidden",
        f"selected sanitizer_variant_id must equal {sanitizer_variant_id}",
        f"selected target_platform_id must equal {target_platform_id}; wildcard or fallback platform routing is forbidden",
        f"selected package_channel_id must equal {package_channel_id}",
        "package summary support_truth must be false",
        "package summary native_execution_claimed must be false",
        "package manifest must include sanitizer_package_variant metadata, runtime library ids, runtime manifest path, and runtime manifest digest",
        "packaged sanitizer runtime link directories must come from artifacts/runtime/sanitizer/<variant> inside the generated package root",
        "missing runtime libraries, missing package/runtime manifests, mixed release/sanitizer runtime packages, and unsupported hosts must fail closed",
        "generated sanitizer runtime evidence is review-only and cannot promote platform or sanitizer support truth",
    )


ASAN_SANITIZER_RUNTIME_EVIDENCE_CONTRACT = SanitizerRuntimeEvidenceActionContract(
    action_id=ASAN_SANITIZER_RUNTIME_EVIDENCE_ACTION_ID,
    issue_ref=8230,
    sanitizer_label="ASan",
    sanitizer_variant_id="address",
    package_variant_row_id="objc3c.package.sanitizer.asan.reserved",
    package_channel_id="windows-x64-sanitizer-asan",
    package_id="org.objc3c.runtime:objc3c-runtime-asan",
    target_platform_ids=("windows-x64",),
    runtime_library_manifest_path="share/objc3c/sanitizer/asan-runtime-libraries.json",
    report_paths=_report_paths("address"),
    required_inputs=_required_inputs(
        runtime_library_manifest_path="share/objc3c/sanitizer/asan-runtime-libraries.json",
    ),
    required_outputs=_required_outputs("address"),
    fail_closed_preconditions=_fail_closed_preconditions(
        sanitizer_variant_id="address",
        target_platform_id="windows-x64",
        package_channel_id="windows-x64-sanitizer-asan",
    ),
)

UBSAN_SANITIZER_RUNTIME_EVIDENCE_CONTRACT = SanitizerRuntimeEvidenceActionContract(
    action_id=UBSAN_SANITIZER_RUNTIME_EVIDENCE_ACTION_ID,
    issue_ref=8231,
    sanitizer_label="UBSan",
    sanitizer_variant_id="undefined",
    package_variant_row_id="objc3c.package.sanitizer.ubsan.reserved",
    package_channel_id="windows-x64-sanitizer-ubsan",
    package_id="org.objc3c.runtime:objc3c-runtime-ubsan",
    target_platform_ids=("windows-x64",),
    runtime_library_manifest_path="share/objc3c/sanitizer/ubsan-runtime-libraries.json",
    report_paths=_report_paths("undefined"),
    required_inputs=_required_inputs(
        runtime_library_manifest_path="share/objc3c/sanitizer/ubsan-runtime-libraries.json",
    ),
    required_outputs=_required_outputs("undefined"),
    fail_closed_preconditions=_fail_closed_preconditions(
        sanitizer_variant_id="undefined",
        target_platform_id="windows-x64",
        package_channel_id="windows-x64-sanitizer-ubsan",
    ),
)

SANITIZER_RUNTIME_EVIDENCE_ACTION_CONTRACTS = (
    ASAN_SANITIZER_RUNTIME_EVIDENCE_CONTRACT,
    UBSAN_SANITIZER_RUNTIME_EVIDENCE_CONTRACT,
)
SANITIZER_RUNTIME_EVIDENCE_ACTION_SPECS = {
    contract.action_id: contract.action_spec()
    for contract in SANITIZER_RUNTIME_EVIDENCE_ACTION_CONTRACTS
}
SANITIZER_RUNTIME_EVIDENCE_PUBLIC_CONTRACTS = {
    contract.action_id: contract.public_contract_payload()
    for contract in SANITIZER_RUNTIME_EVIDENCE_ACTION_CONTRACTS
}


def _has_pinned_option_override(
    rest: list[str],
    pinned_options: tuple[str, ...] = PINNED_SANITIZER_RUNTIME_EVIDENCE_ARGS,
) -> str | None:
    for value in rest:
        for option in pinned_options:
            if value == option or value.startswith(f"{option}="):
                return option
    return None


def _run_sanitizer_runtime_evidence_action(
    contract: SanitizerRuntimeEvidenceActionContract,
    rest: list[str],
) -> int:
    if rest[:1] == ["--"]:
        rest = rest[1:]
    pinned_override = _has_pinned_option_override(rest)
    if pinned_override is not None:
        print(
            f"{contract.action_id} pins {pinned_override}; sanitizer runtime "
            "evidence actions do not accept aliases, variant fallbacks, "
            "target-platform fallbacks, or report/probe rerouting.",
            file=sys.stderr,
        )
        return 1
    if rest:
        print(
            f"{contract.action_id} does not accept passthrough arguments; "
            "sanitizer runtime evidence actions pin every variant, platform, "
            "package root, report path, probe path, fixture, and run id.",
            file=sys.stderr,
        )
        return 1
    missing_scripts = [
        script
        for script in (
            SANITIZER_RUNTIME_EVIDENCE_CHECKER_PY,
            SANITIZER_RUNTIME_EVIDENCE_PROBE_PY,
        )
        if not script.is_file()
    ]
    if missing_scripts:
        print(
            "sanitizer runtime evidence action is reserved until checker/probe "
            "scripts exist; failing closed without claiming support.",
            file=sys.stderr,
        )
        for script in missing_scripts:
            print(f"missing required script: {script}", file=sys.stderr)
        return 1
    return run(
        [
            sys.executable,
            str(SANITIZER_RUNTIME_EVIDENCE_CHECKER_PY),
            "--sanitizer-variant",
            contract.sanitizer_variant_id,
            "--target-platform",
            contract.target_platform_ids[0],
            "--report",
            contract.primary_report_path,
            "--probe-script",
            str(SANITIZER_RUNTIME_EVIDENCE_PROBE_PY),
            "--package-root",
            contract.pinned_package_root,
            "--manifest-relative-path",
            SANITIZER_RUNTIME_EVIDENCE_MANIFEST_RELATIVE_PATH,
            "--fixture-glob",
            SANITIZER_RUNTIME_EVIDENCE_FIXTURE_GLOB,
            "--parallelism",
            SANITIZER_RUNTIME_EVIDENCE_PARALLELISM,
            "--run-id",
            contract.pinned_run_id,
            *rest,
        ]
    )


def action_check_sanitizer_runtime_evidence_asan(rest: list[str]) -> int:
    return _run_sanitizer_runtime_evidence_action(
        ASAN_SANITIZER_RUNTIME_EVIDENCE_CONTRACT,
        rest,
    )


def action_check_sanitizer_runtime_evidence_ubsan(rest: list[str]) -> int:
    return _run_sanitizer_runtime_evidence_action(
        UBSAN_SANITIZER_RUNTIME_EVIDENCE_CONTRACT,
        rest,
    )


__all__ = [
    "ASAN_SANITIZER_RUNTIME_EVIDENCE_ACTION_ID",
    "ASAN_SANITIZER_RUNTIME_EVIDENCE_CONTRACT",
    "SANITIZER_RUNTIME_EVIDENCE_ACTION_CONTRACTS",
    "SANITIZER_RUNTIME_EVIDENCE_ACTION_SPECS",
    "SANITIZER_RUNTIME_EVIDENCE_CHECKER_PY",
    "SANITIZER_RUNTIME_EVIDENCE_CONTRACT_ID",
    "SANITIZER_RUNTIME_EVIDENCE_FIXTURE_GLOB",
    "SANITIZER_RUNTIME_EVIDENCE_MANIFEST_RELATIVE_PATH",
    "SANITIZER_RUNTIME_EVIDENCE_PACKAGE_ROOT",
    "SANITIZER_RUNTIME_EVIDENCE_PARALLELISM",
    "PINNED_SANITIZER_RUNTIME_EVIDENCE_ARGS",
    "SANITIZER_RUNTIME_EVIDENCE_PROBE_PY",
    "SANITIZER_RUNTIME_EVIDENCE_PUBLIC_CONTRACTS",
    "SANITIZER_RUNTIME_EVIDENCE_REPORT_ROOT",
    "SanitizerRuntimeEvidenceActionContract",
    "UBSAN_SANITIZER_RUNTIME_EVIDENCE_ACTION_ID",
    "UBSAN_SANITIZER_RUNTIME_EVIDENCE_CONTRACT",
    "action_check_sanitizer_runtime_evidence_asan",
    "action_check_sanitizer_runtime_evidence_ubsan",
]
