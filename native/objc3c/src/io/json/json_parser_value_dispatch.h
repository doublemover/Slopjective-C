#pragma once

namespace objc3::io::json {

class JsonParserCursor;
class JsonParserValueDelegate;
class JsonValue;

bool DispatchJsonParserValue(JsonParserCursor &cursor,
                             JsonParserValueDelegate &delegate,
                             JsonValue &out);

}  // namespace objc3::io::json
