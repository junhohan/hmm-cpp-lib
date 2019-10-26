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

#ifndef umbrella_hpp
#define umbrella_hpp

#include <iostream>
#include <string>

namespace hmm {

class Umbrella;
bool operator<(const Umbrella& l, const Umbrella& r);
bool operator==(const Umbrella& l, const Umbrella& r);

class Umbrella {
 public:
  Umbrella();
  Umbrella(const std::string& umbr);
  ~Umbrella();
  friend bool operator<(const Umbrella& l, const Umbrella& r);
  const static std::string kYesUmbr;
  const static std::string kNoUmbr;
  std::string umbrella_;
};

}  // namespace hmm
#endif  // umbrella_hpp
