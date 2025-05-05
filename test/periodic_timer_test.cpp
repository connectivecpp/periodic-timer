/** @file
 *
 * @brief Test scenarios for @c periodic_timer class template.
 *
 * @author Cliff Green
 *
 * @copyright (c) 2017-2025 by Cliff Green
 *
 * Distributed under the Boost Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#define CATCH_CONFIG_ENABLE_CHRONO_STRINGMAKER

#include "catch2/catch_test_macros.hpp"


#include <chrono>
#include <thread>
#include <optional>
#include <system_error>

#include "asio/executor_work_guard.hpp"

#include "timer/periodic_timer.hpp"

constexpr int Expected = 9;
int count = 0;

template <typename D>
bool lambda_util (std::error_code err, D elap) {
  ++count;
  INFO ("count = " << count << ", err code = " << err.value() << ", " << err.message());
  return count < Expected;
}

using wk_guard = asio::executor_work_guard<asio::io_context::executor_type>;

void wait_util (std::chrono::milliseconds ms, wk_guard& wg, std::thread& thr) {
  std::this_thread::sleep_for(ms);
  wg.reset();
  thr.join();
}


template <typename Clock>
void test_util () {

  SECTION ( "Setup clock and duration") {

    asio::io_context ioc;
    chops::periodic_timer<Clock> timer {ioc};
    wk_guard wg { asio::make_work_guard(ioc) };

    std::thread thr([&ioc] () { ioc.run(); } );
    count = 0;

    SECTION ( "100 ms duration" ) {
      auto test_dur { 100 };
      timer.start_duration_timer(std::chrono::milliseconds(test_dur),
        [] (std::error_code err, typename Clock::duration elap) { 
          return lambda_util(err, elap);
        }
      );

      wait_util (std::chrono::milliseconds((Expected+1)*test_dur), wg, thr);

      REQUIRE (count == Expected);
    }
    SECTION ( "200 ms duration, start time is 2 seconds in the future" ) {
      auto test_dur { 200 };
      timer.start_duration_timer(std::chrono::milliseconds(test_dur), Clock::now() + std::chrono::seconds(2),
        [] (std::error_code err, typename Clock::duration elap) { 
          return lambda_util(err, elap);
        }
      );

      wait_util(std::chrono::milliseconds((Expected+1)*test_dur + 2000), wg, thr);

      REQUIRE (count == Expected);
    }
    SECTION ( "100 ms duration, timer pops on timepoints" ) {
      auto test_dur { 100 };
      timer.start_timepoint_timer(std::chrono::milliseconds(test_dur),
        [] (std::error_code err, typename Clock::duration elap) { 
          return lambda_util(err, elap);
        }
      );

      wait_util (std::chrono::milliseconds((Expected+1)*test_dur), wg, thr);

      REQUIRE (count == Expected);
    }
    SECTION ( "200 ms duration, timer pops on timepoints starting 2 seconds in the future" ) {
      auto test_dur { 200 };
      timer.start_timepoint_timer(std::chrono::milliseconds(test_dur), Clock::now() + std::chrono::seconds(2),
        [] (std::error_code err, typename Clock::duration elap) { 
          return lambda_util(err, elap);
        }
      );

      wait_util(std::chrono::milliseconds((Expected+1)*test_dur + 2000), wg, thr);

      REQUIRE (count == Expected);
    }

  }
}

TEST_CASE ( "Steady clock periodic timer", "[periodic_timer] [steady_clock]" ) {

  test_util<std::chrono::steady_clock>();

}
TEST_CASE ( "System clock periodic timer", "[periodic_timer] [system_clock]" ) {

  test_util<std::chrono::system_clock>();

}
TEST_CASE ( "High resolution clock periodic timer", "[periodic_timer] [high_resolution_clock]" ) {

  test_util<std::chrono::high_resolution_clock>();

}

