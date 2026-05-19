"""Experience and workflow surface fields for the repo superclean checker."""

from __future__ import annotations

from .model import SurfaceField
from .paths import REPO_SUPERCLEAN_SOURCE_OF_TRUTH_RELATIVE


EXPERIENCE_SURFACE_FIELDS = (
    SurfaceField(
        "bonus_experience_surfaces",
        {
            "playground": {
                "source_roots": [
                    "showcase/auroraBoard/main.objc3",
                    "showcase/signalMesh/main.objc3",
                    "showcase/patchKit/main.objc3",
                    "tests/tooling/fixtures/native/hello.objc3",
                ],
                "artifact_roots": [
                    "tmp/artifacts/playground",
                    "tmp/reports/playground",
                    "tmp/artifacts/showcase",
                ],
                "public_actions": [
                    "materialize-playground-workspace",
                    "compile-objc3c",
                    "inspect-playground-repro",
                    "inspect-compile-observability",
                    "trace-compile-stages",
                ],
            },
            "runtime_inspector": {
                "source_roots": [
                    "native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp",
                    "scripts/probe_objc3c_llvm_capabilities.py",
                    "native/objc3c/src/runtime/objc3_runtime.cpp",
                    "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
                    "tests/tooling/runtime/block_arc_runtime_abi_probe.cpp",
                    "tests/tooling/runtime/task_runtime_hardening_probe.cpp",
                ],
                "report_roots": [
                    "tmp/reports/objc3c-public-workflow",
                    "tmp/reports/developer-tooling",
                ],
                "public_actions": [
                    "inspect-runtime-inspector",
                    "inspect-capability-explorer",
                    "benchmark-runtime-inspector",
                    "trace-compile-stages",
                    "validate-developer-tooling",
                ],
            },
            "template_and_demo_harness": {
                "source_roots": [
                    "scripts/materialize_objc3c_project_template.py",
                    "showcase/README.md",
                    "showcase/portfolio.json",
                    "showcase/tutorial_walkthrough.json",
                    "docs/tutorials/getting_started.md",
                    "docs/tutorials/build_run_verify.md",
                    "docs/tutorials/guided_walkthrough.md",
                ],
                "report_roots": [
                    "tmp/artifacts/project-template",
                    "tmp/reports/project-template",
                    "tmp/reports/showcase",
                    "tmp/reports/tutorials",
                ],
                "public_actions": [
                    "materialize-project-template",
                    "validate-showcase",
                    "validate-runnable-showcase",
                    "validate-getting-started",
                ],
            },
        },
        "bonus_experience_surfaces drifted",
    ),
    SurfaceField(
        "bonus_tool_integration_surface",
        {
            "source_of_truth_artifact": REPO_SUPERCLEAN_SOURCE_OF_TRUTH_RELATIVE,
            "report_root": "tmp/reports/objc3c-public-workflow",
            "package_stage_root": "tmp/pkg/objc3c-native-runnable-toolchain",
            "portfolio_contract": "showcase/portfolio.json",
            "guided_walkthrough_manifest": "showcase/tutorial_walkthrough.json",
            "public_actions": [
                "inspect-bonus-tool-integration",
                "materialize-project-template",
                "materialize-playground-workspace",
                "benchmark-runtime-inspector",
                "validate-showcase",
                "validate-runnable-showcase",
                "validate-getting-started",
                "package-runnable-toolchain",
            ],
        },
        "bonus_tool_integration_surface drifted",
    ),
)
