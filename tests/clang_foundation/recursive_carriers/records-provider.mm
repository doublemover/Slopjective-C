#include "records.h"

int record_constructions, record_copies, record_moves;
int record_destructions, record_live, record_bodies;
int record_throw_copy = -1, record_throw_construct = -1;
int record_events[128], record_event_count;

RecordLeaf::RecordLeaf(int value) : value(value) {
  if (value == record_throw_construct)
    throw 83;
  ++record_constructions;
  ++record_live;
}
RecordLeaf::RecordLeaf(const RecordLeaf &source) : value(source.value) {
  if (value == record_throw_copy)
    throw 84;
  ++record_copies;
  ++record_live;
}
RecordLeaf::RecordLeaf(RecordLeaf &&source) noexcept : value(source.value) {
  source.value = -source.value;
  ++record_moves;
  ++record_live;
}
RecordLeaf::~RecordLeaf() noexcept {
  ++record_destructions;
  --record_live;
  if (record_event_count < 128)
    record_events[record_event_count++] = value;
}

CarrierRecord::CarrierRecord(int first, int second)
    : first(Ok(RecordOuter, RecordInner,
               Some(RecordInner, Some(RecordLeaf, RecordLeaf(first))))),
      second(Some(RecordLeaf, RecordLeaf(second))) {}

CarrierRecord::~CarrierRecord() noexcept {
  ++record_bodies;
  if (record_event_count < 128)
    record_events[record_event_count++] = 900;
}

CarrierDerived::CarrierDerived(int value)
    : CarrierRecord(value, value + 1),
      tail(Some(RecordLeaf, RecordLeaf(value + 2))) {}

CarrierRecord record_make(int value) {
  return CarrierRecord(value, value + 1);
}
CarrierRecord record_roundtrip(CarrierRecord value) {
  return value;
}
int record_inspect(CarrierRecord value) {
  return isOk(value.first) && hasValue(value.second) ? record_live : -1;
}
