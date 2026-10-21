typedef Optional(id) MatchOptional;
typedef Result(id, id) MatchResult;
MatchOptional match_optional(MatchOptional input) {
  return match(input) {
    case .Some(let payload) => Some(id, payload);
    case .None => None(id);
  };
}
MatchResult match_result(MatchResult input) {
  return match(input) {
    case .Ok(let payload) => Ok(id, id, payload);
    case .Err(let payload) => Err(id, id, payload);
  };
}
