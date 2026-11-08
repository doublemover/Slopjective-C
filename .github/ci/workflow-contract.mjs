import { readFile, readdir } from "node:fs/promises";
import { fileURLToPath } from "node:url";
import { resolve, join } from "node:path";
import * as yaml from "js-yaml";
import { validateConfig } from "./adaptive-ci.mjs";

const dependencyActions = ["linux-host-tools", "python-tools"].map(
  (name) => `./.github/actions/${name}`,
);
const pinnedAction = (uses) =>
  /^[^@]+@[a-f0-9]{40}$/.test(uses) || dependencyActions.includes(uses);

export function validateDependencyRouting(workflows) {
  for (const name of [
    "native-assurance.yml",
    "implementation-analysis.yml",
    "llvm-assurance.yml",
    "adaptive-closeout.yml",
  ]) {
    const workflow = yaml.load(workflows[name]);
    const setup = Object.values(workflow.jobs)
      .flatMap((job) => job.steps ?? [])
      .filter((step) => step.name === "Pinned host tools");
    if (setup.length !== 1 || setup[0].uses !== "./.github/actions/linux-host-tools")
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
  const gate = workflow.jobs?.gate;
  if (
    gate?.name !== "Extended Linux native assurance gate" ||
    gate.if !== "always()" ||
    JSON.stringify(gate.needs?.toSorted()) !== JSON.stringify(["native", "plan"]) ||
    gate["continue-on-error"]
  ) {
    throw new Error("Native aggregate must always evaluate both required prerequisites");
  }
  if (
    !(gate.steps ?? []).some(
      (step) => step.run?.includes("scripts.objc3c_assurance verify") && step.if === "always()",
    )
  ) {
    throw new Error("Native aggregate must validate exact-revision receipt contents");
  }
  const execute = workflow.jobs.native.steps.find((step) =>
    step.run?.includes("scripts.objc3c_assurance run"),
  );
  validateNativeContainer(workflow.jobs.native);
  const nativeJob = workflow.jobs.native;
  if (
    nativeJob.strategy?.["fail-fast"] !== false ||
    nativeJob.strategy?.matrix?.shard !== "${{ fromJSON(needs.plan.outputs.shards) }}" ||
    nativeJob.env?.NATIVE_SHARD !== "${{ matrix.shard }}" ||
    !execute?.run.includes('--shard "$NATIVE_SHARD"') ||
    !(gate.steps ?? []).some(
      (step) =>
        step.run?.includes("scripts.objc3c_assurance verify --sharded") && step.if === "always()",
    ) ||
    !(gate.steps ?? []).some(
      (step) =>
        step.uses?.startsWith("actions/download-artifact@") &&
        step.with?.pattern?.startsWith("native-results-") &&
        step.with?.["merge-multiple"] === true,
    ) ||
    !(nativeJob.steps ?? []).some(
      (step) =>
        step.if === "always()" &&
        step.run?.includes("scripts.objc3c_assurance publish") &&
        step.run.includes('--destination "out/native-assurance/evidence/$NATIVE_SHARD"'),
    )
  ) {
    throw new Error(
      "Native compiler profiles require independent execution and a complete sharded receipt gate",
    );
  }
  if (!execute?.run.includes('git config --global --add safe.directory "$GITHUB_WORKSPACE"')) {
    throw new Error(
      "Activated container execution must trust only its exact checked-out workspace",
    );
  }
  for (const id of ["plan", "native", "gate"]) {
    const job = workflow.jobs?.[id];
    if (!job || job["continue-on-error"])
      throw new Error(`Missing or failure-normalizing native job ${id}`);
    for (const step of job.steps ?? []) {
      if (step["continue-on-error"]) throw new Error("Native failures must remain failures");
      if (step.uses && !pinnedAction(step.uses))
        throw new Error("Native actions must be SHA-pinned");
      if (
        step.uses?.startsWith("actions/upload-artifact@") &&
        (step.with?.["if-no-files-found"] !== "error" || !(step.with?.["retention-days"] <= 7))
      ) {
        throw new Error("Mandatory native evidence must fail closed with bounded retention");
      }
    }
  }
}

export function validateNativeRelease(nativeText, releaseText, workflows = {}) {
  validateNativeGate(nativeText);
  const native = yaml.load(nativeText);
  const release = yaml.load(releaseText);
  const call = release.jobs?.build;
  const install = release.jobs?.install;
  const archive =
    native.jobs?.native?.steps?.filter(
      (step) =>
        step.with?.name === "objc3c-linux-sdk" && step.uses?.startsWith("actions/upload-artifact@"),
    ) ?? [];
  if (
    !Object.hasOwn(native.on, "workflow_call") ||
    JSON.stringify(Object.keys(release.on ?? {})) !== JSON.stringify(["workflow_dispatch"]) ||
    release.permissions?.contents !== "read" ||
    call?.uses !== "./.github/workflows/native-assurance.yml" ||
    call.if ||
    call["continue-on-error"] ||
    JSON.stringify(install?.needs?.toSorted()) !==
      JSON.stringify(["build", "implementation-analysis"]) ||
    archive.length !== 1 ||
    archive[0].if !== "success() && matrix.shard == 'core'" ||
    !(install.steps ?? []).some(
      (step) =>
        step.uses?.startsWith("actions/download-artifact@") &&
        step.with?.name === "objc3c-linux-sdk",
    )
  ) {
    throw new Error(
      "SDK release must manually reuse passing native qualification and its exact archive",
    );
  }
  for (const [name, text] of Object.entries(workflows)) {
    if (name === "platform-host-evidence.yml") continue;
    for (const job of Object.values(yaml.load(text).jobs ?? {})) {
      if (/\/(?:native-assurance|platform-host-evidence)\.yml(?:@|$)/.test(job.uses ?? ""))
        throw new Error("Only the manual SDK release may call extended native qualification");
    }
  }
}

export function validateImplementationGate(text, releaseText) {
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
    Object.hasOwn(events, "pull_request_target") ||
    events.pull_request?.paths ||
    events.pull_request?.["paths-ignore"] ||
    workflow.permissions?.contents !== "read"
  ) {
    throw new Error("Extended analysis must remain manual/periodic with exact release sources");
  }
  const gate = workflow.jobs?.gate;
  if (
    gate?.name !== "Extended Linux implementation analysis gate" ||
    gate.if !== "always()" ||
    JSON.stringify(gate.needs?.toSorted()) !== JSON.stringify(["analysis", "plan"]) ||
    gate["continue-on-error"] ||
    !(gate.steps ?? []).some(
      (step) =>
        step.run?.includes("scripts.objc3c_implementation_assurance verify") &&
        step.if === "always()",
    )
  ) {
    throw new Error(
      "Implementation aggregate must always verify both exact-revision prerequisites",
    );
  }
  const analysis = workflow.jobs?.analysis;
  const execute = (analysis?.steps ?? []).find((step) =>
    step.run?.includes("scripts.objc3c_implementation_assurance run"),
  );
  if (
    analysis?.if !== "needs.plan.outputs.analysis == 'true'" ||
    analysis?.["continue-on-error"] ||
    !execute?.run.includes('git config --global --add safe.directory "$GITHUB_WORKSPACE"')
  ) {
    throw new Error(
      "Implementation analysis must execute the selected trusted checkout without normalizing failure",
    );
  }
  for (const id of ["plan", "analysis", "gate"]) {
    const job = workflow.jobs?.[id];
    if (!job || job["continue-on-error"])
      throw new Error(`Missing or failure-normalizing implementation job ${id}`);
    for (const step of job.steps ?? []) {
      if (step["continue-on-error"])
        throw new Error("Implementation analysis failures must remain failures");
      if (step.uses && !pinnedAction(step.uses))
        throw new Error("Implementation analysis actions must be SHA-pinned");
      if (
        step.uses?.startsWith("actions/upload-artifact@") &&
        (step.with?.["if-no-files-found"] !== "error" || !(step.with?.["retention-days"] <= 7))
      ) {
        throw new Error("Implementation evidence must fail closed with bounded retention");
      }
    }
  }
  const release = yaml.load(releaseText);
  const call = release.jobs?.["implementation-analysis"];
  const installNeeds = release.jobs?.install?.needs;
  if (
    call?.uses !== "./.github/workflows/implementation-analysis.yml" ||
    call.with?.source_sha !== "${{ github.sha }}" ||
    !Array.isArray(installNeeds) ||
    JSON.stringify(installNeeds.toSorted()) !== JSON.stringify(["build", "implementation-analysis"])
  ) {
    throw new Error("Linux SDK installation must depend on exact-SHA implementation analysis");
  }
}

export function validateOptionalLLVM(text) {
  const workflow = yaml.load(text);
  if (
    JSON.stringify(Object.keys(workflow.on ?? {})) !== JSON.stringify(["workflow_dispatch"]) ||
    workflow.permissions?.contents !== "read"
  )
    throw new Error("Extended LLVM campaigns must be explicitly requested");
  const job = workflow.jobs?.["llvm-assurance"];
  validateNativeContainer(job ?? {});
  if (
    !Number.isInteger(job?.["timeout-minutes"]) ||
    job["timeout-minutes"] > 360 ||
    job["timeout-minutes"] <= 0 ||
    job["continue-on-error"] ||
    !(job.steps ?? []).some((step) =>
      step.run?.includes("scripts.objc3c_toolchain.llvm_campaign"),
    ) ||
    !(job.steps ?? []).some((step) =>
      step.run?.includes("scripts.objc3c_toolchain.semantic_oracles"),
    )
  )
    throw new Error("Optional LLVM campaigns must retain their bounded real owners");
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
      !Number.isInteger(job?.["timeout-minutes"]) ||
      job["timeout-minutes"] <= 0 ||
      job["timeout-minutes"] > 15 ||
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
        "Quick PR checks must be automatic, bounded and independent of extended compiler builds",
      );
  }
}

function concreteJobNames(id, job) {
  const name = job.name ?? id;
  if (!name.includes("${{")) return [name];
  const matrix = job.strategy?.matrix;
  if (!matrix || typeof matrix !== "object" || Array.isArray(matrix))
    throw new Error("Expected a bounded static workload matrix");
  let names = [name];
  for (const [key, values] of Object.entries(matrix)) {
    if (
      !/^[a-zA-Z_][a-zA-Z0-9_]*$/.test(key) ||
      !Array.isArray(values) ||
      values.length === 0 ||
      values.length > 16 ||
      values.some((value) => typeof value !== "string" || value.includes("${{"))
    )
      throw new Error("Invalid static workload matrix");
    names = names.flatMap((value) =>
      values.map((item) => value.replaceAll(`\${{ matrix.${key} }}`, item)),
    );
    if (names.length > 16) throw new Error("Workload matrix exceeds its reviewed bound");
  }
  if (names.some((value) => value.includes("${{")) || new Set(names).size !== names.length)
    throw new Error("Unresolved or duplicate workload matrix names");
  return names;
}

export async function validateWorkflowContracts(config, readWorkflow) {
  validateConfig(config);
  const dispatcher = yaml.load(await readWorkflow("adaptive-maintenance.yml"));
  for (const suite of config.suites) {
    const call = dispatcher.jobs?.[`run-${suite.id}`];
    if (
      call?.uses !== `./.github/workflows/${suite.workflow}` ||
      call.with?.source_sha !== "${{ needs.decide.outputs.source_sha }}"
    ) {
      throw new Error(`Missing immutable-source caller for ${suite.id}`);
    }
    const workflow = yaml.load(await readWorkflow(suite.workflow));
    const event = workflow.on?.workflow_call;
    if (
      !event?.inputs?.source_sha?.required ||
      event.inputs.source_sha.type !== "string" ||
      Object.keys(workflow.on).some((name) => name !== "workflow_call")
    ) {
      throw new Error(
        `Workload ${suite.workflow} must remain reusable-only with a required source SHA`,
      );
    }
    for (const expected of suite.expectedJobs) {
      const jobs = Object.entries(workflow.jobs ?? {}).filter(([id, job]) =>
        concreteJobNames(id, job).includes(expected.name),
      );
      if (jobs.length !== 1) throw new Error(`Missing or duplicate expected job ${expected.name}`);
      const [id, job] = jobs[0];
      const receiptGate =
        job.if === "always()" &&
        (job.steps ?? []).some(
          (step) =>
            step.run?.includes("scripts.objc3c_assurance verify --sharded") &&
            step.if === "always()",
        );
      if ((job.steps ?? []).some((step) => step.run?.includes("scripts.objc3c_assurance run"))) {
        validateNativeContainer(job);
      }
      if (
        (Object.hasOwn(job, "if") && !receiptGate) ||
        job["continue-on-error"] ||
        !(job["timeout-minutes"] > 0 && job["timeout-minutes"] <= 360)
      ) {
        throw new Error(
          `Workload ${expected.name} must execute with a bounded failure-preserving job`,
        );
      }
      for (const pattern of expected.requiredSteps) {
        const steps = (job.steps ?? []).filter((step) => new RegExp(pattern).test(step.name ?? ""));
        if (
          steps.length !== 1 ||
          !steps[0].run ||
          (steps[0].if && !(receiptGate && steps[0].if === "always()")) ||
          steps[0]["continue-on-error"]
        ) {
          throw new Error(`Missing or conditional workload step ${pattern}`);
        }
      }
      if (!receiptGate) {
        const gate = workflow.jobs.gate;
        const needs = Array.isArray(gate?.needs) ? gate.needs : [gate?.needs];
        if (
          job.strategy?.["fail-fast"] !== false ||
          job.env?.NATIVE_SHARD !== "${{ matrix.shard }}" ||
          !(job.steps ?? []).some(
            (step) =>
              step.run?.includes('run --periodic-sha "$SOURCE_SHA"') &&
              step.run.includes('--shard "$NATIVE_SHARD"'),
          ) ||
          gate?.if !== "always()" ||
          !needs.includes(id) ||
          !(gate.steps ?? []).some(
            (step) =>
              step.if === "always()" &&
              step.run?.includes("scripts.objc3c_assurance verify --sharded"),
          )
        ) {
          throw new Error(
            `Workload ${expected.name} lacks independent profiles and a complete receipt verifier`,
          );
        }
      }
    }
    const actual = Object.entries(workflow.jobs ?? {}).flatMap(([id, job]) =>
      concreteJobNames(id, job),
    );
    if (
      JSON.stringify(actual.toSorted()) !==
      JSON.stringify(suite.expectedJobs.map((job) => job.name).toSorted())
    )
      throw new Error("Configured workload inventory differs from the concrete native jobs");
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
