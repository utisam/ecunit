#include <cassert>
#include <array>
#include <iostream>
#include <limits>
#include <ecunit.h>
#include <ecunit-runner.h>

using namespace std;

void on_set_up(const char* name) {
    cout << "set up: " << name << endl;
}

const char* failed_string(bool failed) {
    return (failed ? "Failed" : "OK");
}

void on_tear_down(const ecunit__test_result_t* result) {
    cout << "tear down: " << result->name << " ... " << failed_string(result->failed) << endl;

    if (result->assertions_wrapped_at) {
        cout << "\tsome assertions are omitted" << endl;
    }
    ecunit__test_result_iter_t iter;
    for (ecunit__test_result_iter__init(&iter, result); ecunit__test_result_iter__condition(&iter, result); ecunit__test_result_iter__next(&iter, result)) {
        switch (iter.ptr->type) {
        case assertion_type__statement:
            cout << "\tassertion: " << iter.ptr->detail.statement << " ... " << failed_string(iter.ptr->failed) << endl;
            break;
        case assertion_type__cmp_int:
            cout << "\tassertion: " << iter.ptr->detail.cmp_int.a << ' ' << ecunit__operator_string(iter.ptr->detail.cmp_int.op) << ' ' << iter.ptr->detail.cmp_int.b << " ... " << failed_string(iter.ptr->failed) << endl;
            break;
        case assertion_type__cmp_uint:
            cout << "\tassertion: " << iter.ptr->detail.cmp_uint.a << ' ' << ecunit__operator_string(iter.ptr->detail.cmp_uint.op) << ' ' << iter.ptr->detail.cmp_uint.b << " ... " << failed_string(iter.ptr->failed) << endl;
            break;
        default:
            cout << "\tassertion ... " << failed_string(iter.ptr->failed) << endl;
            break;
        }
    }
}

ECUNIT_RUNNER__BEGIN_SUITE(sample_suite) {
    ECUNIT_RUNNER__BEGIN_CASE("many assertions") {
        assert(ECUNIT__ASSERT(1 < 1));
        assert(!ECUNIT__ASSERT(1 < 2));
        assert(!ECUNIT__ASSERT(1 < 3));
        assert(!ECUNIT__ASSERT(1 < 4));
    } ECUNIT_RUNNER__END_CASE

    ECUNIT_RUNNER__BEGIN_CASE("use compare") {
        assert(!ECUNIT__ASSERT_CMP_INT(1, ==, 1));
        assert(!ECUNIT__ASSERT_CMP_INT(1, <, 2));
        assert(!ECUNIT__ASSERT_CMP_INT(1, <=, 3));
        assert(ECUNIT__ASSERT_CMP_INT(1, >, 4));
        assert(ECUNIT__ASSERT_CMP_INT(1, >=, 4));
    } ECUNIT_RUNNER__END_CASE
} ECUNIT_RUNNER__END_SUITE

int main() {
    static array<ecunit__assertion_t, 3> assertions_buf;

    const ecunit__config_t config = {
        .on_set_up = on_set_up,
        .on_tear_down = on_tear_down,
        .assertions_buf = assertions_buf.data(),
        .assertions_buf_len = assertions_buf.size(),
    };
    ecunit__init(&config);

    ecunit_runnner__run_suite(sample_suite);

    ECUNIT__BEGIN_CASE("No runner (continue even if fail)") {
        // assert is only for test ecunit itself. No need in actual use.
        assert(ECUNIT__ASSERT(1 < 1));
        assert(!ECUNIT__ASSERT(1 < 2));
    } ECUNIT__END_CASE

    ECUNIT__BEGIN_CASE("Signed/Unsigned") {
        assert(!ECUNIT__ASSERT_CMP_INT(numeric_limits<unsigned int>::max(), <, numeric_limits<int>::max()));
        assert(ECUNIT__ASSERT_CMP_UINT(numeric_limits<unsigned int>::max(), <, numeric_limits<int>::max()));
    } ECUNIT__END_CASE

    jmp_buf env;
    const ecunit__config_t config_with_jmp_buf = {
        .on_set_up = on_set_up,
        .on_tear_down = on_tear_down,
        .assertions_buf = assertions_buf.data(),
        .assertions_buf_len = assertions_buf.size(),
        .p_jmp_buf = &env,
    };
    ecunit__init(&config_with_jmp_buf);

    ECUNIT__BEGIN_CASE("No runner with longjmp") {
        assert(ECUNIT__ASSERT(1 < 1));
        assert(!ECUNIT__ASSERT(1 < 2)); // Not executed
    } ECUNIT__END_CASE

    // Run suite with longjmp
    ecunit_runnner__run_suite(sample_suite);

    return 0;
}