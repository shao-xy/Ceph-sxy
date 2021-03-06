// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#ifndef __SXY_COMMON_DELTATRACER_H_
#define __SXY_COMMON_DELTATRACER_H_

namespace sxy {

template <typename T>
class DeltaTracer {
    T last;
  public:
    DeltaTracer() {}
    virtual ~DeltaTracer() {}

    void set_last(T last_) { last = last_; }

    T get(T now) {
      T delta = now - last;
      last = now;
      return delta;
    }
    virtual void clear() {
      last = T();
    }
};

template <typename T, typename P>
class DeltaTracerWatcher {
    DeltaTracer<T> tracer;
    T last_delta;

  protected:
    P check_arg;

  public:
    DeltaTracerWatcher(P check_arg) : check_arg(check_arg) { }

    void init_now() {
      tracer.set_last(check_now(check_arg));
    }

    T get(bool update = false) {
      if (update) {
	last_delta = tracer.get(check_now(check_arg));
      }
      return last_delta;
    }

    virtual T check_now(P p) = 0;
};

}; /* namespace sxy */

#endif /* sxy/common/DeltaTracer.h */
