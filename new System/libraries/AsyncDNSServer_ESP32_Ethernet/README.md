# AsyncDNSServer_ESP32_Ethernet Library


[![arduino-library-badge](https://www.ardu-badge.com/badge/AsyncDNSServer_ESP32_Ethernet.svg?)](https://www.ardu-badge.com/AsyncDNSServer_ESP32_Ethernet)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/AsyncDNSServer_ESP32_Ethernet.svg)](https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet/releases)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/AsyncDNSServer_ESP32_Ethernet.svg)](http://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet/issues)


<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Donate to my libraries using BuyMeACoffee" style="height: 50px !important;width: 181px !important;" ></a>
<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://img.shields.io/badge/buy%20me%20a%20coffee-donate-orange.svg?logo=buy-me-a-coffee&logoColor=FFDD00" style="height: 20px !important;width: 200px !important;" ></a>

---
---

## Table of Contents


* [Why do we need this AsyncDNSServer_ESP32_Ethernet library](#why-do-we-need-this-AsyncDNSServer_ESP32_Ethernet-library)
  * [Features](#features)
  * [Why Async is better](#why-async-is-better)
  * [Currently supported Boards](#currently-supported-boards)
  * [To-be supported Boards](#To-be-supported-boards)
* [Changelog](changelog.md)
* [Prerequisites](#prerequisites)
  * [For ESP32](#For-ESP32)
  * [For ESP32_S2/S3/C3 ](#For-ESP32_S2S3C3)
* [Installation](#installation)
  * [Use Arduino Library Manager](#use-arduino-library-manager)
  * [Manual Install](#manual-install)
  * [VS Code & PlatformIO](#vs-code--platformio)
* [Libraries' Patches](#libraries-patches)
  * [1. For fixing ESP32 compile error](#1-for-fixing-esp32-compile-error)
* [HOWTO Fix `Multiple Definitions` Linker Error](#howto-fix-multiple-definitions-linker-error) 
* [HOWTO Use analogRead() with ESP32 running WiFi and/or BlueTooth (BT/BLE)](#howto-use-analogread-with-esp32-running-wifi-andor-bluetooth-btble)
  * [1. ESP32 has 2 ADCs, named ADC1 and ADC2](#1--esp32-has-2-adcs-named-adc1-and-adc2)
  * [2. ESP32 ADCs functions](#2-esp32-adcs-functions)
  * [3. ESP32 WiFi uses ADC2 for WiFi functions](#3-esp32-wifi-uses-adc2-for-wifi-functions)
* [HOWTO Setting up the Async DNS Server](#howto-setting-up-the-async-dns-server)
* [How to connect W5500, W6100 or ENC28J60 to ESP32](#How-to-connect-W5500-W6100-or-ENC28J60-to-ESP32)
* [How to connect W5500, W6100 or ENC28J60 to ESP32_S2/S3/C3](#How-to-connect-W5500-W6100-or-ENC28J60-to-ESP32_S2S3C3)
* [Examples](#examples)
  * [1. For ESP32_W5500](#1-For-ESP32_W5500)
    * [ 1. AsyncCaptivePortalAdvanced_ESP32_W5500](examples/ESP32_W5500/AsyncCaptivePortalAdvanced_ESP32_W5500)
    * [ 2. AsyncCaptivePortal_ESP32_W5500](examples/ESP32_W5500/AsyncCaptivePortal_ESP32_W5500) 
    * [ 3. AsyncDNServerFull_ESP32_W5500](examples/ESP32_W5500/AsyncDNServerFull_ESP32_W5500)
    * [ 4. AsyncDNSServer_ESP32_W5500](examples/ESP32_W5500/AsyncDNSServer_ESP32_W5500)
  * [2. For ESP32_ENC](#2-For-ESP32_ENC)
  * [3. For ESP32_SC_W5500](#3-For-ESP32_SC_W5500)
  * [4. For ESP32_SC_ENC](#4-For-ESP32_SC_ENC)
  * [5. For ESP32_W6100](#5-For-ESP32_W6100)
  * [6. For ESP32_SC_W6100](#6-For-ESP32_SC_W6100)
* [Example AsyncDNSServer_ESP32_SC_W5500](#example-AsyncDNSServer_ESP32_SC_W5500)
* [Debug](#debug)
* [Troubleshooting](#troubleshooting)
* [Issues](#issues)
* [TO DO](#to-do)
* [DONE](#done)
* [Contributions and Thanks](#contributions-and-thanks)
* [Contributing](#contributing)
* [License](#license)
* [Copyright](#copyright)

---
---

### Why do we need this [AsyncDNSServer_ESP32_Ethernet library](https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet)

#### Features

This [AsyncDNSServer_ESP32_Ethernet library](https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet) is a fully `asynchronous DNSServer` library, designed for a trouble-free, multi-connection network environment, for **ESP32/S2/S3/C3 boards using LwIP W5500, W6100 or ENC28J60 Ethernet**.

This library is based on, modified from:

1. [Develo's ESPAsyncDNSServer](https://github.com/devyte/ESPAsyncDNSServer)

to apply the better and faster **asynchronous** feature of the **powerful** [AsyncDNSServer_ESP32_Ethernet Library](https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet) into **ESP32/S2/S3/C3 boards using LwIP W5500, W6100 or ENC28J60 Ethernet**.


#### Why Async is better

- Using asynchronous network means that you can handle **more than one connection at the same time**
- You are called once the packet is ready
- After a DNS Client connected to this Async DNS server, you are **immediately ready** to handle other connections while the Server is taking care of receiving and responding to the UDP packets in the background.
- You are not required to check in a tight loop() the arrival of the DNS requesting packets to process them.
- **Speed is OMG**


#### Currently supported Boards

1. **ESP32 boards**
2. **ESP32_S3-based boards (ESP32S3_DEV, ESP32_S3_BOX, UM TINYS3, UM PROS3, UM FEATHERS3, etc.)**
3. **ESP32-S2 (ESP32-S2 Saola, AI-Thinker ESP-12K, etc.)**
4. **ESP32-C3 (ARDUINO_ESP32C3_DEV, etc.)**

 using `LwIP W5500, W6100 or ENC28J60 Ethernet`
 
--- 

#### ESP32S3_DEV

<p align="center">
    <img src="https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet/raw/main/Images/ESP32S3_DEV.png">
</p> 


#### ESP32S2_DEV

<p align="center">
    <img src="https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet/raw/main/Images/ESP32S2_DEV.png">
</p> 


#### ESP32C3_DEV

<p align="center">
    <img src="https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet/raw/main/Images/ESP32_C3_DevKitC_02.png">
</p> 


---

##### W5500

`FULL_DUPLEX, 100Mbps`

<p align="center">
    <img src="https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet/raw/main/Images/W5500.png">
</p>

<p align="center">
    <img src="https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet/raw/main/Images/W5500_small.png">
</p> 
 

---

#### W6100

`FULL_DUPLEX, 100Mbps`

<p align="center">
    <img src="https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet/raw/main/Images/W6100.png">
</p>
  
---


##### ENC28J60

`FULL_DUPLEX, 10Mbps`

<p align="center">
    <img src="https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet/raw/main/Images/ENC28J60.png">
</p>

 
---

### To-be Supported Boards

#### 1. New ESP32 using LwIP W5500, W6100 or ENC28J60 Ethernet



---
---

## Prerequisites

 1. [`Arduino IDE 1.8.19+` for Arduino](https://github.com/arduino/Arduino/)
 2. [`ESP32 Core 2.0.6+`](https://github.com/espressif/arduino-esp32) for ESP32-based boards. [![Latest release](https://img.shields.io/github/release/espressif/arduino-esp32.svg)](https://github.com/espressif/arduino-esp32/releases/latest/)
 
### For ESP32
 
 3. [`WebServer_ESP32_W5500 library 1.5.2+`](https://github.com/khoih-prog/WebServer_ESP32_W5500) for `ESP32` using W5500. To install, check [![arduino-library-badge](https://www.ardu-badge.com/badge/WebServer_ESP32_W5500.svg?)](https://www.ardu-badge.com/WebServer_ESP32_W5500)
 4. [`WebServer_ESP32_W6100 library 1.5.2+`](https://github.com/khoih-prog/WebServer_ESP32_W6100) for `ESP32` using W6100. To install, check [![arduino-library-badge](https://www.ardu-badge.com/badge/WebServer_ESP32_W6100.svg?)](https://www.ardu-badge.com/WebServer_ESP32_W6100)
 5. [`WebServer_ESP32_ENC library 1.5.1+`](https://github.com/khoih-prog/WebServer_ESP32_ENC) for `ESP32` using ENC28J60. To install, check [![arduino-library-badge](https://www.ardu-badge.com/badge/WebServer_ESP32_ENC.svg?)](https://www.ardu-badge.com/WebServer_ESP32_ENC)
 6. [`AsyncUDP_ESP32_W5500 library 2.0.0+`](https://github.com/khoih-prog/AsyncUDP_ESP32_W5500) for `ESP32` using W5500. To install, check [![arduino-library-badge](https://www.ardu-badge.com/badge/AsyncUDP_ESP32_W5500.svg?)](https://www.ardu-badge.com/AsyncUDP_ESP32_W5500)
 7. [`AsyncUDP_ESP32_W6100 library 2.0.0+`](https://github.com/khoih-prog/AsyncUDP_ESP32_W6100) for `ESP32` using W6100. To install, check [![arduino-library-badge](https://www.ardu-badge.com/badge/AsyncUDP_ESP32_W6100.svg?)](https://www.ardu-badge.com/AsyncUDP_ESP32_W6100)
 8. [`AsyncUdp_ESP32_ENC library 2.0.0+`](https://github.com/khoih-prog/AsyncUdp_ESP32_ENC) for `ESP32` using ENC28J60. To install, check [![arduino-library-badge](https://www.ardu-badge.com/badge/AsyncUdp_ESP32_ENC.svg?)](https://www.ardu-badge.com/AsyncUdp_ESP32_ENC)
 
### For ESP32_S2/S3/C3 
 
 9. [`WebServer_ESP32_SC_W5500 library 1.2.1+`](https://github.com/khoih-prog/WebServer_ESP32_SC_W5500) for `ESP32_S2/S3/C3` using W5500. To install, check [![arduino-library-badge](https://www.ardu-badge.com/badge/WebServer_ESP32_SC_W5500.svg?)](https://www.ardu-badge.com/WebServer_ESP32_SC_W5500)
10. [`WebServer_ESP32_SC_W6100 library 1.2.1+`](https://github.com/khoih-prog/WebServer_ESP32_SC_W6100) for `ESP32_S2/S3/C3` using W6100. To install, check [![arduino-library-badge](https://www.ardu-badge.com/badge/WebServer_ESP32_SC_W6100.svg?)](https://www.ardu-badge.com/WebServer_ESP32_SC_W6100)
11. [`WebServer_ESP32_SC_ENC library 1.2.0+`](https://github.com/khoih-prog/WebServer_ESP32_SC_ENC) for `ESP32_S2/S3/C3` using ENC28J60. To install, check [![arduino-library-badge](https://www.ardu-badge.com/badge/WebServer_ESP32_SC_ENC.svg?)](https://www.ardu-badge.com/WebServer_ESP32_SC_ENC)
12. [`AsyncUDP_ESP32_SC_Ethernet library 2.2.0+`](https://github.com/khoih-prog/AsyncUDP_ESP32_SC_Ethernet) for `ESP32_S2/S3/C3` using W5500, W6100 or ENC28J60. To install, check [![arduino-library-badge](https://www.ardu-badge.com/badge/AsyncUDP_ESP32_SC_Ethernet.svg?)](https://www.ardu-badge.com/AsyncUDP_ESP32_SC_Ethernet)
 
---
---

### Installation

The suggested way to install is to:

#### Use Arduino Library Manager

The best way is to use `Arduino Library Manager`. Search for `AsyncDNSServer_ESP32_Ethernet`, then select / install the latest version. You can also use this link [![arduino-library-badge](https://www.ardu-badge.com/badge/AsyncDNSServer_ESP32_Ethernet.svg?)](https://www.ardu-badge.com/AsyncDNSServer_ESP32_Ethernet) for more detailed instructions.

### Manual Install

1. Navigate to [AsyncDNSServer_ESP32_Ethernet](https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet) page.
2. Download the latest release `AsyncDNSServer_ESP32_Ethernet-main.zip`.
3. Extract the zip file to `AsyncDNSServer_ESP32_Ethernet-main` directory 
4. Copy the whole `AsyncDNSServer_ESP32_Ethernet-main` folder to Arduino libraries' directory such as `~/Arduino/libraries/`.

### VS Code & PlatformIO:

1. Install [VS Code](https://code.visualstudio.com/)
2. Install [PlatformIO](https://platformio.org/platformio-ide)
3. Install [**AsyncDNSServer_ESP32_Ethernet** library](https://registry.platformio.org/libraries/AsyncDNSServer_ESP32_Ethernet) by using [Library Manager](https://registry.platformio.org/libraries/AsyncDNSServer_ESP32_Ethernet/installation). Search for AsyncDNSServer_ESP32_Ethernet in [Platform.io Author's Libraries](https://platformio.org/lib/search?query=author:%22Khoi%20Hoang%22)
4. Use included [platformio.ini](platformio/platformio.ini) file from examples to ensure that all dependent libraries will installed automatically. Please visit documentation for the other options and examples at [Project Configuration File](https://docs.platformio.org/page/projectconf.html)

---
---

### Libraries' Patches

#### 1. For fixing ESP32 compile error

To fix [`ESP32 compile error`](https://github.com/espressif/arduino-esp32), just copy the following file into the [`ESP32`](https://github.com/espressif/arduino-esp32) cores/esp32 directory (e.g. ./arduino-1.8.19/hardware/espressif/cores/esp32) to overwrite the old file:
- [Server.h](LibraryPatches/esp32/cores/esp32/Server.h)

---
---

### HOWTO Fix `Multiple Definitions` Linker Error

The current library implementation, using `xyz-Impl.h` instead of standard `xyz.cpp`, possibly creates certain `Multiple Definitions` Linker error in certain use cases.

You can include this `.hpp` file

```cpp
// Can be included as many times as necessary, without `Multiple Definitions` Linker Error
#include "AsyncDNSServer_ESP32_Ethernet.hpp"     //https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet
```

in many files. But be sure to use the following `.h` file **in just 1 `.h`, `.cpp` or `.ino` file**, which must **not be included in any other file**, to avoid `Multiple Definitions` Linker Error

```cpp
// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "AsyncDNSServer_ESP32_Ethernet.h"       //https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet
```

---
---

### HOWTO Use analogRead() with ESP32 running WiFi and/or BlueTooth (BT/BLE)

Please have a look at [**ESP_WiFiManager Issue 39: Not able to read analog port when using the autoconnect example**](https://github.com/khoih-prog/ESP_WiFiManager/issues/39) to have more detailed description and solution of the issue.

#### 1.  ESP32 has 2 ADCs, named ADC1 and ADC2

#### 2. ESP32 ADCs functions

- `ADC1` controls ADC function for pins **GPIO32-GPIO39**
- `ADC2` controls ADC function for pins **GPIO0, 2, 4, 12-15, 25-27**

#### 3.. ESP32 WiFi uses ADC2 for WiFi functions

Look in file [**adc_common.c**](https://github.com/espressif/esp-idf/blob/master/components/driver/adc_common.c#L61)

> In `ADC2`, there're two locks used for different cases:
> 1. lock shared with app and Wi-Fi:
>    ESP32:
>         When Wi-Fi using the `ADC2`, we assume it will never stop, so app checks the lock and returns immediately if failed.
>    ESP32S2:
>         The controller's control over the ADC is determined by the arbiter. There is no need to control by lock.
> 
> 2. lock shared between tasks:
>    when several tasks sharing the `ADC2`, we want to guarantee
>    all the requests will be handled.
>    Since conversions are short (about 31us), app returns the lock very soon,
>    we use a spinlock to stand there waiting to do conversions one by one.
> 
> adc2_spinlock should be acquired first, then adc2_wifi_lock or rtc_spinlock.


- In order to use `ADC2` for other functions, we have to **acquire complicated firmware locks and very difficult to do**
- So, it's not advisable to use `ADC2` with WiFi/BlueTooth (BT/BLE).
- Use `ADC1`, and pins **GPIO32-GPIO39**
- If somehow it's a must to use those pins serviced by `ADC2` (**GPIO0, 2, 4, 12, 13, 14, 15, 25, 26 and 27**), use the **fix mentioned at the end** of [**ESP_WiFiManager Issue 39: Not able to read analog port when using the autoconnect example**](https://github.com/khoih-prog/ESP_WiFiManager/issues/39) to work with ESP32 WiFi/BlueTooth (BT/BLE)

---
---

## HOWTO Setting up the Async DNS Server

For ESP32 using `LwIP W5500 Ethernet`


```cpp
#include <AsyncDNSServer_ESP32_Ethernet.h>
#include <ESPAsyncWebServer.h>

const byte DNS_PORT = 53;

IPAddress apIP;

AsyncDNSServer dnsServer;

AsyncWebServer server(80);

void setup()
{
  ...
  
  ///////////////////////////////////

  // To be called before ETH.begin()
  ESP32_W5500_onEvent();

  // start the ethernet connection and the server:
  // Use DHCP dynamic IP and random mac
  //bool begin(int MISO_GPIO, int MOSI_GPIO, int SCLK_GPIO, int CS_GPIO, int INT_GPIO, int SPI_CLOCK_MHZ,
  //           int SPI_HOST, uint8_t *W5500_Mac = W5500_Default_Mac);
  ETH.begin( MISO_GPIO, MOSI_GPIO, SCK_GPIO, CS_GPIO, INT_GPIO, SPI_CLOCK_MHZ, ETH_SPI_HOST );
  //ETH.begin( MISO_GPIO, MOSI_GPIO, SCK_GPIO, CS_GPIO, INT_GPIO, SPI_CLOCK_MHZ, ETH_SPI_HOST, mac[millis() % NUMBER_OF_MAC] );

  // Static IP, leave without this line to get IP via DHCP
  //bool config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = 0, IPAddress dns2 = 0);
  //ETH.config(myIP, myGW, mySN, myDNS);

  ESP32_W5500_waitForConnect();

  ///////////////////////////////////

  apIP = ETH.localIP();

  ///////////////////////////////////

	// modify TTL associated  with the domain name (in seconds)
	// default is 60 seconds
	dnsServer.setTTL(300);
	// set which return code will be used for all other domains (e.g. sending
	// ServerFailure instead of NonExistentDomain will reduce number of queries
	// sent by clients)
	// default is AsyncDNSReplyCode::NonExistentDomain
	dnsServer.setErrorReplyCode(AsyncDNSReplyCode::ServerFailure);

	// start DNS server for a specific domain name
	dnsServer.start(DNS_PORT, "*", apIP);

  ...
}

void loop() 
{
}

```


---
---

### How to connect W5500, W6100 or ENC28J60 to ESP32

You can change the `INT` pin to another one. Default is `GPIO4`

```cpp
// Must connect INT to GPIOxx or not working
#define INT_GPIO            4
```


|W5500, W6100 or ENC28J60|<--->|ESP32|
|:-:|:-:|:-:|
|MOSI|<--->|GPIO23|
|MISO|<--->|GPIO19|
|SCK|<--->|GPIO18|
|CS/SS|<--->|GPIO5|
|INT|<--->|GPIO4|
|RST|<--->|RST|
|GND|<--->|GND|
|3.3V|<--->|3.3V|


---
---

### How to connect W5500, W6100 or ENC28J60 to ESP32_S2/S3/C3


##### W5500

`FULL_DUPLEX, 100Mbps`

<p align="center">
    <img src="https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet/raw/main/Images/W5500.png">
</p>

<p align="center">
    <img src="https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet/raw/main/Images/W5500_small.png">
</p> 
 

---

#### W6100

`FULL_DUPLEX, 100Mbps`

<p align="center">
    <img src="https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet/raw/main/Images/W6100.png">
</p>
   
---


##### ENC28J60

`FULL_DUPLEX, 10Mbps`

<p align="center">
    <img src="https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet/raw/main/Images/ENC28J60.png">
</p>

---


#### ESP32S3_DEV

<p align="center">
    <img src="https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet/raw/main/Images/ESP32S3_DEV.png">
</p> 

You can change the `INT` pin to another one. Default is `GPIO4`

```cpp
// Must connect INT to GPIOxx or not working
#define INT_GPIO            4
```

---

|W5500, W6100 or ENC28J60|<--->|ESP32_S3|
|:-:|:-:|:-:|
|MOSI|<--->|GPIO11|
|MISO|<--->|GPIO13|
|SCK|<--->|GPIO12|
|CS/SS|<--->|GPIO10|
|INT|<--->|GPIO4|
|RST|<--->|RST|
|GND|<--->|GND|
|3.3V|<--->|3.3V|

---

#### ESP32S2_DEV


<p align="center">
    <img src="https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet/raw/main/Images/ESP32S2_DEV.png">
</p> 


You can change the `INT` pin to another one. Default is `GPIO4`

```cpp
// Must connect INT to GPIOxx or not working
#define INT_GPIO            4
```

|W5500, W6100 or ENC28J60|<--->|ESP32_S2|
|:-:|:-:|:-:|
|MOSI|<--->|GPIO35|
|MISO|<--->|GPIO37|
|SCK|<--->|GPIO36|
|CS/SS|<--->|GPIO34|
|INT|<--->|GPIO4|
|RST|<--->|RST|
|GND|<--->|GND|
|3.3V|<--->|3.3V|


---

#### ESP32C3_DEV

<p align="center">
    <img src="https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet/raw/main/Images/ESP32_C3_DevKitC_02.png">
</p> 


You can change the `INT` pin to another one. Default is `GPIO4`

```cpp
// Must connect INT to GPIOxx or not working
#define INT_GPIO            10
```

|W5500, W6100 or ENC28J60|<--->|ESP32_C3|
|:-:|:-:|:-:|
|MOSI|<--->|GPIO6|
|MISO|<--->|GPIO5|
|SCK|<--->|GPIO4|
|CS/SS|<--->|GPIO7|
|INT|<--->|GPIO10|
|RST|<--->|RST|
|GND|<--->|GND|
|3.3V|<--->|3.3V|


---
---

### Examples

#### 1. For ESP32_W5500

 1. [AsyncCaptivePortalAdvanced_ESP32_W5500](examples/ESP32_W5500/AsyncCaptivePortalAdvanced_ESP32_W5500)
 2. [AsyncCaptivePortal_ESP32_W5500](examples/ESP32_W5500/AsyncCaptivePortal_ESP32_W5500)
 3. [AsyncDNServerFull_ESP32_W5500](examples/ESP32_W5500/AsyncDNServerFull_ESP32_W5500) 
 4. [AsyncDNSServer_ESP32_W5500](examples/ESP32_W5500/AsyncDNSServer_ESP32_W5500)
 
#### 2. For ESP32_ENC

 1. [AsyncCaptivePortalAdvanced_ESP32_ENC](examples/ESP32_ENC/AsyncCaptivePortalAdvanced_ESP32_ENC)
 2. [AsyncCaptivePortal_ESP32_ENC](examples/ESP32_ENC/AsyncCaptivePortal_ESP32_ENC)
 3. [AsyncDNServerFull_ESP32_ENC](examples/ESP32_ENC/AsyncDNServerFull_ESP32_ENC) 
 4. [AsyncDNSServer_ESP32_ENC](examples/ESP32_ENC/AsyncDNSServer_ESP32_ENC)

#### 3. For ESP32_SC_W5500

 1. [AsyncCaptivePortalAdvanced_ESP32_SC_W5500](examples/ESP32_SC_W5500/AsyncCaptivePortalAdvanced_ESP32_SC_W5500)
 2. [AsyncCaptivePortal_ESP32_SC_W5500](examples/ESP32_SC_W5500/AsyncCaptivePortal_ESP32_SC_W5500)
 3. [AsyncDNServerFull_ESP32_SC_W5500](examples/ESP32_SC_W5500/AsyncDNServerFull_ESP32_SC_W5500) 
 4. [AsyncDNSServer_ESP32_SC_W5500](examples/ESP32_SC_W5500/AsyncDNSServer_ESP32_SC_W5500)
 
#### 4. For ESP32_SC_ENC

 1. [AsyncCaptivePortalAdvanced_ESP32_SC_ENC](examples/ESP32_SC_ENC/AsyncCaptivePortalAdvanced_ESP32_SC_ENC)
 2. [AsyncCaptivePortal_ESP32_SC_ENC](examples/ESP32_SC_ENC/AsyncCaptivePortal_ESP32_SC_ENC)
 3. [AsyncDNServerFull_ESP32_SC_ENC](examples/ESP32_SC_ENC/AsyncDNServerFull_ESP32_SC_ENC) 
 4. [AsyncDNSServer_ESP32_SC_ENC](examples/ESP32_SC_ENC/AsyncDNSServer_ESP32_SC_ENC)
 
#### 5. For ESP32_W6100

 1. [AsyncCaptivePortalAdvanced_ESP32_W6100](examples/ESP32_W6100/AsyncCaptivePortalAdvanced_ESP32_W6100)
 2. [AsyncCaptivePortal_ESP32_W6100](examples/ESP32_W6100/AsyncCaptivePortal_ESP32_W6100)
 3. [AsyncDNServerFull_ESP32_W6100](examples/ESP32_W6100/AsyncDNServerFull_ESP32_W6100) 
 4. [AsyncDNSServer_ESP32_W6100](examples/ESP32_W6100/AsyncDNSServer_ESP32_W6100)
 
#### 6. For ESP32_SC_W6100

 1. [AsyncCaptivePortalAdvanced_ESP32_SC_W6100](examples/ESP32_SC_W6100/AsyncCaptivePortalAdvanced_ESP32_SC_W6100)
 2. [AsyncCaptivePortal_ESP32_SC_W6100](examples/ESP32_SC_W6100/AsyncCaptivePortal_ESP32_SC_W6100)
 3. [AsyncDNServerFull_ESP32_SC_W6100](examples/ESP32_SC_W6100/AsyncDNServerFull_ESP32_SC_W6100) 
 4. [AsyncDNSServer_ESP32_SC_W6100](examples/ESP32_SC_W6100/AsyncDNSServer_ESP32_SC_W6100)
 
   
--- 
---

### Example [AsyncDNSServer_ESP32_SC_W5500](examples/ESP32_SC_W5500/AsyncDNSServer_ESP32_SC_W5500)


https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet/blob/47558b20590536b7492533d2b5eeafaad3e57e53/examples/ESP32_SC_W5500/AsyncDNSServer_ESP32_SC_W5500/AsyncDNSServer_ESP32_SC_W5500.ino#L11-L186

---
---

### Debug

Debug is enabled by default on Serial. To disable, use level 0

```cpp
#define ASYNC_DNS_ESP32_ETHERNET_DEBUG_PORT      Serial

// Use from 0 to 4. Higher number, more debugging messages and memory usage.
#define _ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_      0
```

You can also change the debugging level from 0 to 4

```cpp
#define ASYNC_DNS_ESP32_ETHERNET_DEBUG_PORT      Serial


// Use from 0 to 4. Higher number, more debugging messages and memory usage.
#define _ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_    4
```

---

### Troubleshooting

If you get compilation errors, more often than not, you may need to install a newer version of Arduino IDE, the [Arduino `ESP32`](https://github.com/espressif/arduino-esp32) core or depending libraries.

Sometimes, the library will only work if you update the [`ESP32`](https://github.com/espressif/arduino-esp32) core to the latest and stable version because I'm always using the latest stable cores/libraries.


---
---


### Issues

Submit issues to: [AsyncDNSServer_ESP32_Ethernet issues](https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet/issues)

---


## TO DO

 1. Fix bug. Add enhancement
 2. Add support to more `LwIP Ethernet` shields


## DONE

 1. Add support to **ESP32/S3-based boards using LwIP W5500 or ENC28J60 Ethernet**
 2. Add debugging features
 3. Add astyle using `allman` style. Restyle the library
 4. Add support to **ESP32_S2/C3-based boards using LwIP W5500 or ENC28J60 Ethernet**
 4. Add support to **ESP32/S2/S3/C3-based boards using LwIP W6100 Ethernet**
 
 
---
---

### Contributions and Thanks

1. Based on and modified from [Develo's ESPAsyncDNSServer Library](https://github.com/devyte/ESPAsyncDNSServer).


<table>
  <tr>
    <td align="center"><a href="https://github.com/devyte"><img src="https://github.com/devyte.png" width="100px;" alt="devyte"/><br/><sub><b>⭐️ Develo</b></sub></a><br/></td>
  </tr> 
</table>

---

### Contributing

If you want to contribute to this project:

- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

---
---

### License

- The library is licensed under [GPLv3](https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet/blob/main/LICENSE)

---

## Copyright

- Copyright (c) 2016- Develo
- Copyright (c) 2022- Khoi Hoang


