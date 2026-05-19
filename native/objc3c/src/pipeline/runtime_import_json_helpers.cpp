#include "pipeline/runtime_import_json_helpers.h"

namespace objc3c::pipeline {

RuntimeImportJsonParser::RuntimeImportJsonParser(const std::string &input)
    : input_(input) {}

bool RuntimeImportJsonParser::Parse(RuntimeImportJsonValue &value,
                                    std::string &error) {
  SkipWhitespace();
  if (!ParseValue(value, error)) {
    return false;
  }
  SkipWhitespace();
  if (!AtEnd()) {
    error = "unexpected trailing JSON content";
    return false;
  }
  return true;
}

}  // namespace objc3c::pipeline
