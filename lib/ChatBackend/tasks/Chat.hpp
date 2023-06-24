// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef CHATBACKEND_TASKS_CHAT_HPP_
#define CHATBACKEND_TASKS_CHAT_HPP_

#include <ChatClient.h>
#include "Task.hpp"

namespace chat_backend::tasks {

class Chat : public Task {
  static Chat* _instance;
  ChatClient _client;
  std::vector<String> _delimiters;
  String _temporary;
  unsigned int _threshold = 50;
  bool _thinking = false;
  bool _busy = false;
  QueueHandle_t _queue;

 public:
  explicit Chat(const char* rootCACertificate);
  bool busy() const;
  void apiKey(const char* key);

  void model(const char* name);
  void maxHistory(int size);

  bool chat(const char* content);
  void threshold(unsigned int length);

protected:
  void setup() override;
  void loop() override;

private:
  void add(const char* chunk);
  void flush();
  void send(const String& line);
};

}  // chat_backend::tasks

#endif  // CHATBACKEND_TASKS_CHAT_HPP_
