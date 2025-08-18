/****************************************************************************************************************************
  AsyncUDP_ESP32_W6100.h

  AsyncUDP_ESP32_W6100 is a Async UDP library for the ESP32_W6100 (ESP32 + LwIP W6100)

  Based on and modified from ESPAsyncUDP Library (https://github.com/me-no-dev/ESPAsyncUDP)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncUDP_ESP32_W6100
  Licensed under GPLv3 license

  Version: 2.0.0

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  2.0.0   K Hoang      10/01/2023 Initial coding for ESP32_W6100. Bump up version to v2.0.0 to sync with AsyncUDP v2.0.0
 *****************************************************************************************************************************/

#pragma once

#ifndef ASYNC_UDP_ESP32_W6100_H
#define ASYNC_UDP_ESP32_W6100_H

////////////////////////////////////////////////

#if ( ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) ) && ( ARDUINO_ESP32_GIT_VER != 0x46d5afb1 ) )

#if (_ASYNC_UDP_ESP32_W6100_LOGLEVEL_ > 2)
  #warning Using code for ESP32 core v2.0.0+ in AsyncUDP_ESP32_W6100.h
#endif

#define ASYNC_UDP_ESP32_W6100_VERSION      "AsyncUDP_ESP32_W6100 v2.0.0 for core v2.0.0+"

extern "C"
{
#include "lwip/ip_addr.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
}

#else

#if (_ASYNC_UDP_ESP32_W6100_LOGLEVEL_ > 2)
  #warning Using code for ESP32 core v1.0.6- in AsyncUDP_ESP32_W6100.h
#endif

#define ASYNC_UDP_ESP32_W6100_VERSION      "AsyncUDP_ESP32_W6100 v2.0.0 for core v1.0.6-"

extern "C"
{
#include "lwip/ip_addr.h"
#include <tcpip_adapter.h>
#include "freertos/queue.h"
#include "freertos/semphr.h"
}
#endif

#include "IPAddress.h"
#include "IPv6Address.h"
#include "Print.h"
#include <functional>

////////////////////////////////////////////////

#include <WebServer_ESP32_W6100.h>     // https://github.com/khoih-prog/WebServer_ESP32_W6100

#include "AsyncUDP_ESP32_W6100_Debug.h"

#include "AsyncUDP_ESP32_W6100.hpp"
#include "AsyncUDP_ESP32_W6100_Impl.h"

////////////////////////////////////////////////

#endif    //ASYNC_UDP_ESP32_W6100_H
