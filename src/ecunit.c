#include <ecunit.h>

#include <string.h>

static ecunit__config_t ecunit__config;

static ecunit__test_result_t ecunit__result;

void ecunit__init(const ecunit__config_t* config) {
    memcpy(&ecunit__config, config, sizeof(ecunit__config_t));
}

jmp_buf* __ecunit__get_p_jmp_buf() {
    return ecunit__config.p_jmp_buf;
}

void ecunit__begin_case(const char* name) {
    ecunit__result.name = name;
    ecunit__result.assertions_begin = ecunit__result.assertions_end = ecunit__config.assertions_buf;
    ecunit__result.assertions_wrapped_at = NULL;
    ecunit__result.failed = false;

    if (ecunit__config.on_set_up) {
        ecunit__config.on_set_up(name);
    }
}

void ecunit__end_case() {
    if (ecunit__config.on_tear_down) {
        ecunit__config.on_tear_down(&ecunit__result);
    }
}

static bool ecunit__append_assertion(const ecunit__assertion_t* p_record) {
    ecunit__assertion_t* const assertions_end = ecunit__config.assertions_buf + ecunit__config.assertions_buf_len;
    if (ecunit__result.assertions_end == assertions_end) {
        ecunit__result.assertions_end = ecunit__config.assertions_buf;
        ecunit__result.assertions_wrapped_at = assertions_end;
    }

    memcpy(ecunit__result.assertions_end, p_record, sizeof(ecunit__assertion_t));
    if (p_record->failed) {
        ecunit__result.failed = true;
    }
    ecunit__result.assertions_end++;

    if (p_record->failed && ecunit__config.p_jmp_buf) {
        longjmp(*ecunit__config.p_jmp_buf, 1);
    }
    return p_record->failed;
}

bool __ecunit__assert(bool assertion, const char* statement) {
    const ecunit__assertion_t record = {
        .failed = !assertion,
        .type = assertion_type__statement,
        .detail = {
            .statement = statement,
        },
    };
    return ecunit__append_assertion(&record);
}

bool __ecunit__cmp_int(int a, int b, ecunit__operator_t op);
bool __ecunit__cmp_uint(unsigned int a, unsigned int b, ecunit__operator_t op);

bool __ecunit__assert_cmp_int(int a, int b, ecunit__operator_t op) {
    ecunit__assertion_t record = {
        .type = assertion_type__cmp_int,
        .failed = __ecunit__cmp_int(a, b, op),
        .detail = {
            .cmp_int = {
                .a = a,
                .b = b,
                .op = op,
            },
        },
    };
    return ecunit__append_assertion(&record);
}

bool __ecunit__assert_cmp_uint(unsigned int a, unsigned int b, ecunit__operator_t op) {
    ecunit__assertion_t record = {
        .type = assertion_type__cmp_uint,
        .failed = __ecunit__cmp_uint(a, b, op),
        .detail = {
            .cmp_uint = {
                .a = a,
                .b = b,
                .op = op,
            },
        },
    };
    return ecunit__append_assertion(&record);
}
