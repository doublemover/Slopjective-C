#include "io/json/json_schema_unsupported_annotation_keyword_contracts.h"

namespace objc3::io::json {

bool IsJsonSchemaAnnotationKeyword(std::string_view key) {
  return key == "$schema" || key == "$id" || key == "$comment" ||
         key == "title" || key == "description" || key == "format";
}

}  // namespace objc3::io::json
