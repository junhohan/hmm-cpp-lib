/**
* Copyright (C) 2016, BMW AG
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

#ifndef TRANSITION_H_
#define TRANSITION_H_

#include <iostream>

namespace hmm {

template <typename S>
class Transition;
template <typename S>
bool operator==(const Transition<S>& lhs, const Transition<S>& rhs);
template <typename S>
bool operator<(const Transition<S>& lhs, const Transition<S>& rhs);

template <typename S>
class Transition {
 public:
  const S fromCandidate;
  const S toCandidate;
  friend bool operator==<>(const Transition<S>& lhs, const Transition<S>& rhs);
  friend bool operator< <>(const Transition<S>& lhs, const Transition<S>& rhs);

  Transition(S fromCandidate, S toCandidate)
      : fromCandidate(fromCandidate), toCandidate(toCandidate) {}
  ~Transition() {}

  std::string ToString() {
    return "Transition [fromCandidate=" + fromCandidate +
           ", toCandidate=" + toCandidate + "]";
  }
};

template <typename S>
bool operator==(const Transition<S>& lhs, const Transition<S>& rhs) {
  auto rst = lhs.fromCandidate == rhs.fromCandidate &&
             lhs.toCandidate == rhs.toCandidate;
  return rst;
}
template <typename S>
bool operator<(Transition<S> const& lhs, Transition<S> const& rhs) {
  if (lhs.fromCandidate < rhs.fromCandidate) {
    return true;
  } else if (lhs.fromCandidate == rhs.fromCandidate) {
    if (lhs.toCandidate < rhs.toCandidate) {
      return true;
    }
  }
  return false;
}

}  // namespace hmm

#endif  // TRANSITION_H_
