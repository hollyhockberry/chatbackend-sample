// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef CHATBACKEND_IMPLE_SIMPLEVOXAWAKE_HPP_
#define CHATBACKEND_IMPLE_SIMPLEVOXAWAKE_HPP_

#include <esp_ns.h>
#include <simplevox.h>
#include "modules/WakeTrigger.hpp"

namespace chat_backend::impl {

class SimpleVoxWakeTrigger : public modules::WakeTrigger {
  int16_t* rawAudio;
  int16_t* rxBuffer;
  ns_handle_t nsInst;
  simplevox::VadEngine vadEngine;
  simplevox::MfccEngine mfccEngine;
  simplevox::MfccFeature* mfcc = nullptr;

 public:
  void begin() override;
  bool canWake() const override;

  bool record() override;
  bool listen() override;

 private:
  int16_t* rxMic();
  int recordRaw();
};

}  // chat_backend::impl

#endif  // CHATBACKEND_IMPLE_SIMPLEVOXAWAKE_HPP_
