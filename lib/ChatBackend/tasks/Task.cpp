// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include "Task.hpp"
#include "MessagePacket.hpp"

namespace chat_backend::tasks {

Task::Task(const char* taskName, uint32_t stackSize, TickType_t tick)
: _taskName(taskName), _stackSize(stackSize) {
  _context.task = this;
  _context.tick = tick;
}

void Task::begin(QueueHandle_t queue, SemaphoreHandle_t mutex) {
  _sendQueue = queue;
  _resourceMutex = mutex;
  setup();
  if (_stackSize > 0 && _context.tick > 0) {
    ::xTaskCreate(main, _taskName == nullptr ? "" : _taskName, _stackSize, &_context, 1, nullptr);
  }
}

void Task::main(void* arg) {
  auto context = static_cast<TaskContext*>(arg);
  auto task = static_cast<Task*>(context->task);

  while (true) {
    task->loop();
    ::vTaskDelay(context->tick);
  }
}

void Task::sendMessage(const char* type, const char* message) {
  auto packet = new MessagePacket{ type, message };
  ::xQueueSend(_sendQueue, &packet, 0);
}

void Task::sendMessage(const char* type) {
  auto packet = new MessagePacket{ type };
  ::xQueueSend(_sendQueue, &packet, 0);
}

void Task::takeMutex() {
  ::xSemaphoreTake(_resourceMutex, portMAX_DELAY);
}

void Task::giveMutex() {
  ::xSemaphoreGive(_resourceMutex);
}

}  // chat_backend::modules
