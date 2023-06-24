// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef CHATBACKEND_TASKS_MESSAGEPACKET_HPP_
#define CHATBACKEND_TASKS_MESSAGEPACKET_HPP_

#include <WString.h>

namespace chat_backend::tasks {

struct MessagePacket {
  String type;
  String content;
};

}  // chat_backend::tasks

#endif  // CHATBACKEND_TASKS_MESSAGEPACKET_HPP_
