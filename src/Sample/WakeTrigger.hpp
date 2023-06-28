// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef SAMPLE_WAKETRIGGER_HPP_
#define SAMPLE_WAKETRIGGER_HPP_

#include <Avatar.h>
#include <modules/WakeTrigger.hpp>
#include <implements/SimpleVoxWakeTrigger.hpp>
#include <implements/DF2301QWakeTrigger.hpp>

namespace sample {

class WakeTrigger : public chat_backend::modules::WakeTrigger {
  chat_backend::impl::SimpleVoxWakeTrigger _simple;
  chat_backend::impl::DF2301QWakeTrigger _df2301q;
  chat_backend::modules::WakeTrigger* _wake;
 public:
  WakeTrigger() {
    _wake =
#ifdef USE_DF2301Q
        &_df2301q;
#else  // USE_DF2301Q
        &_simple;
#endif  // USE_DF2301Q
  }
  void begin() override {
    return _wake->begin();
  }
  bool canWake() const override {
    return _wake->canWake();
  }
  bool record() override {
    return _wake->record();
  }
  bool listen() override {
    return _wake->listen();
  }
};

}  // sample

#endif  // SAMPLE_WAKETRIGGER_HPP_
