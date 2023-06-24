// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include <M5Unified.h>
#include "sample/Sample.hpp"

namespace {
constexpr const char* OPENAI_APIKEY = "*** YOUR API KEY ***";
constexpr const char* VOICEVOX_APIKEY = nullptr;
}  // namespace

sample::Sample backend;

void setup() {
  M5.Log.setLogLevel(m5::log_target_serial, ESP_LOG_VERBOSE);
  backend.begin(OPENAI_APIKEY, VOICEVOX_APIKEY);
}

void loop() {
 backend.loop();
}
