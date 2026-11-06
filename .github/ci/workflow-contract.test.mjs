import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import test from "node:test";
import * as yaml from "js-yaml";
import {
  validateImplementationGate,
  validateNativeContainer,
  validateNativeGate,
  validateOptionalLLVM,
  validateQuickChecks,
  validateWorkflowContracts,
} from "./workflow-contract.mjs";

const config = JSON.parse(await readFile(new URL("./adaptive-ci.json", import.meta.url), "utf8"));
const workflows = Object.fromEntries(
  await Promise.all(
    ["adaptive-maintenance.yml", ...config.suites.map((suite) => suite.workflow)].map(
      async (name) => [
        name,
        await readFile(new URL(`../workflows/${name}`, import.meta.url), "utf8"),
      ],
    ),
  ),
);
const check = (files = workflows) =>
  validateWorkflowContracts(config, async (name) => {
    if (!(name in files)) throw new Error(`Missing reusable workflow ${name}`);
    return files[name];
  });
const native = await readFile(
  new URL("../workflows/native-assurance.yml", import.meta.url),
  "utf8",
);
const implementation = await readFile(
  new URL("../workflows/implementation-analysis.yml", import.meta.url),
  "utf8",
);
const release = await readFile(
  new URL("../workflows/platform-host-evidence.yml", import.meta.url),
  "utf8",
);
const llvmOptional = await readFile(new URL("../workflows/llvm-assurance.yml", import.meta.url), "utf8");
const toolingQuick = await readFile(new URL("../workflows/task-hygiene.yml", import.meta.url), "utf8");
const documentationQuick = await readFile(new URL("../workflows/spec-quality.yml", import.meta.url), "utf8");

test("quick PR feedback stays separate from optional compiler qualification", () => {
  validateQuickChecks(toolingQuick, documentationQuick);
  validateOptionalLLVM(llvmOptional);
});

for (const trigger of ["pull_request", "push", "workflow_run"]) {
  test(`optional LLVM campaign rejects automatic ${trigger} execution`, () => {
    const workflow = yaml.load(llvmOptional);
    workflow.on[trigger] = null;
    assert.throws(() => validateOptionalLLVM(yaml.dump(workflow)));
  });
}

for (const fault of ["missing PR", "unbounded time", "hidden failure", "cold compiler build", "obsolete feedback"]) {
  test(`quick PR checks reject ${fault}`, () => {
    const workflow = yaml.load(toolingQuick);
    if (fault === "missing PR") delete workflow.on.pull_request;
    if (fault === "unbounded time") workflow.jobs.tooling["timeout-minutes"] = 360;
    if (fault === "hidden failure") workflow.jobs.tooling["continue-on-error"] = true;
    if (fault === "cold compiler build") workflow.jobs.tooling.steps.push({ run: "python -m scripts.objc3c_assurance run" });
    if (fault === "obsolete feedback") workflow.concurrency["cancel-in-progress"] = false;
    assert.throws(() => validateQuickChecks(yaml.dump(workflow), documentationQuick));
  });
}

test("extended native workflow retains its full receipt-verifying aggregate", () => {
  validateNativeGate(native);
});

test("extended implementation analysis retains exact periodic and release gates", () => {
  validateImplementationGate(implementation, release);
});

for (const [name, workflowBefore, workflowAfter, releaseBefore, releaseAfter] of [
  ["cancelled cold build", "  cancel-in-progress: false", "  cancel-in-progress: true"],
  ["missing aggregate", "  gate:", "  renamed:"],
  ["conditional aggregate", "    if: always()\n    needs:", "    if: success()\n    needs:"],
  ["automatic expensive PR", "on:\n", "on:\n  pull_request:\n"],
  ["missing release source", "      source_sha:\n        required: true", "      source_sha:\n        required: false"],
  ["missing evidence", "if-no-files-found: error", "if-no-files-found: ignore"],
  ["floating action", "@3d3c42e5aac5ba805825da76410c181273ba90b1", "@main"],
  ["release dependency", "", "", "    needs: [build, implementation-analysis]", "    needs: build"],
]) {
  test(`implementation workflow rejects ${name}`, () => {
    assert.throws(() => validateImplementationGate(
      implementation.replace(workflowBefore ?? "", workflowAfter ?? ""),
      release.replace(releaseBefore ?? "", releaseAfter ?? ""),
    ));
  });
}

test("native containers admit TSan personality and debugger process controls", async () => {
  const llvm = await readFile(new URL("../workflows/llvm-assurance.yml", import.meta.url), "utf8");
  for (const [text, name] of [
    [native, "native"],
    [workflows["adaptive-closeout.yml"], "compiler-closeout"],
    [llvm, "llvm-assurance"],
  ]) {
    const job = yaml.load(text).jobs[name];
    validateNativeContainer(job);
    assert.throws(() => validateNativeContainer({ ...job, steps: [] }));
    for (const container of [
      "debian:13",
      { image: "debian:13" },
      { image: "debian:13", options: "--privileged" },
    ]) {
      assert.throws(() => validateNativeContainer({ ...job, container }));
    }
  }
});
for (const [name, before, after] of [
  ["missing aggregate", "  gate:", "  renamed:"],
  ["conditional aggregate", "    if: always()\n    needs:", "    if: success()\n    needs:"],
  ["automatic expensive PR", "on:\n", "on:\n  pull_request:\n"],
  ["privileged PR trigger", "on:\n", "on:\n  pull_request_target:\n"],
  ["missing dependency", "    needs: [plan, native]", "    needs: [plan]"],
  ["missing artifact normalized", "if-no-files-found: error", "if-no-files-found: ignore"],
  [
    "missing activated checkout trust",
    'git config --global --add safe.directory "$GITHUB_WORKSPACE"',
    "echo missing-checkout-trust",
  ],
  ["cancelled sibling shard", "      fail-fast: false", "      fail-fast: true"],
  ["unsharded verification", "verify --sharded", "verify"],
  ["unpartitioned execution", '--shard "$NATIVE_SHARD"', ""],
  ["overwriting merged receipts", "merge-multiple: true", "merge-multiple: false"],
  ["floating action", "@3d3c42e5aac5ba805825da76410c181273ba90b1", "@main"],
]) {
  test(`native workflow rejects ${name}`, () => {
    assert.throws(() => validateNativeGate(native.replace(before, after)));
  });
}

test("current cadence resolves to actual jobs and required workload/verification steps", async () => {
  await check();
});
for (const [name, mutate] of [
  [
    "dangling reference",
    (files) => {
      delete files["adaptive-closeout.yml"];
    },
  ],
  [
    "renamed job",
    (files) => {
      files["adaptive-closeout.yml"] = files["adaptive-closeout.yml"].replace(
        "name: compiler-closeout",
        "name: silently-renamed",
      );
    },
  ],
  [
    "missing native command",
    (files) => {
      files["adaptive-closeout.yml"] = files["adaptive-closeout.yml"].replace(
        "Build full native objc3c closeout surface",
        "Tooling mocks only",
      );
    },
  ],
  [
    "mutable source ref",
    (files) => {
      files["adaptive-maintenance.yml"] = files["adaptive-maintenance.yml"].replace(
        "source_sha: ${{ needs.decide.outputs.source_sha }}",
        "source_sha: main",
      );
    },
  ],
  [
    "new cadence",
    (files) => {
      files["adaptive-closeout.yml"] = files["adaptive-closeout.yml"].replace(
        "  workflow_call:",
        "  schedule: []\n  workflow_call:",
      );
    },
  ],
  [
    "conditional workload",
    (files) => {
      files["adaptive-closeout.yml"] = files["adaptive-closeout.yml"].replace(
        "    timeout-minutes: 360",
        "    if: false\n    timeout-minutes: 360",
      );
    },
  ],
  [
    "failure normalized",
    (files) => {
      files["adaptive-closeout.yml"] = files["adaptive-closeout.yml"].replace(
        "    timeout-minutes: 360",
        "    continue-on-error: true\n    timeout-minutes: 360",
      );
    },
  ],
  [
    "missing final verifier",
    (files) => {
      files["adaptive-closeout.yml"] = files["adaptive-closeout.yml"].replace(
        "scripts.objc3c_assurance verify",
        "echo pretending-to-verify",
      );
    },
  ],
  [
    "missing coverage shard",
    (files) => {
      files["adaptive-closeout.yml"] = files["adaptive-closeout.yml"].replace(
        "shard: [core, frontend-fuzz, source-coverage]", "shard: [core, frontend-fuzz]",
      );
    },
  ],
  [
    "duplicate matrix names",
    (files) => {
      files["adaptive-closeout.yml"] = files["adaptive-closeout.yml"].replace(
        "shard: [core, frontend-fuzz, source-coverage]", "shard: [core, core, source-coverage]",
      );
    },
  ],
  [
    "periodic aggregate without prerequisites",
    (files) => {
      files["adaptive-closeout.yml"] = files["adaptive-closeout.yml"].replace(
        "needs: compiler-closeout", "needs: nonexistent",
      );
    },
  ],
]) {
  test(`workflow contract rejects ${name}`, async () => {
    const files = { ...workflows };
    mutate(files);
    await assert.rejects(check(files));
  });
}
