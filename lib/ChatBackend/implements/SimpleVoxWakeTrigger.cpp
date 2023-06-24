// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include <M5Unified.h>
#include <memory>
#include <SPIFFS.h>
#include "SimpleVoxWakeTrigger.hpp"

#define base_path "/spiffs"
#define file_name "/wakeword.bin"

namespace {
constexpr int kSampleRate = 16000;
constexpr int kRxBufferNum = 3;
constexpr int audioLength = kSampleRate * 3;  // 3 seconds
}  // namespace

namespace chat_backend::impl {

void SimpleVoxWakeTrigger::begin() {
  auto vadConfig = vadEngine.config();
  vadConfig.sample_rate = kSampleRate;
  auto mfccConfig = mfccEngine.config();
  mfccConfig.sample_rate = kSampleRate;

  constexpr uint32_t memCaps = (CONFIG_SPIRAM) ? (MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM) : MALLOC_CAP_8BIT;
  rawAudio = (int16_t*)heap_caps_malloc(audioLength * sizeof(*rawAudio), memCaps);
  rxBuffer = (int16_t*)heap_caps_malloc(kRxBufferNum * vadConfig.frame_length() * sizeof(*rxBuffer), MALLOC_CAP_8BIT);

  nsInst = ::ns_pro_create(vadConfig.frame_time_ms, 1, vadConfig.sample_rate);
  if (nsInst == nullptr) {
    M5_LOGE("Failed to initialize ns.");
    while(true) delay(10);
  }
  if (!vadEngine.init(vadConfig)) {
    M5_LOGE("Failed to initialize vad.");
    while(true) delay(10);
  }
  if (!mfccEngine.init(mfccConfig)) {
    M5_LOGE("Failed to initialize mfcc.");
    while(true) delay(10);
  }
  
  //TODO(hollyhockberry): 
  SPIFFS.begin(true);
  if (SPIFFS.exists(file_name)) {
    M5_LOGV("File exists !!");
    mfcc = mfccEngine.loadFile(base_path file_name);
  }
}

bool SimpleVoxWakeTrigger::canWake() const {
  return mfcc != nullptr;
}

bool SimpleVoxWakeTrigger::record() {
  while (true) {
    auto length = recordRaw();
    if (length > 0) {
      if (mfcc != nullptr) {
        delete mfcc;
      }
      mfcc = mfccEngine.create(rawAudio, length);
      if (mfcc) {
        mfccEngine.saveFile(base_path file_name, *mfcc);
        vadEngine.reset();
        M5_LOGV("done");
        return true;
      }
      vadEngine.reset();
    }
    ::vTaskDelay(10);
  }
}

bool SimpleVoxWakeTrigger::listen() {
  if (!canWake()) {
    return false;
  }
  auto length = recordRaw();
  if (length > 0) {
    std::unique_ptr<simplevox::MfccFeature> feature(mfccEngine.create(rawAudio, length));
    const auto dist = simplevox::calcDTW(*mfcc, *feature);
    M5_LOGD("dist: %d", dist);
    vadEngine.reset();
    return dist < 220;
  }
  return false;
}

int16_t* SimpleVoxWakeTrigger::rxMic() {
  static int rxIndex = 0;
  const int frameLength = vadEngine.config().frame_length();
  if (!M5.Mic.record(&rxBuffer[frameLength * rxIndex], frameLength, kSampleRate)) {
    return nullptr;
  }
  rxIndex++;
  rxIndex = (rxIndex >= kRxBufferNum) ? 0 : rxIndex;
  return &rxBuffer[rxIndex * frameLength];
}

int SimpleVoxWakeTrigger::recordRaw() {
  auto* data = rxMic();
  if (data == nullptr) {
    return -1;
  }
  ::ns_process(nsInst, data, data);
  return vadEngine.detect(rawAudio, audioLength, data);
}

}  // chat_backend::impl
