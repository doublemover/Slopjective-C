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

typedef int (^MatchBlock)(void);
MatchOptional match_mutable(MatchOptional input) {
  return match(input) { case var whole => (whole = None(id), whole); };
}
id match_object(MatchOptional input) {
  return match(input) {
    case .Some(let payload) => payload;
    case .None => (id)0;
  };
}
MatchBlock match_block(MatchOptional input) {
  return match(input) {
    case .Some(let payload) => ^{ return payload == (id)0 ? 0 : 42; };
    case .None => ^{ return -1; };
  };
}
int match_scalar(int input) {
  return match(input) { case var value => (value += 4, value); };
}
