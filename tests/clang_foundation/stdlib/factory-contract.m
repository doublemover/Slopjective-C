@import objc3.errors;
__attribute__((objc_root_class)) @interface FactoryError <Error> @end
@interface RefinedFactoryError : FactoryError @end
Result(int, FactoryError *) checked_optional(Optional(int) value, FactoryError *error) {
  return okOr(value, ^FactoryError * { return error; });
}
Result(int, RefinedFactoryError *) checked_status(int status, RefinedFactoryError *error) {
  return resultFromStatus(status, 42, ^RefinedFactoryError *(int code) {
    (void)code; return error;
  });
}
Result(int, FactoryError *) checked_error_out(int success, FactoryError *error) {
  return resultFromErrorOut(success, 42, error, ^FactoryError *(FactoryError *raw) {
    return raw;
  });
}
#ifdef INVALID_ERROR_FACTORY
Result(int, int) invalid_factory(Optional(int) value) {
  return okOr(value, ^int { return 7; });
}
#endif
