import { readFile, readdir } from "node:fs/promises";
import { fileURLToPath } from "node:url";
import { resolve, join } from "node:path";
import * as yaml from "js-yaml";
import { validateConfig } from "./adaptive-ci.mjs";

const dependencyActions = ["linux-host-tools", "python-tools"].map(
  (name) => `./.github/actions/${name}`,
);
const previewArchiveCache = "./.github/actions/linux-preview-archive-cache";
const pinnedAction = (uses) =>
  /^[^@]+@[a-f0-9]{40}$/.test(uses) || dependencyActions.includes(uses) ||
  uses === previewArchiveCache;

export function validateDependencyRouting(workflows) {
  for (const name of [
    "native-assurance.yml",
    "implementation-analysis.yml",
    "llvm-assurance.yml",
  ]) {
    const workflow = yaml.load(workflows[name]);
    const owners =
      name === "native-assurance.yml"
        ? [
            "diagnostic-native",
            "product-producer",
            "product-consumers",
            "independent-native",
            "coverage-producer",
            "coverage-partitions",
          ]
        : Object.entries(workflow.jobs ?? {})
            .filter(([, job]) =>
              (job.steps ?? []).some((step) => step.name === "Pinned host tools"),
            )
            .map(([id]) => id);
    if (
      !owners.length ||
      owners.some(
        (id) =>
          (workflow.jobs?.[id]?.steps ?? []).filter(
            (step) => step.uses === "./.github/actions/linux-host-tools",
          ).length !== 1,
      )
    )
      throw new Error(`${name} must reuse the shared pinned dependency setup`);
  }
  for (const name of ["task-hygiene.yml", "spec-quality.yml", "planning-lint.yml"]) {
    const workflow = yaml.load(workflows[name]);
    const steps = Object.values(workflow.jobs).flatMap((job) => job.steps ?? []);
    if (!steps.some((step) => step.uses === "./.github/actions/python-tools"))
      throw new Error(`${name} must reuse Python dependencies`);
  }
  for (const [name, text] of Object.entries(workflows)) {
    const steps = Object.values(yaml.load(text).jobs ?? {}).flatMap((job) => job.steps ?? []);
    if (
      steps.some(
        (step) => step.uses?.startsWith("actions/setup-node@") && step.with?.cache !== "npm",
      )
    )
      throw new Error(`${name} must reuse npm downloads`);
  }
}

export function validateDependencyActions(actions) {
  for (const name of dependencyActions) {
    if (!actions[name]) throw new Error(`Missing dependency action ${name}`);
    const action = yaml.load(actions[name]);
    if (action.runs?.using !== "composite") throw new Error("Dependency setup must be composite");
    const steps = action.runs.steps ?? [];
    for (const step of steps) {
      if (step["continue-on-error"] || (step.uses && !pinnedAction(step.uses)))
        throw new Error("Dependency actions must preserve failures and pin external actions");
    }
    const admit = steps.findIndex((step) => step.run?.includes("dependency_cache admit"));
    const provision = steps.findIndex((step) =>
      /assurance.bootstrap|uv.*sync/.test(step.run ?? ""),
    );
    const seal = steps.findIndex((step) => step.run?.includes("dependency_cache seal"));
    const saves = steps.flatMap((step, index) =>
      step.uses?.startsWith("actions/cache/save@") ? [index] : [],
    );
    const restores = steps.flatMap((step, index) =>
      step.uses?.startsWith("actions/cache/restore@") ? [index] : [],
    );
    if (
      !restores.length ||
      !saves.length ||
      !restores.every((index) => index < admit) ||
      !(admit < provision && provision <= seal) ||
      !saves.every((index) => index > seal)
    )
      throw new Error(
        "Restore, admission, provisioning, sealing and publication must stay ordered",
      );
  }
}

export function validatePreviewArchiveCache(text) {
  const action = yaml.load(text);
  const steps = action.runs?.steps ?? [];
  const [coordinates, restore] = steps;
  if (
    action.runs?.using !== "composite" || steps.length !== 2 ||
    steps.some((step) => step["continue-on-error"]) ||
    coordinates.id !== "coordinates" ||
    !coordinates.run?.includes("published_archive(manifest, host)['sha256']") ||
    !coordinates.run.includes("checksum + '.tar.zst'") ||
    !/^actions\/cache\/restore@[0-9a-f]{40}$/.test(restore.uses) ||
    restore.id !== "restore" || restore.if || restore.with?.["restore-keys"] ||
    restore.with?.path !== "${{ steps.coordinates.outputs.archive }}" ||
    restore.with?.key !== "${{ steps.coordinates.outputs.key }}" ||
    action.outputs?.key?.value !== restore.with.key ||
    action.outputs?.archive?.value !== restore.with.path ||
    action.outputs?.["cache-hit"]?.value !== "${{ steps.restore.outputs.cache-hit }}"
  )
    throw new Error("Preview cache must restore exact pinned archives");
}

export function validatePreviewArchiveWriter(text, job) {
  const steps = yaml.load(text).jobs?.[job]?.steps ?? [];
  const restore = steps.findIndex((step) => step.id === "preview-cache" && step.uses === previewArchiveCache);
  const install = steps.findIndex((step) => step.run?.includes('--archive-cache "$PREVIEW_CACHE"'));
  const save = steps.findIndex((step) => step.uses?.startsWith("actions/cache/save@"));
  const writer = steps[save];
  if (!(0 <= restore && restore < install && install < save) ||
      !/^actions\/cache\/save@[0-9a-f]{40}$/.test(writer.uses) ||
      writer.if !== "github.event_name == 'workflow_dispatch' && steps.preview-cache.outputs.cache-hit != 'true'" ||
      writer.with?.path !== "${{ steps.preview-cache.outputs.archive }}" ||
      writer.with?.key !== "${{ steps.preview-cache.outputs.key }}") {
    throw new Error("Preview cache must save admitted archives only on explicit dispatch");
  }
}

export function validateNativePR(text) {
  const workflow = yaml.load(text);
  const gate = workflow.jobs?.["native-pr"];
  const steps = gate?.steps ?? [];
  if (
    Object.keys(workflow.on ?? {}).join() !== "pull_request" ||
    workflow.on.pull_request !== null ||
    Object.keys(workflow.permissions ?? {}).join() !== "contents" ||
    workflow.permissions.contents !== "read" ||
    Object.keys(workflow.jobs ?? {}).join() !== "native-pr" ||
    !gate || gate.if || gate.needs || gate.permissions || gate["continue-on-error"] ||
    steps.some((step) => step["continue-on-error"] || (step.uses && !pinnedAction(step.uses)))
  )
    throw new Error("Native PR gate must cover every PR on one runner with pinned, read-only execution");

  const checkout = steps.find((step) => step.uses?.startsWith("actions/checkout@"));
  const previewManifest = steps.find((step) => step.id === "preview-manifest");
  const previewCache = steps.find((step) => step.uses === previewArchiveCache);
  const previewInstall = steps.find((step) => step.run?.includes("-m scripts.objc3c_assurance.preview_download"));
  const execute = steps.find((step) => step.id === "run");
  const upload = steps.find((step) => step.uses?.startsWith("actions/upload-artifact@"));
  const download = steps.find((step) => step.uses?.startsWith("actions/download-artifact@"));
  const verify = steps.find((step) => step.run?.includes("--verify-evidence out/native-pr-evidence"));
  if (
    checkout?.with?.["persist-credentials"] !== false ||
    checkout.with["fetch-depth"] !== 0 ||
    !steps.some((step) => step.uses === "./.github/actions/linux-host-tools") ||
    !previewManifest?.run?.includes('git show "$PR_BASE:docs/releases/preview-manifest.json"') ||
    steps.some((step) => step.uses?.startsWith("actions/cache/save@")) ||
    previewCache?.with?.manifest !== "${{ steps.preview-manifest.outputs.path }}" ||
    previewCache?.if !== "steps.preview-manifest.outputs.path != ''" ||
    previewInstall?.if !== previewCache?.if ||
    previewInstall?.env?.PREVIEW_CACHE !== "${{ steps.preview-cache.outputs.directory }}" ||
    !previewInstall?.run.includes('--archive-cache "$PREVIEW_CACHE"') ||
    !execute?.run?.includes("-m scripts.objc3c_assurance.pr_native") ||
    execute.if ||
    !execute.run.includes('--head "$PR_HEAD" --base "$PR_BASE" --tested "$TESTED_SHA"') ||
    !upload?.if?.includes("always()") ||
    upload.with?.["if-no-files-found"] !== "error" ||
    !["github.sha", "github.run_id", "github.run_attempt"].every((value) =>
      upload.with.name?.includes(value),
    ) ||
    !(steps.indexOf(execute) < steps.indexOf(upload) && steps.indexOf(upload) < steps.indexOf(download) &&
      steps.indexOf(download) < steps.indexOf(verify)) ||
    download?.if || download?.with?.name !== upload.with.name || download.with.path !== "out/native-pr-evidence" ||
    !verify || verify.if || !verify.run.includes('-m scripts.objc3c_assurance.pr_native') ||
    !verify.run.includes('--head "$PR_HEAD" --base "$PR_BASE" --tested "$TESTED_SHA"') ||
    !verify.run.includes('--execution "${{ steps.run.outcome }}"')
  )
    throw new Error(
      "Native PR gate must execute and retain evidence for the exact tested revision",
    );
}

export function validateNativeContainer(job) {
  if (
    job.container?.image !== "debian:13" ||
    job.container?.options !== "--security-opt seccomp=unconfined"
  ) {
    throw new Error("Native sanitizer/debugger container must permit its process layout controls");
  }
  if (
    !(job.steps ?? []).some(
      (step) =>
        step.run?.includes("tests/tooling/fixtures/native_container/personality.c") &&
        step.run.includes("out/native-container-control") &&
        !step.if &&
        !step["continue-on-error"],
    )
  ) {
    throw new Error("Native container must execute its real process layout preflight");
  }
}

const nativeOutcomes = {
  PLAN_RESULT: "plan",
  PRODUCER_RESULT: "product-producer",
  CONSUMERS_RESULT: "product-consumers",
  ASAN_PRODUCER_RESULT: "asan-foundation-producer",
  INSTRUMENTED_CONSUMERS_RESULT: "instrumented-product-consumers",
  FUZZ_RESULT: "independent-native",
  COVERAGE_PRODUCER_RESULT: "coverage-producer",
  COVERAGE_PARTITIONS_RESULT: "coverage-partitions",
  COVERAGE_RESULT: "coverage-aggregate",
};
const releaseOutcomes = {
  INSTALL_RESULT: "archive-install",
  INSTALL_AGGREGATE_RESULT: "archive-install-aggregate",
  IMPLEMENTATION_ANALYSIS_RESULT: "implementation-analysis",
};

function validatesNativeOutcomes(step) {
  if (!step?.run || step.if || step["continue-on-error"] ||
      step.env?.INSTALL_REQUIRED !== "${{ inputs.archive_install }}") return false;
  const required = step.run.match(/^results=\(([^)]+)\)$/m)?.[1].trim().split(/\s+/);
  const release = step.run.match(/^if \[\[ "\$INSTALL_REQUIRED" == true \]\]; then\n +results\+=\(([^)]+)\)\nfi$/m)?.[1].trim().split(/\s+/);
  return JSON.stringify(required) === JSON.stringify(Object.keys(nativeOutcomes)) &&
    JSON.stringify(release) === JSON.stringify(Object.keys(releaseOutcomes)) &&
    Object.entries({ ...nativeOutcomes, ...releaseOutcomes }).every(
      ([name, job]) => step.env[name] === `\${{ needs.${job}.result }}`,
    ) &&
    step.run.includes('for name in "${results[@]}"; do') &&
    step.run.includes('if [[ "${!name}" != success ]]; then') &&
    step.run.includes("failed=1") &&
    step.run.includes('[[ "$failed" == 0 ]]');
}

export function validateNativeGate(text) {
  const workflow = yaml.load(text);
  const events = workflow.on ?? {};
  if (
    !Object.hasOwn(events, "workflow_dispatch") ||
    !Object.hasOwn(events, "schedule") ||
    Object.hasOwn(events, "pull_request") ||
    Object.hasOwn(events, "push") ||
    Object.keys(events).some(
      (name) => !["workflow_dispatch", "schedule", "workflow_call"].includes(name),
    ) ||
    Object.hasOwn(events, "pull_request_target") ||
    events.pull_request?.paths ||
    events.pull_request?.["paths-ignore"] ||
    workflow.permissions?.contents !== "read"
  ) {
    throw new Error(
      "Extended native qualification must remain manual/periodic with read-only contents",
    );
  }
  const diagnosis = events.workflow_dispatch?.inputs?.diagnostic_core_only;
  if (
    diagnosis?.type !== "boolean" ||
    diagnosis.default !== false ||
    events.workflow_call?.inputs?.diagnostic_core_only ||
    workflow["run-name"] !==
      "${{ inputs.diagnostic_core_only && 'Linux core diagnosis (partial surface)' || 'Extended Linux native assurance' }}"
  )
    throw new Error("Core diagnosis must be explicit, partial, and unavailable to release callers");

  const jobs = workflow.jobs ?? {};
  const gate = jobs.gate;
  const requiredGateNeeds = [
    "archive-install",
    "archive-install-aggregate",
    "asan-foundation-producer",
    "coverage-aggregate",
    "coverage-partitions",
    "coverage-producer",
    "implementation-analysis",
    "independent-native",
    "instrumented-product-consumers",
    "plan",
    "product-consumers",
    "product-producer",
  ];
  if (
    gate?.name !== "Extended Linux native assurance gate" ||
    gate.if !== "!cancelled() && !inputs.diagnostic_core_only" ||
    JSON.stringify(gate.needs?.toSorted()) !== JSON.stringify(requiredGateNeeds) ||
    gate["continue-on-error"]
  )
    throw new Error("Native aggregate must evaluate every required prerequisite on active runs");

  const diagnostic = jobs["diagnostic-native"];
  const producer = jobs["product-producer"];
  const consumers = jobs["product-consumers"];
  const asanProducer = jobs["asan-foundation-producer"];
  const instrumentedConsumers = jobs["instrumented-product-consumers"];
  const independent = jobs["independent-native"];
  const coverageProducer = jobs["coverage-producer"];
  const coveragePartitions = jobs["coverage-partitions"];
  const coverageAggregate = jobs["coverage-aggregate"];
  const retainedAggregate = (jobs["diagnostic-coverage-aggregate"]?.steps ?? []).find((step) =>
    step.run?.includes("scripts.objc3c_assurance.coverage_matrix aggregate"),
  );
  if (!retainedAggregate?.run.trimEnd().endsWith('exit "$status"'))
    throw new Error("Retained coverage diagnosis must preserve the aggregate exit status");
  const transport = jobs["diagnostic-transport"];
  const foundationBundle = (asanProducer?.steps ?? []).find(
    (step) => step.name === "Preserve exact sanitizer compiler bundle",
  );
  const foundationDiagnostics = (asanProducer?.steps ?? []).find(
    (step) => step.name === "Preserve sanitizer compiler diagnostics",
  );
  if (
    foundationBundle?.if ||
    !foundationBundle?.with?.path?.includes("out/instrumented-foundation/profile.tar.zst") ||
    foundationBundle.with?.["if-no-files-found"] !== "error" ||
    foundationDiagnostics?.if !== "always() && steps.asan-foundation-run.outcome != 'skipped'" ||
    !foundationDiagnostics?.with?.name?.startsWith("asan-foundation-diagnostics-") ||
    !foundationDiagnostics?.with?.path?.includes("producer/producer.json") ||
    foundationDiagnostics.with.path.includes("profile.tar.zst") ||
    foundationDiagnostics.with?.["if-no-files-found"] !== "error"
  )
    throw new Error(
      "Sanitizer producer must publish success bytes and always retain failure evidence",
    );
  const fullOwnerCondition =
    "!inputs.diagnostic_core_only && inputs.diagnostic_transport_run == '' && (github.event_name == 'schedule' || github.event_name == 'workflow_dispatch')";
  for (const job of [producer, asanProducer, independent, coverageProducer]) {
    const selection = (job?.steps ?? []).find((step) =>
      step.run?.includes("scripts.objc3c_assurance plan"),
    );
    if (job?.needs || job?.if !== fullOwnerCondition || selection?.if ||
        !selection?.run.includes('--plan out/native-assurance/selection/plan.json --periodic-sha "$GITHUB_SHA"') ||
        !selection.run.includes('--expect-host "$OBJC3C_REQUESTED_HOST"') ||
        !selection.run.includes('git config --global --add safe.directory "$GITHUB_WORKSPACE"'))
      throw new Error("Independent Linux producers must select the exact periodic revision on their own runner");
  }
  if (
    diagnostic?.if !==
      "needs.plan.outputs.native == 'true' && inputs.diagnostic_core_only && inputs.diagnostic_transport_run == '' && inputs.diagnostic_coverage_run == ''" ||
    events.workflow_dispatch.inputs.diagnostic_transport_run?.default !== "" ||
    events.workflow_dispatch.inputs.diagnostic_transport_artifact?.default !== "" ||
    transport?.if !== "inputs.diagnostic_core_only && inputs.diagnostic_transport_run != ''" ||
    events.workflow_call?.inputs?.diagnostic_transport_run ||
    events.workflow_call?.inputs?.diagnostic_transport_artifact ||
    !usesHostedDeadline(transport)
  )
    throw new Error(
      "Artifact transport must be explicit, separate from compiler execution and use the hosted deadline",
    );

  const productGroups = (consumers?.strategy?.matrix?.include ?? []).map((entry) => entry.group);
  const instrumentedGroups = instrumentedConsumers?.strategy?.matrix?.group ?? [];
  const consumerRun = (consumers?.steps ?? []).find((step) =>
    step.run?.includes("scripts.objc3c_assurance.release_matrix run"),
  );
  const fuzzRun = (independent?.steps ?? []).find((step) =>
    step.run?.includes("scripts.objc3c_assurance run"),
  );
  const fuzzPublish = (independent?.steps ?? []).find((step) =>
    step.run?.includes("scripts.objc3c_assurance publish"),
  );
  const fuzzEvidence = (independent?.steps ?? []).find(
    (step) => step.name === "Preserve fuzz evidence",
  );
  const coverageRun = (coveragePartitions?.steps ?? []).find((step) =>
    step.run?.includes("scripts.objc3c_assurance.coverage_matrix partition"),
  );
  if (
    consumers?.strategy?.["fail-fast"] !== true ||
    JSON.stringify(productGroups) !==
      JSON.stringify([
        "foundation",
        "runtime",
        "frontend",
        "installed-sanitizers",
        "swift",
        "semantic",
      ]) ||
    JSON.stringify(consumers.needs?.toSorted()) !== JSON.stringify(["plan", "product-producer"]) ||
    !consumers.if?.includes("needs.product-producer.result == 'success'") ||
    consumers.env?.RELEASE_GROUP !== "${{ matrix.group }}" ||
    !consumerRun?.run.includes('--group "$RELEASE_GROUP"') ||
    instrumentedConsumers?.strategy?.["fail-fast"] !== true ||
    JSON.stringify(instrumentedGroups) !==
      JSON.stringify([
        ...Array.from({ length: 10 }, (_, index) => `asan-products-partition-${index}`),
        "runtime-schedules",
      ]) ||
    JSON.stringify(instrumentedConsumers?.needs?.toSorted()) !==
      JSON.stringify(["asan-foundation-producer", "plan", "product-producer"]) ||
    !instrumentedConsumers?.if?.includes("needs.asan-foundation-producer.result == 'success'") ||
    instrumentedConsumers?.env?.RELEASE_GROUP !== "${{ matrix.group }}" ||
    !(instrumentedConsumers?.steps ?? []).some((step) =>
      step.run?.includes("scripts.objc3c_assurance.instrumented_foundation restore"),
    ) ||
    !(instrumentedConsumers?.steps ?? []).some((step) =>
      step.run?.includes('--group "$RELEASE_GROUP"'),
    ) ||
    coveragePartitions?.strategy?.["fail-fast"] !== true ||
    JSON.stringify(coveragePartitions?.strategy?.matrix?.partition) !==
      JSON.stringify([0, 1, 2, 3, 4, 5, 6, 7]) ||
    JSON.stringify(coveragePartitions.needs?.toSorted()) !==
      JSON.stringify(["coverage-producer", "plan"]) ||
    !coveragePartitions.if?.includes("needs.coverage-producer.result == 'success'") ||
    coveragePartitions.env?.COVERAGE_PARTITION !== "${{ matrix.partition }}" ||
    !coverageRun?.run.includes('--index "$COVERAGE_PARTITION"') ||
    !fuzzRun?.run.includes("--shard frontend-fuzz") ||
    fuzzPublish?.if !== "always() && steps.fuzz-run.outcome != 'skipped'" ||
    fuzzEvidence?.if !== "always() && steps.fuzz-publish.outcome == 'success'"
  )
    throw new Error("Native owners must retain their complete independent execution matrices");

  for (const [job, runId, evidenceName] of [
    [producer, "product-producer-run", "Preserve producer evidence"],
    [consumers, "product-consumer-run", "Preserve consumer evidence"],
    [asanProducer, "asan-foundation-run", "Preserve sanitizer compiler diagnostics"],
    [instrumentedConsumers, "instrumented-consumer-run", "Preserve instrumented consumer evidence"],
    [coverageProducer, "coverage-producer-run", "Preserve producer coverage evidence"],
    [coveragePartitions, "coverage-partition-run", "Preserve coverage partition evidence"],
  ]) {
    const steps = job?.steps ?? [];
    const owner = steps.find((step) => step.id === runId);
    const evidence = steps.find((step) => step.name === evidenceName);
    if (
      !owner?.run ||
      evidence?.if !== `always() && steps.${runId}.outcome != 'skipped'` ||
      !evidence?.uses?.startsWith("actions/upload-artifact@")
    )
      throw new Error("Native failure evidence must follow an invoked typed owner");
  }

  if (
    JSON.stringify(coverageAggregate?.needs?.toSorted()) !==
      JSON.stringify(["coverage-partitions", "coverage-producer", "plan"]) ||
    !(coverageAggregate?.steps ?? []).some(
      (step) =>
        step.run?.includes("scripts.objc3c_assurance.coverage_matrix aggregate") &&
        step.run.includes("--shard source-coverage") &&
        step.run.includes("evidence/source-coverage"),
    ) ||
    !(coverageAggregate?.steps ?? []).some(
      (step) =>
        step.uses?.startsWith("actions/download-artifact@") &&
        step.with?.pattern?.startsWith("coverage-partition-") &&
        step.with?.["merge-multiple"] === true,
    )
  )
    throw new Error("Native aggregate jobs must prove complete disjoint owner unions");

  const outcomeCheck = gate.steps?.[0];
  const verifier = (gate?.steps ?? []).find((step) =>
    step.run?.includes("scripts.objc3c_assurance verify --sharded"),
  );
  if (
    !verifier ||
    verifier.if ||
    verifier["continue-on-error"] ||
    !validatesNativeOutcomes(outcomeCheck) ||
    !verifier.run.includes("scripts.objc3c_assurance.release_matrix aggregate") ||
    !verifier.run.includes("--output out/native-assurance/release-evidence/core") ||
    verifier.run.indexOf("release_matrix aggregate") > verifier.run.indexOf("verify --sharded") ||
    !(gate.steps ?? []).some(
      (step) =>
        step.uses?.startsWith("actions/download-artifact@") &&
        step.with?.pattern?.startsWith("native-partial-") &&
        step.with?.path === "out/release-matrix/partials" &&
        step.with?.["merge-multiple"] === true,
    ) ||
    verifier.env?.ASSURANCE_OUTCOMES !== '{"plan":"success","native":"success"}' ||
    !(gate.steps ?? []).some(
      (step) =>
        step.uses?.startsWith("actions/download-artifact@") &&
        step.with?.pattern?.startsWith("native-results-") &&
        step.with?.["merge-multiple"] === true,
    )
  )
    throw new Error("Native terminal gate must reject missing owners and incomplete receipts");

  const executionJobs = [
    diagnostic,
    producer,
    consumers,
    asanProducer,
    instrumentedConsumers,
    independent,
    coverageProducer,
    coveragePartitions,
  ];
  if (
    executionJobs.some(
      (job) =>
        !(job?.steps ?? []).some((step) =>
          step.run?.includes('git config --global --add safe.directory "$GITHUB_WORKSPACE"'),
        ),
    )
  )
    throw new Error(
      "Activated container execution must trust only its exact checked-out workspace",
    );
  for (const job of [diagnostic, producer, consumers, asanProducer, instrumentedConsumers])
    validateNativeContainer(job ?? {});
  for (const job of executionJobs) {
    if (
      job?.container?.image !== "debian:13" ||
      job?.container?.options !== "--security-opt seccomp=unconfined"
    )
      throw new Error("Native execution must retain its bounded Debian container");
  }

  for (const id of [
    "diagnostic-coverage-aggregate",
    "diagnostic-transport",
    "plan",
    "diagnostic-native",
    "product-producer",
    "product-consumers",
    "asan-foundation-producer",
    "instrumented-product-consumers",
    "independent-native",
    "coverage-producer",
    "coverage-partitions",
    "coverage-aggregate",
    "archive-install",
    "gate",
  ]) {
    const job = jobs[id];
    if (!job || job["continue-on-error"])
      throw new Error(`Missing or failure-normalizing native job ${id}`);
    for (const step of job.steps ?? []) {
      if (step["continue-on-error"]) throw new Error("Native failures must remain failures");
      if (step.uses && !pinnedAction(step.uses))
        throw new Error("Native actions must be SHA-pinned");
      if (
        step.uses?.startsWith("actions/download-artifact@") &&
        Object.hasOwn(step.with ?? {}, "merge-multiple") &&
        step.with["merge-multiple"] !== true
      )
        throw new Error("Native artifact unions must never overwrite sibling receipts");
      if (
        step.uses?.startsWith("actions/upload-artifact@") &&
        (step.with?.["if-no-files-found"] !== "error" || !(step.with?.["retention-days"] <= 7))
      )
        throw new Error("Mandatory native evidence must fail closed with bounded retention");
    }
  }
}

export function validateNativeRelease(nativeText, releaseText, workflows = {}) {
  validateNativeGate(nativeText);
  const native = yaml.load(nativeText);
  const release = yaml.load(releaseText);
  const call = release.jobs?.build;
  const install = native.jobs?.["archive-install"];
  const nativeGate = native.jobs?.gate;
  const installAggregate = native.jobs?.["archive-install-aggregate"];
  const implementationCall = native.jobs?.["implementation-analysis"];
  const sanitizerProducer = native.jobs?.["asan-foundation-producer"];
  const analysisFoundation = (sanitizerProducer?.steps ?? []).find(
    (step) => step.id === "analysis-foundation",
  );
  const analysisArchive = (sanitizerProducer?.steps ?? []).find(
    (step) => step.name === "Preserve the exact implementation analysis bundle",
  );
  const releaseGate = release.jobs?.release;
  const sdkArtifact =
    "objc3c-linux-sdk-${{ inputs.linux_host || 'linux-x86_64' }}-${{ github.run_id }}-${{ github.run_attempt }}";
  const hostInput = "${{ inputs.linux_host }}";
  const archive =
    native.jobs?.["product-producer"]?.steps?.filter(
      (step) =>
        step.with?.name === sdkArtifact && step.uses?.startsWith("actions/upload-artifact@"),
    ) ?? [];
  if (
    !Object.hasOwn(native.on, "workflow_call") ||
    !native.concurrency?.group?.includes("${{ github.sha }}") ||
    native.concurrency?.["cancel-in-progress"] !== false ||
    JSON.stringify(Object.keys(release.on ?? {})) !== JSON.stringify(["workflow_dispatch"]) ||
    release.permissions?.contents !== "read" ||
    call?.uses !== "./.github/workflows/native-assurance.yml" ||
    call.if ||
    call.with?.linux_host !== hostInput ||
    call.with?.archive_install !== true ||
    native.on.workflow_call.inputs?.archive_install?.type !== "boolean" ||
    native.on.workflow_call.inputs.archive_install.default !== false ||
    Object.hasOwn(call.with ?? {}, "diagnostic_core_only") ||
    call["continue-on-error"] ||
    releaseGate?.needs !== "build" ||
    releaseGate.if !== "!cancelled()" ||
    releaseGate["continue-on-error"] ||
    !releaseGate.steps?.some(
      (step) =>
        step.env?.SDK_RESULT === "${{ needs.build.result }}" &&
        !Object.hasOwn(step.env, "ANALYSIS_RESULT") &&
        step.run?.includes('[[ "$SDK_RESULT" != success ]]') &&
        step.run?.includes("exit 1") &&
        !step["continue-on-error"],
    ) ||
    JSON.stringify(install?.needs?.toSorted()) !== JSON.stringify(["plan", "product-producer"]) ||
    install.if !== "inputs.archive_install && needs.plan.outputs.native == 'true'" ||
    install["runs-on"] !== native.jobs["product-producer"]["runs-on"] ||
    install.strategy?.["fail-fast"] !== true ||
    JSON.stringify(install.strategy?.matrix?.distribution) !==
      JSON.stringify(["debian:13", "ubuntu:26.04"]) ||
    JSON.stringify(install.strategy?.matrix?.scope) !==
      JSON.stringify(["smoke", "package-build", "package-surface", "package-modules",
        "async", "isolation", "features", "carriers"]) ||
    installAggregate?.needs !== "archive-install" ||
    installAggregate?.if !== "inputs.archive_install" ||
    installAggregate?.strategy?.["fail-fast"] !== true ||
    installAggregate?.["continue-on-error"] ||
    JSON.stringify(installAggregate?.strategy?.matrix?.include) !== JSON.stringify([
      { distribution: "debian:13", receipt: "debian-13" },
      { distribution: "ubuntu:26.04", receipt: "ubuntu-26.04" },
    ]) ||
    !installAggregate.steps?.some(
      (step) => step.uses?.startsWith("actions/download-artifact@") &&
        step.with?.pattern === "objc3c-linux-install-scope-${{ inputs.linux_host || 'linux-x86_64' }}-${{ github.run_id }}-${{ github.run_attempt }}-${{ matrix.receipt }}-*",
    ) ||
    !installAggregate.steps?.some(
      (step) => !step.if && !step["continue-on-error"] &&
        step.run?.includes("scripts.objc3c_toolchain.release_install --aggregate"),
    ) ||
    !install.steps?.some(
      (step) =>
        step.env?.DISTRIBUTION === "${{ matrix.distribution }}" &&
        step.env?.SCOPE === "${{ matrix.scope }}" &&
        step.run?.includes('--scope "$SCOPE"'),
    ) ||
    !nativeGate.needs?.includes("archive-install") ||
    implementationCall?.needs !== "asan-foundation-producer" ||
    implementationCall.if !==
      "inputs.archive_install && needs.asan-foundation-producer.outputs.analysis_archive_sha256 != ''" ||
    implementationCall.uses !== "./.github/workflows/implementation-analysis.yml" ||
    implementationCall.with?.source_sha !== "${{ github.sha }}" ||
    implementationCall.with?.linux_host !== hostInput ||
    implementationCall.with?.foundation_archive_sha256 !==
      "${{ needs.asan-foundation-producer.outputs.analysis_archive_sha256 }}" ||
    implementationCall["continue-on-error"] ||
    sanitizerProducer?.outputs?.analysis_archive_sha256 !==
      "${{ steps.analysis-foundation.outputs.archive_sha256 }}" ||
    analysisFoundation?.if !== "inputs.archive_install" ||
    !analysisFoundation.run?.includes("scripts.objc3c_implementation_assurance foundation") ||
    !analysisFoundation.run.includes(
      "--sanitizer-producer out/instrumented-foundation/producer",
    ) ||
    analysisArchive?.if !== "inputs.archive_install" ||
    !analysisArchive.uses?.startsWith("actions/upload-artifact@") ||
    analysisArchive.with?.name !==
      "implementation-foundation-${{ github.sha }}-${{ inputs.linux_host || 'linux-x86_64' }}-${{ github.run_id }}-${{ github.run_attempt }}" ||
    analysisArchive.with?.path !== "out/implementation-assurance/foundation-product/" ||
    analysisArchive.with?.["if-no-files-found"] !== "error" ||
    !nativeGate.needs?.includes("implementation-analysis") ||
    archive.length !== 1 ||
    archive[0].if ||
    !(install.steps ?? []).some(
      (step) =>
        step.uses?.startsWith("actions/download-artifact@") && step.with?.name === sdkArtifact,
    )
  ) {
    throw new Error(
      "SDK release must manually reuse passing native qualification and its exact archive",
    );
  }
  for (const [name, text] of Object.entries(workflows)) {
    if (name === "platform-host-evidence.yml") continue;
    const caller = yaml.load(text);
    for (const [id, job] of Object.entries(caller.jobs ?? {})) {
      if (!/\/(?:adaptive-closeout|native-assurance|platform-host-evidence)\.yml(?:@|$)/.test(job.uses ?? "")) continue;
      if (name === "adaptive-closeout.yml" && id === "native" &&
          Object.keys(caller.on ?? {}).join() === "workflow_call" && job.needs === "source" &&
          job.uses === "./.github/workflows/native-assurance.yml" && job.with?.archive_install === false) continue;
      if (name === "adaptive-maintenance.yml" && id === "run-closeout" &&
          job.uses === "./.github/workflows/adaptive-closeout.yml" &&
          job.if === "needs.decide.outputs.run_closeout == 'true'") continue;
      throw new Error("Only manual SDK release and admitted adaptive closeout may call extended native qualification");
    }
  }
}

export function validateImplementationGate(text) {
  const workflow = yaml.load(text);
  if (workflow.concurrency?.["cancel-in-progress"] !== false || !workflow.concurrency?.group) {
    throw new Error(
      "Implementation analysis must finish its active cold build and queue the newest revision",
    );
  }
  const events = workflow.on ?? {};
  if (
    Object.hasOwn(events, "pull_request") ||
    Object.hasOwn(events, "push") ||
    Object.keys(events).some(
      (name) => !["workflow_dispatch", "schedule", "workflow_call"].includes(name),
    ) ||
    !Object.hasOwn(events, "schedule") ||
    !Object.hasOwn(events, "workflow_dispatch") ||
    !events.workflow_call?.inputs?.source_sha?.required ||
    events.workflow_call.inputs.source_sha.type !== "string" ||
    events.workflow_call.inputs?.foundation_archive_sha256?.type !== "string" ||
    events.workflow_call.inputs.foundation_archive_sha256.default !== "" ||
    Object.hasOwn(events, "pull_request_target") ||
    events.pull_request?.paths ||
    events.pull_request?.["paths-ignore"] ||
    workflow.permissions?.contents !== "read"
  ) {
    throw new Error("Extended analysis must remain manual/periodic with exact release sources");
  }
  const gate = workflow.jobs?.gate;
  const outcomes = gate?.steps?.[0];
  const verify = (gate?.steps ?? []).find((step) =>
    step.run?.includes("scripts.objc3c_implementation_assurance verify"),
  );
  if (
    gate?.name !== "Extended Linux implementation analysis gate" ||
    gate.if !== "!cancelled()" ||
    JSON.stringify(gate.needs?.toSorted()) !== JSON.stringify(["analysis", "foundation", "plan"]) ||
    gate["continue-on-error"] ||
    !verify || verify.if || verify["continue-on-error"] ||
    !outcomes?.run || outcomes.if || outcomes["continue-on-error"] ||
    outcomes.env?.PLAN_RESULT !== "${{ needs.plan.result }}" ||
    outcomes.env?.REQUIRED !== "${{ needs.plan.outputs.analysis }}" ||
    outcomes.env?.ANALYSIS_RESULT !== "${{ needs.analysis.result }}" ||
    outcomes.env?.FOUNDATION_RESULT !== "${{ needs.foundation.result }}" ||
    outcomes.env?.SHARED_ARCHIVE !== "${{ inputs.foundation_archive_sha256 }}" ||
    !outcomes.run.includes('[[ "$PLAN_RESULT" != success ]]') ||
    !outcomes.run.includes('[[ "$REQUIRED" == true && "$ANALYSIS_RESULT" != success ]]') ||
    !outcomes.run.includes(`[[ "$REQUIRED" == true && "$SHARED_ARCHIVE" == '' && "$FOUNDATION_RESULT" != success ]]`) ||
    !outcomes.run.includes("exit 1") ||
    !verify.env?.ASSURANCE_OUTCOMES?.includes(
      '"foundation":"${{ inputs.foundation_archive_sha256 != \'\' && \'success\' || needs.foundation.result }}"',
    ) ||
    !verify.env.ASSURANCE_OUTCOMES.includes(
      '"foundation_archive":"${{ inputs.foundation_archive_sha256 || needs.foundation.outputs.archive_sha256 }}"',
    )
  ) {
    throw new Error(
      "Implementation aggregate must always verify the revision, producer hash and partition results",
    );
  }
  const foundation = workflow.jobs?.foundation;
  const foundationStep = (foundation?.steps ?? []).find((step) => step.id === "foundation-run");
  const foundationUpload = (foundation?.steps ?? []).find((step) =>
    step.name === "Preserve the matching compiler and compilation graph",
  );
  if (
    foundation?.needs !== "plan" ||
    foundation.if !==
      "needs.plan.outputs.analysis == 'true' && inputs.foundation_archive_sha256 == ''" ||
    foundation.outputs?.archive_sha256 !== "${{ steps.foundation-run.outputs.archive_sha256 }}" ||
    !foundationStep?.run?.includes("scripts.objc3c_implementation_assurance foundation") ||
    !foundationUpload?.uses?.startsWith("actions/upload-artifact@") ||
    !foundationUpload.with?.name?.includes("-${{ github.run_id }}-${{ github.run_attempt }}") ||
    foundationUpload.with?.["retention-days"] !== 1 || foundationUpload.if
  ) {
    throw new Error("Implementation analysis must publish one exact same-attempt compiler foundation");
  }
  const analysis = workflow.jobs?.analysis;
  const execute = (analysis?.steps ?? []).find((step) =>
    step.run?.includes("scripts.objc3c_implementation_assurance run"),
  );
  const evidence = (analysis?.steps ?? []).find(
    (step) => step.name === "Preserve analysis evidence",
  );
  const foundationDownload = (analysis?.steps ?? []).find((step) =>
    step.uses?.startsWith("actions/download-artifact@") &&
    step.with?.path === "out/implementation-assurance/foundation-input",
  );
  if (
    JSON.stringify(analysis?.needs?.toSorted()) !== JSON.stringify(["foundation", "plan"]) ||
    analysis.if !==
      "${{ !cancelled() && needs.plan.outputs.analysis == 'true' && (needs.foundation.result == 'success' || inputs.foundation_archive_sha256 != '') }}" ||
    analysis.strategy?.["fail-fast"] !== true ||
    analysis.strategy?.matrix?.partition !== "${{ fromJSON(needs.plan.outputs.partitions) }}" ||
    analysis?.["continue-on-error"] ||
    execute?.id !== "analysis-run" ||
    !execute?.run.includes('git config --global --add safe.directory "$GITHUB_WORKSPACE"') ||
    !execute?.run.includes("--foundation-bundle out/implementation-assurance/foundation-input") ||
    !execute.run.includes('--foundation-archive-sha256 "$ANALYSIS_FOUNDATION_SHA256"') ||
    analysis.env?.ANALYSIS_FOUNDATION_SHA256 !==
      "${{ inputs.foundation_archive_sha256 || needs.foundation.outputs.archive_sha256 }}" ||
    foundationDownload?.with?.name !== foundationUpload.with.name ||
    evidence?.if !== "always() && steps.analysis-run.outcome != 'skipped'"
  ) {
    throw new Error(
      "Implementation analysis must execute the selected trusted checkout without normalizing failure",
    );
  }
  for (const id of ["plan", "foundation", "analysis", "gate"]) {
    const job = workflow.jobs?.[id];
    if (!job || job["continue-on-error"])
      throw new Error(`Missing or failure-normalizing implementation job ${id}`);
    for (const step of job.steps ?? []) {
      if (step["continue-on-error"])
        throw new Error("Implementation analysis failures must remain failures");
      if (step.uses && !pinnedAction(step.uses))
        throw new Error("Implementation analysis actions must be SHA-pinned");
      const failureDiagnostics = id === "foundation" &&
        step.name === "Preserve compiler producer diagnostics" && step.if === "failure()";
      if (
        step.uses?.startsWith("actions/upload-artifact@") &&
        ((!failureDiagnostics && step.with?.["if-no-files-found"] !== "error") ||
          !(step.with?.["retention-days"] > 0 && step.with?.["retention-days"] <= 7))
      ) {
        throw new Error("Implementation evidence must fail closed with bounded retention");
      }
    }
  }
}

export function validateOptionalLLVM(text) {
  const workflow = yaml.load(text);
  const cacheMode = workflow.on?.workflow_dispatch?.inputs?.dependency_cache;
  const assuranceMode = workflow.on?.workflow_dispatch?.inputs?.assurance_mode;
  const graphMode = workflow.on?.workflow_dispatch?.inputs?.verify_compiler_graph;
  const nativeCompiler = "inputs.dependency_cache != 'any' && inputs.dependency_cache != 'warm'";
  const job = workflow.jobs?.["llvm-assurance"];
  const installed = workflow.jobs?.["installed-preview"];
  if (
    cacheMode?.default !== "compiler" ||
    JSON.stringify(cacheMode.options) !== JSON.stringify(["compiler", "any", "warm"]) ||
    assuranceMode?.default !== "development" ||
    graphMode?.type !== "boolean" ||
    graphMode?.default !== false ||
    JSON.stringify(assuranceMode.options) !==
      JSON.stringify([
        "development",
        "installed-preview",
        "source-feedback",
        "compiler-instrumentation",
      ]) ||
    job?.if !== `${nativeCompiler} && inputs.assurance_mode != 'installed-preview'` ||
    installed?.if !== `${nativeCompiler} && inputs.assurance_mode == 'installed-preview'` ||
    workflow.jobs?.["dependency-cache"]?.if !==
      "inputs.dependency_cache == 'any' || inputs.dependency_cache == 'warm'"
  )
    throw new Error(
      "The default compiler campaign and dependency-only modes must be complementary",
    );
  if (
    JSON.stringify(Object.keys(workflow.on ?? {})) !== JSON.stringify(["workflow_dispatch"]) ||
    workflow.permissions?.contents !== "read"
  )
    throw new Error("Extended LLVM campaigns must be explicitly requested");
  validateNativeContainer(job ?? {});
  const scopedStep = (owner, required, mode) =>
    (owner.steps ?? []).some(
      (step) =>
        step.if === (mode && `inputs.assurance_mode == '${mode}'`) &&
        !step["continue-on-error"] &&
        required.every((command) => step.run?.includes(command)),
    );
  if (
    [job, installed].some((owner) => !usesHostedDeadline(owner) || owner["continue-on-error"]) ||
    !scopedStep(job, ["scripts.objc3c_toolchain.llvm_campaign"], "development") ||
    !scopedStep(job, ["scripts.objc3c_toolchain.semantic_oracles"], "development") ||
    !scopedStep(
      job,
      [
        "--instrumentation-profile compiler-source-coverage",
        "scripts.objc3c_toolchain.compiler_source_coverage",
      ],
      "source-feedback",
    ) ||
    !scopedStep(
      job,
      ["scripts.objc3c_toolchain.semantic_source_feedback", "--coverage-qualification"],
      "source-feedback",
    ) ||
    !scopedStep(
      job,
      [
        "--instrumentation-profile compiler-asan-ubsan",
        "scripts.objc3c_toolchain.compiler_instrumentation",
      ],
      "compiler-instrumentation",
    ) ||
    !scopedStep(installed, [
      "scripts.objc3c_assurance.preview_download",
      "scripts.objc3c_toolchain.llvm_installed_ir",
      "--seal-sha256",
    ])
  )
    throw new Error("Optional LLVM campaigns must retain their real owners and hosted deadline");
}

function usesHostedDeadline(job) {
  return Boolean(job) && (!Object.hasOwn(job, "timeout-minutes") || job["timeout-minutes"] === 360);
}

export function validateQuickChecks(toolingText, documentationText) {
  for (const [text, id] of [
    [toolingText, "tooling"],
    [documentationText, "documentation"],
  ]) {
    const workflow = yaml.load(text);
    const events = workflow.on ?? {};
    const job = workflow.jobs?.[id];
    if (
      !Object.hasOwn(events, "pull_request") ||
      !Object.hasOwn(events, "push") ||
      events.pull_request?.paths ||
      events.pull_request?.["paths-ignore"] ||
      workflow.permissions?.contents !== "read" ||
      workflow.concurrency?.["cancel-in-progress"] !== true ||
      !usesHostedDeadline(job) ||
      job.if ||
      job["continue-on-error"] ||
      (job.steps ?? []).some(
        (step) =>
          step["continue-on-error"] ||
          /scripts\.objc3c_(assurance run|implementation_assurance run|toolchain\.(llvm_campaign|codec_fuzz|compiler_source_coverage))/.test(
            step.run ?? "",
          ),
      )
    )
      throw new Error(
        "Quick PR checks must be automatic and independent of extended compiler builds, with the hosted deadline",
      );
  }
}

export async function validateWorkflowContracts(config, readWorkflow) {
  validateConfig(config);
  const dispatcher = yaml.load(await readWorkflow("adaptive-maintenance.yml"));
  const slots = (dispatcher.on?.schedule ?? []).map(({ cron }) => String(cron));
  const suiteIds = config.suites.map((suite) => suite.id);
  if (
    JSON.stringify(Object.keys(dispatcher.on ?? {}).sort()) !== JSON.stringify(["schedule", "workflow_dispatch"]) ||
    JSON.stringify(slots.toSorted()) !== JSON.stringify(config.suites.map(({ cron }) => cron).toSorted()) ||
    JSON.stringify(dispatcher.on?.workflow_dispatch?.inputs?.suite?.options) !== JSON.stringify(suiteIds) ||
    dispatcher.jobs?.decide?.outputs?.source_sha !== "${{ steps.cadence.outputs.source_sha }}" ||
    dispatcher["concurrency"]?.["cancel-in-progress"] !== false
  ) throw new Error("Adaptive dispatcher differs from its reviewed cadence, identity or budget");

  for (const suite of config.suites) {
    const call = dispatcher.jobs?.[`run-${suite.id}`];
    if (
      call?.uses !== `./.github/workflows/${suite.workflow}` || call.needs !== "decide" ||
      call.if !== `needs.decide.outputs.run_${suite.id.replaceAll("-", "_")} == 'true'` ||
      call.with?.source_sha !== "${{ needs.decide.outputs.source_sha }}"
    ) throw new Error(`Missing immutable-source caller for ${suite.id}`);
    const workflow = yaml.load(await readWorkflow(suite.workflow));
    const event = workflow.on?.workflow_call;
    if (
      !event?.inputs?.source_sha?.required || event.inputs.source_sha.type !== "string" ||
      Object.keys(workflow.on).join() !== "workflow_call" ||
      workflow.permissions?.contents !== "read" || workflow.permissions?.actions !== "read"
    ) throw new Error(`Workload ${suite.workflow} must remain reusable-only with a required source SHA`);

    const { source, native, gate } = workflow.jobs ?? {};
    if (native?.uses !== "./.github/workflows/native-assurance.yml" || native.needs !== "source" ||
        Object.hasOwn(native, "if") || native["continue-on-error"] ||
        native.with?.linux_host !== "${{ inputs.linux_host }}" || native.with?.archive_install !== false) {
      throw new Error("Periodic closeout must use the canonical native workflow after source admission");
    }
    const canonical = await readWorkflow("native-assurance.yml");
    const nativeWorkflow = yaml.load(canonical);
    const nativeEvent = nativeWorkflow.on?.workflow_call;
    if (nativeEvent?.inputs?.linux_host?.type !== "string" || nativeEvent.inputs.archive_install?.type !== "boolean")
      throw new Error("Periodic native target must accept the canonical reusable inputs");
    for (const [id, command] of [["plan", "plan"], ["gate", "verify --sharded"]]) {
      if (!(nativeWorkflow.jobs?.[id]?.steps ?? []).some(({ run }) =>
        run?.includes(`scripts.objc3c_assurance ${command}`) &&
        run.includes('"$GITHUB_EVENT_NAME" == schedule') && run.includes('--periodic-sha "$GITHUB_SHA"') &&
        run.includes('"${periodic[@]}"'))) {
        throw new Error("Canonical periodic selection and verification must use the immutable dispatcher SHA");
      }
    }
    validateNativeGate(canonical);

    const guard = source?.steps?.[0];
    const verifier = gate?.steps?.[0];
    if (Object.hasOwn(source ?? {}, "if") || guard?.env?.SOURCE_SHA !== "${{ inputs.source_sha }}" ||
        !guard.run?.includes('"$SOURCE_SHA" != "$GITHUB_SHA"') || !guard.run?.includes("exit 1") ||
        gate?.if !== "!cancelled()" || JSON.stringify(gate.needs?.toSorted()) !== JSON.stringify(["native", "source"]) ||
        verifier?.env?.SOURCE_RESULT !== "${{ needs.source.result }}" ||
        verifier?.env?.NATIVE_RESULT !== "${{ needs.native.result }}" ||
        verifier?.env?.SOURCE_SHA !== "${{ inputs.source_sha }}" ||
        !verifier.run?.includes('"$SOURCE_RESULT" != success || "$NATIVE_RESULT" != success || "$SOURCE_SHA" != "$GITHUB_SHA"') ||
        !verifier.run?.includes("exit 1")) {
      throw new Error("Periodic gate must require the exact source and every native outcome");
    }
    const localJobs = [source, gate];
    if (Object.keys(workflow.jobs).length !== 3 || localJobs.some((job) =>
      job["continue-on-error"] || !usesHostedDeadline(job) ||
      job.steps.length !== 1 || job.steps.some((step) => !step.run || step.if || step["continue-on-error"])
    ) || JSON.stringify(localJobs.map(({ name }) => name).toSorted()) !==
         JSON.stringify(suite.expectedJobs.map(({ name }) => name).toSorted())) {
      throw new Error("Periodic workflow must preserve its required job inventory and hosted deadlines");
    }
    for (const expected of suite.expectedJobs) {
      const job = localJobs.find(({ name }) => name === expected.name);
      if (!expected.requiredSteps.length || expected.requiredSteps.some((pattern) =>
        job.steps.filter(({ name }) => new RegExp(pattern).test(name)).length !== 1))
        throw new Error(`Periodic workflow omitted required steps for ${expected.name}`);
    }
  }
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) {
  const root = resolve(fileURLToPath(new URL("../workflows/", import.meta.url)));
  validateDependencyActions(
    Object.fromEntries(
      await Promise.all(
        dependencyActions.map(async (name) => [
          name,
          await readFile(resolve(root, "../..", name, "action.yml"), "utf8"),
        ]),
      ),
    ),
  );
  validatePreviewArchiveCache(await readFile(resolve(root, "../..", previewArchiveCache, "action.yml"), "utf8"));
  const config = JSON.parse(await readFile(new URL("./adaptive-ci.json", import.meta.url), "utf8"));
  await validateWorkflowContracts(config, (name) => readFile(join(root, name), "utf8"));
  const allWorkflows = Object.fromEntries(
    await Promise.all(
      (await readdir(root))
        .filter((name) => name.endsWith(".yml"))
        .map(async (name) => [name, await readFile(join(root, name), "utf8")]),
    ),
  );
  validateDependencyRouting(allWorkflows);
  validateNativePR(allWorkflows["native-pr.yml"]);
  validatePreviewArchiveWriter(allWorkflows["linux-preview-calibration.yml"], "calibration");
  validatePreviewArchiveWriter(allWorkflows["llvm-assurance.yml"], "installed-preview");
  validateNativeRelease(
    allWorkflows["native-assurance.yml"],
    allWorkflows["platform-host-evidence.yml"],
    allWorkflows,
  );
  validateImplementationGate(
    await readFile(join(root, "implementation-analysis.yml"), "utf8"),
    await readFile(join(root, "platform-host-evidence.yml"), "utf8"),
  );
  validateOptionalLLVM(await readFile(join(root, "llvm-assurance.yml"), "utf8"));
  validateQuickChecks(
    await readFile(join(root, "task-hygiene.yml"), "utf8"),
    await readFile(join(root, "spec-quality.yml"), "utf8"),
  );
}
