import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import test from "node:test";
import * as yaml from "js-yaml";
import {
  validateImplementationGate,
  validateDependencyActions,
  validateDependencyRouting,
  validateNativeContainer,
  validateNativeGate,
  validateNativeRelease,
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
const llvmOptional = await readFile(
  new URL("../workflows/llvm-assurance.yml", import.meta.url),
  "utf8",
);
const toolingQuick = await readFile(
  new URL("../workflows/task-hygiene.yml", import.meta.url),
  "utf8",
);
const documentationQuick = await readFile(
  new URL("../workflows/spec-quality.yml", import.meta.url),
  "utf8",
);

const dependencyActions = Object.fromEntries(
  await Promise.all(
    ["linux-host-tools", "python-tools"].map(async (name) => [
      `./.github/actions/${name}`,
      await readFile(new URL(`../actions/${name}/action.yml`, import.meta.url), "utf8"),
    ]),
  ),
);
const dependencyWorkflows = {
  ...workflows,
  "native-assurance.yml": native,
  "implementation-analysis.yml": implementation,
  "llvm-assurance.yml": llvmOptional,
  "task-hygiene.yml": toolingQuick,
  "spec-quality.yml": documentationQuick,
};
for (const name of ["planning-lint.yml", "adaptive-policy-checks.yml"])
  dependencyWorkflows[name] = await readFile(
    new URL(`../workflows/${name}`, import.meta.url),
    "utf8",
  );
test("every native, quick and planning owner reuses pinned dependency caches", () => {
  validateDependencyRouting(dependencyWorkflows);
});
for (const [name, text] of Object.entries(dependencyWorkflows)) {
  if (!/\.\/\.github\/actions\/(linux-host-tools|python-tools)|cache: npm/.test(text)) continue;
  test(`${name} rejects bypassing dependency reuse`, () => {
    assert.throws(() =>
      validateDependencyRouting({
        ...dependencyWorkflows,
        [name]: text
          .replaceAll("./.github/actions/linux-host-tools", "uncached-setup")
          .replaceAll("./.github/actions/python-tools", "uncached-setup")
          .replaceAll("cache: npm", "cache: ''"),
      }),
    );
  });
}
test("shared dependency setup admits restored bytes before execution and publishes misses early", () => {
  validateDependencyActions(dependencyActions);
});
for (const name of Object.keys(dependencyActions)) {
  for (const fault of [
    "missing",
    "floating",
    "hidden failure",
    "admit after execution",
    "save before seal",
  ]) {
    test(`${name} rejects ${fault}`, () => {
      const actions = { ...dependencyActions };
      const action = yaml.load(actions[name]);
      const steps = action.runs.steps;
      if (fault === "missing") delete actions[name];
      else {
        if (fault === "floating")
          steps.find((step) => step.uses).uses = "actions/cache/restore@main";
        if (fault === "hidden failure") steps[0]["continue-on-error"] = true;
        if (fault === "admit after execution") {
          const index = steps.findIndex((step) => step.run?.includes("dependency_cache admit"));
          steps.push(...steps.splice(index, 1));
        }
        if (fault === "save before seal") {
          const index = steps.findIndex((step) => step.uses?.startsWith("actions/cache/save@"));
          steps.unshift(...steps.splice(index, 1));
        }
        actions[name] = yaml.dump(action);
      }
      assert.throws(() => validateDependencyActions(actions));
    });
  }
}

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

for (const fault of ["ambiguous default", "default skipped", "cache overlaps compiler"]) {
  test(`manual LLVM dispatch rejects ${fault}`, () => {
    const workflow = yaml.load(llvmOptional);
    if (fault === "ambiguous default")
      workflow.on.workflow_dispatch.inputs.dependency_cache.default = false;
    if (fault === "default skipped")
      workflow.jobs["llvm-assurance"].if = "inputs.dependency_cache == 'off'";
    if (fault === "cache overlaps compiler") workflow.jobs["dependency-cache"].if = "always()";
    assert.throws(() => validateOptionalLLVM(yaml.dump(workflow)));
  });
}

for (const fault of [
  "missing PR",
  "unbounded time",
  "hidden failure",
  "cold compiler build",
  "obsolete feedback",
]) {
  test(`quick PR checks reject ${fault}`, () => {
    const workflow = yaml.load(toolingQuick);
    if (fault === "missing PR") delete workflow.on.pull_request;
    if (fault === "unbounded time") workflow.jobs.tooling["timeout-minutes"] = 360;
    if (fault === "hidden failure") workflow.jobs.tooling["continue-on-error"] = true;
    if (fault === "cold compiler build")
      workflow.jobs.tooling.steps.push({ run: "python -m scripts.objc3c_assurance run" });
    if (fault === "obsolete feedback") workflow.concurrency["cancel-in-progress"] = false;
    assert.throws(() => validateQuickChecks(yaml.dump(workflow), documentationQuick));
  });
}

test("extended native workflow retains its full receipt-verifying aggregate", () => {
  validateNativeGate(native);
});

test("manual SDK release reuses a passing native archive", () => {
  validateNativeRelease(native, release);
});
for (const fault of [
  "diagnosis by default",
  "hidden partial scope",
  "partial default matrix",
  "partial release input",
  "partial release caller",
  "partial SDK publication",
]) {
  test(`core diagnosis rejects ${fault}`, () => {
    const owner = yaml.load(native),
      caller = yaml.load(release);
    if (fault === "diagnosis by default")
      owner.on.workflow_dispatch.inputs.diagnostic_core_only.default = true;
    if (fault === "hidden partial scope") delete owner["run-name"];
    if (fault === "partial default matrix")
      owner.jobs.native.strategy.matrix.shard = "${{ fromJSON('[\"core\"]') }}";
    if (fault === "partial release input")
      owner.on.workflow_call = { inputs: { diagnostic_core_only: { type: "boolean" } } };
    if (fault === "partial release caller") caller.jobs.build.with = { diagnostic_core_only: true };
    if (fault === "partial SDK publication")
      owner.jobs.native.steps.find((step) => step.with?.name === "objc3c-linux-sdk").if =
        "success() && matrix.shard == 'core'";
    assert.throws(() => validateNativeRelease(yaml.dump(owner), yaml.dump(caller)));
  });
}
for (const fault of [
  "automatic release",
  "conditional owner",
  "hidden owner failure",
  "missing native gate",
  "failed archive",
]) {
  test(`SDK release rejects ${fault}`, () => {
    const owner = yaml.load(native),
      caller = yaml.load(release);
    if (fault === "automatic release") caller.on.pull_request = null;
    if (fault === "conditional owner") caller.jobs.build.if = "success()";
    if (fault === "hidden owner failure") caller.jobs.build["continue-on-error"] = true;
    if (fault === "missing native gate") delete owner.on.workflow_call;
    if (fault === "failed archive")
      owner.jobs.native.steps.find((step) => step.with?.name === "objc3c-linux-sdk").if =
        "always()";
    assert.throws(() => validateNativeRelease(yaml.dump(owner), yaml.dump(caller)));
  });
}
for (const fault of ["compiler overlaps transport", "implicit transport", "unbounded transport"]) {
  test(`artifact transport rejects ${fault}`, () => {
    const owner = yaml.load(native);
    if (fault === "compiler overlaps transport")
      owner.jobs.native.if = "needs.plan.outputs.native == 'true'";
    if (fault === "implicit transport") owner.jobs["diagnostic-transport"].if = "always()";
    if (fault === "unbounded transport")
      owner.jobs["diagnostic-transport"]["timeout-minutes"] = 360;
    assert.throws(() => validateNativeGate(yaml.dump(owner)));
  });
}
for (const target of ["native-assurance.yml", "platform-host-evidence.yml"]) {
  test(`unapproved caller cannot indirectly enable ${target}`, () => {
    const extra = yaml.dump({
      on: { pull_request: null },
      jobs: {
        expensive: {
          uses: `./.github/workflows/${target}`,
        },
      },
    });
    assert.throws(() => validateNativeRelease(native, release, { "extra.yml": extra }));
  });
}

test("extended implementation analysis retains exact periodic and release gates", () => {
  validateImplementationGate(implementation, release);
});

for (const [name, workflowBefore, workflowAfter, releaseBefore, releaseAfter] of [
  ["cancelled cold build", "  cancel-in-progress: false", "  cancel-in-progress: true"],
  ["missing aggregate", "  gate:", "  renamed:"],
  ["conditional aggregate", "    if: always()\n    needs:", "    if: success()\n    needs:"],
  ["automatic expensive PR", "on:\n", "on:\n  pull_request:\n"],
  [
    "missing release source",
    "      source_sha:\n        required: true",
    "      source_sha:\n        required: false",
  ],
  ["missing evidence", "if-no-files-found: error", "if-no-files-found: ignore"],
  ["floating action", "@3d3c42e5aac5ba805825da76410c181273ba90b1", "@main"],
  ["release dependency", "", "", "    needs: [build, implementation-analysis]", "    needs: build"],
]) {
  test(`implementation workflow rejects ${name}`, () => {
    assert.throws(() =>
      validateImplementationGate(
        implementation.replace(workflowBefore ?? "", workflowAfter ?? ""),
        release.replace(releaseBefore ?? "", releaseAfter ?? ""),
      ),
    );
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
  [
    "conditional aggregate",
    "    if: always() && !inputs.diagnostic_core_only\n    needs:",
    "    if: success()\n    needs:",
  ],
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
        "shard: [core, frontend-fuzz, source-coverage]",
        "shard: [core, frontend-fuzz]",
      );
    },
  ],
  [
    "duplicate matrix names",
    (files) => {
      files["adaptive-closeout.yml"] = files["adaptive-closeout.yml"].replace(
        "shard: [core, frontend-fuzz, source-coverage]",
        "shard: [core, core, source-coverage]",
      );
    },
  ],
  [
    "periodic aggregate without prerequisites",
    (files) => {
      files["adaptive-closeout.yml"] = files["adaptive-closeout.yml"].replace(
        "needs: compiler-closeout",
        "needs: nonexistent",
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
