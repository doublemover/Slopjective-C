import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import test from "node:test";
import * as yaml from "js-yaml";
import {
  validateNativeContainer,
  validateNativeGate,
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

test("native PR workflow has a guaranteed receipt-verifying aggregate", () => {
  validateNativeGate(native);
});

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
  ["path-filtered PR", "  pull_request:", '  pull_request:\n    paths: ["native/**"]'],
  ["privileged PR trigger", "  pull_request:", "  pull_request_target:"],
  ["missing dependency", "    needs: [plan, native]", "    needs: [plan]"],
  ["missing artifact normalized", "if-no-files-found: error", "if-no-files-found: ignore"],
  [
    "missing activated checkout trust",
    'git config --global --add safe.directory "$GITHUB_WORKSPACE"',
    "echo missing-checkout-trust",
  ],
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
]) {
  test(`workflow contract rejects ${name}`, async () => {
    const files = { ...workflows };
    mutate(files);
    await assert.rejects(check(files));
  });
}
