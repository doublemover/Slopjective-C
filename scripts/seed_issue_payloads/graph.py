from __future__ import annotations

from typing import Any

from seed_issue_payloads.model import ACCEPTANCE_GATE_ID_RE
from seed_issue_payloads.model import SEED_ID_RE
from seed_issue_payloads.model import SHARD_CLASSES
from seed_issue_payloads.model import WAVE_ID_RE
from seed_issue_payloads.model import ParseError
from seed_issue_payloads.model import SeedMetadata
from seed_issue_payloads.model import TemplateMetadata
from seed_issue_payloads.parse_utils import expect_dict
from seed_issue_payloads.parse_utils import expect_int
from seed_issue_payloads.parse_utils import expect_list
from seed_issue_payloads.parse_utils import expect_nonempty_str
from seed_issue_payloads.parse_utils import parse_string_list


def parse_seeds(payload: dict[str, Any]) -> list[SeedMetadata]:
    graph = expect_dict(payload.get("graph"), "root.graph")
    seed_rows = expect_list(graph.get("seeds"), "root.graph.seeds")
    if not seed_rows:
        raise ParseError("root.graph.seeds must not be empty")

    parsed: list[SeedMetadata] = []
    seen_seed_ids: set[str] = set()
    for idx, raw_seed in enumerate(seed_rows):
        context = f"root.graph.seeds[{idx}]"
        seed = expect_dict(raw_seed, context)

        missing_keys = sorted(
            key
            for key in (
                "seed_id",
                "proposed_issue_title",
                "wave_id",
                "depends_on",
                "shard_class",
                "acceptance_gate_id",
                "priority",
            )
            if key not in seed
        )
        if missing_keys:
            raise ParseError(f"{context} missing required key(s): {', '.join(missing_keys)}")

        seed_id = expect_nonempty_str(seed.get("seed_id"), f"{context}.seed_id")
        if not SEED_ID_RE.match(seed_id):
            raise ParseError(f"{context}.seed_id has invalid format: {seed_id}")
        if seed_id in seen_seed_ids:
            raise ParseError(f"duplicate seed id in root.graph.seeds: {seed_id}")
        seen_seed_ids.add(seed_id)

        title = expect_nonempty_str(
            seed.get("proposed_issue_title"),
            f"{context}.proposed_issue_title",
        )
        wave_id = expect_nonempty_str(seed.get("wave_id"), f"{context}.wave_id")
        if not WAVE_ID_RE.match(wave_id):
            raise ParseError(f"{context}.wave_id has invalid format: {wave_id}")

        depends_on_raw = expect_list(seed.get("depends_on"), f"{context}.depends_on")
        depends_on: list[str] = []
        for dep_idx, raw_dep in enumerate(depends_on_raw):
            dependency = expect_nonempty_str(
                raw_dep,
                f"{context}.depends_on[{dep_idx}]",
            )
            if not SEED_ID_RE.match(dependency):
                raise ParseError(
                    f"{context}.depends_on[{dep_idx}] has invalid seed id: {dependency}"
                )
            depends_on.append(dependency)

        shard_class = expect_nonempty_str(seed.get("shard_class"), f"{context}.shard_class")
        if shard_class not in SHARD_CLASSES:
            raise ParseError(
                f"{context}.shard_class must be one of {sorted(SHARD_CLASSES)}; got {shard_class}"
            )

        acceptance_gate_id = expect_nonempty_str(
            seed.get("acceptance_gate_id"),
            f"{context}.acceptance_gate_id",
        )
        if not ACCEPTANCE_GATE_ID_RE.match(acceptance_gate_id):
            raise ParseError(
                f"{context}.acceptance_gate_id has invalid format: {acceptance_gate_id}"
            )

        priority = expect_dict(seed.get("priority"), f"{context}.priority")
        priority_score = expect_int(
            priority.get("priority_score"),
            f"{context}.priority.priority_score",
        )
        duv = expect_int(priority.get("duv"), f"{context}.priority.duv")
        dc = expect_int(priority.get("dc"), f"{context}.priority.dc")
        tier = expect_nonempty_str(priority.get("tier"), f"{context}.priority.tier")

        parsed.append(
            SeedMetadata(
                seed_id=seed_id,
                title=title,
                wave_id=wave_id,
                depends_on=tuple(depends_on),
                shard_class=shard_class,
                acceptance_gate_id=acceptance_gate_id,
                priority_score=priority_score,
                duv=duv,
                dc=dc,
                tier=tier,
            )
        )

    known_seed_ids = {seed.seed_id for seed in parsed}
    for seed in parsed:
        for dependency in seed.depends_on:
            if dependency == seed.seed_id:
                raise ParseError(f"seed {seed.seed_id} must not depend on itself")
            if dependency not in known_seed_ids:
                raise ParseError(
                    f"seed {seed.seed_id} depends on unknown seed id: {dependency}"
                )

    return parsed


def parse_templates(payload: dict[str, Any]) -> dict[str, TemplateMetadata]:
    batch_skeletons = expect_dict(payload.get("batch_skeletons"), "root.batch_skeletons")
    batches = expect_list(batch_skeletons.get("batches"), "root.batch_skeletons.batches")
    if not batches:
        raise ParseError("root.batch_skeletons.batches must not be empty")

    templates: dict[str, TemplateMetadata] = {}
    for batch_idx, raw_batch in enumerate(batches):
        batch_context = f"root.batch_skeletons.batches[{batch_idx}]"
        batch = expect_dict(raw_batch, batch_context)
        issue_templates = expect_list(
            batch.get("issue_templates"),
            f"{batch_context}.issue_templates",
        )
        for template_idx, raw_template in enumerate(issue_templates):
            context = f"{batch_context}.issue_templates[{template_idx}]"
            template = expect_dict(raw_template, context)
            seed_id = expect_nonempty_str(template.get("seed_id"), f"{context}.seed_id")
            labels = parse_string_list(template.get("labels"), f"{context}.labels")
            body_fields = expect_dict(template.get("body_fields"), f"{context}.body_fields")
            validation_commands = parse_string_list(
                body_fields.get("validation_commands"),
                f"{context}.body_fields.validation_commands",
            )

            if seed_id in templates:
                raise ParseError(f"duplicate issue template seed id: {seed_id}")
            templates[seed_id] = TemplateMetadata(
                labels=labels,
                validation_commands=validation_commands,
            )

    if not templates:
        raise ParseError("root.batch_skeletons.batches must contain at least one issue template")
    return templates


def wave_index(wave_id: str) -> int:
    return int(wave_id[1:])


def seed_sort_key(seed: SeedMetadata) -> tuple[int, int, int, int, str]:
    return (
        wave_index(seed.wave_id),
        -seed.priority_score,
        -seed.duv,
        seed.dc,
        seed.seed_id,
    )
