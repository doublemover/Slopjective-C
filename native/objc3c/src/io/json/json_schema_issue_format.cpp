#include "io/json/json_schema.h"

#include <sstream>

namespace objc3::io::json {

std::string JsonSchemaIssue::Format() const {
  std::ostringstream out;
  out << domain << "." << code << " instance=" << instance_path
      << " schema=" << schema_path << ": " << message;
  return out.str();
}

}  // namespace objc3::io::json
