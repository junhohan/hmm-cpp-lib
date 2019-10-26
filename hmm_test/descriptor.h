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

#ifndef DESCRIPTOR_H_
#define DESCRIPTOR_H_

#include <iostream>
#include <string>

namespace hmm {

class Descriptor;
bool operator<(const Descriptor& l, const Descriptor& r);
bool operator==(const Descriptor& l, const Descriptor& r);

class Descriptor {
 public:
  Descriptor();
  Descriptor(const std::string& desc);
  ~Descriptor();
  friend bool operator<(const Descriptor& l, const Descriptor& r);
  friend bool operator==(const Descriptor& l, const Descriptor& r);
  const static std::string kR2R;
  const static std::string kR2S;
  const static std::string kS2R;
  const static std::string kS2S;
  std::string desc_;
};

}  // namespace hmm

#endif  // DESCRIPTOR_H_
