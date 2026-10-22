#include "records.h"

int main() {
  {
    CarrierRecord source = record_make(70);
    if (record_live != 2 || record_inspect(source) != 4 || record_live != 2)
      return 1;
    CarrierRecord copied = record_roundtrip(source);
    if (record_live != 4 || record_copies != 4 || record_moves != 2 ||
        !isOk(copied.first) || !hasValue(copied.second))
      return 2;
    CarrierRecord moved = record_roundtrip(static_cast<CarrierRecord &&>(source));
    if (record_live != 6 || record_copies != 4 || record_moves != 6 ||
        !isOk(source.first) || !hasValue(source.second))
      return 3;
  }
  return record_live || record_constructions + record_copies + record_moves !=
                            record_destructions ? 4 : 0;
}
