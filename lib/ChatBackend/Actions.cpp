// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include "Actions.hpp"
#include "tasks/MessagePacket.hpp"

namespace chat_backend {

void Actions::sendMessage(const char* type, const char* message, Resources& resources) {
  auto packet = new tasks::MessagePacket{ type, message };
  ::xQueueSend(resources.queue, &packet, 0);
}

void Actions::sendMessage(const char* type, Resources& resources) {
  auto packet = new tasks::MessagePacket{ type };
  ::xQueueSend(resources.queue, &packet, 0);
}

void Actions::takeMutex(Resources& resources) {
  ::xSemaphoreTake(resources.mutex, portMAX_DELAY);
}

void Actions::giveMutex(Resources& resources) {
  ::xSemaphoreGive(resources.mutex);
}

}  // chat_backend
