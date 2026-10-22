#include "records.h"

void *operator new(__SIZE_TYPE__, void *storage) noexcept { return storage; }

int main() {
  { CarrierDefaulted empty; if (hasValue(empty.field)) return 18; }
  {
    CarrierRecord source(1, 2);
    if (record_live != 2 || record_inspect(source) != 4 ||
        record_live != 2 || record_copies != 2 || record_bodies != 1)
      return 1;
    CarrierRecord copied(source);
    CarrierRecord moved(static_cast<CarrierRecord &&>(copied));
    if (record_live != 6 || record_copies != 4 || record_moves != 2 ||
        !isOk(copied.first) || !hasValue(copied.second) ||
        !isOk(moved.first) || !hasValue(moved.second))
      return 2;
  }
  if (record_live || record_bodies != 4 || record_destructions != 8)
    return 3;

  // A failed second field destroys the completed first field exactly once.
  {
    CarrierRecord source(3, 4);
    int destroyed = record_destructions, bodies = record_bodies;
    record_throw_copy = 4;
    try {
      CarrierRecord failed(source);
      return 4;
    } catch (int code) {
      if (code != 84 || record_live != 2 ||
          record_destructions != destroyed + 1 || record_bodies != bodies)
        return 5;
    }
    record_throw_copy = -1;
  }
  if (record_live)
    return 6;
  {
    int destroyed = record_destructions, bodies = record_bodies;
    record_throw_construct = 6;
    try {
      CarrierRecord failed(5, 6);
      return 7;
    } catch (int code) {
      if (code != 83 || record_live || record_destructions != destroyed + 1 ||
          record_bodies != bodies)
        return 8;
    }
    record_throw_construct = -1;
  }

  record_event_count = 0;
  { CarrierRecord ordered(10, 11); }
  if (record_event_count != 3 || record_events[0] != 900 ||
      record_events[1] != 11 || record_events[2] != 10 || record_live)
    return 9;

  {
    CarrierAggregate aggregate{Some(RecordLeaf, RecordLeaf(20)),
                               Some(RecordLeaf, RecordLeaf(21))};
    CarrierAggregate copied(aggregate);
    CarrierAggregate moved(static_cast<CarrierAggregate &&>(copied));
    CarrierAggregate brace_copy{aggregate.first, aggregate.second};
    if (record_live != 8 || !hasValue(brace_copy.first))
      return 10;
  }
  if (record_live)
    return 11;
  {
    CarrierDerived source(30);
    CarrierDerived copied(source);
    CarrierDerived moved(static_cast<CarrierDerived &&>(copied));
    CarrierEnclosing enclosing{record_make(40), 42};
    CarrierEnclosing second(enclosing);
    if (record_live != 13 || second.marker != 42 || !hasValue(moved.tail))
      return 12;
  }
  if (record_live)
    return 13;

  // Real source classes retain their ordinary native new/delete lifecycle.
  {
    CarrierRecord *allocated = new CarrierRecord(50, 51);
    if (record_live != 2)
      return 14;
    delete allocated;
    alignas(CarrierRecord) unsigned char storage[sizeof(CarrierRecord)];
    CarrierRecord *placed = new (storage) CarrierRecord(52, 53);
    placed->~CarrierRecord();
    placed = new (storage) CarrierRecord(54, 55);
    placed->~CarrierRecord();
  }
  if (record_live)
    return 15;

  // An aggregate's first carrier is also rolled back if a later field throws.
  {
    int destroyed = record_destructions;
    record_throw_construct = 61;
    try {
      CarrierAggregate failed{Some(RecordLeaf, RecordLeaf(60)),
                               Some(RecordLeaf, RecordLeaf(61))};
      return 16;
    } catch (int code) {
      if (code != 83 || record_live || record_destructions != destroyed + 1)
        return 17;
    }
    record_throw_construct = -1;
  }
  return 0;
}
