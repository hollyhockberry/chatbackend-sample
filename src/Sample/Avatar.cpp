// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include "Avatar.hpp"

namespace sample {

Avatar* Avatar::_instance = nullptr;

Avatar::Avatar() {
  _instance = this;
}

void Avatar::begin() {
  _balloonText = "";
  _balloonTime = 0;
  _avatar.init();
  _avatar.addTask([](void* arg){ _instance->lipSync(arg); }, "lipsync");
  _avatar.setSpeechFont(&fonts::efontJA_14);
  ::xTaskCreate(main, "avatar", 2048, this, 1, nullptr);
}

void Avatar::assignAudio(AudioOutputM5Speaker* audio) {
  _audio = audio;
}

void Avatar::main(void* arg) {
  auto task = static_cast<Avatar*>(arg);
  while (true) {
    task->loop();
    ::vTaskDelay(100);
  }
}

void Avatar::loop() {
  if (_balloonTime >= 0 &&  (::millis() - _balloonTime) > 3000) {
    _balloonTime = 0;
    _balloonText = "";
    _avatar.setSpeechText(_systemMessage.isEmpty()
          ? _balloonText.c_str()
          : _systemMessage.c_str());
  }
}

void Avatar::expression(const String& content) {
  if (content.equals("Happy")) {
    _avatar.setExpression(m5avatar::Expression::Happy);
  }
  else if (content.equals("Angry")) {
    _avatar.setExpression(m5avatar::Expression::Angry);
  }
  else if (content.equals("Sad")) {
    _avatar.setExpression(m5avatar::Expression::Sad);
  }
  else if (content.equals("Doubt")) {
    _avatar.setExpression(m5avatar::Expression::Doubt);
  }
  else if (content.equals("Sleepy")) {
    _avatar.setExpression(m5avatar::Expression::Sleepy);
  }
  else if (content.equals("Neutral")) {
    _avatar.setExpression(m5avatar::Expression::Neutral);
  }
  else if (!content.isEmpty()) {
    M5_LOGE("?? %s", content.c_str());
  }
}

void Avatar::balloon(const String& content) {
  _balloonTime = ::millis();
  _balloonText = content;
  if (_systemMessage.isEmpty()) {
    _avatar.setSpeechText(_balloonText.c_str());
  }
}

void Avatar::systemMessage(const String& content) {
  _systemMessage = content;
  if (!_systemMessage.isEmpty()) {
    _avatar.setSpeechText(_systemMessage.c_str());
  } else if (!_balloonText.isEmpty()) {
    _avatar.setSpeechText(_balloonText.c_str());
  } else {
    _avatar.setSpeechText("");
  }
}

void Avatar::lipSync(void *args) {
  auto *ctx = static_cast<m5avatar::DriveContext *>(args);
  auto *avatar = ctx->getAvatar();
  while (true) {
    if (_audio) {
      auto level = abs(*_audio->getBuffer());
      if (level < 100) {
        level = 0;
      }
      if (level > 15000) {
        level = 15000;
      }
      avatar->setMouthOpenRatio(level / 15000.f);
      float gazeX, gazeY;
      avatar->getGaze(&gazeY, &gazeX);
      avatar->setRotation(gazeX * 5);
    }
    ::vTaskDelay(50);
  }
}

}  // sample
