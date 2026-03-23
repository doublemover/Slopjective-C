# M270-B003 Expectations

- Contract id: `objc3c-part7-actor-race-hazard-escape-diagnostics/m270-b003-v1`
- Dependency: `objc3c-part7-actor-isolation-sendability-enforcement/m270-b002-v1`
- Surface path: `frontend.pipeline.semantic_surface.objc_part7_actor_race_hazard_and_escape_diagnostics`
- Live sema must fail closed for actor-method task handoff without:
  - race guard coverage
  - replay proof coverage
  - actor isolation coverage
- Escaping block literals remain fail-closed in actor methods that perform task handoff.
