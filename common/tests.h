#pragma once

#include <unordered_map>
#include <string>
#include <sstream>
#include <iostream>
#include <type_traits>

#include "helpers.h"

/** Defines new test.

    The test must specify the suite name and the test name as identifiers and may optionally specify accessors, i.e. classes that the test will inherit from.

    TODO The definition uses the __VA_OPT__ feature from C++20 when compiled with Clang, the default behavior on msvc and the GNU extension of ## __VA_ARGS__ if compiled with gcc. This is a mess, and as soon as __VA_OPT__ is supported on all three compilers that version should be used exclusively.
 */
#if (defined ARCH_MACOS) && (defined __clang__)
#define TEST(SUITE_NAME, TEST_NAME, ...) \
    class Test_ ## SUITE_NAME ## _ ## TEST_NAME : public tiny::Test __VA_OPT__(,) __VA_ARGS__ { \
    private: \
        Test_ ## SUITE_NAME ## _ ## TEST_NAME (char const * suiteName, char const * testName): \
            tiny::Test(suiteName, testName) { \
        } \
        void run_() override; \
        static Test_ ## SUITE_NAME ## _ ## TEST_NAME singleton_; \
    } \
    Test_ ## SUITE_NAME ## _ ## TEST_NAME ::singleton_{# SUITE_NAME, # TEST_NAME }; \
    inline void Test_ ## SUITE_NAME ## _ ## TEST_NAME ::run_()
#else
#define TEST(SUITE_NAME, TEST_NAME, ...) \
    class Test_ ## SUITE_NAME ## _ ## TEST_NAME : public tiny::Test, ## __VA_ARGS__ { \
    private: \
        Test_ ## SUITE_NAME ## _ ## TEST_NAME (char const * suiteName, char const * testName): \
            tiny::Test(suiteName, testName) { \
        } \
        void run_() override; \
        static Test_ ## SUITE_NAME ## _ ## TEST_NAME singleton_; \
    } \
    Test_ ## SUITE_NAME ## _ ## TEST_NAME ::singleton_{# SUITE_NAME, # TEST_NAME }; \
    inline void Test_ ## SUITE_NAME ## _ ## TEST_NAME ::run_()
#endif
/** Checks that given assumption holds, and fails the test immediately if it does not.
 */

#define CHECK(...) if (expectTrue(__FILE__, __LINE__, # __VA_ARGS__, __VA_ARGS__)) {} else THROW(tiny::CheckFailure())
#define CHECK_EQ(...) if (expectEq(__FILE__, __LINE__, # __VA_ARGS__, __VA_ARGS__)) {} else THROW(tiny::CheckFailure())
#define CHECK_NULL(...) if (expectNull(__FILE__, __LINE__, # __VA_ARGS__, __VA_ARGS__)) {} else THROW(tiny::CheckFailure())

/** Check that the given assumption holds, but does not fail the test immediately if the assumption fails.
 */
#define EXPECT(...) if (expectTrue(__FILE__, __LINE__, # __VA_ARGS__, __VA_ARGS__)) {} else {}
#define EXPECT_EQ(...) if (expectEq(__FILE__, __LINE__, # __VA_ARGS__, __VA_ARGS__)) {} else {}
#define EXPECT_NULL(...) if (expectNull(__FILE__, __LINE__, # __VA_ARGS__, __VA_ARGS__)) {} else {}

/** Checks that given expression throws an exception of given type, or its subtype
 */
#define CHECK_THROWS(TYPE, ...) try { addCheck(); __VA_ARGS__;  addFailedException(__FILE__, __LINE__, # TYPE); THROW(tiny::CheckFailure()); } catch (tiny::CheckFailure & e) { throw e; } catch (TYPE const &) { }

#define EXPECT_THROWS(TYPE, ...)  try { addCheck(); __VA_ARGS__;  addFailedException(__FILE__, __LINE__, # TYPE); } catch (TYPE const &) { }

namespace tiny {

    /** Exception thrown by CHECK macros to immediately end the test execution.
     */
    class CheckFailure : public std::runtime_error {
    };

    class TestSuite;

    /** A single test.
     */
    class Test {
    public:
        Test(std::string const & suiteName, std::string const & testName);

        virtual ~Test() = default;

        std::string const & name() const {
            return name_;
        }

        TestSuite & suite() const {
            return suite_;
        }

        /** Executes the given test.
         */
        bool run(std::ostream & s);

    protected:
        void addCheck() {
            ++report_->checks;
        }

        void addFailedException(char const * file, size_t line, char const * exceptionType) {
            ++report_->failedChecks;
            report_->output << "Expected exception " << exceptionType << ", but none thrown." << std::endl << "  at " << file << "(" << line << ")" << std::endl;
        }

        template<typename T>
        bool expectTrue(char const * file, size_t line, char const * expr, T const & x) {
            ++report_->checks;
            if (x)
                return true;
            ++report_->failedChecks;
            report_->output << "Expected true in " << expr << ", but value " << x << " found at " << file << "(" << line << ")" << std::endl;
            return false;
        }

        template<typename T>
        bool expectNull(char const * file, size_t line, char const * expr, T const & x) {
            ++report_->checks;
            if (x == nullptr)
                return true;
            ++report_->failedChecks;
            report_->output << "Expected nullptr in " << expr << ", but value " << x << " found at " << file << "(" << line << ")" << std::endl;
            return false;
        }

        template<typename T, typename W>
        typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value, bool>::type expectEq(char const * file, size_t line, char const * expr, T const & x, W const & y) {
            ++report_->checks;
            if (x == static_cast<typename std::make_signed<W>::type>(y))
                return true;
            ++report_->failedChecks;
            report_->output << "Expected equality in " << expr << ", but values " << x << " and " << y << " found at " << file << "(" << line << ")" << std::endl;
            return false;
        }

        template<typename T, typename W>
        typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value && ! std::is_same<T, bool>::value, bool>::type expectEq(char const * file, size_t line, char const * expr, T const & x, W const & y) {
            ++report_->checks;
            if (x == static_cast<typename std::make_unsigned<W>::type>(y))
                return true;
            ++report_->failedChecks;
            report_->output << "Expected equality in " << expr << ", but values " << x << " and " << y << " found at " << file << "(" << line << ")" << std::endl;
            return false;
        }

        template<typename T, typename W>
        typename std::enable_if<! std::is_integral<T>::value || std::is_same<T, bool>::value, bool>::type expectEq(char const * file, size_t line, char const * expr, T const & x, W const & y) {
            ++report_->checks;
            if (x == y)
                return true;
            ++report_->failedChecks;
            report_->output << "Expected equality in " << expr << ", but values " << x << " and " << y << " found at " << file << "(" << line << ")" << std::endl;
            return false;
        }


    private:

        class Report {
        public:
            std::stringstream output;
            size_t duration = 0;
            size_t checks = 0;
            size_t failedChecks = 0;
            std::stringstream unhandledException;
        };

        virtual void run_() = 0;

        std::string name_;
        TestSuite & suite_;
        Report * report_;

    }; // Test

    /** A test suite is a collection of tests.
     */
    class TestSuite {
    public:
        std::string const & name() const {
            return name_;
        }

        size_t size() const {
            return tests_.size();
        }

        bool run(std::ostream & s) {
            s << "==== Suite " << name_ << " (" << tests_.size() << " tests):" << std::endl;
            size_t failed = 0;
            for (auto & i : tests_) {
                Test * t = i.second;
                if (! t->run(s))
                  ++failed;
            }
            return failed == 0;
        }

    private:
        friend class Test;
        friend class Tests;

        explicit TestSuite(std::string const & name):
            name_(name) {
        }

        std::string name_;

        std::unordered_map<std::string, Test *> tests_;

    }; // TestSuite

    /** Base static class for management and running of the tests.
     */
    class Tests {
    public:
        /** Runs the tests.
         */
        static int RunAll(int argc, char * argv[]) {
            // TODO actually implement logging, disabling test suites, etc.
            MARK_AS_UNUSED(argc);
            MARK_AS_UNUSED(argv);
            std::ostream & s = std::cout;
            size_t failed = 0;
            for (auto & i : TestSuites_()) {
                TestSuite * suite = i.second;
                if (!suite->run(s))
                    ++failed;
            }
            return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
        }

    private:

        friend class Test;
        friend class TestSuite;

        /** Returns the list of test suites.
         */
        static std::unordered_map<std::string, TestSuite *> & TestSuites_() {
            static std::unordered_map<std::string, TestSuite *> testSuites_;
            return testSuites_;
        }

        static TestSuite & GetOrCreateSuite_(std::string const & name) {
            auto & testSuites = TestSuites_();
            auto i = testSuites.find(name);
            if (i == testSuites.end())
                i = testSuites.insert(std::make_pair(name, new TestSuite(name))).first;
            return * (i->second);
        }

    }; // Tests

    inline Test::Test(std::string const & suiteName, std::string const & testName):
        name_(testName),
        suite_(Tests::GetOrCreateSuite_(suiteName)) {
        suite_.tests_.insert(std::make_pair(testName, this));
    }

    inline bool Test::run(std::ostream & s) {
        report_ = new Report();
        try {
            run_();
        } catch (CheckFailure const &) {
        } catch (std::exception const & e) {
            report_->unhandledException << e.what();
        } catch (...) {
            report_->unhandledException << "Unknown exception";
        }
        std::string unhandledException = report_->unhandledException.str();
        bool result = report_->failedChecks == 0 && unhandledException.empty();
        s << (result ? "PASS " : "FAIL ") << suite_.name() << "." << name_ << " checks: " << report_->checks << "/" << report_->failedChecks << std::endl;
        if (!result) {
            std::string output = report_->output.str();
            if (! output.empty())
                s << output;
            if (! unhandledException.empty())
                s << "Unhandled exception: " << std::endl << "   " << unhandledException << std::endl;
        }
        delete report_;
        return result;
    }

} // namespace tiny

