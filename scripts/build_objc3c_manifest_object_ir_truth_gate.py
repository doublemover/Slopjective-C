from __future__ import annotations

import argparse
import hashlib
import json
import os
import shutil
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REPORT_DIR = ROOT / "reports" / "claimability" / "manifest-object-ir-truth-gate"
JSON_OUT = REPORT_DIR / "manifest_object_ir_truth_gate_summary.json"
MD_OUT = REPORT_DIR / "manifest_object_ir_truth_gate_summary.md"

ISSUE = "#8018"
CONTRACT_ID = "objc3c.manifest.object.ir.truth.gate.v1"
COMPILER = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
SCRATCH = ROOT / "tmp" / "artifacts" / "objc3c-native" / "manifest-object-ir-truth-gate"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "dispatch" / "parser_container_inherited_ivar_layout.objc3"
NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_parser_container_ivar_layout_cycle.objc3"

LOWERING_CONTRACT_H = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMANTIC_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
STATIC_ANALYSIS = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_static_analysis.cpp"
CONFORMANCE_MANIFEST = ROOT / "tests" / "conformance" / "lowering_abi" / "manifest.json"
CONFORMANCE_README = ROOT / "tests" / "conformance" / "lowering_abi" / "README.md"
CONFORMANCE_POSITIVE = ROOT / "tests" / "conformance" / "lowering_abi" / "TRUTH-8018-01.json"
CONFORMANCE_NEGATIVE = ROOT / "tests" / "conformance" / "lowering_abi" / "TRUTH-8018-02.json"
SUPPORT_CLASSIFICATION = ROOT / "reports" / "claimability" / "support-classification" / "support_classification_summary.json"
PUBLIC_CLAIM_DRIFT = ROOT / "reports" / "claimability" / "public-claim-drift" / "public_claim_drift_summary.json"
DASHBOARD_BLOCKERS = ROOT / "reports" / "claimability" / "dashboard-release-blockers" / "dashboard_release_blocker_contract_summary.json"

REQUIRED_ARTIFACTS = [
    "module.manifest.json",
    "module.ll",
    "module.obj",
    "module.runtime-registration-descriptor.json",
    "module.runtime-registration-manifest.json",
    "module.runtime-metadata.bin",
    "module.runtime-metadata-discovery.json",
    "module.runtime-metadata-linker-options.rsp",
    "module.objc3-conformance-report.json",
    "module.objc3-conformance-publication.json",
    "module.objc3-advanced-feature-gate.json",
    "module.objc3-release-candidate-matrix.json",
]

DETERMINISTIC_ARTIFACTS = [
    "module.manifest.json",
    "module.ll",
    "module.obj",
    "module.runtime-registration-descriptor.json",
    "module.runtime-registration-manifest.json",
    "module.runtime-metadata.bin",
    "module.runtime-metadata-discovery.json",
    "module.runtime-metadata-linker-options.rsp",
    "module.objc3-conformance-report.json",
    "module.objc3-conformance-publication.json",
]

REQUIRED_IR_TOKENS = [
    "manifest_object_ir_truth_gate = contract=objc3c.manifest.object.ir.truth.gate.v1",
    "runtime_metadata_emission_gate = contract=objc3c.runtime.metadata.emission.gate.v1",
    "runtime_metadata_object_emission_closeout = contract=objc3c.runtime.cross.lane.object.emission.closeout.v1",
    "versioned_conformance_report_lowering = contract=objc3c.versioned.conformance.report.lowering.v1",
    "runtime_capability_reporting = contract=objc3c.runtime.capability.reporting.v1",
    "runtime_bootstrap_ctor_init_emission = contract=objc3c.runtime.constructor.init.stub.emission.v1",
    "runtime_registration_table_image_local_initialization = contract=objc3c.runtime.registration.table.image.local.initialization.v1",
    "manifest_artifact=module.manifest.json",
    "object_artifact=module.obj",
    "conformance_report_artifact=module.objc3-conformance-report.json",
]

REQUIRED_MANIFEST_KEYS = [
    "runtime_metadata_source_records",
    "runtime_bootstrap_lowering_registration_artifact_surface",
    "runtime_realization_lowering_reflection_artifact_surface",
    "runtime_release_candidate_claim_abi_surface",
    "runtime_block_arc_runtime_abi_surface",
    "runtime_unified_concurrency_runtime_abi_surface",
    "lowering_error_handling_result_and_bridging_artifact_replay",
    "interfaces",
]

REQUIRED_OBJECT_SECTIONS = [
    "objc3.runtime.image_info",
    "objc3.runtime.class_descriptors",
    "objc3.runtime.protocol_descriptors",
    "objc3.runtime.category_descriptors",
    "objc3.runtime.property_descriptors",
    "objc3.runtime.ivar_descriptors",
    "objc3.runtime.selector_pool",
    "objc3.runtime.string_pool",
    "objc3.runtime.discovery_root",
    "objc3.runtime.linker_anchor",
    "objc3.runtime.image_root",
    "objc3.runtime.registration_descriptor",
]

REQUIRED_OBJECT_SYMBOLS = [
    "__objc3_image_info",
    "__objc3_sec_class_descriptors",
    "__objc3_sec_protocol_descriptors",
    "__objc3_sec_category_descriptors",
    "__objc3_sec_property_descriptors",
    "__objc3_sec_ivar_descriptors",
    "objc3_runtime_metadata_discovery_root_",
    "objc3_runtime_metadata_link_anchor_",
    "__objc3_runtime_registration_table_",
    "__objc3_runtime_image_root_",
    "__objc3_runtime_registration_descriptor_",
    "objc3_runtime_stage_registration_table_for_bootstrap",
    "objc3_runtime_register_image",
]


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def find_llvm_tool(name: str) -> Path | None:
    found = shutil.which(name)
    if found:
        return Path(found)
    llvm_root = os.environ.get("LLVM_ROOT")
    candidates = []
    if llvm_root:
        candidates.append(Path(llvm_root) / "bin" / f"{name}.exe")
    candidates.append(Path("C:/Program Files/LLVM/bin") / f"{name}.exe")
    for candidate in candidates:
        if candidate.is_file():
            return candidate
    return None


def run_compiler(source: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    if out_dir.exists():
        shutil.rmtree(out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    return subprocess.run(
        [
            str(COMPILER),
            str(source),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
        ],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )


def inspect_object(obj_path: Path) -> dict:
    readobj = find_llvm_tool("llvm-readobj")
    objdump = find_llvm_tool("llvm-objdump")
    if not readobj or not objdump:
        return {
            "tools_available": False,
            "readobj": str(readobj) if readobj else "",
            "objdump": str(objdump) if objdump else "",
            "sections": {section: False for section in REQUIRED_OBJECT_SECTIONS},
            "symbols": {symbol: False for symbol in REQUIRED_OBJECT_SYMBOLS},
        }
    section_result = subprocess.run(
        [str(readobj), "--sections", str(obj_path)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    symbol_result = subprocess.run(
        [str(objdump), "--syms", str(obj_path)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    return {
        "tools_available": section_result.returncode == 0 and symbol_result.returncode == 0,
        "readobj": rel(readobj) if readobj.is_relative_to(ROOT) else str(readobj),
        "objdump": rel(objdump) if objdump.is_relative_to(ROOT) else str(objdump),
        "sections": {
            section: section in section_result.stdout
            for section in REQUIRED_OBJECT_SECTIONS
        },
        "symbols": {
            symbol: symbol in symbol_result.stdout
            for symbol in REQUIRED_OBJECT_SYMBOLS
        },
    }


def artifact_hashes(out_dir: Path, names: list[str]) -> dict[str, str]:
    return {
        name: sha256(out_dir / name)
        for name in names
        if (out_dir / name).is_file()
    }


def build_positive_runs() -> dict:
    runs = {}
    for run_name in ("run1", "run2"):
        out_dir = SCRATCH / "positive" / run_name
        result = run_compiler(POSITIVE_FIXTURE, out_dir)
        artifacts = {name: (out_dir / name).is_file() for name in REQUIRED_ARTIFACTS}
        hashes = artifact_hashes(out_dir, DETERMINISTIC_ARTIFACTS)
        manifest = json.loads(read(out_dir / "module.manifest.json")) if artifacts["module.manifest.json"] else {}
        conformance = (
            json.loads(read(out_dir / "module.objc3-conformance-report.json"))
            if artifacts["module.objc3-conformance-report.json"]
            else {}
        )
        ir_text = read(out_dir / "module.ll") if artifacts["module.ll"] else ""
        runs[run_name] = {
            "out_dir": rel(out_dir),
            "exit_code": result.returncode,
            "compiled": result.returncode == 0,
            "artifacts": artifacts,
            "hashes": hashes,
            "ir_tokens": {token: token in ir_text for token in REQUIRED_IR_TOKENS},
            "manifest_keys": {key: key in manifest for key in REQUIRED_MANIFEST_KEYS},
            "object_backend": read(out_dir / "module.object-backend.txt").strip()
            if (out_dir / "module.object-backend.txt").is_file()
            else "",
            "conformance_report": {
                "schema_id": conformance.get("schema_id", ""),
                "ready": conformance.get("ready") is True,
                "runtime_capability_ready": conformance.get("runtime_capability_report", {}).get("ready") is True,
                "public_schema_id": conformance.get("public_conformance_report", {}).get("schema_id", ""),
            },
        }
    return runs


def build_negative_runs() -> dict:
    runs = {}
    for run_name in ("run1", "run2"):
        out_dir = SCRATCH / "negative" / run_name
        result = run_compiler(NEGATIVE_FIXTURE, out_dir)
        diagnostics_path = out_dir / "module.diagnostics.json"
        diagnostics = json.loads(read(diagnostics_path)).get("diagnostics", []) if diagnostics_path.is_file() else []
        observed_codes = sorted({item.get("code", "") for item in diagnostics})
        runs[run_name] = {
            "out_dir": rel(out_dir),
            "exit_code": result.returncode,
            "rejected": result.returncode != 0,
            "observed_codes": observed_codes,
            "expected_codes_present": "O3P150" in observed_codes and "O3S260" in observed_codes,
            "emitted_artifacts_absent": {
                name: not (out_dir / name).is_file()
                for name in ["module.manifest.json", "module.ll", "module.obj"]
            },
            "diagnostics_hash": sha256(diagnostics_path) if diagnostics_path.is_file() else "",
        }
    return runs


def check_source_tokens() -> dict[str, bool]:
    source_checks = {
        "contract_header": [
            (LOWERING_CONTRACT_H, "kObjc3ManifestObjectIrTruthGateContractId"),
            (LOWERING_CONTRACT_H, "kObjc3ManifestObjectIrTruthGateManifestModel"),
            (LOWERING_CONTRACT_H, "kObjc3ManifestObjectIrTruthGateObjectModel"),
            (LOWERING_CONTRACT_H, "kObjc3ManifestObjectIrTruthGateClaimModel"),
            (LOWERING_CONTRACT_H, "Objc3ManifestObjectIrTruthGateSummary"),
        ],
        "contract_cpp": [
            (LOWERING_CONTRACT_CPP, "Objc3ManifestObjectIrTruthGateSummary()"),
            (LOWERING_CONTRACT_CPP, "module.manifest.json,module.ll,module.obj"),
            (LOWERING_CONTRACT_CPP, "kObjc3ManifestObjectIrTruthGateFailureModel"),
        ],
        "ir_emitter": [
            (IR_EMITTER, "manifest_object_ir_truth_gate"),
            (IR_EMITTER, "Objc3ManifestObjectIrTruthGateSummary()"),
            (IR_EMITTER, "runtime_metadata_object_emission_closeout"),
        ],
        "sema_pipeline": [
            (SEMA_PASS_MANAGER, "SemaPassManager"),
            (SEMANTIC_PASSES, "ResolveGlobalInitializerValues"),
            (STATIC_ANALYSIS, "BlockAlwaysReturns"),
        ],
    }
    result: dict[str, bool] = {}
    for group, checks in source_checks.items():
        for path, token in checks:
            result[f"{group}:{rel(path)}::{token}"] = token in read(path)
    return result


def check_conformance() -> dict[str, bool]:
    manifest = json.loads(read(CONFORMANCE_MANIFEST))
    files = {"TRUTH-8018-01.json", "TRUTH-8018-02.json"}
    readme = read(CONFORMANCE_README)
    return {
        "positive_fixture_exists": CONFORMANCE_POSITIVE.is_file(),
        "negative_fixture_exists": CONFORMANCE_NEGATIVE.is_file(),
        "manifest_references_truth": any(
            set(group.get("files", [])) >= files
            for group in manifest.get("groups", [])
        ),
        "readme_references_truth": "TRUTH-8018-01.json" in readme
        and "TRUTH-8018-02.json" in readme,
    }


def build_summary() -> dict:
    positive_runs = build_positive_runs()
    negative_runs = build_negative_runs()
    object_inspection = inspect_object(SCRATCH / "positive" / "run1" / "module.obj")
    source_checks = check_source_tokens()
    conformance = check_conformance()
    claim_gate_reports = {
        "support_classification": SUPPORT_CLASSIFICATION.is_file(),
        "public_claim_drift": PUBLIC_CLAIM_DRIFT.is_file(),
        "dashboard_release_blockers": DASHBOARD_BLOCKERS.is_file(),
    }

    run1_hashes = positive_runs["run1"]["hashes"]
    run2_hashes = positive_runs["run2"]["hashes"]
    deterministic_artifacts = {
        name: name in run1_hashes and run1_hashes.get(name) == run2_hashes.get(name)
        for name in DETERMINISTIC_ARTIFACTS
    }
    negative_diagnostics_deterministic = (
        negative_runs["run1"]["diagnostics_hash"]
        == negative_runs["run2"]["diagnostics_hash"]
        and bool(negative_runs["run1"]["diagnostics_hash"])
    )
    no_source_truth_under_tmp = all(
        not rel(path).startswith("tmp/")
        for path in [
            POSITIVE_FIXTURE,
            NEGATIVE_FIXTURE,
            LOWERING_CONTRACT_H,
            LOWERING_CONTRACT_CPP,
            IR_EMITTER,
            SEMA_PASS_MANAGER,
            SEMANTIC_PASSES,
            STATIC_ANALYSIS,
            CONFORMANCE_MANIFEST,
            CONFORMANCE_README,
            CONFORMANCE_POSITIVE,
            CONFORMANCE_NEGATIVE,
            SUPPORT_CLASSIFICATION,
            PUBLIC_CLAIM_DRIFT,
            DASHBOARD_BLOCKERS,
            JSON_OUT,
            MD_OUT,
        ]
    )

    checks = {
        "positive_runs_compiled": all(run["compiled"] for run in positive_runs.values()),
        "positive_artifacts_present": all(
            value for run in positive_runs.values() for value in run["artifacts"].values()
        ),
        "deterministic_artifact_hashes": all(deterministic_artifacts.values()),
        "positive_ir_tokens": all(
            value for run in positive_runs.values() for value in run["ir_tokens"].values()
        ),
        "positive_manifest_keys": all(
            value for run in positive_runs.values() for value in run["manifest_keys"].values()
        ),
        "positive_conformance_reports_ready": all(
            run["conformance_report"]["ready"]
            and run["conformance_report"]["runtime_capability_ready"]
            and run["conformance_report"]["schema_id"] == "objc3c-versioned-conformance-report-v1"
            and run["conformance_report"]["public_schema_id"] == "objc3-conformance-report/v1"
            for run in positive_runs.values()
        ),
        "object_backend_llvm_direct": all(
            run["object_backend"] == "llvm-direct" for run in positive_runs.values()
        ),
        "object_sections_present": object_inspection["tools_available"]
        and all(object_inspection["sections"].values()),
        "object_symbols_present": object_inspection["tools_available"]
        and all(object_inspection["symbols"].values()),
        "negative_runs_rejected": all(
            run["rejected"] and run["expected_codes_present"]
            for run in negative_runs.values()
        ),
        "negative_no_manifest_ir_object": all(
            value
            for run in negative_runs.values()
            for value in run["emitted_artifacts_absent"].values()
        ),
        "negative_diagnostics_deterministic": negative_diagnostics_deterministic,
        "source_tokens": all(source_checks.values()),
        "conformance": all(conformance.values()),
        "claim_gate_reports": all(claim_gate_reports.values()),
        "no_source_truth_under_tmp": no_source_truth_under_tmp,
    }

    return {
        "schema_version": "1.0.0",
        "issue": ISSUE,
        "contract_id": CONTRACT_ID,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "counts": {
            "required_artifact_count": len(REQUIRED_ARTIFACTS),
            "deterministic_artifact_count": len(DETERMINISTIC_ARTIFACTS),
            "required_ir_token_count": len(REQUIRED_IR_TOKENS),
            "required_manifest_key_count": len(REQUIRED_MANIFEST_KEYS),
            "required_object_section_count": len(REQUIRED_OBJECT_SECTIONS),
            "required_object_symbol_count": len(REQUIRED_OBJECT_SYMBOLS),
            "positive_run_count": len(positive_runs),
            "negative_run_count": len(negative_runs),
        },
        "checks": checks,
        "positive_fixture": rel(POSITIVE_FIXTURE),
        "negative_fixture": rel(NEGATIVE_FIXTURE),
        "positive_runs": positive_runs,
        "negative_runs": negative_runs,
        "deterministic_artifacts": deterministic_artifacts,
        "object_inspection": object_inspection,
        "source_checks": source_checks,
        "conformance": conformance,
        "claim_gate_reports": claim_gate_reports,
        "scratch_directory": rel(SCRATCH),
        "scratch_is_not_source_truth": True,
    }


def render_markdown(summary: dict) -> str:
    counts = summary["counts"]
    lines = [
        "# Manifest/Object/IR Truth Gate",
        "",
        f"- Issue: `{summary['issue']}`",
        f"- Contract: `{summary['contract_id']}`",
        f"- Status: `{summary['status']}`",
        f"- Required artifacts: `{counts['required_artifact_count']}`",
        f"- Deterministic artifacts: `{counts['deterministic_artifact_count']}`",
        f"- Object sections checked: `{counts['required_object_section_count']}`",
        f"- Object symbols checked: `{counts['required_object_symbol_count']}`",
        f"- Scratch output: `{summary['scratch_directory']}` (not source truth)",
        "",
        "## Checks",
        "",
    ]
    for name, value in summary["checks"].items():
        lines.append(f"- `{name}`: `{str(value).lower()}`")
    lines.extend(["", "## Artifact Determinism", ""])
    for name, value in summary["deterministic_artifacts"].items():
        lines.append(f"- `{name}`: `{str(value).lower()}`")
    lines.extend(["", "## Object Evidence", ""])
    for section, value in summary["object_inspection"]["sections"].items():
        lines.append(f"- section `{section}`: `{str(value).lower()}`")
    for symbol, value in summary["object_inspection"]["symbols"].items():
        lines.append(f"- symbol `{symbol}`: `{str(value).lower()}`")
    lines.append("")
    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true", help="fail if committed report files are stale")
    args = parser.parse_args()

    summary = build_summary()
    json_text = json.dumps(summary, indent=2, sort_keys=True) + "\n"
    md_text = render_markdown(summary)

    if args.check:
        stale = [
            rel(path)
            for path, text in ((JSON_OUT, json_text), (MD_OUT, md_text))
            if not path.is_file() or read(path) != text
        ]
        if stale:
            print("status: FAIL")
            print("stale reports:")
            for path in stale:
                print(f"- {path}")
            return 1
        print(f"status: {summary['status']}")
        return 0 if summary["status"] == "PASS" else 1

    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json_text, encoding="utf-8")
    MD_OUT.write_text(md_text, encoding="utf-8")
    print(f"status: {summary['status']}")
    print(f"wrote {rel(JSON_OUT)}")
    print(f"wrote {rel(MD_OUT)}")
    return 0 if summary["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
