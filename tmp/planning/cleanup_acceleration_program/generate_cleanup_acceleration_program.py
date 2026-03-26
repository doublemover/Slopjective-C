import json
from pathlib import Path

root = Path(r"C:/Users/sneak/Development/Slopjective-C")
out = root / "tmp" / "planning" / "cleanup_acceleration_program"
ms_dir = out / "milestones"
out.mkdir(parents=True, exist_ok=True)
ms_dir.mkdir(parents=True, exist_ok=True)

epic = "epic:cleanup-acceleration-and-runtime-correction"


def make_issue(code, lane, short, title_core, kind, milestone_focus, exit_gate, surfaces, deps=None, notes=None, corrections=None, validation_posture=None):
    deps = deps or []
    notes = notes or []
    corrections = corrections or []
    title = f"[{code.split('-')[0]}][Lane-{lane}][{short}] {title_core} - {kind}"
    acceptance_by_lane = {
        "A": [
            "Freeze or complete the source, architecture, inventory, or budget boundary truthfully without claiming product behavior that does not exist yet.",
            "Make scope limits, dependency edges, non-goals, and explicit replacement surfaces legible so downstream implementation can simplify instead of proliferate.",
            "Publish deterministic inventory or target evidence suitable for replay, review, and later milestone consumption.",
        ],
        "B": [
            "Land a truthful semantic, policy, migration, or exception model that narrows ambiguity and removes excuses for one-off milestone-specific behavior.",
            "Prefer consolidation over additive scaffolding; remove redundant or parallel paths where the replacement is ready, and define compatibility windows where immediate removal is unsafe.",
            "Specify what is allowed to remain, what is temporary, and what is prohibited going forward.",
        ],
        "C": [
            "Freeze or implement the shared lowering, artifact, workflow, preservation, or naming contract for this subsystem using existing surfaces where possible.",
            "Emit deterministic replay, provenance, migration, or source-of-truth evidence where applicable.",
            "Avoid proof-only or compatibility-only side paths unless the issue exists specifically to fence, label, and retire them.",
        ],
        "D": [
            "Freeze or implement the live runtime, CI, operator, or governance behavior for the milestone subsystem.",
            "Back the result with executable, replayable, or automatically enforced proof instead of metadata-only claims.",
            "Harden determinism, compatibility, failure behavior, and rollback handling where the system becomes live.",
        ],
        "E": [
            "Publish a truthful gate or closeout matrix over the live surfaces landed earlier in the milestone.",
            "Use shared acceptance harnesses and real evidence rather than milestone-local success theater.",
            "Do not widen public or internal claims beyond what the evidence, budgets, and cross-surface proofs actually support.",
        ],
    }
    validation_posture_by_lane = {
        "A": "generator_contract_only",
        "B": "static_policy_guard",
        "C": "shared_acceptance_harness",
        "D": "shared_acceptance_harness",
        "E": "shared_acceptance_harness",
    }
    validation_evidence = {
        "shared_acceptance_harness": "shared acceptance harness or executable integration proof",
        "static_policy_guard": "targeted static checker and pytest only when the guard has nontrivial logic",
        "migration_bridge": "migration verifier and compatibility evidence",
        "generator_contract_only": "template/schema checker; no readiness runner by default",
    }
    validation_posture = validation_posture or validation_posture_by_lane[lane]
    body = [
        title,
        "",
        "## Outcome",
        f"Deliver `{code}` for **{title_core}** within the milestone focus: {milestone_focus}",
        "",
        "## Why this matters",
        f"This issue exists because {milestone_focus} The milestone exit gate is: {exit_gate}",
    ]
    if corrections:
        body += ["", "## Design corrections folded in"] + [f"- {item}" for item in corrections]
    body += [
        "",
        "## Acceptance criteria",
    ] + [f"- {item}" for item in acceptance_by_lane[lane]] + [
        "",
        "## Primary implementation surfaces",
    ] + [f"- `{item}`" for item in surfaces] + [
        "",
        "## Dependencies",
    ]
    if deps:
        body += [f"- `{item}`" for item in deps]
    else:
        body += ["- None. This issue establishes the milestone boundary."]
    body += [
        "",
        "## Validation posture",
        f"- Class: `{validation_posture}`",
        f"- Default evidence: {validation_evidence[validation_posture]}",
    ]
    if notes:
        body += ["", "## Notes"] + [f"- {item}" for item in notes]
    return {
        "code": code,
        "short_code": short,
        "lane": lane,
        "kind": kind,
        "title": title,
        "validation_posture": validation_posture,
        "body": "\n".join(body),
    }


milestones = []


def add_milestone(code, title, focus, exit_gate, domain, improvements, shared_surfaces, targets, issue_defs):
    issues = []
    for issue_def in issue_defs:
        issues.append(
            make_issue(
                code=issue_def["code"],
                lane=issue_def["lane"],
                short=issue_def["short"],
                title_core=issue_def["core"],
                kind=issue_def["kind"],
                milestone_focus=focus,
                exit_gate=exit_gate,
                surfaces=issue_def.get("surfaces", shared_surfaces),
                deps=issue_def.get("deps"),
                notes=issue_def.get("notes"),
                corrections=issue_def.get("corrections", improvements),
                validation_posture=issue_def.get("validation_posture"),
            )
        )
    desc_lines = [
        f"Focus: {focus}",
        f"Exit gate: {exit_gate}",
        "",
        "Design corrections folded in:",
    ] + [f"- {item}" for item in improvements] + [
        "",
        "Success targets:",
    ] + [f"- {item}" for item in targets] + [
        "",
        "Shared surfaces:",
    ] + [f"- `{item}`" for item in shared_surfaces] + [
        "",
        f"Issue budget: {len(issues)}",
        f"Domain: `{domain}`",
        f"Epic: `{epic}`",
    ]
    milestones.append(
        {
            "code": code,
            "title": f"{code} {title}",
            "description": "\n".join(desc_lines),
            "domain": domain,
            "epic": epic,
            "issue_budget": len(issues),
            "focus": focus,
            "exit_gate": exit_gate,
            "improvements": improvements,
            "targets": targets,
            "shared_surfaces": shared_surfaces,
            "issues": issues,
        }
    )


add_milestone(
    "M313",
    "Validation architecture, acceptance suites, and checker collapse",
    "Replace the current checker-heavy validation sprawl with a deliberate test pyramid, shared executable acceptance suites, a small justified static guardrail set, and explicit migration/quarantine rules for legacy validation surfaces.",
    "Validation truth comes primarily from executable subsystem suites and integration harnesses; checker count, static noise, and milestone-local validation scaffolds are materially reduced without losing coverage or replayability.",
    "domain:validation-consolidation",
    [
        "Rejects the false binary of either keeping thousands of milestone-local checkers forever or deleting them all at once; replacement happens through a governed migration pyramid with explicit quarantine surfaces.",
        "Treats acceptance suites and executable integration harnesses as the real source of truth, with static policy checks kept only where they still add unique value.",
        "Adds exception policy, namespace policy, and numerical ratchets so validation cleanup becomes enforceable instead of aspirational.",
    ],
    [
        "scripts/ check_*.py and run_*_readiness.py surfaces",
        "tests/tooling/ and subsystem acceptance suites",
        "CI workflow validation topology",
        "tmp/reports/ validation evidence layout",
        "docs/ validation and operator runbooks",
    ],
    [
        "Reduce total `check_*.py` count by at least 80% from the current baseline before milestone closeout, with every retained checker explicitly classified.",
        "Establish active, migration-only, and archival validation namespaces and move legacy surfaces into the correct bucket.",
        "Require new validation surfaces to default to shared harnesses or acceptance suites unless an exception record exists.",
    ],
    [
        {"code": "M313-A001", "lane": "A", "short": "A001", "core": "Validation surface inventory and taxonomy", "kind": "Contract and architecture freeze"},
        {"code": "M313-A002", "lane": "A", "short": "A002", "core": "Validation reduction targets and ratchet map", "kind": "Source completion", "deps": ["M313-A001"]},
        {"code": "M313-A003", "lane": "A", "short": "A003", "core": "Subsystem acceptance-suite boundary and migration map", "kind": "Source completion", "deps": ["M313-A002"]},
        {"code": "M313-B001", "lane": "B", "short": "B001", "core": "Testing pyramid and static-guard retention model", "kind": "Contract and architecture freeze", "deps": ["M313-A003"]},
        {"code": "M313-B002", "lane": "B", "short": "B002", "core": "Shared compiler-runtime integration harness", "kind": "Core feature implementation", "deps": ["M313-B001"]},
        {"code": "M313-B003", "lane": "B", "short": "B003", "core": "Checker consolidation and migration policy", "kind": "Core feature implementation", "deps": ["M313-B002"]},
        {"code": "M313-B004", "lane": "B", "short": "B004", "core": "Legacy validation quarantine and namespace policy", "kind": "Core feature implementation", "deps": ["M313-B003"]},
        {"code": "M313-B005", "lane": "B", "short": "B005", "core": "New-checker and temporary-validation exception policy", "kind": "Edge case and compatibility completion", "deps": ["M313-B004"]},
        {"code": "M313-C001", "lane": "C", "short": "C001", "core": "Acceptance artifact schema and replay contract", "kind": "Contract and architecture freeze", "deps": ["M313-B005"]},
        {"code": "M313-C002", "lane": "C", "short": "C002", "core": "Subsystem executable acceptance suites", "kind": "Core feature implementation", "deps": ["M313-C001"]},
        {"code": "M313-C003", "lane": "C", "short": "C003", "core": "Historical checker compatibility bridge and deprecation surface", "kind": "Core feature expansion", "deps": ["M313-C002"]},
        {"code": "M313-D001", "lane": "D", "short": "D001", "core": "CI validation topology and scheduling contract", "kind": "Contract and architecture freeze", "deps": ["M313-C003"]},
        {"code": "M313-D002", "lane": "D", "short": "D002", "core": "CI migration to acceptance-first validation", "kind": "Core feature implementation", "deps": ["M313-D001"]},
        {"code": "M313-E001", "lane": "E", "short": "E001", "core": "Validation-noise reduction gate", "kind": "Contract and architecture freeze", "deps": ["M313-D002"]},
        {"code": "M313-E002", "lane": "E", "short": "E002", "core": "Validation consolidation closeout matrix", "kind": "Cross-lane integration sync", "deps": ["M313-E001"]},
    ],
)

add_milestone(
    "M314",
    "Command-surface reduction, dead-path removal, and workflow simplification",
    "Collapse the repo command surface into a small coherent public interface, define a single operator-facing orchestration model, move internal complexity behind parameterized runners, and remove dead prototype paths that only add confusion.",
    "The public workflow surface is compact and legible, dead prototype code is gone, operator workflows are documented around one clear orchestration model, and normal tasks no longer depend on thousands of ad hoc `npm` aliases or parallel script dialects.",
    "domain:workflow-simplification",
    [
        "Replaces raw script-count thinking with a better split: a compact public entrypoint surface backed by parameterized runners and documented workflows.",
        "Treats orchestration-layer choice as an architectural decision instead of letting `npm`, Python, PowerShell, and CI YAML all act as competing public interfaces.",
        "Adds alias-deprecation windows and budget enforcement so workflow cleanup sticks without abrupt breakage.",
    ],
    [
        "package.json script surface",
        "scripts/ build, test, docs, and workflow runner surfaces",
        "README.md and operator workflow docs",
        "compiler/ prototype surfaces",
        "CI and local workflow entrypoints",
    ],
    [
        "Reduce public `package.json` scripts to at most 25 documented entrypoints, with everything else moved behind internal task runners or direct scripts.",
        "Define one primary operator-facing orchestration layer and ban new public entrypoints that bypass it without exception approval.",
        "Retire the dead prototype compiler surface completely and leave no ambiguous operator path that still references it.",
    ],
    [
        {"code": "M314-A001", "lane": "A", "short": "A001", "core": "Command-surface inventory and script-budget policy", "kind": "Contract and architecture freeze"},
        {"code": "M314-A002", "lane": "A", "short": "A002", "core": "Orchestration-layer policy and operator workflow map", "kind": "Source completion", "deps": ["M314-A001"]},
        {"code": "M314-B001", "lane": "B", "short": "B001", "core": "Public-versus-internal command model", "kind": "Contract and architecture freeze", "deps": ["M314-A002"]},
        {"code": "M314-B002", "lane": "B", "short": "B002", "core": "Public package script collapse", "kind": "Core feature implementation", "deps": ["M314-B001"]},
        {"code": "M314-B003", "lane": "B", "short": "B003", "core": "Build-test-doc workflow runner unification", "kind": "Core feature implementation", "deps": ["M314-B002"]},
        {"code": "M314-B004", "lane": "B", "short": "B004", "core": "Alias deprecation and compatibility window", "kind": "Core feature implementation", "deps": ["M314-B003"]},
        {"code": "M314-B005", "lane": "B", "short": "B005", "core": "Prototype compiler surface retirement", "kind": "Edge case and compatibility completion", "deps": ["M314-B004"], "notes": ["This issue explicitly covers removal of `compiler/objc3c/semantic.py` and any residual prototype-only Python compiler surface, with compatibility notes if historical docs reference it."]},
        {"code": "M314-C001", "lane": "C", "short": "C001", "core": "Task-runner and workflow API contract", "kind": "Contract and architecture freeze", "deps": ["M314-B005"]},
        {"code": "M314-C002", "lane": "C", "short": "C002", "core": "Parameterized task runner implementation", "kind": "Core feature implementation", "deps": ["M314-C001"]},
        {"code": "M314-C003", "lane": "C", "short": "C003", "core": "Public command budget enforcement and CLI docs synchronization", "kind": "Core feature expansion", "deps": ["M314-C002"]},
        {"code": "M314-D001", "lane": "D", "short": "D001", "core": "Maintainer operator workflow and docs contract", "kind": "Contract and architecture freeze", "deps": ["M314-C003"]},
        {"code": "M314-E001", "lane": "E", "short": "E001", "core": "Workflow simplification gate", "kind": "Contract and architecture freeze", "deps": ["M314-D001"]},
        {"code": "M314-E002", "lane": "E", "short": "E002", "core": "Command-surface closeout matrix", "kind": "Cross-lane integration sync", "deps": ["M314-E001"]},
    ],
)

add_milestone(
    "M315",
    "Source de-scaffolding, stable identifiers, artifact authenticity, and proof hygiene",
    "Strip issue-era residue out of product and generated surfaces, define stable non-milestone identifiers for enduring contracts, classify synthetic versus genuine artifacts honestly, and make replay/proof surfaces defensible instead of theatrical.",
    "Product and generated source-of-truth surfaces no longer depend on milestone-coded residue, stable replacement identifiers exist, synthetic artifacts are clearly labeled and contained, and proof surfaces carry provenance that distinguishes real compilation outputs from hand-authored fixtures.",
    "domain:source-hygiene-and-authenticity",
    [
        "Separates archival planning history from product and generated source-of-truth surfaces so implementation files stop carrying milestone-era noise as comments, constexpr labels, or contract strings.",
        "Treats synthetic fixtures as legitimate only when explicitly labeled and fenced; anything presented as replay or proof evidence must carry provenance and a regeneration path.",
        "Adds stable replacement identifiers so milestone-marker removal does not simply create a new ad hoc naming mess.",
    ],
    [
        "native/objc3c/src/ source comments, constexpr labels, and contract strings",
        "tracked .ll, .json, and other replay/proof artifact surfaces",
        "scripts/ provenance, artifact, and policy guards",
        "docs/ and planning surfaces that distinguish archival references from product implementation",
        "CI policy enforcement surfaces",
    ],
    [
        "Reduce `m2xx-*` references inside `native/objc3c/src` to zero and classify the remaining repo-wide references as archival, transitional, or prohibited.",
        "Classify 100% of tracked replay/proof artifacts as synthetic fixture, genuine generated artifact, or historical archive, with provenance rules for every genuine class.",
        "Define a stable feature-surface identifier scheme so future contracts and emitted evidence do not depend on milestone numbers.",
    ],
    [
        {"code": "M315-A001", "lane": "A", "short": "A001", "core": "Repo-wide milestone residue inventory", "kind": "Contract and architecture freeze"},
        {"code": "M315-A002", "lane": "A", "short": "A002", "core": "Native-source milestone-residue inventory", "kind": "Source completion", "deps": ["M315-A001"]},
        {"code": "M315-A003", "lane": "A", "short": "A003", "core": "Artifact authenticity and provenance classification inventory", "kind": "Source completion", "deps": ["M315-A002"]},
        {"code": "M315-B001", "lane": "B", "short": "B001", "core": "Stable feature-surface identifier and annotation policy", "kind": "Contract and architecture freeze", "deps": ["M315-A003"]},
        {"code": "M315-B002", "lane": "B", "short": "B002", "core": "Product-code annotation and provenance policy", "kind": "Contract and architecture freeze", "deps": ["M315-B001"]},
        {"code": "M315-B003", "lane": "B", "short": "B003", "core": "Milestone-marker removal from native source", "kind": "Core feature implementation", "deps": ["M315-B002"]},
        {"code": "M315-B004", "lane": "B", "short": "B004", "core": "Synthetic-versus-genuine IR fixture compatibility semantics", "kind": "Core feature implementation", "deps": ["M315-B003"]},
        {"code": "M315-B005", "lane": "B", "short": "B005", "core": "Comment constexpr and contract-string decontamination sweep", "kind": "Edge case and compatibility completion", "deps": ["M315-B004"]},
        {"code": "M315-C001", "lane": "C", "short": "C001", "core": "Source-of-truth and generated-artifact contract", "kind": "Contract and architecture freeze", "deps": ["M315-B005"]},
        {"code": "M315-C002", "lane": "C", "short": "C002", "core": "Artifact authenticity schema and evidence contract", "kind": "Contract and architecture freeze", "deps": ["M315-C001"]},
        {"code": "M315-C003", "lane": "C", "short": "C003", "core": "Genuine replay artifact regeneration and provenance capture", "kind": "Core feature implementation", "deps": ["M315-C002"]},
        {"code": "M315-C004", "lane": "C", "short": "C004", "core": "Synthetic fixture relocation labeling and parity-test updates", "kind": "Core feature expansion", "deps": ["M315-C003"]},
        {"code": "M315-D001", "lane": "D", "short": "D001", "core": "Source-hygiene and proof-policy enforcement contract", "kind": "Contract and architecture freeze", "deps": ["M315-C004"]},
        {"code": "M315-D002", "lane": "D", "short": "D002", "core": "Anti-noise enforcement implementation", "kind": "Core feature implementation", "deps": ["M315-D001"], "notes": ["This issue should add policy guards that reject new `m2xx-*` residue in product code, unlabeled synthetic proof artifacts, and reintroduction of dead prototype compiler surfaces."]},
        {"code": "M315-E001", "lane": "E", "short": "E001", "core": "Source-hygiene and proof-hygiene gate", "kind": "Contract and architecture freeze", "deps": ["M315-D002"]},
        {"code": "M315-E002", "lane": "E", "short": "E002", "core": "De-scaffolding and authenticity closeout matrix", "kind": "Cross-lane integration sync", "deps": ["M315-E001"]},
    ],
)

add_milestone(
    "M316",
    "Runtime corrective tranche: realized dispatch, synthesized accessors, and native output truth",
    "Close the concrete runtime/codegen gaps that remain despite the larger completion roadmap: real selector-pool-backed dispatch through the realized object model, real synthesized property accessor LLVM IR, executable runtime-backed synthesized accessors, and honest native compilation evidence.",
    "The runtime has a real dispatch path analogous to `objc_msgSend` over the realized selector/object model, synthesized properties generate executable getter/setter IR and run at runtime, and native output evidence is provably tied to genuine compilation flows instead of placeholder proof artifacts.",
    "domain:runtime-corrective-completion",
    [
        "Makes the existing backlog sharper by turning vague object-model and storage closure into explicit corrective issues for the two missing live behaviors: realized dispatch and synthesized accessor codegen/runtime.",
        "Adds a dedicated native-output truth track so replay and proof artifacts cannot mask missing codegen or runtime behavior.",
        "Treats this as a corrective tranche that must amend the existing M279/M280 runtime issues rather than creating a competing parallel roadmap.",
    ],
    [
        "native/objc3c/src/runtime/ selector lookup, dispatch, and realization surfaces",
        "native/objc3c/src/ir/ and lower/ property synthesis and accessor emission surfaces",
        "native/objc3c/src/pipeline/ and io/ artifact provenance surfaces",
        "tests/tooling/runtime/ executable probes and native IR/object evidence",
        "docs/spec/runtime claim surfaces that must match the live implementation",
    ],
    [
        "Executable dispatch proof must cover superclass lookup, category-attached methods, selector identity via the existing selector pool, and observable object-state mutation through real method bodies.",
        "Synthesized accessor proof must cover getter and setter IR generation, ivar-backed storage access, runtime execution, and reflection coherence for at least one nontrivial property fixture.",
        "Every artifact cited as native-output proof in this milestone must be reproducible from a documented command path and carry provenance that distinguishes it from synthetic parity fixtures.",
    ],
    [
        {"code": "M316-A001", "lane": "A", "short": "A001", "core": "Runtime corrective tranche architecture and non-goal map", "kind": "Contract and architecture freeze"},
        {"code": "M316-A002", "lane": "A", "short": "A002", "core": "Selector dispatch property synthesis and native-output truth gap inventory", "kind": "Source completion", "deps": ["M316-A001"]},
        {"code": "M316-B001", "lane": "B", "short": "B001", "core": "Realized dispatch semantic model over selector-pool-backed object graphs", "kind": "Contract and architecture freeze", "deps": ["M316-A002"]},
        {"code": "M316-B002", "lane": "B", "short": "B002", "core": "Dispatch acceptance workload and behavioral proof model", "kind": "Source completion", "deps": ["M316-B001"]},
        {"code": "M316-B003", "lane": "B", "short": "B003", "core": "Selector-pool-backed message dispatch semantics", "kind": "Core feature implementation", "deps": ["M316-B002"], "notes": ["This issue is the semantic half of the `objc_msgSend`-equivalent path and should explicitly consume the already-implemented selector pool rather than inventing a parallel dispatch registry."]},
        {"code": "M316-B004", "lane": "B", "short": "B004", "core": "Property synthesis and storage-accessor semantic completion", "kind": "Core feature implementation", "deps": ["M316-B003"]},
        {"code": "M316-B005", "lane": "B", "short": "B005", "core": "Native IR truthfulness and proof-authenticity diagnostics", "kind": "Edge case and compatibility completion", "deps": ["M316-B004"]},
        {"code": "M316-C001", "lane": "C", "short": "C001", "core": "Dispatch and synthesized-accessor lowering contract", "kind": "Contract and architecture freeze", "deps": ["M316-B005"]},
        {"code": "M316-C002", "lane": "C", "short": "C002", "core": "Real method dispatch and selector thunk lowering", "kind": "Core feature implementation", "deps": ["M316-C001"]},
        {"code": "M316-C003", "lane": "C", "short": "C003", "core": "Synthesized getter-setter LLVM IR generation", "kind": "Core feature implementation", "deps": ["M316-C002"], "notes": ["This issue should close the current `missing synthesized accessor getter binding` / `setter binding` failure path in `objc3_ir_emitter.cpp` by producing real bodies, not just metadata placeholders."]},
        {"code": "M316-C004", "lane": "C", "short": "C004", "core": "Native compilation output provenance and replay preservation", "kind": "Core feature expansion", "deps": ["M316-C003"]},
        {"code": "M316-D001", "lane": "D", "short": "D001", "core": "Runtime ABI and executable proof contract for dispatch and accessors", "kind": "Contract and architecture freeze", "deps": ["M316-C004"]},
        {"code": "M316-D002", "lane": "D", "short": "D002", "core": "Live realized dispatch runtime implementation", "kind": "Core feature implementation", "deps": ["M316-D001"]},
        {"code": "M316-D003", "lane": "D", "short": "D003", "core": "Live synthesized accessor execution and runtime-reflection coherence", "kind": "Core feature implementation", "deps": ["M316-D002"]},
        {"code": "M316-E001", "lane": "E", "short": "E001", "core": "Runtime corrective tranche gate", "kind": "Contract and architecture freeze", "deps": ["M316-D003"]},
        {"code": "M316-E002", "lane": "E", "short": "E002", "core": "Runnable object-model storage and native-truth matrix", "kind": "Cross-lane integration sync", "deps": ["M316-E001"]},
    ],
)

add_milestone(
    "M317",
    "Backlog publication realignment and supersession hygiene",
    "Reshape the existing GitHub backlog to fit the cleaner architecture before further large-scale feature work lands, simplify issue authoring for publication, and align overlapping milestones to the corrective program.",
    "Current and near-future issue work is aligned to the cleaner program, overlapping milestones are amended or superseded instead of left ambiguous, and publication-time planning stops reinforcing noisy patterns.",
    "domain:backlog-realignment",
    [
        "Treats backlog surgery as immediate engineering work: ambiguous or overlapping issues slow delivery and hide missing work just as much as bad code does.",
        "Separates publication-time realignment from long-term governance so the necessary issue amendments happen before they are buried under later work.",
        "Uses explicit supersession and dependency rewrites so corrective milestones sharpen, rather than duplicate, existing runtime and cleanup plans.",
    ],
    [
        "GitHub milestone and issue bodies for overlapping runtime, cleanup, and validation work",
        "local planning seed generators and publish scripts under tmp/",
        "issue templates, planning docs, and contribution/maintainer policy surfaces",
        "future milestone descriptions and dependency statements that must consume the corrective program",
        "publication-time consistency reports and review artifacts",
    ],
    [
        "Amend all explicitly overlapping issues before or alongside publication of the corrective milestones; do not leave duplicate runtime or cleanup ownership unresolved.",
        "Update future milestone descriptions and issue dependencies where they would otherwise lock in the current noisy validation and workflow model.",
        "Make publication-time supersession and amendment logic machine-auditable so future backlog edits are replayable.",
    ],
    [
        {"code": "M317-A001", "lane": "A", "short": "A001", "core": "Backlog overlap and correction inventory", "kind": "Contract and architecture freeze"},
        {"code": "M317-A002", "lane": "A", "short": "A002", "core": "Current GitHub issue amendment and supersession map", "kind": "Source completion", "deps": ["M317-A001"]},
        {"code": "M317-B001", "lane": "B", "short": "B001", "core": "Immediate backlog surgery and supersession model", "kind": "Contract and architecture freeze", "deps": ["M317-A002"]},
        {"code": "M317-B002", "lane": "B", "short": "B002", "core": "Existing milestone and issue amendments implementation", "kind": "Core feature implementation", "deps": ["M317-B001"]},
        {"code": "M317-B003", "lane": "B", "short": "B003", "core": "Future milestone dependency rewrites for post-M292 work", "kind": "Edge case and compatibility completion", "deps": ["M317-B002"]},
        {"code": "M317-C001", "lane": "C", "short": "C001", "core": "Planning packet and issue-template simplification contract", "kind": "Contract and architecture freeze", "deps": ["M317-B003"]},
        {"code": "M317-C002", "lane": "C", "short": "C002", "core": "Issue template and seed-generator simplification implementation", "kind": "Core feature implementation", "deps": ["M317-C001"]},
        {"code": "M317-D001", "lane": "D", "short": "D001", "core": "Publication-time consistency audit contract", "kind": "Contract and architecture freeze", "deps": ["M317-C002"]},
        {"code": "M317-E001", "lane": "E", "short": "E001", "core": "Backlog realignment gate", "kind": "Contract and architecture freeze", "deps": ["M317-D001"]},
        {"code": "M317-E002", "lane": "E", "short": "E002", "core": "Backlog publication closeout matrix", "kind": "Cross-lane integration sync", "deps": ["M317-E001"]},
    ],
)

add_milestone(
    "M318",
    "Governance hardening, anti-noise budgets, and sustainable progress enforcement",
    "Add the long-lived governance, exception handling, and CI policy needed to prevent the repo from sliding back into checker proliferation, command sprawl, milestone-coded product strings, and ambiguous proof surfaces.",
    "Future progress is bounded by enforceable budgets, explicit exception processes, sustainable review checklists, and automatic policy checks rather than maintainer memory or taste.",
    "domain:governance-and-sustainability",
    [
        "Separates long-term governance from immediate backlog surgery so the repo gets both publication-time correction and durable anti-regression enforcement.",
        "Treats exceptions as first-class policy objects with ownership and expiry, instead of allowing noisy temporary surfaces to linger indefinitely.",
        "Links planning hygiene, CI budgets, and review practice so future work is easier and faster because the repo shape stays disciplined.",
    ],
    [
        "CI and policy guards for script counts, checker growth, milestone residue, and artifact authenticity",
        "planning docs that define how work is added, amended, and retired going forward",
        "maintainer review checklists and contribution policy surfaces",
        "local planning seed generators and publish scripts under tmp/",
        "reporting surfaces for budgets, waivers, and enforcement outcomes",
    ],
    [
        "Create explicit budgets for public commands, retained checkers, synthetic fixtures, and milestone-coded residue, with CI enforcement and waiver reporting.",
        "Require every exception to have an owner, rationale, and expiry path.",
        "Make sustainable-progress enforcement visible in normal maintainer workflows instead of hiding it behind occasional cleanup projects.",
    ],
    [
        {"code": "M318-A001", "lane": "A", "short": "A001", "core": "Anti-noise governance inventory and budget map", "kind": "Contract and architecture freeze"},
        {"code": "M318-A002", "lane": "A", "short": "A002", "core": "Exception process and ownership model", "kind": "Source completion", "deps": ["M318-A001"]},
        {"code": "M318-B001", "lane": "B", "short": "B001", "core": "Governance and budget enforcement policy", "kind": "Contract and architecture freeze", "deps": ["M318-A002"]},
        {"code": "M318-B002", "lane": "B", "short": "B002", "core": "Sustainable planning hygiene policy", "kind": "Core feature implementation", "deps": ["M318-B001"]},
        {"code": "M318-B003", "lane": "B", "short": "B003", "core": "Maintainer review checklist and regression handling model", "kind": "Edge case and compatibility completion", "deps": ["M318-B002"]},
        {"code": "M318-C001", "lane": "C", "short": "C001", "core": "Governance automation and CI policy contract", "kind": "Contract and architecture freeze", "deps": ["M318-B003"]},
        {"code": "M318-C002", "lane": "C", "short": "C002", "core": "Budget enforcement and regression alarms implementation", "kind": "Core feature implementation", "deps": ["M318-C001"]},
        {"code": "M318-C003", "lane": "C", "short": "C003", "core": "New-work proposal and publication workflow tooling", "kind": "Core feature expansion", "deps": ["M318-C002"]},
        {"code": "M318-D001", "lane": "D", "short": "D001", "core": "Long-term governance and waiver reporting contract", "kind": "Contract and architecture freeze", "deps": ["M318-C003"]},
        {"code": "M318-E001", "lane": "E", "short": "E001", "core": "Sustainable progress gate", "kind": "Contract and architecture freeze", "deps": ["M318-D001"]},
        {"code": "M318-E002", "lane": "E", "short": "E002", "core": "Governance hardening closeout matrix", "kind": "Cross-lane integration sync", "deps": ["M318-E001"]},
    ],
)

seed = {
    "program_title": "Cleanup acceleration and runtime corrective program",
    "purpose": "Create a corrective program that removes noisy repo patterns, tightens evidence quality, closes concrete runtime/codegen gaps, amends overlapping backlog work, and makes future milestone execution materially easier and faster.",
    "milestones": milestones,
    "totals": {
        "milestones": len(milestones),
        "issues": sum(item["issue_budget"] for item in milestones),
    },
    "observed_repo_facts": {
        "npm_script_count": 7737,
        "checker_scripts": 2781,
        "checker_pytests": 2767,
        "native_source_m2xx_refs": 3002,
        "tracked_ll_files": 78,
        "tracked_stub_ll_files": 2,
        "dead_prototype_compiler_file": "compiler/objc3c/semantic.py",
    },
    "why_now": [
        "The repo currently carries too many milestone-local validation scripts and command aliases for new work to stay fast or legible.",
        "Product code and proof artifacts still carry issue-era residue that obscures the real implementation state.",
        "The existing completion backlog is directionally right but not sharp enough about realized dispatch, synthesized accessors, native-output authenticity, or anti-regression governance.",
    ],
}

(out / "cleanup_acceleration_program_seed.json").write_text(json.dumps(seed, indent=2) + "\n", encoding="utf-8")

summary_lines = [
    "# Cleanup Acceleration And Runtime Corrective Program",
    "",
    f"- milestones: {seed['totals']['milestones']}",
    f"- issues: {seed['totals']['issues']}",
    "",
    "## Why this program exists",
    "",
    "- Collapse validation, command, and planning noise so future work moves faster.",
    "- Remove issue-era contamination from product code and proof surfaces.",
    "- Close the concrete runtime/codegen gaps that the current long-range backlog still describes too loosely.",
    "- Amend the current GitHub backlog where existing issue wording is too vague or points work at the wrong place.",
    "- Add durable governance so cleanup gains do not decay after publication.",
    "",
    "## Observed repo facts",
    "",
    f"- `npm` scripts in `package.json`: {seed['observed_repo_facts']['npm_script_count']}",
    f"- `check_*.py` scripts: {seed['observed_repo_facts']['checker_scripts']}",
    f"- `test_check_*.py` files: {seed['observed_repo_facts']['checker_pytests']}",
    f"- `m2xx-*` references inside `native/objc3c/src`: {seed['observed_repo_facts']['native_source_m2xx_refs']}",
    f"- tracked `.ll` files: {seed['observed_repo_facts']['tracked_ll_files']}",
    f"- tracked stub `.ll` files: {seed['observed_repo_facts']['tracked_stub_ll_files']}",
    f"- dead prototype compiler surface: `{seed['observed_repo_facts']['dead_prototype_compiler_file']}`",
    "",
    "## Milestones",
    "",
]
for item in milestones:
    summary_lines.append(f"- {item['code']}: {item['title']} ({item['issue_budget']} issues)")
(out / "cleanup_acceleration_program_summary.md").write_text("\n".join(summary_lines) + "\n", encoding="utf-8")

review_lines = [
    "# Cleanup Acceleration Program Review And Improvements",
    "",
    "This local packet is intentionally more corrective and more surgical than the current public backlog.",
    "",
    "## Global design corrections folded in",
    "",
    "- Validation cleanup is not framed as ‘delete all checkers’; it is framed as a migration to subsystem acceptance suites with a small justified static guardrail layer, namespace quarantine, and numerical ratchets.",
    "- Command-surface cleanup is not framed as an arbitrary `10 scripts only` rule; it is framed as a compact public entrypoint surface backed by parameterized internal runners and one explicit orchestration model.",
    "- Product-code decontamination is separated from archival planning history so milestone-coded strings can be removed from implementation files without losing repo memory, and replaced by stable feature-surface identifiers.",
    "- Synthetic IR fixtures are treated as legitimate only when explicitly labeled and fenced; proof/replay claims require provenance and a regeneration path.",
    "- Runtime completion work is made explicit where the current backlog is too vague: selector-pool-backed dispatch, synthesized property accessor IR/runtime behavior, and genuine native-output proof.",
    "- Publication-time backlog realignment is separated from long-term governance so the repo gets both immediate correction and durable anti-regression enforcement.",
    "",
    "## Why this shape is better than the naive alternatives",
    "",
    "- Better than ‘replace every checker with an integration test’: some static policy checks still add value, but thousands of milestone-local checkers do not.",
    "- Better than ‘cut npm scripts to 10’: it preserves a usable public front door while moving complexity behind stable task runners.",
    "- Better than ‘just clean comments later’: native-source residue and synthetic-proof ambiguity actively hide real implementation gaps.",
    "- Better than relying on the current M279/M280 wording alone: those milestones are directionally correct, but they do not yet force the exact missing runtime/codegen behaviors.",
    "- Better than bundling everything under a vague superclean milestone: this packet separates structural cleanup, proof discipline, runtime correction, publication-time issue surgery, and long-term governance.",
    "",
    "## Milestone-by-milestone intent",
    "",
]
for item in milestones:
    review_lines += [
        f"### {item['code']} {item['title'][5:]}",
        "",
        f"- Focus: {item['focus']}",
        f"- Exit gate: {item['exit_gate']}",
        "- Corrections folded in:",
    ] + [f"  - {detail}" for detail in item["improvements"]] + [
        "- Success targets:",
    ] + [f"  - {detail}" for detail in item["targets"]] + [""]
(out / "cleanup_acceleration_program_review.md").write_text("\n".join(review_lines) + "\n", encoding="utf-8")

alter_lines = [
    "# Current GitHub Issue Alteration Guidance",
    "",
    "This file records the recommended edits, narrowings, supersessions, and dependency rewrites that should accompany publication of the cleanup acceleration program.",
    "",
    "## Existing issues that should be amended rather than left vague",
    "",
    "### M277 scaffold-retirement issue",
    "",
    "- `#7399` `[M277][Lane-B][B002] Transitional scaffold retirement inventory - Core feature implementation`",
    "- Recommended change: narrow this issue explicitly to architecture-level inventory and retirement mapping only.",
    "- Reason: the repo now needs dedicated milestones for checker collapse, command-surface reduction, source/proof hygiene, and governance. `#7399` should not become an unfocused dumping ground for all cleanup concerns.",
    "",
    "### M279 object-model runtime issues",
    "",
    "- `#7421` `[M279][Lane-B][B002] Class-metaclass-protocol runtime realization semantics - Core feature implementation`",
    "- Recommended change: amend the body to explicitly require realized method lookup semantics that consume the already-implemented selector pool and define the `objc_msgSend`-equivalent dispatch behavior over the realized object graph.",
    "- `#7425` `[M279][Lane-C][C002] Dispatch table and reflection record lowering - Core feature implementation`",
    "- Recommended change: amend the body to explicitly require lowering for real selector-thunk or method-dispatch paths, not just reflection metadata publication.",
    "- `#7428` `[M279][Lane-D][D002] Live realization lookup and reflection runtime implementation - Core feature implementation`",
    "- Recommended change: amend the body to explicitly require executable message dispatch through the realized class/category/protocol graph, backed by runtime probes rather than metadata-only evidence.",
    "- Relationship to corrective program: these three issues should be amended together with publication of `M316` so the corrective tranche sharpens them rather than duplicating them.",
    "",
    "### M280 property/storage issues",
    "",
    "- `#7434` `[M280][Lane-B][B002] Property synthesis and storage-binding semantics - Core feature implementation`",
    "- Recommended change: amend the body to explicitly require synthesized getter/setter semantics and storage-accessor policy completion, not only descriptor/layout semantics.",
    "- `#7438` `[M280][Lane-C][C002] Synthesized accessor body and ivar-layout lowering - Core feature implementation`",
    "- Recommended change: amend the body to explicitly require real LLVM IR body generation for synthesized getter/setter accessors, closing the current missing-binding failure path in `objc3_ir_emitter.cpp`.",
    "- `#7441` `[M280][Lane-D][D002] Live property ivar accessor and reflection runtime implementation - Core feature implementation`",
    "- Recommended change: amend the body to explicitly require executable synthesized accessor behavior at runtime and coherence with property/ivar reflection.",
    "- Relationship to corrective program: these three issues should be amended together with publication of `M316` so the corrective tranche sharpens them rather than duplicating them.",
    "",
    "### M288 superclean milestone",
    "",
    "- `#7529-#7538` `M288`",
    "- Recommended change: keep `M288` focused on human-facing repo polish, instruction coherence, naming cleanup, and presentation.",
    "- Reason: structural validation collapse, command-surface simplification, source-of-truth cleanup, proof-authenticity enforcement, and anti-noise governance belong in the corrective milestones here, not in a vague ‘superclean’ bucket.",
    "",
    "### M293-M304 future work interaction",
    "",
    "- Future performance, validation, and reporting milestones should be amended on publication to consume the `M313` acceptance harness and validation budgets once those exist.",
    "- Future publishing work should also assume the `M314` public command budget and orchestration model once they exist.",
    "- Reason: publishing more benchmark/reporting work on top of the current checker-heavy and command-heavy shape would lock in the very noise this corrective program is meant to remove.",
    "",
    "## Existing local facts that motivate new issues",
    "",
    "- `package.json` currently exposes `7737` scripts.",
    "- The repo contains `2781` `check_*.py` scripts and `2767` `test_check_*.py` files.",
    "- `native/objc3c/src` currently contains about `3002` `m2xx-*` references across product source and native docs.",
    "- `compiler/objc3c/semantic.py` is dead prototype code and should be retired explicitly.",
    "- Two tracked `.ll` files are intentionally synthetic `ret i32 0` parity fixtures; this is not inherently wrong, but the repo lacks an explicit authenticity/provenance policy that distinguishes synthetic fixtures from genuine replay or proof outputs.",
    "",
    "## Publication and execution order recommendation",
    "",
    "1. Publish `M313-M315` first so validation, workflow, naming, and proof hygiene improve before more large-scale feature work lands.",
    "2. Publish `M317` immediately after or in the same tranche, because backlog amendment and supersession work is a publication-time prerequisite, not a nice-to-have later.",
    "3. Amend `#7399`, `#7421`, `#7425`, `#7428`, `#7434`, `#7438`, and `#7441` together with publication of `M316`.",
    "4. Publish `M318` once the corrective program exists on GitHub so the long-term governance and budget enforcement can refer to concrete milestone codes.",
]
(out / "github_issue_alteration_guidance.md").write_text("\n".join(alter_lines) + "\n", encoding="utf-8")

readme_lines = [
    "# Cleanup Acceleration Program",
    "",
    "This directory contains a local corrective program that should be reviewed before publication to GitHub.",
    "",
    "Files:",
    "- `cleanup_acceleration_program_seed.json`: structured milestone/issue seed.",
    "- `cleanup_acceleration_program_summary.md`: short overview and counts.",
    "- `cleanup_acceleration_program_review.md`: design review and corrections folded in.",
    "- `github_issue_alteration_guidance.md`: explicit amendments, supersessions, and dependency rewrites for current GitHub issues.",
    "- `milestones/`: one markdown file per proposed milestone for human review.",
    "",
    "Program goals:",
    "- shrink validation and command noise with hard targets and ratchets",
    "- remove issue-era contamination from product code and source-of-truth artifacts",
    "- sharpen runtime completion around real dispatch, synthesized accessors, and honest native outputs",
    "- align the existing backlog before further work compounds ambiguity",
    "- add governance so the repo does not drift back into the same patterns",
]
(out / "README.md").write_text("\n".join(readme_lines) + "\n", encoding="utf-8")

for item in milestones:
    lines = [
        f"# {item['title']}",
        "",
        f"- domain: `{item['domain']}`",
        f"- epic: `{item['epic']}`",
        f"- issues: `{item['issue_budget']}`",
        "",
        "## Focus",
        "",
        item["focus"],
        "",
        "## Exit gate",
        "",
        item["exit_gate"],
        "",
        "## Corrections folded in",
        "",
    ] + [f"- {detail}" for detail in item["improvements"]] + [
        "",
        "## Success targets",
        "",
    ] + [f"- {detail}" for detail in item["targets"]] + [
        "",
        "## Shared surfaces",
        "",
    ] + [f"- `{surface}`" for surface in item["shared_surfaces"]] + [
        "",
        "## Issues",
        "",
    ]
    for issue in item["issues"]:
        lines.append(f"- `{issue['code']}` {issue['title']}")
    (ms_dir / f"{item['code']}.md").write_text("\n".join(lines) + "\n", encoding="utf-8")

print(json.dumps({"out": str(out), "milestones": len(milestones), "issues": seed["totals"]["issues"]}, indent=2))
