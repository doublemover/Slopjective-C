# Commands

Command support is generated from checked-in workflow contracts and public
command docs. Capability claims route through `docs/support/capability_matrix.md`.

The public command model is:

- `package.json` exposes one script: `objc3c`.
- `npm run objc3c -- <action>` invokes the `scripts.objc3c_workflow`
  module.
- `scripts/objc3c_workflow/action_catalog.py` owns action names, tiers, pass-through
  behavior, backend descriptions, and guarantee-owner text.
- `scripts/objc3c_workflow/registry_views.py`,
  `scripts/objc3c_workflow/action_integrity.py`,
  `scripts/objc3c_workflow/request_dispatch.py`, and
  `scripts/objc3c_workflow/path_bootstrap.py` own the internal read-only
  registry, handler-integrity, request-dispatch, and direct-entrypoint
  bootstrap surfaces.
- `docs/runbooks/objc3c_public_command_surface.md` is the operator-facing
  appendix for the package bridge.

User-facing workflow docs advertise the npm bridge only. Retired package-script
names, public-script metadata tables, helper invocations, native build commands,
and implementation-only script names are not public command surface.
