/*
 * Copyright (c) 2024 HiSilicon Technologies CO., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H

#define CONFIG_WIFI_SSID "小泡泡"      // 要连接的WiFi 热点账号
#define CONFIG_WIFI_PWD "zxc1177656"        // 要连接的WiFi 热点密码
#define CONFIG_SERVER_IP "192.168.110.138" // 要连接的服务器IP
#define CONFIG_SERVER_PORT 8000            // 要连接的服务器端口
errcode_t wifi_connect(void);
#endif