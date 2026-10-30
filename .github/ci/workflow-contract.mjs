import { readFile } from "node:fs/promises";
import { fileURLToPath } from "node:url";
import { resolve, join } from "node:path";
import * as yaml from "js-yaml";
import { validateConfig } from "./adaptive-ci.mjs";

export function validateNativeGate(text) {
  const workflow = yaml.load(text);
  const events = workflow.on ?? {};
  if (
    !Object.hasOwn(events, "pull_request") ||
    !Object.hasOwn(events, "push") ||
    Object.hasOwn(events, "pull_request_target") ||
    events.pull_request?.paths ||
    events.pull_request?.["paths-ignore"] ||
    workflow.permissions?.contents !== "read"
  ) {
    throw new Error("Native gate must cover every unprivileged PR with read-only contents");
  }
  const gate = workflow.jobs?.gate;
  if (
    gate?.name !== "Linux native assurance gate" ||
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
      if (step.uses && !/^[^@]+@[a-f0-9]{40}$/.test(step.uses))
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
      const jobs = Object.entries(workflow.jobs ?? {}).filter(
        ([id, job]) => (job.name ?? id) === expected.name,
      );
      if (jobs.length !== 1) throw new Error(`Missing or duplicate expected job ${expected.name}`);
      const [, job] = jobs[0];
      if (
        Object.hasOwn(job, "if") ||
        job["continue-on-error"] ||
        !(job["timeout-minutes"] > 0 && job["timeout-minutes"] <= 360)
      ) {
        throw new Error(
          `Workload ${expected.name} must execute with a bounded failure-preserving job`,
        );
      }
      for (const pattern of expected.requiredSteps) {
        const steps = (job.steps ?? []).filter((step) => new RegExp(pattern).test(step.name ?? ""));
        if (steps.length !== 1 || !steps[0].run || steps[0].if || steps[0]["continue-on-error"]) {
          throw new Error(`Missing or conditional workload step ${pattern}`);
        }
      }
      if (
        !(job.steps ?? []).some(
          (step) => step.run?.includes("scripts.objc3c_assurance verify") && step.if === "always()",
        )
      ) {
        throw new Error(`Workload ${expected.name} lacks an always-running receipt verifier`);
      }
    }
  }
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) {
  const root = resolve(fileURLToPath(new URL("../workflows/", import.meta.url)));
  const config = JSON.parse(await readFile(new URL("./adaptive-ci.json", import.meta.url), "utf8"));
  await validateWorkflowContracts(config, (name) => readFile(join(root, name), "utf8"));
  validateNativeGate(await readFile(join(root, "native-assurance.yml"), "utf8"));
}
