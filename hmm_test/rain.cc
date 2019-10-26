/**
* Copyright (C) 2015~2016, BMW AG
* Author: Stefan Holder (stefan.holder@bmw.de)
* C++ Conversion: Junho Han (junhohan.kr@gmail.com)
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "rain.h"

namespace hmm {

const std::string Rain::kRain = "RAIN";
const std::string Rain::kSun = "SUN";

Rain::Rain() : weather_("") {}
Rain::Rain(const std::string& weather) { weather_ = weather; }
Rain::~Rain() {}

bool operator<(const Rain& l, const Rain& r) {
  if (l.weather_ < r.weather_) {
    return true;
  }
  return false;
}

bool operator==(const Rain& l, const Rain& r) {
  if (l.weather_ == r.weather_) {
    return true;
  }
  return false;
}

}  // namespace hmm
