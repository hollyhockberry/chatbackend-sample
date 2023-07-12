// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include <M5Unified.h>
#include "Chat.hpp"
#include "MessagePacket.hpp"

namespace chat_backend::tasks {

Chat* Chat::_instance = nullptr;

Chat::Chat(const char* rootCACertificate) : Task("chat", 8192, 50), _client(nullptr, rootCACertificate) {
  _instance = this;
  _delimiters = { "。", "！", "？", "!", "?", "." };
}

bool Chat::busy() const {
  return _busy;
}

void Chat::apiKey(const char* key) {
  _client.ApiKey(key);
}

void Chat::model(const char* name) {
  _client.Model(name);
}

void Chat::maxHistory(int size) {
  _client.MaxHistory(size);
}

void Chat::threshold(unsigned int length) {
// TODO(hollyhockberry): mutual exclusion
  _threshold = length;
}

void Chat::setup() {
  _thinking = false;
  _temporary.clear();

  _queue = ::xQueueCreate(20, sizeof(String*));
  _client.begin();
  _client.AddSystem(
    "感情を表現した会話のシミュレーションを行います。"\
    "会話におけるあなたの感情を[]で囲んで回答の先頭に付加してください。"\
    "感情は Happy, Angry, Sad, Doubt, Sleepy のいずれかを選択してください。"
  );
}

void Chat::loop() {
  String* message;
  const auto ret = ::xQueueReceive(_queue, &message, portMAX_DELAY);
  M5_LOGV("begin");
  _thinking = true;
  _busy = true;
  if (_client.ChatStream(message->c_str(), [](const char* arg){ _instance->add(arg); })) {
    flush();
  }
  _busy = false;
  M5_LOGV("end");
  delete message;
}

bool Chat::chat(const char* content) {
  if (content == nullptr) {
    return false;
  }
  auto message = new String(content);
  ::xQueueSend(_queue, &message, 0);
  return true;
}

void Chat::add(const char* chunk) {
  if (_thinking) {
    _thinking = false;
    sendMessage("endThink");
  }
  _temporary += chunk;
  if (_threshold <= 0) {
    return;
  }
  String str(chunk);
  for (const auto& delimiter : _delimiters) {
    if (str.indexOf(delimiter) < 0) {
      continue;;
    }
    if (_temporary.length() > _threshold) {
      const auto idx = _temporary.lastIndexOf(delimiter);
      const auto end = idx + delimiter.length();
      auto sub = _temporary.substring(0, end);
      send(sub);
      _temporary.remove(0, end);
    }
  }
}

void Chat::flush() {
  if (_temporary.length() > 0) {
    send(_temporary);
    _temporary.clear();
  }
}

void Chat::send(const String& line) {
  M5_LOGV("%s", line.c_str());
  String message = "";
  String emotion = "";
  auto n = 0;
  while (true) {
    auto s = line.indexOf('[', n);
    auto e = line.indexOf(']', n);
    if (s < 0 || e < 0) {
      message += line.substring(n);
      break;
    }
    if (s > 0) {
      message += line.substring(n, s);
    }
    emotion = line.substring(s + 1, e).c_str();
    n = e + 1;
  }
  sendMessage("expression", emotion.c_str());
  sendMessage("say", message.c_str());
}

}  // chat_backend::tasks
