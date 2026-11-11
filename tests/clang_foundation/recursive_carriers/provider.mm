#include "boundary.h"

int boundary_constructions, boundary_copies, boundary_moves;
int boundary_destructions, boundary_live, boundary_borrows;
bool boundary_throw_copy;

BoundaryItem::BoundaryItem(int value) : value(value) {
  ++boundary_constructions;
  ++boundary_live;
}
BoundaryItem::BoundaryItem(const BoundaryItem &source) : value(source.value) {
  if (boundary_throw_copy)
    throw 71;
  ++boundary_copies;
  ++boundary_live;
}
BoundaryItem::BoundaryItem(BoundaryItem &&source) : value(source.value) {
  source.value = -1;
  ++boundary_moves;
  ++boundary_live;
}
BoundaryItem::~BoundaryItem() {
  ++boundary_destructions;
  --boundary_live;
}

const BoundaryTree &boundary_borrow(const BoundaryTree &source) {
  ++boundary_borrows;
  return source;
}
BoundaryTree &boundary_mutable_borrow(BoundaryTree &source) {
  ++boundary_borrows;
  return source;
}
