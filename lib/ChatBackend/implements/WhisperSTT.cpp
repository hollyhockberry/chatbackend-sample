// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include <ArduinoJson.h>
#include <M5Unified.h>
#include <WiFiClientSecure.h>
#include "WhisperSTT.hpp"

namespace {
constexpr size_t record_number = 300;
constexpr size_t record_length = 150;
constexpr size_t record_size = record_number * record_length;
constexpr int headerSize = 44;

int16_t* makeHeader(byte* header) {
  const auto wavDataSize = record_number * record_length * 2;
  header[0] = 'R';
  header[1] = 'I';
  header[2] = 'F';
  header[3] = 'F';
  unsigned int fileSizeMinus8 = wavDataSize + headerSize - 8;
  header[4] = (byte)(fileSizeMinus8 & 0xFF);
  header[5] = (byte)((fileSizeMinus8 >> 8) & 0xFF);
  header[6] = (byte)((fileSizeMinus8 >> 16) & 0xFF);
  header[7] = (byte)((fileSizeMinus8 >> 24) & 0xFF);
  header[8] = 'W';
  header[9] = 'A';
  header[10] = 'V';
  header[11] = 'E';
  header[12] = 'f';
  header[13] = 'm';
  header[14] = 't';
  header[15] = ' ';
  header[16] = 0x10;  // linear PCM
  header[17] = 0x00;
  header[18] = 0x00;
  header[19] = 0x00;
  header[20] = 0x01;  // linear PCM
  header[21] = 0x00;
  header[22] = 0x01;  // monoral
  header[23] = 0x00;
  header[24] = 0x80;  // sampling rate 16000
  header[25] = 0x3E;
  header[26] = 0x00;
  header[27] = 0x00;
  header[28] = 0x00;  // Byte/sec = 16000x2x1 = 32000
  header[29] = 0x7D;
  header[30] = 0x00;
  header[31] = 0x00;
  header[32] = 0x02;  // 16bit monoral
  header[33] = 0x00;
  header[34] = 0x10;  // 16bit
  header[35] = 0x00;
  header[36] = 'd';
  header[37] = 'a';
  header[38] = 't';
  header[39] = 'a';
  header[40] = (byte)(wavDataSize & 0xFF);
  header[41] = (byte)((wavDataSize >> 8) & 0xFF);
  header[42] = (byte)((wavDataSize >> 16) & 0xFF);
  header[43] = (byte)((wavDataSize >> 24) & 0xFF);
  return (int16_t*)&header[headerSize];
}

constexpr char* API_HOST = "api.openai.com";
constexpr int API_PORT = 443;
constexpr char* API_PATH = "/v1/audio/transcriptions";

} // namespace

namespace chat_backend::impl {

WhisperSTT::WhisperSTT(const char* rootCACertificate) : _rootCACertificate(rootCACertificate) {
}

void WhisperSTT::apiKey(const char* key) {
  _apiKey = key;
}

void WhisperSTT::begin() {
}

void sendHeader(WiFiClientSecure& client, const char* apiKey, const byte* buffer, size_t size) {
  char boundary[64] = "------------------------";
  for (auto i = 0; i < 2; ++i) {
    ltoa(random(0x7fffffff), boundary + strlen(boundary), 16);
  }
  const String header = "--" + String(boundary) + "\r\n"
    "Content-Disposition: form-data; name=\"model\"\r\n\r\nwhisper-1\r\n"
    "--" + String(boundary) + "\r\n"
    "Content-Disposition: form-data; name=\"language\"\r\n\r\nja\r\n"
    "--" + String(boundary) + "\r\n"
    "Content-Disposition: form-data; name=\"file\"; filename=\"speak.wav\"\r\n"
    "Content-Type: application/octet-stream\r\n\r\n";
  const String footer = "\r\n--" + String(boundary) + "--\r\n";

  client.printf("POST %s HTTP/1.1\n", API_PATH);
  client.printf("Host: %s\n", API_HOST);
  client.println("Accept: */*");
  client.printf("Authorization: Bearer %s\n", apiKey);
  client.printf("Content-Length: %d\n", header.length() + size + footer.length());
  client.printf("Content-Type: multipart/form-data; boundary=%s\n", boundary);
  client.println();
  client.print(header.c_str());
  client.flush();

  auto ptr = buffer;
  auto remainings = size;
  while (remainings > 0) {
    auto sz = (remainings > 512) ? 512 : remainings;
    client.write(ptr, sz);
    client.flush();
    remainings -= sz;
    ptr += sz;
  }
  client.flush();

  // footer
  client.print(footer.c_str());
  client.flush();
}

bool waitResponse(WiFiClientSecure& client) {
  const auto now = ::millis();
  while (client.available() == 0) {
    if (::millis() - now > 10000) {
      M5_LOGE("Timeout !");
      return false;
    }
  }
  return true;
}

bool WhisperSTT::record() {
  M5_LOGV("begin");
  if (_buffer) {
    delete _buffer;
  }
  _size = record_size * sizeof(int16_t) + headerSize;
  _buffer = static_cast<byte*>(::heap_caps_malloc(_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));

  ::memset(_buffer, 0, _size);
  auto *wavData = makeHeader(_buffer);
  for (int i = 0; i < record_number; ++i) {
    auto ptr = &wavData[i * record_length];
    M5.Mic.record(ptr, record_length, 16000);
  }
  M5_LOGV("end");
  return true;
}

bool WhisperSTT::transcribe(String& out) {
  if (_buffer == nullptr) {
    return false;
  }
  out = "";
  auto client = new WiFiClientSecure();
  if (_rootCACertificate) {
    client->setCACert(_rootCACertificate);
  } else {
    client->setInsecure();
  }
  if (!client->connect(API_HOST, API_PORT)) {
    M5_LOGE("connection error");
    delete client;
    delete _buffer;
    _buffer = nullptr;
    return false;
  }
  sendHeader(*client, _apiKey.c_str(), _buffer, _size);

  if (waitResponse(*client)) {
    bool isBody = false;
    String body = "";
    while(client->available()){
      const auto line = client->readStringUntil('\r');
      if (isBody) {
        body += line;
      } else if (line.equals("\n")) {
        isBody = true;
      }
    }
    StaticJsonDocument<200> doc;
    ::deserializeJson(doc, body);
    out = doc["text"].as<String>();
  }
  delete client;
  delete _buffer;
  _buffer = nullptr;
  return !out.isEmpty();
}

}  // chat_backend::impl
