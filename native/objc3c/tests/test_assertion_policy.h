// Every native test, including custom-command objects, must retain its checks.
#pragma once
#ifdef NDEBUG
#error "Native regression tests require active assertions"
#endif
