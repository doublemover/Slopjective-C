// The SDK must reject this at the same source location in both driver modes.
typedef Result(id, id) Outcome;

Outcome missing_assignment(int success, id value) {
  Outcome outcome;
  if (success)
    outcome = Ok(id, id, value);
  return outcome;
}
