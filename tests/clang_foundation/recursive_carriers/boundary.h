#ifndef RECURSIVE_CARRIERS_BOUNDARY_H
#define RECURSIVE_CARRIERS_BOUNDARY_H

struct BoundaryItem {
  int value;
  explicit BoundaryItem(int);
  BoundaryItem(const BoundaryItem &);
  BoundaryItem(BoundaryItem &&);
  ~BoundaryItem();
};
using BoundaryInner = Optional(BoundaryItem);
using BoundaryOuter = Optional(BoundaryInner);
using BoundaryTree = Result(BoundaryOuter, BoundaryInner);

extern int boundary_constructions, boundary_copies, boundary_moves;
extern int boundary_destructions, boundary_live, boundary_borrows;
extern bool boundary_throw_copy;
const BoundaryTree &boundary_borrow(const BoundaryTree &);
BoundaryTree &boundary_mutable_borrow(BoundaryTree &);

#endif
