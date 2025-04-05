/** @mainpage Periodic Timer, Lightweight Timer Utility Using Asio
 *
 * ## Overview
 *
 * The @c periodic_timer class template is an asynchronous periodic timer that wraps
 * and simplifies Asio timers when periodic callbacks are needed. The periodicity can
 * be based on either a simple duration or on timepoints based on a duration. When the
 * timer is started, the application specifies whether each callback is invoked based
 * on a duration (e.g. one second after the last callback), or on timepoints (e.g.
 * a callback will be invoked each second according to the clock).
 *
 * The Asio library does not directly provide periodic timers. Instead, application 
 * code using Asio must chain together function object callbacks until satisfied.
 *
 * The @c periodic_timer class template is a thin wrapper supplying the chaining, with 
 * two options for the periodicity. In addition, elapsed times are computed and provided 
 * to the application supplied callback.
 *
 * One periodicity option is to invoke the application callback after a duration. It is 
 * simple and provides consistency, specially if the system clock is allowed to be adjusted.
 *
 * The second periodicity option is to invoke the application callback based on 
 * timepoints. This works well in environments where activities must be processed on regular 
 * time intervals, regardless of how much processing is performed in each application 
 * callback. For example, an application may desire a timer callback to be invoked once 
 * every 500 milliseconds and the callback takes 15 milliseconds to excecute. Also 
 * occasionally the operating environment takes an extra 10 or 20 milliseconds before 
 * invoking the callback. Using a duration instead of a timepoint the actual interval is 
 * now 515 milliseconds between callback invocations, with occasional intervals up to 535 
 * milliseconds. A timepoint timer will instead invoke the callback every 500 milliseconds 
 * (depending on operating system precision and load) regardless of how much time is taken 
 * by the previous callback.
 *
 * The main disadvantange of timepoint based callbacks is that they can be affected by 
 * system clock adjustments. In addition, if the timepoint interval is small and a large 
 * amount of processing is performed by the callback, "overflow" can occur, where the next 
 * timepoint callback is overrun by the current processing.
 *
 * An excellent article on this topic by Tony DaSilva can be [read here]
 * (https://bulldozer00.blog/2013/12/27/periodic-processing-with-standard-c11-facilities/).
 *
 * An asynchronous timer is more resource-friendly regarding system resources than creating a 
 * thread that sleeps. In particular, creating hundreds or thousands of timers is very 
 * expensive in a "thread per timer" design.
 *
 * ### Additional Details
 *
 * @c periodic_timer is a class template corresponding to a single timer object. Designs 
 * needing many periodic timer objects, possibly sorted by time (e.g. a priority queue based 
 * on a time stamp) will need to create their own containers and data structures, using 
 * @c periodic_timer as a container element.
 *
 * Asynchronous processing is performed by the Asio @c io_context (C++ executor context) passed 
 * in to the constructor by the application.
 * 
 * A @c periodic_timer stops when the application supplied function object 
 * returns @c false rather than @c true.
 *
 * A periodic timer can be used as a "one-shot" timer by finishing after 
 * one invocation (i.e. unconditionally return @c false from the function 
 * object).
 *
 * @note This class does not perform "this" reference counting. It is up to 
 * the application code to guarantee that a @c periodic_timer has not been 
 * destructed before handlers (function object callbacks) are invoked.
 *
 * A common idiom is to use @c std::enable_shared_from_this, call 
 * @c std::shared_from_this, and store the result in the function object 
 * callback object.
 *  
 * @note @c std::chrono facilities seem to be underspecified on @c noexcept,
 * very few of the functions in @c periodic_timer are @c noexcept.
 *
 * @author Cliff Green
 *
 * @copyright (c) 2017-2025 by Cliff Green
 *
 * Distributed under the Boost Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef PERIODIC_TIMER_HPP_INCLUDED
#define PERIODIC_TIMER_HPP_INCLUDED

#include "asio/basic_waitable_timer.hpp"
#include "asio/io_context.hpp"

#include <chrono>
#include <system_error>
#include <utility> // std::move, std::forward

namespace chops {

template <typename Clock = std::chrono::steady_clock>
class periodic_timer {
public:

  using duration = typename Clock::duration;
  using time_point = typename Clock::time_point;

private:

  asio::basic_waitable_timer<Clock> m_timer;

private:
  template <typename F>
  void duration_handler_impl(const time_point& last_tp, const duration& dur, 
                             const std::error_code& err, F&& func) {
    time_point now_time { Clock::now() };
    // pass err and elapsed time to app function obj
    if (!func(err, now_time - last_tp) || 
        err == asio::error::operation_aborted) {
      return; // app is finished with timer for now or timer was cancelled
    }
    m_timer.expires_after(dur);
    m_timer.async_wait( [now_time, dur, f = std::move(func), this]
            (const std::error_code& e) {
        duration_handler_impl(now_time, dur, e, std::move(f));
      }
    );
  }
  template <typename F>
  void timepoint_handler_impl(const time_point& last_tp, const duration& dur, 
                              const std::error_code& err, F&& func) {
    // pass err and elapsed time to app function obj
    if (!func(err, (Clock::now() - last_tp)) || 
        err == asio::error::operation_aborted) {
      return; // app is finished with timer for now or timer was cancelled
    }
    m_timer.expires_at(last_tp + dur + dur);
    m_timer.async_wait( [f = std::move(func), last_tp, dur, this]
            (const std::error_code& e) {
        timepoint_handler_impl(last_tp+dur, dur, e, std::move(f));
      }
    );
  }

public:

  /**
   * Construct a @c periodic_timer with an @c io_context. Other information such as duration 
   * will be supplied when @c start is called.
   *
   * Constructing a @c periodic_timer does not start the actual timer. Calling one of the 
   * @c start methods starts the timer.
   *
   * The application supplied function object for any of the @c start methods requires the 
   * following signature:
   * @code
   *   bool (std::error_code, duration);
   * @endcode
   *
   * The @c duration parameter provides an elapsed time from the previous callback.
   *
   * The clock for the asynchronous timer defaults to @c std::chrono::steady_clock.
   * Other clock types can be used if desired (e.g. @c std::chrono::high_resolution_clock 
   * or @c std::chrono::system_clock). Note that some clocks allow time to be externally 
   * adjusted, which may influence the interval between the callback invocation.
   *
   * Move semantics are allowed for this type, but not copy semantics. When a move 
   * construction or move assignment completes, all timers are cancelled with 
   * appropriate notification, and @c start will need to be called.
   *
   * @param ioc @c io_context for asynchronous processing.
   *
   */
  explicit periodic_timer(asio::io_context& ioc) noexcept : m_timer(ioc) { }

  periodic_timer() = delete; // no default ctor

  // disallow copy construction and copy assignment
  periodic_timer(const periodic_timer&) = delete;
  periodic_timer& operator=(const periodic_timer&) = delete;

  // allow move construction and move assignment
  periodic_timer(periodic_timer&&) = default;
  periodic_timer& operator=(periodic_timer&& rhs) {
    m_timer.cancel();
    m_timer = std::move(rhs.m_timer);
  }

  // modifying methods

  /**
   * Start the timer, and the application supplied function object will be invoked 
   * after an amount of time specified by the duration parameter.
   *
   * The function object will continue to be invoked as long as it returns @c true.
   *
   * @param dur Interval to be used between callback invocations.
   *
   * @param func Function object to be invoked. 
   *
   */
  template <typename F>
  void start_duration_timer(const duration& dur, F&& func) {
    m_timer.expires_after(dur);
    m_timer.async_wait( [dur, f = std::move(func), this] (const std::error_code& e) {
        duration_handler_impl(Clock::now(), dur, e, std::move(f));
      }
    );
  }
  /**
   * Start the timer, and the application supplied function object will be invoked 
   * first at a specified time point, then afterwards as specified by the duration 
   * parameter.
   *
   * The function object will continue to be invoked as long as it returns @c true.
   *
   * @param dur Interval to be used between callback invocations.
   *
   * @param when Time point when the first timer callback will be invoked.
   *
   * @param func Function object to be invoked.
   *
   */
  template <typename F>
  void start_duration_timer(const duration& dur, const time_point& when, F&& func) {
    m_timer.expires_at(when);
    m_timer.async_wait( [dur, f = std::move(func), this] (const std::error_code& e) {
        duration_handler_impl(Clock::now(), dur, e, std::move(f));
      }
    );
  }
  /**
   * Start the timer, and the application supplied function object will be invoked 
   * on timepoints with an interval specified by the duration.
   *
   * The function object will continue to be invoked as long as it returns @c true.
   *
   * @param dur Interval to be used between callback invocations.
   *
   * @param func Function object to be invoked. 
   *
   */
  template <typename F>
  void start_timepoint_timer(const duration& dur, F&& func) {
    start_timepoint_timer(dur, (Clock::now() + dur), std::forward<F>(func));
  }
  /**
   * Start the timer on the specified timepoint, and the application supplied function object 
   * will be invoked on timepoints with an interval specified by the duration.
   *
   * The function object will continue to be invoked as long as it returns @c true.
   *
   * @param dur Interval to be used between callback invocations.
   *
   * @param when Time point when the first timer callback will be invoked.
   *
   * @param func Function object to be invoked. 
   *
   * @note The elapsed time for the first callback invocation is artificially set to the 
   * duration interval.
   */
  template <typename F>
  void start_timepoint_timer(const duration& dur, const time_point& when, F&& func) {
    m_timer.expires_at(when);
    m_timer.async_wait( [when, dur, f = std::move(func), this]
            (const std::error_code& e) {
        timepoint_handler_impl((when-dur), dur, e, std::move(f));
      }
    );
  }

  /**
   * Cancel the timer. The application function object will be called with an 
   * "operation aborted" error code.
   *
   * A cancel may implicitly be called if the @c periodic_timer object is move copy 
   * constructed or move assigned.
   */
  void cancel() {
    m_timer.cancel();
  }
};

} // end namespace

#endif

