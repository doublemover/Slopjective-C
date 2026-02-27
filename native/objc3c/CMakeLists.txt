cmake_minimum_required(VERSION 3.20)
project(objc3c_native LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# M132-A002 target decomposition contract.
# Current state: monolithic `src/main.cpp`.
# Planned steady-state split:
#   objc3c_lex      -> lex/*
#   objc3c_parse    -> parse/* (depends on objc3c_lex)
#   objc3c_sema     -> sema/* (depends on objc3c_parse)
#   objc3c_lower    -> lower/* (depends on objc3c_sema)
#   objc3c_ir       -> ir/* (depends on objc3c_lower)
#   objc3c_pipeline -> pipeline/* (depends on stage libs)
#   objc3c_frontend -> libobjc3c_frontend/* (depends on objc3c_pipeline)
#   objc3c_driver   -> driver/* + io/* + frontend API
#
# Dependency-direction authority is defined in `src/ARCHITECTURE.md` and checked
# by `scripts/check_objc3c_dependency_boundaries.py`.

add_executable(objc3c-native
  src/main.cpp
)

# Placeholder interface targets so boundary rules can map to concrete target
# names before extraction work lands.
add_library(objc3c_lex INTERFACE)
add_library(objc3c_parse INTERFACE)
add_library(objc3c_sema INTERFACE)
add_library(objc3c_lower INTERFACE)
add_library(objc3c_ir INTERFACE)
add_library(objc3c_pipeline INTERFACE)
add_library(objc3c_frontend INTERFACE)
add_library(objc3c_driver INTERFACE)
