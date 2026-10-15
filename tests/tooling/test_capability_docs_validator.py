from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path
from typing import Any

import pytest

ROOT = Path(__file__).resolve().parents[2]
VALIDATOR_PATH = ROOT / "scripts" / "validate_capability_docs.py"

PUBLIC_CAPABILITY_DOCS = (
    ROOT / "README.md",
    ROOT / "docs" / "support" / "README.md",
    ROOT / "docs" / "support" / "capability_matrix.md",
    ROOT / "docs" / "support" / "hard_cutover_capability_truth.md",
    ROOT / "docs" / "support" / "evidence_map.md",
    ROOT / "docs" / "workflows" / "commands.md",
    ROOT / "docs" / "workflows" / "validation.md",
    ROOT / "docs" / "workflows" / "ci.md",
    ROOT / "site" / "src" / "README.md",
    ROOT / "site" / "src" / "OWNERSHIP.md",
    ROOT / "site" / "src" / "index.body.md",
    ROOT / "site" / "index.md",
)

FORBIDDEN_PUBLIC_DOC_SNIPPETS = (
    "lint" + "-default",
    "python -m scripts." + "objc3c_workflow",
    "python scripts/" + "objc3c_workflow",
    "npm run lint",
    "npm run build",
    "supported through a retired adapter",
    "accepted by an alternate parser path",
    "available through a retired mode label",
    "retired-source lane accepts old syntax",
    "implemented because a roadmap says it is planned",
    "complete because a generated report says so without a matching implemented row",
    "future support rows",
    "future-spec surface",
    "projected completion claim",
)

PARSER_CLAIM = {
    "claim_id": "objc3c.behavior.parser.canonical-syntax",
    "owner_phase": "parser",
    "behavior_fixture": "tests/native/parser/positive/canonical_module_main.objc3",
    "executable_command": "npm run objc3c -- test-behavior-matrix",
}

RUNTIME_CLAIM = {
    "claim_id": "objc3c.behavior.runtime.strict-dispatch-error",
    "owner_phase": "runtime",
    "behavior_fixture": "tests/native/runtime/dispatch/message_send_runtime_dispatch_strict_error.objc3",
    "executable_command": "npm run objc3c -- test-behavior-matrix",
}


def _load_validator():
    scripts_path = str(ROOT / "scripts")
    if scripts_path not in sys.path:
        sys.path.insert(0, scripts_path)
    spec = importlib.util.spec_from_file_location("validate_capability_docs", VALIDATOR_PATH)
    assert spec is not None
    assert spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def _manifest(*claims: dict[str, str]) -> dict[str, Any]:
    return {
        "schema_version": 1,
        "support_claims": list(claims),
        "fixtures": [
            {
                "path": claim["behavior_fixture"],
                "origin": "hand-authored",
                "owner_phase": claim["owner_phase"],
                "behavior_family": "validator-test",
                "fixture_kind": "positive",
                "expected_diagnostic_code": "",
            }
            for claim in claims
        ],
    }


def _manifest_with_retired_fixture(*claims: dict[str, str]) -> dict[str, Any]:
    manifest = _manifest(*claims)
    manifest["fixtures"].append(
        {
            "path": "tests/native/parser/negative/legacy_null_literal_alias_rejected.objc3",
            "origin": "hand-authored",
            "owner_phase": "parser",
            "behavior_family": "negative",
            "fixture_kind": "rejection",
            "expected_diagnostic_code": "O3C002",
        }
    )
    return manifest


def _parser_phase_contract(support_claim: str = PARSER_CLAIM["claim_id"]) -> dict[str, Any]:
    return {
        "phase_contracts": [
            {
                "phase": "parser",
                "support_claim": support_claim,
                "fixture_root": "tests/native/parser",
                "canonical_positive_evidence": [
                    PARSER_CLAIM["behavior_fixture"],
                ],
                "retired_surface_evidence": [
                    "tests/native/parser/negative/legacy_null_literal_alias_rejected.objc3",
                ],
                "generated_fixture_authority": False,
            }
        ]
    }


def _runtime_object_model_row() -> dict[str, Any]:
    return {
        "id": "runtime.object-model.interface-method-table",
        "title": "Object model interface method table",
        "state": "implemented",
        "summary": "Runtime object-model row used by validator tests.",
        "support_claims": [
            "objc3c.behavior.runtime.object-model-interface-method-table"
        ],
        "owner_modules": ["native/objc3c/src/runtime/classes/class_graph.cpp"],
        "evidence": [
            {
                "kind": "test",
                "path": "tests/native/runtime/object_model/interface_method_table_contract.objc3",
                "command": "npm run objc3c -- test-behavior-matrix",
            },
            {
                "kind": "source",
                "path": "native/objc3c/src/runtime/classes/class_graph.cpp",
            },
        ],
    }


def _runtime_object_model_manifest() -> dict[str, Any]:
    return {
        "schema_version": 1,
        "support_claims": [
            {
                "claim_id": "objc3c.behavior.runtime.object-model-interface-method-table",
                "owner_phase": "runtime",
                "behavior_fixture": "tests/native/runtime/object_model/interface_method_table_contract.objc3",
                "executable_command": "npm run objc3c -- test-behavior-matrix",
            }
        ],
        "fixtures": [
            {
                "path": "tests/native/runtime/object_model/interface_method_table_contract.objc3",
                "origin": "hand-authored",
                "owner_phase": "runtime",
                "behavior_family": "object_model",
                "fixture_kind": "positive",
                "expected_diagnostic_code": "",
            }
        ],
    }


def _runtime_object_model_runnable_catalog() -> dict[str, Any]:
    return {
        "contract_id": "objc3c.conformance.support_claim_runnable_evidence_catalog.v1",
        "schema_version": 1,
        "policy": {
            "tmp_source_truth_allowed": False,
            "generated_report_boundary": "tmp/reports/conformance/runnable-claim-trace-summary.json",
        },
        "rows": [
            {
                "support_claim": "objc3c.behavior.runtime.object-model-interface-method-table",
                "capability_id": "runtime.object-model.interface-method-table",
                "owner_phase": "runtime",
                "conformance_fixture": "tests/conformance/lowering_abi/OBJFND-8058-01.json",
                "traceability_fixture": "tests/conformance/lowering_abi/OBJFND-8059-01.json",
                "runnable_command": "npm run objc3c -- test-runtime-acceptance-fast",
                "positive_evidence": [
                    "tests/native/runtime/object_model/interface_method_table_contract.objc3",
                    "tests/tooling/fixtures/native/category_attachment_protocol_runtime_library.objc3",
                    "tests/tooling/runtime/category_attachment_protocol_runtime_probe.cpp",
                ],
                "negative_evidence": [
                    "tests/tooling/fixtures/native/execution/negative/category_attachment_collision.objc3"
                ],
                "required_diagnostic_codes": ["O3S200", "O3RT004"],
            }
        ],
    }


def test_public_capability_docs_reject_retired_public_surface_claims() -> None:
    for path in PUBLIC_CAPABILITY_DOCS:
        text = path.read_text(encoding="utf-8")
        for snippet in FORBIDDEN_PUBLIC_DOC_SNIPPETS:
            assert snippet not in text, f"{path.relative_to(ROOT)} contains {snippet!r}"


def _parser_row() -> dict[str, Any]:
    return {
        "id": "compiler.parser.core-declarations",
        "title": "Canonical parser syntax",
        "state": "implemented",
        "summary": "Parser claim used by validator tests.",
        "support_claims": [PARSER_CLAIM["claim_id"]],
        "evidence": [
            {
                "kind": "test",
                "path": PARSER_CLAIM["behavior_fixture"],
                "command": PARSER_CLAIM["executable_command"],
            }
        ],
    }


def test_support_claim_links_accept_manifest_fixture_evidence() -> None:
    validator = _load_validator()

    validator._validate_support_claim_links([_parser_row()], _manifest(PARSER_CLAIM))


def test_manifest_support_claims_reject_evidence_log_commands() -> None:
    validator = _load_validator()
    evidence_log_claim = {
        **PARSER_CLAIM,
        "executable_command": "python scripts/render_behavior_report.py",
    }

    with pytest.raises(validator.CapabilityDocsError, match="must use executable command"):
        validator._manifest_support_claims(_manifest(evidence_log_claim))


def test_support_claim_links_require_behavior_matrix_fixture_evidence() -> None:
    validator = _load_validator()
    row = _parser_row()
    row["evidence"] = [
        {
            "kind": "test",
            "path": "tests/tooling/test_objc3c_parser_extraction.py",
            "command": "python -m pytest tests/tooling/test_objc3c_parser_extraction.py",
        }
    ]

    with pytest.raises(validator.CapabilityDocsError, match="must include executable evidence"):
        validator._validate_support_claim_links([row], _manifest(PARSER_CLAIM))


def test_support_claim_links_require_every_manifest_claim_in_the_matrix() -> None:
    validator = _load_validator()

    with pytest.raises(validator.CapabilityDocsError, match="missing from capability matrix"):
        validator._validate_support_claim_links([_parser_row()], _manifest(PARSER_CLAIM, RUNTIME_CLAIM))


def test_conformance_phase_contracts_require_canonical_manifest_claims() -> None:
    validator = _load_validator()

    validator._validate_conformance_manifest_links(
        _manifest_with_retired_fixture(PARSER_CLAIM),
        _parser_phase_contract(),
    )

    with pytest.raises(validator.CapabilityDocsError, match="not backed by canonical manifest"):
        validator._validate_conformance_manifest_links(
            _manifest_with_retired_fixture(PARSER_CLAIM),
            _parser_phase_contract("objc3c.behavior.parser.future-claim"),
        )


def test_conformance_phase_contracts_reject_generated_fixture_authority_for_support_claims() -> None:
    validator = _load_validator()
    phase_contract = _parser_phase_contract()
    phase_contract["phase_contracts"][0]["generated_fixture_authority"] = True

    with pytest.raises(validator.CapabilityDocsError, match="generated_fixture_authority"):
        validator._validate_conformance_manifest_links(
            _manifest_with_retired_fixture(PARSER_CLAIM),
            phase_contract,
        )


def test_support_claim_runnable_evidence_catalog_accepts_positive_and_negative_runtime_traceability() -> None:
    validator = _load_validator()

    validator._validate_support_claim_runnable_evidence_catalog(
        [_runtime_object_model_row()],
        _runtime_object_model_manifest(),
        _runtime_object_model_runnable_catalog(),
    )


def test_support_claim_runnable_evidence_catalog_rejects_tmp_source_truth() -> None:
    validator = _load_validator()
    catalog = _runtime_object_model_runnable_catalog()
    catalog["rows"][0]["negative_evidence"] = ["tmp/reports/conformance/negative.json"]

    with pytest.raises(validator.CapabilityDocsError, match="cannot use tmp as source truth"):
        validator._validate_support_claim_runnable_evidence_catalog(
            [_runtime_object_model_row()],
            _runtime_object_model_manifest(),
            catalog,
        )


def test_support_claim_runnable_evidence_catalog_requires_manifest_behavior_fixture() -> None:
    validator = _load_validator()
    catalog = _runtime_object_model_runnable_catalog()
    catalog["rows"][0]["positive_evidence"] = [
        "tests/tooling/fixtures/native/category_attachment_protocol_runtime_library.objc3"
    ]

    with pytest.raises(validator.CapabilityDocsError, match="canonical manifest behavior fixture"):
        validator._validate_support_claim_runnable_evidence_catalog(
            [_runtime_object_model_row()],
            _runtime_object_model_manifest(),
            catalog,
        )


def test_support_doc_claim_token_scan_rejects_claims_absent_from_matrix() -> None:
    validator = _load_validator()

    validator._validate_public_doc_claim_tokens(
        "Documented `objc3c.behavior.parser.canonical-syntax`.",
        {PARSER_CLAIM["claim_id"]},
    )

    with pytest.raises(validator.CapabilityDocsError, match="not declared"):
        validator._validate_public_doc_claim_tokens(
            "Documented `objc3c.behavior.parser.future-claim`.",
            {PARSER_CLAIM["claim_id"]},
        )


def test_support_doc_renderer_includes_manifest_and_phase_claim_authority() -> None:
    validator = _load_validator()
    docs = validator.render_support_docs(
        matrix={
            "matrix_version": "unit-test",
            "schema_path": "schemas/objc3c-capability-matrix-v1.schema.json",
            "evidence_map_path": "docs/support/evidence_map.json",
        },
        rows=[_parser_row()],
        evidence_map={
            "projection_contract": {
                "source": "docs/support/capability_matrix.json#/capabilities/*/evidence",
                "owner": "scripts/capability_docs_validator/evidence_map.py",
                "row_key": ["capability_id", "support_claim", "evidence_kind", "path", "command"],
                "drift_rule": "unit",
            },
            "evidence_policy": {
                "public_command_surface": "npm run objc3c -- <action>",
                "command_required_for": ["replayable implemented behavior evidence"],
                "command_forbidden_for": ["source ownership rows"],
                "row_role_rule": "unit",
            },
            "rows": [
                {
                    "capability_id": "compiler.parser.core-declarations",
                    "support_claim": PARSER_CLAIM["claim_id"],
                    "evidence_kind": "test",
                    "path": PARSER_CLAIM["behavior_fixture"],
                    "command": PARSER_CLAIM["executable_command"],
                }
            ],
        },
        manifest=_manifest_with_retired_fixture(PARSER_CLAIM),
        phase_owner_contracts=_parser_phase_contract(),
    )

    matrix_doc = docs[validator.MATRIX_DOC]
    assert "## Support Claim Authority" in matrix_doc
    assert PARSER_CLAIM["claim_id"] in matrix_doc
    assert PARSER_CLAIM["behavior_fixture"] in matrix_doc
    assert "## Phase Owner Contract" in matrix_doc


def test_runtime_concurrency_claim_is_implemented_and_probe_backed() -> None:
    matrix = json.loads(
        (ROOT / "docs" / "support" / "capability_matrix.json").read_text(
            encoding="utf-8"
        )
    )
    rows = {row["id"]: row for row in matrix["capabilities"]}
    row = rows["runtime.concurrency.async-actors"]

    assert row["state"] == "implemented"
    assert row["support_claims"] == [
        "objc3c.behavior.runtime.concurrency-async-actors"
    ]
    evidence_paths = {evidence["path"] for evidence in row["evidence"]}
    assert {
        "tests/native/runtime/concurrency/actor_executor_contract.objc3",
        "scripts/objc3c_runtime_acceptance/domains/concurrency_live_runtime_cases.py",
        "tests/tooling/runtime/continuation_runtime_helper_probe.cpp",
        "tests/tooling/runtime/live_task_runtime_and_executor_implementation_probe.cpp",
        "tests/tooling/runtime/live_actor_mailbox_runtime_probe.cpp",
    } <= evidence_paths


def test_developer_experience_first_run_claim_is_bounded_and_evidence_backed() -> None:
    matrix = json.loads(
        (ROOT / "docs" / "support" / "capability_matrix.json").read_text(
            encoding="utf-8"
        )
    )
    manifest = json.loads(
        (ROOT / "tests" / "fixtures" / "canonical" / "manifest.json").read_text(
            encoding="utf-8"
        )
    )
    catalog = json.loads(
        (
            ROOT
            / "tests"
            / "conformance"
            / "support_claim_runnable_evidence_catalog.json"
        ).read_text(encoding="utf-8")
    )
    rows = {row["id"]: row for row in matrix["capabilities"]}
    row = rows["tooling.developer-experience.first-run-product-path"]
    support_claim = "objc3c.behavior.tooling.first-run-product-path"
    behavior_fixture = (
        "tests/tooling/fixtures/developer_tooling/"
        "developer_experience_completion_contract.json"
    )
    public_command = "npm run objc3c -- validate-getting-started"

    assert row["state"] == "implemented"
    assert row["support_claims"] == [support_claim]
    assert "does not claim a full IDE" in row["summary"]
    assert "full LSP" in row["summary"]
    assert {
        (evidence["kind"], evidence["path"], evidence.get("command", ""))
        for evidence in row["evidence"]
    } >= {
        ("test", behavior_fixture, public_command),
        (
            "test",
            "tests/tooling/fixtures/developer_tooling/first_run_workflow_contract.json",
            public_command,
        ),
        (
            "test",
            "tests/tooling/fixtures/adoption_legibility/migration_outputs/objc2_swift_cpp_negative_diagnostics.json",
            "npm run objc3c -- validate-migration-workflow",
        ),
        (
            "diagnostic",
            "tests/tooling/fixtures/developer_tooling/language_server_capability_publication_policy.json",
            "",
        ),
    }

    manifest_claims = {
        claim["claim_id"]: claim for claim in manifest["support_claims"]
    }
    assert manifest_claims[support_claim] == {
        "claim_id": support_claim,
        "owner_phase": "e2e",
        "behavior_fixture": behavior_fixture,
        "executable_command": public_command,
    }

    catalog_rows = {row["support_claim"]: row for row in catalog["rows"]}
    catalog_row = catalog_rows[support_claim]
    assert catalog_row["capability_id"] == row["id"]
    assert behavior_fixture in catalog_row["positive_evidence"]
    assert {
        "tests/tooling/fixtures/developer_tooling/language_server_capability_publication_policy.json",
        "tests/tooling/fixtures/adoption_legibility/migration_outputs/objc2_swift_cpp_negative_diagnostics.json",
    } <= set(catalog_row["negative_evidence"])


def test_platform_and_application_framework_issue_rows_are_support_catalog_backed() -> None:
    matrix = json.loads(
        (ROOT / "docs" / "support" / "capability_matrix.json").read_text(
            encoding="utf-8"
        )
    )
    evidence_map = json.loads(
        (ROOT / "docs" / "support" / "evidence_map.json").read_text(
            encoding="utf-8"
        )
    )
    catalog = json.loads(
        (
            ROOT
            / "tests"
            / "conformance"
            / "support_claim_runnable_evidence_catalog.json"
        ).read_text(encoding="utf-8")
    )

    expected = {
        "platform.windows-x64.tier1": (
            "objc3c.behavior.platform.windows-x64-tier1",
            "npm run objc3c -- build-platform-support-matrix",
            "tests/tooling/fixtures/platform_hardening/platform_toolchain_support_evidence.json",
        ),
        "applications.framework-samples.object-runtime-library": (
            "objc3c.behavior.application-framework-samples.object-runtime-library",
            "npm run objc3c -- validate-application-framework-samples",
            "showcase/applicationFrameworkSamples/libraries/routeModelKit/main.objc3",
        ),
        "applications.framework-samples.interop-adapter-library": (
            "objc3c.behavior.application-framework-samples.interop-adapter-library",
            "npm run objc3c -- validate-application-framework-samples",
            "showcase/applicationFrameworkSamples/libraries/interopAdapterKit/main.objc3",
        ),
        "applications.framework-samples.stdlib-text-collections-cli": (
            "objc3c.behavior.application-framework-samples.stdlib-text-collections-cli",
            "npm run objc3c -- validate-application-framework-samples",
            "showcase/applicationFrameworkSamples/apps/workflowStdlibCLI/main.objc3",
        ),
        "applications.framework-samples.async-runtime-application": (
            "objc3c.behavior.application-framework-samples.async-runtime-application",
            "npm run objc3c -- validate-application-framework-samples",
            "showcase/applicationFrameworkSamples/apps/asyncRuntimeConsole/main.objc3",
        ),
    }
    matrix_rows = {row["id"]: row for row in matrix["capabilities"]}
    evidence_rows = {
        (
            row["capability_id"],
            row.get("support_claim"),
            row.get("command"),
            row["path"],
        )
        for row in evidence_map["rows"]
    }
    catalog_rows = {row["support_claim"]: row for row in catalog["rows"]}

    assert {8177, 8178} <= set(catalog["issue_refs"])
    for capability_id, (support_claim, command, positive_path) in expected.items():
        row = matrix_rows[capability_id]
        assert row["state"] == "implemented"
        assert row["support_claims"] == [support_claim]
        assert (capability_id, support_claim, command, positive_path) in evidence_rows
        catalog_row = catalog_rows[support_claim]
        assert catalog_row["capability_id"] == capability_id
        assert catalog_row["runnable_command"] == command
        assert positive_path in catalog_row["positive_evidence"]


def test_runtime_object_model_interface_claim_is_narrow_and_evidence_backed() -> None:
    matrix = json.loads(
        (ROOT / "docs" / "support" / "capability_matrix.json").read_text(
            encoding="utf-8"
        )
    )
    rows = {row["id"]: row for row in matrix["capabilities"]}
    row = rows["runtime.object-model.interface-method-table"]

    assert row["state"] == "implemented"
    assert row["support_claims"] == [
        "objc3c.behavior.runtime.object-model-interface-method-table"
    ]
    evidence_paths = {evidence["path"] for evidence in row["evidence"]}
    assert {
        "tests/native/runtime/object_model/interface_method_table_contract.objc3",
        "native/objc3c/src/runtime/classes/class_graph.cpp",
    } <= evidence_paths
    assert rows["runtime.object-model.class-realization"]["support_claims"] == [
        "objc3c.behavior.runtime.object-model-class-realization"
    ]
    assert rows["runtime.object-model.category-protocol-registration"]["support_claims"] == [
        "objc3c.behavior.runtime.object-model-category-protocol-registration"
    ]
    assert rows["runtime.object-model.property-ivar-reflection"]["support_claims"] == [
        "objc3c.behavior.runtime.object-model-property-ivar-reflection"
    ]
    assert rows["runtime.object-model.registration-replay"]["support_claims"] == [
        "objc3c.behavior.runtime.object-model-registration-replay"
    ]
    assert rows["runtime.object-model.bounded-query-snapshots"]["support_claims"] == [
        "objc3c.behavior.runtime.object-model-bounded-query-snapshots"
    ]
    assert rows["runtime.object-model.full-realization"]["state"] == "implemented"
    assert rows["runtime.object-model.full-realization"]["support_claims"] == [
        "objc3c.behavior.runtime.object-model.full-realization"
    ]


def test_cross_lane_manifest_support_claims_are_matrix_backed() -> None:
    matrix = json.loads(
        (ROOT / "docs" / "support" / "capability_matrix.json").read_text(
            encoding="utf-8"
        )
    )
    manifest = json.loads(
        (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "cross_lane_e2e"
            / "manifest.json"
        ).read_text(encoding="utf-8")
    )
    rows = {row["id"]: row for row in matrix["capabilities"]}
    implemented_claims = {
        claim
        for row in rows.values()
        if row["state"] == "implemented"
        for claim in row.get("support_claims", [])
    }

    for family in manifest["families"]:
        for capability_id in family["capability_rows"]:
            assert capability_id in rows, (family["family_id"], capability_id)
        for claim in family["support_claims"]:
            assert claim in implemented_claims, (family["family_id"], claim)


def test_object_model_implemented_rows_reject_broad_realization_language() -> None:
    validator = _load_validator()
    row = {
        "id": "runtime.object-model.interface-method-table",
        "title": "Full object-model runtime realization",
        "state": "implemented",
        "summary": "Broad object-model behavior over every runtime surface.",
        "support_claims": [
            "objc3c.behavior.runtime.object-model-interface-method-table"
        ],
        "owner_modules": ["native/objc3c/src/runtime/classes/class_graph.cpp"],
        "evidence": [
            {
                "kind": "test",
                "path": "tests/native/runtime/object_model/interface_method_table_contract.objc3",
                "command": "npm run objc3c -- test-behavior-matrix",
            },
            {
                "kind": "source",
                "path": "native/objc3c/src/runtime/classes/class_graph.cpp",
            },
        ],
    }

    with pytest.raises(validator.CapabilityDocsError, match="must stay narrow"):
        validator._validate_object_model_scope([row])


def test_object_model_implemented_rows_allow_storage_reflection_owners() -> None:
    validator = _load_validator()
    row = {
        "id": "runtime.object-model.property-ivar-reflection",
        "title": "Property and ivar reflection",
        "state": "implemented",
        "summary": "Property accessors and ivar layout are backed by runtime storage owners.",
        "support_claims": [
            "objc3c.behavior.runtime.object-model-property-ivar-reflection"
        ],
        "owner_modules": [
            "native/objc3c/src/runtime/storage/property_layout_realization.cpp",
            "native/objc3c/src/runtime/reflection/property_snapshot_api.cpp",
        ],
        "evidence": [
            {
                "kind": "test",
                "path": "tests/native/runtime/object_model/property_ivar_reflection_contract.objc3",
                "command": "npm run objc3c -- test-behavior-matrix",
            },
            {
                "kind": "source",
                "path": "native/objc3c/src/runtime/storage/property_layout_realization.cpp",
            },
        ],
    }

    validator._validate_object_model_scope([row])


def _minimal_umbrella_readiness(blocker_id: str = "missing-integrated-proof") -> dict[str, Any]:
    requirement = {
        "id": "integrated-proof",
        "description": "Integrated proof remains blocked in the synthetic readiness row.",
        "status": "blocked",
        "blocker_id": blocker_id,
    }
    return {
        "schema_version": "objc3c-umbrella-readiness-v1",
        "readiness_version": "test",
        "schema_path": "schemas/objc3c-umbrella-readiness-v1.schema.json",
        "matrix_path": "docs/support/capability_matrix.json",
        "evidence_map_path": "docs/support/evidence_map.json",
        "projection_policy": {
            "authoritative_data": [
                "docs/support/umbrella_readiness.json",
                "docs/support/capability_matrix.json",
                "docs/support/evidence_map.json",
            ],
            "human_projection": "docs/support/umbrella_readiness.md",
            "validator": "scripts/check_objc3c_umbrella_readiness.py",
            "consumer_rule": "readiness does not create support claims",
        },
        "entries": [
            {
                "umbrella_capability_id": "runtime.example.full",
                "current_state": "reserved",
                "target_state": "implemented",
                "readiness_state": "blocked",
                "intended_public_meaning": "Synthetic umbrella readiness row.",
                "forbidden_overclaims": ["generated reports as source truth"],
                "required_prerequisite_rows": [
                    {
                        "capability_id": "runtime.example.narrow",
                        "required_state": "implemented",
                        "reason": "narrow row must be implemented",
                    }
                ],
                "required_source_anchors": [
                    {
                        "id": "source-anchor",
                        "description": "README exists as synthetic source evidence.",
                        "status": "satisfied",
                        "path": "README.md",
                    }
                ],
                "required_public_commands": [],
                "required_positive_fixtures": [requirement],
                "required_negative_fixtures": [requirement],
                "required_runtime_probes": [],
                "required_abi_governance_rows": [],
                "required_docs": [
                    {
                        "id": "support-doc",
                        "description": "Support README exists as synthetic doc evidence.",
                        "status": "satisfied",
                        "path": "docs/support/README.md",
                    }
                ],
                "generated_output_boundary": {
                    "source_truth_allowed": False,
                    "unsupported_sources": [
                        "tmp/",
                        "temp/",
                        "generated markdown projections",
                        "issue comments",
                        "PR bodies",
                    ],
                    "rule": "generated outputs cannot satisfy readiness",
                },
                "promotion_blockers": [
                    {
                        "blocker_id": "missing-integrated-proof",
                        "summary": "Synthetic blocker.",
                        "missing_work": ["integrated proof"],
                    }
                ],
                "final_promotion_criteria": ["all requirements are satisfied"],
            }
        ],
    }


def _minimal_readiness_rows() -> list[dict[str, Any]]:
    return [
        {
            "id": "runtime.example.full",
            "title": "Synthetic full umbrella",
            "state": "reserved",
            "summary": "Synthetic umbrella row.",
            "evidence": [{"kind": "doc", "path": "docs/support/README.md"}],
        },
        {
            "id": "runtime.example.narrow",
            "title": "Synthetic narrow row",
            "state": "implemented",
            "summary": "Synthetic implemented prerequisite.",
            "support_claims": ["objc3c.behavior.runtime.example-narrow"],
            "evidence": [
                {
                    "kind": "test",
                    "path": "README.md",
                    "command": "npm run objc3c -- test-behavior-matrix",
                }
            ],
        },
    ]


def test_umbrella_readiness_accepts_blocked_rows_with_declared_blockers() -> None:
    _load_validator()
    from capability_docs_validator.umbrella_readiness import validate_umbrella_readiness

    validate_umbrella_readiness(
        _minimal_umbrella_readiness(),
        rows=_minimal_readiness_rows(),
        evidence_map={
            "rows": [
                {
                    "capability_id": "runtime.example.narrow",
                    "support_claim": "objc3c.behavior.runtime.example-narrow",
                    "evidence_kind": "test",
                    "path": "README.md",
                    "command": "npm run objc3c -- test-behavior-matrix",
                }
            ]
        },
    )


def test_umbrella_readiness_rejects_blocked_requirements_without_declared_blocker() -> None:
    validator = _load_validator()
    from capability_docs_validator.umbrella_readiness import validate_umbrella_readiness

    readiness = _minimal_umbrella_readiness(blocker_id="undeclared-blocker")

    with pytest.raises(validator.CapabilityDocsError, match="declared promotion blocker"):
        validate_umbrella_readiness(
            readiness,
            rows=_minimal_readiness_rows(),
            evidence_map={
                "rows": [
                    {
                        "capability_id": "runtime.example.narrow",
                        "support_claim": "objc3c.behavior.runtime.example-narrow",
                        "evidence_kind": "test",
                        "path": "README.md",
                        "command": "npm run objc3c -- test-behavior-matrix",
                    }
                ]
            },
        )


def test_umbrella_readiness_rejects_generated_output_as_source_truth() -> None:
    validator = _load_validator()
    from capability_docs_validator.umbrella_readiness import validate_umbrella_readiness

    readiness = _minimal_umbrella_readiness()
    readiness["entries"][0]["generated_output_boundary"]["unsupported_sources"].remove("tmp/")

    with pytest.raises(validator.CapabilityDocsError, match="generated output boundary"):
        validate_umbrella_readiness(
            readiness,
            rows=_minimal_readiness_rows(),
            evidence_map={
                "rows": [
                    {
                        "capability_id": "runtime.example.narrow",
                        "support_claim": "objc3c.behavior.runtime.example-narrow",
                        "evidence_kind": "test",
                        "path": "README.md",
                        "command": "npm run objc3c -- test-behavior-matrix",
                    }
                ]
            },
        )
