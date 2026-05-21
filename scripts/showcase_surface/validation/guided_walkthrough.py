from __future__ import annotations

from typing import Any

from .constants import GUIDED_WALKTHROUGH_CONTRACT_ID


def validate_guided_walkthrough_contract(walkthrough_payload: dict[str, Any]) -> str | None:
    if walkthrough_payload.get("contract_id") != GUIDED_WALKTHROUGH_CONTRACT_ID:
        return "guided walkthrough contract_id drifted"
    if walkthrough_payload.get("schema_version") != 1:
        return "guided walkthrough schema_version drifted"
    if walkthrough_payload.get("tutorial_readme") != "docs/tutorials/guided_walkthrough.md":
        return "guided walkthrough tutorial_readme drifted"
    if walkthrough_payload.get("build_run_verify_readme") != "docs/tutorials/build_run_verify.md":
        return "guided walkthrough build_run_verify_readme drifted"
    if walkthrough_payload.get("portfolio_contract") != "showcase/portfolio.json":
        return "guided walkthrough portfolio_contract drifted"
    walkthrough_steps = walkthrough_payload.get("steps")
    if walkthrough_steps != [
        {
            "id": "build-native",
            "workflow_action": "build-native-binaries",
            "public_command": "npm run objc3c -- build-native-binaries",
        },
        {
            "id": "compile-auroraBoard",
            "workflow_action": "compile-objc3c",
            "public_command": (
                "npm run objc3c -- compile-objc3c "
                "showcase/auroraBoard/main.objc3"
            ),
            "example_id": "auroraBoard",
            "source": "showcase/auroraBoard/main.objc3",
            "artifact_root": "tmp/artifacts/showcase/auroraBoard",
        },
        {
            "id": "compile-signalMesh",
            "workflow_action": "compile-objc3c",
            "public_command": (
                "npm run objc3c -- compile-objc3c "
                "showcase/signalMesh/main.objc3"
            ),
            "example_id": "signalMesh",
            "source": "showcase/signalMesh/main.objc3",
            "artifact_root": "tmp/artifacts/showcase/signalMesh",
        },
        {
            "id": "compile-patchKit",
            "workflow_action": "compile-objc3c",
            "public_command": (
                "npm run objc3c -- compile-objc3c "
                "showcase/patchKit/main.objc3"
            ),
            "example_id": "patchKit",
            "source": "showcase/patchKit/main.objc3",
            "artifact_root": "tmp/artifacts/showcase/patchKit",
        },
        {
            "id": "check-showcase-surface",
            "workflow_action": "check-showcase-surface",
            "public_command": "npm run objc3c -- check-showcase-surface",
            "report_root": "tmp/reports/showcase",
        },
        {
            "id": "validate-showcase",
            "workflow_action": "validate-showcase",
            "public_command": "npm run objc3c -- validate-showcase",
            "report_root": "tmp/reports/showcase",
        },
    ]:
        return "guided walkthrough steps drifted"
    return None
