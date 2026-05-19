#pragma once

#include <sstream>
#include <vector>

#include "io/json/json_writer.h"

namespace objc3::artifacts::json {

template <typename RecordT, typename WriteRecordFn>
std::string RenderArtifactRecordArray(const std::vector<RecordT> &records,
                                      WriteRecordFn write_record) {
  std::ostringstream out;
  objc3::io::json::JsonArrayWriter array(out);
  for (const auto &record : records) {
    array.BeginElement();
    write_record(out, record);
  }
  array.End();
  return out.str();
}

}  // namespace objc3::artifacts::json
