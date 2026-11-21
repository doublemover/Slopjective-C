#ifndef RECURSIVE_CARRIERS_RECORDS_H
#define RECURSIVE_CARRIERS_RECORDS_H

extern int record_constructions, record_copies, record_moves;
extern int record_destructions, record_live, record_bodies;
extern int record_throw_copy, record_throw_construct;
extern int record_events[128], record_event_count;

struct RecordLeaf {
  int value;
  explicit RecordLeaf(int);
  RecordLeaf(const RecordLeaf &);
  RecordLeaf(RecordLeaf &&) noexcept;
  ~RecordLeaf() noexcept;
};
using RecordInner = Optional(RecordLeaf);
using RecordOuter = Optional(RecordInner);
using RecordField = Result(RecordOuter, RecordInner);

struct CarrierRecord {
  RecordField first;
  RecordInner second;
  CarrierRecord(int, int);
  CarrierRecord(const CarrierRecord &) = default;
  CarrierRecord(CarrierRecord &&) = default;
  ~CarrierRecord() noexcept;
};

struct CarrierAggregate {
  RecordInner first;
  RecordInner second;
};

struct CarrierDerived : CarrierRecord {
  RecordInner tail;
  CarrierDerived(int);
};

struct CarrierEnclosing {
  CarrierRecord member;
  int marker;
};

struct CarrierDefaulted {
  RecordInner field = None(RecordLeaf);
};

CarrierRecord record_make(int);
CarrierRecord record_roundtrip(CarrierRecord);
int record_inspect(CarrierRecord);

static_assert(__is_constructible(CarrierRecord, const CarrierRecord &));
static_assert(__is_constructible(CarrierRecord, CarrierRecord &&));
static_assert(!__is_trivially_copyable(CarrierRecord));
static_assert(!__is_trivially_constructible(CarrierRecord,
                                           const CarrierRecord &));
static_assert(!__is_trivially_destructible(CarrierRecord));
static_assert(!__is_assignable(CarrierRecord &, const CarrierRecord &));
static_assert(!__is_assignable(CarrierRecord &, CarrierRecord &&));
static_assert(!__is_nothrow_constructible(CarrierRecord,
                                         const CarrierRecord &));
static_assert(__is_nothrow_constructible(CarrierRecord, CarrierRecord &&));
static_assert(__is_nothrow_destructible(CarrierRecord));
static_assert(!__is_constructible(CarrierAggregate));
static_assert(!__is_trivially_copyable(CarrierAggregate));
static_assert(__is_constructible(CarrierDefaulted));

#endif
