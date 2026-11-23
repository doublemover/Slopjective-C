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
  validateNativePR,
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
const nativePR = await readFile(new URL("../workflows/native-pr.yml", import.meta.url), "utf8");
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
test("native PR gate verifies the exact merge and retains failure evidence", () => {
  validateNativePR(nativePR);
  for (const edit of [
    (workflow) => {
      workflow.on.pull_request = { paths: ["native/**"] };
    },
    (workflow) => {
      workflow.permissions.contents = "write";
    },
    (workflow) => {
      workflow.jobs["native-pr"]["continue-on-error"] = true;
    },
    (workflow) => {
      workflow.jobs["native-pr-execution"].steps.find((step) => step.id === "run").if = "false";
    },
    (workflow) => {
      workflow.jobs["native-pr-execution"].steps.at(-1).with.name = "latest";
    },
    (workflow) => { delete workflow.jobs["native-pr"].if; },
    (workflow) => { delete workflow.jobs["native-pr"].needs; },
    (workflow) => { workflow.jobs["native-pr"].steps.splice(2, 1); },
    (workflow) => { workflow.jobs["native-pr"].steps.at(-1).if = "false"; },
  ]) {
    const workflow = yaml.load(nativePR);
    edit(workflow);
    assert.throws(() => validateNativePR(yaml.dump(workflow)), /Native PR gate must/);
  }
});
test("workflow changes run the pinned actionlint and ShellCheck integration", () => {
  const workflow = yaml.load(dependencyWorkflows["adaptive-policy-checks.yml"]);
  const paths = workflow.on.pull_request.paths;
  const steps = workflow.jobs.policy.steps;
  const lint = steps.find((step) => step.name === "GitHub workflow syntax, expressions and shell");
  const restore = steps.find((step) => step.name === "Restore pinned workflow linter");
  const save = steps.find((step) => step.name === "Publish pinned workflow linter");
  assert(paths.includes(".github/actionlint.yaml"));
  assert.match(restore.uses, /^actions\/cache\/restore@[0-9a-f]{40}$/);
  assert.match(save.uses, /^actions\/cache\/save@[0-9a-f]{40}$/);
  assert.match(lint.run, /actionlint_1\.7\.12_linux_amd64\.tar\.gz/);
  assert.match(lint.run, /8aca8db96f1b94770f1b0d72b6dddcb1ebb8123cb3712530b08cc387b349a3d8/);
  assert.match(lint.run, /"\$binary"/);
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

for (const [command, fault] of [
  ["scripts.objc3c_toolchain.llvm_installed_ir", "missing seal admission"],
  ["scripts.objc3c_toolchain.semantic_source_feedback", "missing coverage qualification"],
  [
    "scripts.objc3c_toolchain.compiler_instrumentation",
    "missing instrumentation graph verification",
  ],
]) {
  test(`manual LLVM dispatch rejects ${fault}`, () => {
    const workflow = yaml.load(llvmOptional);
    const job = command.endsWith("llvm_installed_ir") ? "installed-preview" : "llvm-assurance";
    const step = workflow.jobs[job].steps.find(({ run }) => run?.includes(command));
    step.run = step.run.replace(
      fault === "missing seal admission"
        ? "--seal-sha256"
        : fault === "missing coverage qualification"
          ? "--coverage-qualification"
          : command,
      "changed-command",
    );
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

test("native terminal gate merges owner evidence before verifying every shard", () => {
  const missingOwners = yaml.load(native);
  missingOwners.jobs.gate.steps = missingOwners.jobs.gate.steps.filter(
    (step) => step.name !== "Download exact core owner evidence",
  );
  assert.throws(() => validateNativeGate(yaml.dump(missingOwners)), /incomplete receipts/);

  const missingMerge = yaml.load(native);
  const verifier = missingMerge.jobs.gate.steps.find((step) =>
    step.run?.includes("verify --sharded"),
  );
  verifier.run = verifier.run.replace("release_matrix aggregate", "release_matrix skipped");
  assert.throws(() => validateNativeGate(yaml.dump(missingMerge)), /incomplete receipts/);
});

for (const fault of [
  "missing partition",
  "duplicate partition",
  "out-of-range partition",
  "obsolete single consumer",
  "missing schedules",
  "extra consumer",
]) {
  test(`native and release gates reject sanitizer matrix ${fault}`, () => {
    const owner = yaml.load(native);
    const groups = owner.jobs["instrumented-product-consumers"].strategy.matrix.group;
    if (fault === "missing partition") groups.splice(3, 1);
    if (fault === "duplicate partition") groups[3] = groups[0];
    if (fault === "out-of-range partition") groups[3] = "asan-products-partition-10";
    if (fault === "obsolete single consumer")
      groups.splice(0, groups.length, "asan-products", "runtime-schedules");
    if (fault === "missing schedules") groups.pop();
    if (fault === "extra consumer") groups.push("unreviewed-consumer");
    const changed = yaml.dump(owner);
    assert.throws(() => validateNativeGate(changed), /complete independent execution matrices/);
    assert.throws(
      () => validateNativeRelease(changed, release),
      /complete independent execution matrices/,
    );
  });
}

for (const fault of ["missing diagnostics", "conditional diagnostics", "failure bundle"]) {
  test(`sanitizer producer rejects ${fault}`, () => {
    const owner = yaml.load(native);
    const steps = owner.jobs["asan-foundation-producer"].steps;
    const diagnostics = steps.find(
      (step) => step.name === "Preserve sanitizer compiler diagnostics",
    );
    const bundle = steps.find((step) => step.name === "Preserve exact sanitizer compiler bundle");
    if (fault === "missing diagnostics") steps.splice(steps.indexOf(diagnostics), 1);
    if (fault === "conditional diagnostics") diagnostics.if = "success()";
    if (fault === "failure bundle") bundle.if = "always()";
    assert.throws(() => validateNativeGate(yaml.dump(owner)));
  });
}

test("manual SDK release reuses a passing native archive", () => {
  validateNativeRelease(native, release);
});
for (const fault of ["missing exit", "success exit", "status overwritten"]) {
  test(`retained coverage diagnosis rejects ${fault}`, () => {
    const owner = yaml.load(native);
    const step = owner.jobs["diagnostic-coverage-aggregate"].steps.find((step) =>
      step.run?.includes("scripts.objc3c_assurance.coverage_matrix aggregate"),
    );
    const replacement =
      fault === "missing exit" ? "" : fault === "success exit" ? "exit 0" : "true";
    step.run = step.run.replace('exit "$status"', replacement);
    assert.throws(() => validateNativeGate(yaml.dump(owner)), /aggregate exit status/);
  });
}
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
      owner.jobs["coverage-partitions"].strategy.matrix.partition = [0];
    if (fault === "partial release input")
      owner.on.workflow_call = { inputs: { diagnostic_core_only: { type: "boolean" } } };
    if (fault === "partial release caller") caller.jobs.build.with = { diagnostic_core_only: true };
    if (fault === "partial SDK publication")
      owner.jobs["product-producer"].steps.find(
        (step) => step.name === "Preserve qualified SDK archive",
      ).if = "success()";
    assert.throws(() => validateNativeRelease(yaml.dump(owner), yaml.dump(caller)));
  });
}
for (const fault of [
  "automatic release",
  "conditional owner",
  "hidden owner failure",
  "missing native gate",
  "failed archive",
  "older source blocks repaired source",
  "discard active source",
  "foreign build host",
  "foreign analysis host",
  "unbound producer archive",
  "foreign installed archive",
  "install waits for other consumers",
  "skipped install matrix",
  "missing install outcome",
  "missing install aggregate outcome",
  "missing install scope",
  "optional release analysis",
]) {
  test(`SDK release rejects ${fault}`, () => {
    const owner = yaml.load(native),
      caller = yaml.load(release);
    if (fault === "automatic release") caller.on.pull_request = null;
    if (fault === "conditional owner") caller.jobs.build.if = "success()";
    if (fault === "hidden owner failure") caller.jobs.build["continue-on-error"] = true;
    if (fault === "missing native gate") delete owner.on.workflow_call;
    if (fault === "failed archive")
      owner.jobs["product-producer"].steps.find(
        (step) => step.name === "Preserve qualified SDK archive",
      ).if = "always()";
    if (fault === "older source blocks repaired source")
      owner.concurrency.group = owner.concurrency.group.replace("-${{ github.sha }}", "");
    if (fault === "discard active source") owner.concurrency["cancel-in-progress"] = true;
    if (fault === "foreign build host") caller.jobs.build.with.linux_host = "linux-aarch64";
    if (fault === "foreign analysis host")
      owner.jobs["implementation-analysis"].with.linux_host = "linux-aarch64";
    if (fault === "unbound producer archive")
      owner.jobs["product-producer"].steps.find(
        (step) => step.name === "Preserve qualified SDK archive",
      ).with.name = "objc3c-linux-sdk";
    if (fault === "foreign installed archive")
      owner.jobs["archive-install"].steps.find((step) =>
        step.uses?.startsWith("actions/download-artifact@"),
      ).with.name = "objc3c-linux-sdk-linux-aarch64";
    if (fault === "install waits for other consumers")
      owner.jobs["archive-install"].needs.push("coverage-aggregate");
    if (fault === "skipped install matrix") caller.jobs.build.with.archive_install = false;
    if (fault === "missing install outcome")
      owner.jobs.gate.needs = owner.jobs.gate.needs.filter((id) => id !== "archive-install");
    if (fault === "missing install aggregate outcome")
      owner.jobs.gate.steps[0].run = owner.jobs.gate.steps[0].run.replace(" INSTALL_AGGREGATE_RESULT", "");
    if (fault === "missing install scope")
      owner.jobs["archive-install"].strategy.matrix.scope.pop();
    if (fault === "optional release analysis")
      owner.jobs.gate.needs = owner.jobs.gate.needs.filter(
        (id) => id !== "implementation-analysis",
      );

    assert.throws(() => validateNativeRelease(yaml.dump(owner), yaml.dump(caller)));
  });
}
for (const fault of ["compiler overlaps transport", "implicit transport", "unbounded transport"]) {
  test(`artifact transport rejects ${fault}`, () => {
    const owner = yaml.load(native);
    if (fault === "compiler overlaps transport")
      owner.jobs["product-producer"].if = "needs.plan.outputs.native == 'true'";
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
  validateImplementationGate(implementation);
});

for (const [fault, alter] of [
  ["no producer prerequisite", (jobs) => { jobs.gate.needs = ["plan", "analysis"]; }],
  ["unbound producer digest", (jobs) => { delete jobs.foundation.outputs.archive_sha256; }],
  ["consumer starts before shared compilation", (jobs) => { jobs.analysis.needs = ["plan"]; }],
  ["unmatched compiler download", (jobs) => {
    jobs.analysis.steps.find((step) => step.with?.path?.endsWith("/foundation-input"))
      .with.name = "some-other-compiler";
  }],
  ["missing terminal digest", (jobs) => {
    const verify = jobs.gate.steps.find((step) => step.env?.ASSURANCE_OUTCOMES);
    verify.env.ASSURANCE_OUTCOMES = verify.env.ASSURANCE_OUTCOMES.replace("foundation_archive", "unrelated");
  }],
]) {
  test(`shared implementation foundation rejects ${fault}`, () => {
    const workflow = yaml.load(implementation);
    alter(workflow.jobs);
    assert.throws(() => validateImplementationGate(yaml.dump(workflow)));
  });
}

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
]) {
  test(`implementation workflow rejects ${name}`, () => {
    assert.throws(() =>
      validateImplementationGate(implementation.replace(workflowBefore ?? "", workflowAfter ?? "")),
    );
  });
}

test("native containers admit TSan personality and debugger process controls", async () => {
  const llvm = await readFile(new URL("../workflows/llvm-assurance.yml", import.meta.url), "utf8");
  for (const [text, name] of [
    [native, "diagnostic-native"],
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
test("native workflow rejects a missing terminal dependency", () => {
  const owner = yaml.load(native);
  owner.jobs.gate.needs = owner.jobs.gate.needs.filter((id) => id !== "product-producer");
  assert.throws(() => validateNativeGate(yaml.dump(owner)));
});
for (const fault of ["missing outcome", "substituted outcome", "ignored failures"]) {
  test(`native workflow rejects ${fault} in its outcome gate`, () => {
    const owner = yaml.load(native);
    const step = owner.jobs.gate.steps.find((step) => step.env?.PRODUCER_RESULT);
    if (fault === "missing outcome")
      step.run = step.run.replace("results=(PLAN_RESULT PRODUCER_RESULT", "results=(PLAN_RESULT");
    if (fault === "substituted outcome") step.env.PRODUCER_RESULT = "success";
    if (fault === "ignored failures") step["continue-on-error"] = true;
    assert.throws(() => validateNativeGate(yaml.dump(owner)), /terminal gate/);
  });
}
for (const [name, before, after] of [
  ["missing aggregate", "  gate:", "  renamed:"],
  [
    "conditional aggregate",
    "    if: always() && !inputs.diagnostic_core_only\n    needs:",
    "    if: success()\n    needs:",
  ],
  ["automatic expensive PR", "on:\n", "on:\n  pull_request:\n"],
  ["privileged PR trigger", "on:\n", "on:\n  pull_request_target:\n"],
  ["missing artifact normalized", "if-no-files-found: error", "if-no-files-found: ignore"],
  [
    "missing activated checkout trust",
    'git config --global --add safe.directory "$GITHUB_WORKSPACE"',
    "echo missing-checkout-trust",
  ],
  ["ignored sibling failure", "      fail-fast: true", "      fail-fast: false"],
  ["unsharded verification", "verify --sharded", "verify"],
  ["unpartitioned execution", '--index "$COVERAGE_PARTITION"', ""],
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
