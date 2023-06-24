// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include "Actions.hpp"
#include <M5Unified.h>

namespace sample {

void Actions::begin() {
  _lastMonologue = 0;
}

bool Actions::loop(Resources& resources) {
  if (isMonologueMode() && monologue(resources)) {
    return true;
  }
  return false;
}

void Actions::beep(int n, Resources& resources) const {
  takeMutex(resources);
  M5.Mic.end();
  M5.Speaker.begin();
  while (n-- > 0) {
    M5.Speaker.tone(1000, 100);
    ::delay(80);
    M5.Speaker.stop();
    ::delay(20);
  }
  M5.Speaker.end();
  giveMutex(resources);
}

bool Actions::onTouched(int x, int y, Resources& resources) {
  if (x >= 0 && x < M5.Display.width() && y >= 0 && y < 60) {
    sendMessage("listen", resources);
  }
  return false;
}

void Actions::reportBatteryLevel(Resources& resources) {
  beep(resources);
  char buf[100];
  ::sprintf(buf, "%sバッテリーのレベルは%d%%です",
      M5.Power.isCharging() ? "充電中、" : "", M5.Power.getBatteryLevel());
  sendMessage("expression", "Happy", resources);
  sendMessage("say", buf, resources);
}

void Actions::switchMonologueMode(Resources& resources) {
  beep(resources);
  sendMessage("expression", "Happy", resources);
  if (isMonologueMode()) {
    _lastMonologue = 0;
    sendMessage("say", "独り言やめます", resources);
  } else {
    _lastMonologue = ::millis();
    sendMessage("say", "独り言始めます", resources);
  }
}

bool Actions::isMonologueMode() const {
  return _lastMonologue > 0;
}

bool Actions::monologue(Resources& resources) {
  const auto next = 60000;
  if (::millis() - _lastMonologue < next) {
    return false;
  }
  const char* questions[] = {
    "あなたは誰",
    "楽しい",
    "怒った",
    "可愛い",
    "悲しい",
    "眠い",
    "ジョークを言って",
    "泣きたい",
    "怒ったぞ",
    "こんにちは",
    "お疲れ様",
    "詩を書いて",
    "疲れた",
    "お腹空いた",
    "嫌いだ",
    "苦しい",
    "俳句を作って",
    "歌をうたって"
  };
  const auto n = sizeof(questions) / sizeof(const char*);
  const auto question = questions[::random(n)];
  M5_LOGV("%s", question);
  sendMessage("chat", question, resources);
  _lastMonologue = ::millis();
  return true;
}

bool Actions::onButtonClicked(int button, Resources& resources) {
  switch (button) {
  case 0:
    switchMonologueMode(resources);
    return true;
  case 1:
    break;
  case 2:
    reportBatteryLevel(resources);
    return true;
  default:
    break;
  }
M5_LOGI("%d", button);
  return false;
}

bool Actions::onButtonDoubleClicked(int button, Resources& resources) {
  switch (button) {
  case 0:
    break;
  case 1:
    beep(2, resources);
    sendMessage("record", resources);
    return true;
  case 2:
    break;
  default:
    break;
  }
M5_LOGI("%d", button);
  return false;
}

bool Actions::onAvailableStream(Stream& stream, Resources& resources) {
  const auto line = stream.readStringUntil('\n');
  sendMessage("chat", line.c_str(), resources);
  return true;
}

bool Actions::onEnterIdle(Resources& resources) {
  if (isMonologueMode()) {
    _lastMonologue = ::millis();
  }
  return false;
}

bool Actions::onBeginRecording(Resources& resources) {
  sendMessage("expression", "Happy", resources);
  sendMessage("message", "録音中", resources);
  return false;
}

bool Actions::onEndRecording(Resources& resources) {
  sendMessage("message", "", resources);
  sendMessage("balloon", "わかりました", resources);
  return false;
}

bool Actions::onBeginListening(Resources& resources) {
  beep(resources);
  sendMessage("expression", "Happy", resources);
  sendMessage("balloon", "御用でしょうか?", resources);
  return false;
}

bool Actions::onEndListening(Resources& resources) {
  sendMessage("balloon", "わかりました", resources);
  return false;
}

bool Actions::onBeginThinking(Resources& resources) {
  sendMessage("expression", "Doubt", resources);
  sendMessage("message", "考え中…", resources);
  return false;
}

bool Actions::onEndThinking(Resources& resources) {
  sendMessage("expression", "Neutral", resources);
  sendMessage("message", "", resources);
  return false;
}

}  // sample
