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

#ifndef RAIN_H_
#define RAIN_H_

#include <iostream>
#include <string>

// TEST
namespace hmm {

class Rain;
bool operator<(const Rain& l, const Rain& r);
bool operator==(const Rain& l, const Rain& r);

class Rain {
 public:
  Rain();
  Rain(const std::string& weather);
  ~Rain();
  friend bool operator<(const Rain& l, const Rain& r);
  friend bool operator==(const Rain& l, const Rain& r);
  const static std::string kRain;
  const static std::string kSun;
  std::string weather_;
};

// TEST
}  // namespace hmm

#endif  // RAIN_H_
