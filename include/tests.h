#ifndef TESTS_H
#define TESTS_H

#ifdef ENABLE_TESTS

#include <iostream>
#include <cassert>

#define TEST(name) \
    static void test_##name(); \
    struct test_runner_##name { \
        test_runner_##name() { \
            std::cout << "Test: " << #name << " ... "; \
            test_##name(); \
            std::cout << "OK\n"; \
        } \
    } static test_instance_##name; \
    static void test_##name()
#else
#endif
#endif // TESTS_H