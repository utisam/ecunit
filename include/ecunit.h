#pragma once

#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    assertion_type__statement,
    assertion_type__cmp_int,
    assertion_type__cmp_uint,
} assertion_type_t;

typedef enum {
    ecunit__operator__unknown = 0x0,
    ecunit__operator__eq = 1,
    ecunit__operator__lt = 2,
    ecunit__operator__le = 3,
    ecunit__operator__gt = 4,
    ecunit__operator__ge = 5,
} ecunit__operator_t;

static inline const char* ecunit__operator_string(const ecunit__operator_t op) {
    switch (op) {
    case ecunit__operator__eq: return "==";
    case ecunit__operator__lt: return "<";
    case ecunit__operator__le: return "<=";
    case ecunit__operator__gt: return ">";
    case ecunit__operator__ge: return ">=";
    default: return "?";
    }
}

typedef struct {
    int a;
    int b;
    ecunit__operator_t op;
} ecunit__assertion_cmp_int_detail_t;

typedef struct {
    unsigned int a;
    unsigned int b;
    ecunit__operator_t op;
} ecunit__assertion_cmp_uint_detail_t;

typedef struct {
    bool failed;
    assertion_type_t type;
    union {
        const char* statement;
        ecunit__assertion_cmp_int_detail_t cmp_int;
        ecunit__assertion_cmp_uint_detail_t cmp_uint;
    } detail;
} ecunit__assertion_t;

typedef struct {
    const char* name;
    ecunit__assertion_t* assertions_begin;
    ecunit__assertion_t* assertions_end;
    ecunit__assertion_t* assertions_wrapped_at;
    bool failed;
} ecunit__test_result_t;

typedef void (*ecunit__set_up_func_t)(const char* name);
typedef void (*ecunit__tear_down_func_t)(const ecunit__test_result_t* result);

typedef struct {
    ecunit__assertion_t* ptr;
    bool wrapped;
} ecunit__test_result_iter_t;

static inline void ecunit__test_result_iter__init(ecunit__test_result_iter_t* iter, const ecunit__test_result_t* result) {
    if (result->assertions_wrapped_at) {
        iter->ptr = result->assertions_end;
        iter->wrapped = false;
    } else {
        iter->ptr = result->assertions_begin;
        iter->wrapped = true;
    }
}

static inline bool ecunit__test_result_iter__condition(const ecunit__test_result_iter_t* iter, const ecunit__test_result_t* result) {
    return !iter->wrapped || iter->ptr != result->assertions_end;
}

static inline void ecunit__test_result_iter__next(ecunit__test_result_iter_t* iter, const ecunit__test_result_t* result) {
    iter->ptr++;
    if (iter->ptr == result->assertions_wrapped_at) {
        iter->ptr = result->assertions_begin;
        iter->wrapped = true;
    }
}

typedef struct {
    ecunit__set_up_func_t on_set_up;
    ecunit__tear_down_func_t on_tear_down;

    ecunit__assertion_t* assertions_buf;
    size_t assertions_buf_len;

    jmp_buf* p_jmp_buf;
} ecunit__config_t;

void ecunit__init(const ecunit__config_t* config);
jmp_buf* __ecunit__get_p_jmp_buf();

void ecunit__begin_case(const char* case_name);
#define ECUNIT__BEGIN_CASE(NAME) { ecunit__begin_case(NAME); jmp_buf* p_jmp_buf = __ecunit__get_p_jmp_buf(); if (!p_jmp_buf || setjmp(*p_jmp_buf) == 0)
void ecunit__end_case();
#define ECUNIT__END_CASE ecunit__end_case(); }

bool __ecunit__assert(bool assertion, const char* statement);
#define ECUNIT__ASSERT(STMT) __ecunit__assert((bool) (STMT), #STMT)

static inline ecunit__operator_t __ecunit__to_operator(bool eq, bool lt, bool gt) {
    int flag = 0;
    if (eq) {
        flag |= ecunit__operator__eq;
    }
    if (lt) {
        flag |= ecunit__operator__le;
    }
    if (gt) {
        flag |= ecunit__operator__gt;
    }
    if (flag >= 6) {
        return ecunit__operator__unknown;
    }
    return (ecunit__operator_t) flag;
}
#define __ECUNIT__TO_OPERATOR(OP) __ecunit__to_operator((0 OP 0), (0 OP 1), (1 OP 0))

bool __ecunit__assert_cmp_int(int a, int b, ecunit__operator_t op);
#define ECUNIT__ASSERT_CMP_INT(A, OP, B) __ecunit__assert_cmp_int((A), (B), __ECUNIT__TO_OPERATOR(OP))

bool __ecunit__assert_cmp_uint(unsigned int a, unsigned int b, ecunit__operator_t op);
#define ECUNIT__ASSERT_CMP_UINT(A, OP, B) __ecunit__assert_cmp_uint((A), (B), __ECUNIT__TO_OPERATOR(OP))

#ifdef __cplusplus
}
#endif
