/****************************************************************************************************************************
  AsyncDNSServer_ESP32_Ethernet_Debug.h

  For ESP32_Ethernet (ESP32/S2/S3/C3 + LwIP W5500 / W6100 / ENC28J60)

  AsyncDNSServer_ESP32_Ethernet is a library for the ESP32_Ethernet (ESP32/S2/S3/C3 + LwIP W5500 / W6100 / ENC28J60)

  Based on and modified from ESPAsyncDNSServer Library (https://github.com/devyte/ESPAsyncDNSServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet
  Licensed under GPLv3 license

  Version: 1.2.0

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang      18/12/2022 Initial coding for ESP32/ES32_S3 using LwIP W5500 / ENC28J60
  1.1.0   K Hoang      22/12/2022 Add support to ESP32S2/C3 using LwIP W5500 / ENC28J60
  1.2.0   K Hoang      11/01/2023 Add support to ESP32/S2/S3/C3 using LwIP W6100
  *****************************************************************************************************************************/

#pragma once

#ifndef ASYNC_DNS_SERVER_ESP32_ETHERNET_DEBUG_H
#define ASYNC_DNS_SERVER_ESP32_ETHERNET_DEBUG_H

#ifdef ASYNC_DNS_ESP32_ETHERNET_DEBUG_PORT
  #define DBG_PORT_ASYNC_DNS      ASYNC_DNS_ESP32_ETHERNET_DEBUG_PORT
#else
  #define DBG_PORT_ASYNC_DNS      Serial
#endif

// Change _ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_ to set tracing and logging verbosity
// 0: DISABLED: no logging
// 1: ERROR: errors
// 2: WARN: errors and warnings
// 3: INFO: errors, warnings and informational (default)
// 4: DEBUG: errors, warnings, informational and debug

#ifndef _ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_
  #define _ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_       0
#endif

/////////////////////////////////////////////////////////

const char ASYNC_DNS_MARK[] = "[DNS] ";

#define ASYNC_DNS_PRINT        DBG_PORT_ASYNC_DNS.print
#define ASYNC_DNS_PRINTLN      DBG_PORT_ASYNC_DNS.println

#define ASYNC_DNS_PRINT_MARK   ASYNC_DNS_PRINT(ASYNC_DNS_MARK)
#define ASYNC_DNS_PRINT_SP     DBG_PORT_ASYNC_DNS.print(" ")

///////////////////////////////////////

#define DNS_LOG(x)            { ASYNC_DNS_PRINTLN(x); }
#define DNS_LOG0(x)           { ASYNC_DNS_PRINT(x); }
#define DNS_LOG1(x,y)         { ASYNC_DNS_PRINT(x); ASYNC_DNS_PRINTLN(y); }
#define DNS_LOG2(x,y,z)       { ASYNC_DNS_PRINT(x); ASYNC_DNS_PRINT(y); ASYNC_DNS_PRINTLN(z); }
#define DNS_LOG3(x,y,z,w)     { ASYNC_DNS_PRINT(x); ASYNC_DNS_PRINT(y); ASYNC_DNS_PRINT(z); ASYNC_DNS_PRINTLN(w); }

/////////////////////////////////////////////////////////

#define DNS_LOGERROR(x)         if(_ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_>0) { ASYNC_DNS_PRINT_MARK; ASYNC_DNS_PRINTLN(x); }
#define DNS_LOGERROR0(x)        if(_ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_>0) { ASYNC_DNS_PRINT(x); }
#define DNS_LOGERROR1(x,y)      if(_ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_>0) { ASYNC_DNS_PRINT_MARK; ASYNC_DNS_PRINT(x); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINTLN(y); }
#define DNS_LOGERROR2(x,y,z)    if(_ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_>0) { ASYNC_DNS_PRINT_MARK; ASYNC_DNS_PRINT(x); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINT(y); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINTLN(z); }
#define DNS_LOGERROR3(x,y,z,w)  if(_ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_>0) { ASYNC_DNS_PRINT_MARK; ASYNC_DNS_PRINT(x); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINT(y); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINT(z); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINTLN(w); }

/////////////////////////////////////////////////////////

#define DNS_LOGWARN(x)          if(_ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_>1) { ASYNC_DNS_PRINT_MARK; ASYNC_DNS_PRINTLN(x); }
#define DNS_LOGWARN0(x)         if(_ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_>1) { ASYNC_DNS_PRINT(x); }
#define DNS_LOGWARN1(x,y)       if(_ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_>1) { ASYNC_DNS_PRINT_MARK; ASYNC_DNS_PRINT(x); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINTLN(y); }
#define DNS_LOGWARN2(x,y,z)     if(_ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_>1) { ASYNC_DNS_PRINT_MARK; ASYNC_DNS_PRINT(x); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINT(y); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINTLN(z); }
#define DNS_LOGWARN3(x,y,z,w)   if(_ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_>1) { ASYNC_DNS_PRINT_MARK; ASYNC_DNS_PRINT(x); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINT(y); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINT(z); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINTLN(w); }

/////////////////////////////////////////////////////////

#define DNS_LOGINFO(x)          if(_ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_>2) { ASYNC_DNS_PRINT_MARK; ASYNC_DNS_PRINTLN(x); }
#define DNS_LOGINFO0(x)         if(_ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_>2) { ASYNC_DNS_PRINT(x); }
#define DNS_LOGINFO1(x,y)       if(_ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_>2) { ASYNC_DNS_PRINT_MARK; ASYNC_DNS_PRINT(x); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINTLN(y); }
#define DNS_LOGINFO2(x,y,z)     if(_ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_>2) { ASYNC_DNS_PRINT_MARK; ASYNC_DNS_PRINT(x); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINT(y); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINTLN(z); }
#define DNS_LOGINFO3(x,y,z,w)   if(_ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_>2) { ASYNC_DNS_PRINT_MARK; ASYNC_DNS_PRINT(x); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINT(y); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINT(z); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINTLN(w); }

/////////////////////////////////////////////////////////

#define DNS_LOGDEBUG(x)         if(_ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_>3) { ASYNC_DNS_PRINT_MARK; ASYNC_DNS_PRINTLN(x); }
#define DNS_LOGDEBUG0(x)        if(_ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_>3) { ASYNC_DNS_PRINT(x); }
#define DNS_LOGDEBUG1(x,y)      if(_ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_>3) { ASYNC_DNS_PRINT_MARK; ASYNC_DNS_PRINT(x); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINTLN(y); }
#define DNS_LOGDEBUG2(x,y,z)    if(_ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_>3) { ASYNC_DNS_PRINT_MARK; ASYNC_DNS_PRINT(x); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINT(y); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINTLN(z); }
#define DNS_LOGDEBUG3(x,y,z,w)  if(_ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_>3) { ASYNC_DNS_PRINT_MARK; ASYNC_DNS_PRINT(x); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINT(y); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINT(z); ASYNC_DNS_PRINT_SP; ASYNC_DNS_PRINTLN(w); }


/////////////////////////////////////////////////////////

#endif    //ASYNC_DNS_SERVER_ESP32_ETHERNET_DEBUG_H
