// minimal_gtest.h
//
// A tiny GoogleTest-compatible shim. It implements just enough of the
// GoogleTest API to compile and run the test files in this project when a
// real GoogleTest installation is not available (e.g. in CI sandboxes).
//
// In a normal student setup with `apt install libgtest-dev` or vcpkg/conan,
// this file is NOT used - CMake links against real GTest. Tests compile
// either way because the surface API is identical.
//
// Supported macros: TEST, TEST_F, EXPECT_EQ, EXPECT_NE, EXPECT_TRUE,
// EXPECT_FALSE, EXPECT_STREQ, EXPECT_FLOAT_EQ, ASSERT_EQ, ASSERT_NE.

#pragma once

#include <cstring>
#include <iostream>
#include <sstream>
#include <set>
#include <string>
#include <vector>
#include <functional>
#include <cmath>

namespace testing {

class Test {
public:
    virtual ~Test() = default;
    virtual void SetUp() {}
    virtual void TearDown() {}
};

struct TestCase {
    const char* suite;
    const char* name;
    std::function<void()> body;
};

inline std::vector<TestCase>& allTests() {
    static std::vector<TestCase> v;
    return v;
}

struct Registrar {
    Registrar(const char* suite, const char* name, std::function<void()> body) {
        allTests().push_back({suite, name, std::move(body)});
    }
};

inline int runAllTests() {
    int passed = 0, failed = 0;
    for (const auto& t : allTests()) {
        std::cout << "[ RUN      ] " << t.suite << "." << t.name << "\n";
        try {
            t.body();
            std::cout << "[       OK ] " << t.suite << "." << t.name << "\n";
            ++passed;
        } catch (const std::exception& e) {
            std::cout << "[  FAILED  ] " << t.suite << "." << t.name
                      << "  -- " << e.what() << "\n";
            ++failed;
        } catch (...) {
            std::cout << "[  FAILED  ] " << t.suite << "." << t.name << "\n";
            ++failed;
        }
    }
    std::cout << "\n=== " << passed << " passed, " << failed << " failed, "
              << allTests().size() << " total ===\n";
    return failed == 0 ? 0 : 1;
}

} // namespace testing

// Sink that swallows trailing `<< "msg"` chains. We don't actually use the
// message text; this just makes the macros syntactically compatible with
// real GoogleTest's streaming idiom.
struct __mg_Sink { template <class T> __mg_Sink& operator<<(const T&) { return *this; } };

#define __MG_CHECK(cond, expr)                                                 \
    if (!(cond)) {                                                             \
        std::ostringstream __os;                                               \
        __os << "at " << __FILE__ << ":" << __LINE__ << " - " << expr;         \
        throw std::runtime_error(__os.str());                                  \
    }                                                                          \
    __mg_Sink{}

#define EXPECT_TRUE(x)         __MG_CHECK((x),                          "EXPECT_TRUE("  #x ")")
#define EXPECT_FALSE(x)        __MG_CHECK(!(x),                         "EXPECT_FALSE(" #x ")")
#define EXPECT_EQ(a,b)         __MG_CHECK((a)==(b),                     "EXPECT_EQ("    #a ", " #b ")")
#define EXPECT_NE(a,b)         __MG_CHECK((a)!=(b),                     "EXPECT_NE("    #a ", " #b ")")
#define ASSERT_EQ(a,b)         __MG_CHECK((a)==(b),                     "ASSERT_EQ("    #a ", " #b ")")
#define ASSERT_NE(a,b)         __MG_CHECK((a)!=(b),                     "ASSERT_NE("    #a ", " #b ")")
#define EXPECT_STREQ(a,b)      __MG_CHECK(((a)&&(b)&&std::strcmp((a),(b))==0), "EXPECT_STREQ(" #a ", " #b ")")
#define EXPECT_FLOAT_EQ(a,b)   __MG_CHECK(std::abs(double((a))-double((b)))<1e-5, "EXPECT_FLOAT_EQ(" #a ", " #b ")")

// TEST(suite, name): body becomes a free function executed via Registrar.
#define TEST(suite, name)                                                      \
    static void suite##_##name##_body();                                       \
    static testing::Registrar suite##_##name##_reg(                            \
        #suite, #name, [](){ suite##_##name##_body(); });                      \
    static void suite##_##name##_body()

// TEST_F(fixture, name): instantiate fixture, call SetUp via a member
// shim (so we don't need access to the protected SetUp/TearDown from a
// lambda), run body, call TearDown.
#define TEST_F(fixture, name)                                                  \
    struct fixture##_##name##_T : fixture {                                    \
        void TestBody();                                                       \
        void __run() {                                                         \
            this->SetUp();                                                     \
            try { TestBody(); } catch(...) { this->TearDown(); throw; }        \
            this->TearDown();                                                  \
        }                                                                      \
    };                                                                         \
    static testing::Registrar fixture##_##name##_reg(                          \
        #fixture, #name, []() { fixture##_##name##_T t; t.__run(); });         \
    void fixture##_##name##_T::TestBody()
