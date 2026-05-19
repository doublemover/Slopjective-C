#include "lower/contracts/typed_sema_lowering_boundary.h"

#include "lower/contracts/typed_sema_lowering_boundary_counts.h"

Objc3TypedSemaToLoweringBoundary Objc3BuildTypedSemaToLoweringBoundary(
    const Objc3Program &program) {
  Objc3TypedSemaToLoweringBoundary boundary;
  boundary.module_name = program.module_name;
  Objc3PopulateTypedSemaToLoweringBoundaryCounts(program, boundary);

  boundary.deterministic = !boundary.module_name.empty();
  boundary.all_params_have_concrete_type_surface =
      boundary.unknown_param_type_surfaces == 0;
  boundary.diagnostics_clear = boundary.frontend_diagnostic_sites == 0;
  boundary.owner_split_explicit = Objc3LoweringStrictOwnerModelIsReady(
      boundary.typed_semantic_handoff_owner,
      boundary.strict_contract_owner_model,
      boundary.strict_no_retired_route,
      boundary.strict_no_compatibility) &&
      Objc3LoweringStrictOwnerModelIsReady(
          boundary.lowering_consumer_owner,
          boundary.strict_contract_owner_model,
          boundary.strict_no_retired_route,
          boundary.strict_no_compatibility);
  boundary.sema_to_lowering_owner_contract_recorded =
      boundary.owner_split_explicit;
  boundary.ready = Objc3TypedSemaToLoweringBoundaryIsReady(boundary);
  boundary.replay_key = Objc3TypedSemaToLoweringBoundaryReplayKey(boundary);
  return boundary;
}
