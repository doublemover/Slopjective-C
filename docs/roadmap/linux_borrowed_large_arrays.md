# Captured callable arrays beyond eager element tracking

System 2.14.0 preserves invocation effects from array elements outside the eagerly
tracked storage range. A 128-element capture with a consuming callback in its
last slot must invalidate the caller's unprotected loan when an unknown index
may invoke that callback. The earlier summary could lose that effect.

Wildcard symbolic callable inputs are suppressed only for fully eager array
shapes. Larger or nonconstant shapes retain a conservative wildcard. Aggregate
initialization folds unrepresented element initializers into that storage,
including nested field/callable effects and possible address sets. It does not
claim that the final initializer is the only possible referent. Fresh exact
array initialization resets earlier wildcard facts; summary initialization
inside an existing unknown parent keeps its union of alternatives.

Arrays within the eager range retain element precision. Larger arrays may
conservatively include unrepresented siblings; this span does not provide
unbounded element expansion or whole-program callback analysis. Complete memory
fills still clear all represented and summary slots, while partial writes keep
possible sibling facts.

Native source/PCH witnesses cover C++20/23 and ARC/MRC, the 63/64 boundary,
128-element captures, copied closures, constant/dynamic invocation, nested
records, fresh loop initialization and complete overwrite. Frontend witnesses
repeat the missing-consumption case. Broader validation follows implementation.
The SDK candidate was stopped before publication after the edge case was found;
SDK 2.33 remains the accepted archive.
