#pragma once

typedef bool (*ecunit_runnner__suite_t)(unsigned int offset, unsigned int index);

#define ECUNIT_RUNNER__BEGIN_SUITE(NAME) bool NAME(unsigned int offset, unsigned int index) { do
#define ECUNIT_RUNNER__END_SUITE while(false); return false; }

#define ECUNIT_RUNNER__BEGIN_CASE(NAME) if (index == offset++) { ECUNIT__BEGIN_CASE(NAME)
#define ECUNIT_RUNNER__END_CASE ECUNIT__END_CASE return true; }

static inline void ecunit_runnner__run_suite(ecunit_runnner__suite_t suite) {
    unsigned int index = 0;
    while (suite(0, index++));
}
