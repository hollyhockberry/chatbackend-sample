// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef CHATBACKEND_IMPLE_DF2301QWAKETRIGGER_HPP_
#define CHATBACKEND_IMPLE_DF2301QWAKETRIGGER_HPP_

#include <DFRobot_DF2301Q.h>
#include "modules/WakeTrigger.hpp"

namespace chat_backend::impl {

class DF2301QWakeTrigger : public modules::WakeTrigger {
  DFRobot_DF2301Q_I2C _df2301q;
 public:
  DF2301QWakeTrigger(TwoWire *wire = &Wire) : _df2301q(wire) {
  }
  void begin() override {
    _df2301q.begin();
    _df2301q.setMuteMode(1);
    _df2301q.setWakeTime(15);    
  }
  bool canWake() const override {
    return true;
  }
  bool record() override {
    return true;
  }
  bool listen() override {
    static unsigned long last = 0;
    const auto now = ::millis();
    if (now - last < 500) {
      return false;
    }
    last = now;
    const auto CMDID = _df2301q.getCMDID();
    if(0 != CMDID) {
      M5_LOGI("CMDID=%d", CMDID);
      return CMDID == 2;
    }
    return false;
  }
};

}  // chat_backend::impl

#endif  // CHATBACKEND_IMPLE_DF2301QWAKETRIGGER_HPP_
