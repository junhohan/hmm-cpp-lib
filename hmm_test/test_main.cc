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

#include "test_main.h"
#include <cmath>
#include <map>
#include <vector>

#include "descriptor.h"
#include "rain.h"
#include "transition.h"
#include "umbrella.h"
#include "viterbi_algorithm.h"

namespace hmm {

void TestMain::TestComputeMostLikelySequence() {
  printf("\n:: TestComputeMostLikelySequence ::\n");

  std::vector<Rain> candidates;
  candidates.push_back(Rain(Rain::kRain));
  candidates.push_back(Rain(Rain::kSun));

  std::map<Rain, double> emissionLogProbabilitiesForUmbrella;
  emissionLogProbabilitiesForUmbrella.emplace(Rain(Rain::kRain),
                                              (double)log(0.9));
  emissionLogProbabilitiesForUmbrella.emplace(Rain(Rain::kSun),
                                              (double)log(0.2));

  std::map<Rain, double> emissionLogProbabilitiesForNoUmbrella;
  emissionLogProbabilitiesForNoUmbrella.emplace(Rain(Rain::kRain),
                                                (double)log(0.1));
  emissionLogProbabilitiesForNoUmbrella.emplace(Rain(Rain::kSun),
                                                (double)log(0.8));

  std::map<Transition<Rain>, double> transitionLogProbabilities;
  transitionLogProbabilities.emplace(
      Transition<Rain>(Rain(Rain::kRain), Rain(Rain::kRain)), (double)log(0.7));
  transitionLogProbabilities.emplace(
      Transition<Rain>(Rain(Rain::kRain), Rain(Rain::kSun)), (double)log(0.3));
  transitionLogProbabilities.emplace(
      Transition<Rain>(Rain(Rain::kSun), Rain(Rain::kRain)), (double)log(0.3));
  transitionLogProbabilities.emplace(
      Transition<Rain>(Rain(Rain::kSun), Rain(Rain::kSun)), (double)log(0.7));

  //  int index = 0;
  //  for(auto const &item : transitionLogProbabilities) {
  //    auto const &key = item.first;
  //    auto const val = item.second;
  //    printf("transitionLogProbabilities %d: %s %s %f\n",
  //           index,
  //           key.fromCandidate.weather_.c_str(),
  //           key.toCandidate.weather_.c_str(),
  //           val);
  //    ++index;
  //  }

  std::map<Transition<Rain>, Descriptor> transitionDescriptors;
  transitionDescriptors.emplace(
      Transition<Rain>(Rain(Rain::kRain), Rain(Rain::kRain)),
      Descriptor(Descriptor::kR2R));
  transitionDescriptors.emplace(
      Transition<Rain>(Rain(Rain::kRain), Rain(Rain::kSun)),
      Descriptor(Descriptor::kR2S));
  transitionDescriptors.emplace(
      Transition<Rain>(Rain(Rain::kSun), Rain(Rain::kRain)),
      Descriptor(Descriptor::kS2R));
  transitionDescriptors.emplace(
      Transition<Rain>(Rain(Rain::kSun), Rain(Rain::kSun)),
      Descriptor(Descriptor::kS2S));

  ViterbiAlgorithm<hmm::Rain, hmm::Umbrella, hmm::Descriptor> viterbi;
  viterbi.StartWithInitialObservation(hmm::Umbrella(Umbrella::kYesUmbr),
                                      candidates,
                                      emissionLogProbabilitiesForUmbrella);
  viterbi.NextStep(hmm::Umbrella(Umbrella::kYesUmbr), candidates,
                   emissionLogProbabilitiesForUmbrella,
                   transitionLogProbabilities, transitionDescriptors);
  viterbi.NextStep(hmm::Umbrella(Umbrella::kNoUmbr), candidates,
                   emissionLogProbabilitiesForNoUmbrella,
                   transitionLogProbabilities, transitionDescriptors);
  viterbi.NextStep(hmm::Umbrella(Umbrella::kYesUmbr), candidates,
                   emissionLogProbabilitiesForUmbrella,
                   transitionLogProbabilities, transitionDescriptors);
  std::vector<SequenceState<hmm::Rain, hmm::Umbrella, hmm::Descriptor>> result =
      viterbi.ComputeMostLikelySequence();

  if (result.size() != 4) {
    printf("ERR: Result count must be 4, but %d.\n", (int)result.size());
    return;
  }

  // assertEquals(Rain.T, result.get(0).state);
  if (result.at(0).state.weather_ != hmm::Rain::kRain) {
    printf("ERR: Rain Index 0 must be RAIN, but %s.\n",
           result.at(0).state.weather_.c_str());
  }
  if (result.at(1).state.weather_ != hmm::Rain::kRain) {
    printf("ERR: Rain Index 1 must be RAIN, but %s.\n",
           result.at(1).state.weather_.c_str());
  }
  if (result.at(2).state.weather_ != hmm::Rain::kSun) {
    printf("ERR: Rain Index 2 must be RAIN, but %s.\n",
           result.at(2).state.weather_.c_str());
  }
  if (result.at(3).state.weather_ != hmm::Rain::kRain) {
    printf("ERR: Rain Index 3 must be RAIN, but %s.\n",
           result.at(3).state.weather_.c_str());
  }

  if (result.at(0).observation.umbrella_ != hmm::Umbrella::kYesUmbr) {
    printf("ERR: Umbrella Index 0 must be %s, but %s.\n",
           hmm::Umbrella::kYesUmbr.c_str(),
           result.at(0).state.weather_.c_str());
  }
  if (result.at(1).observation.umbrella_ != hmm::Umbrella::kYesUmbr) {
    printf("ERR: Umbrella Index 1 must be %s, but %s.\n",
           hmm::Umbrella::kYesUmbr.c_str(),
           result.at(1).state.weather_.c_str());
  }
  if (result.at(2).observation.umbrella_ != hmm::Umbrella::kNoUmbr) {
    printf("ERR: Umbrella Index 2 must be %s, but %s.\n",
           hmm::Umbrella::kNoUmbr.c_str(), result.at(2).state.weather_.c_str());
  }
  if (result.at(3).observation.umbrella_ != hmm::Umbrella::kYesUmbr) {
    printf("ERR: Umbrella Index 3 must be %s, but %s.\n",
           hmm::Umbrella::kYesUmbr.c_str(),
           result.at(3).state.weather_.c_str());
  }
  if (!result.at(0).transitionDescriptor.desc_.empty()) {
    printf("ERR: Descriptor Index 0 must be empty, but %s.\n",
           result.at(0).transitionDescriptor.desc_.c_str());
  }
  if (result.at(1).transitionDescriptor.desc_ != hmm::Descriptor::kR2R) {
    printf("ERR: Descriptor Index 1 must be %s, but %s.\n",
           hmm::Descriptor::kR2R.c_str(),
           result.at(1).transitionDescriptor.desc_.c_str());
  }
  if (result.at(2).transitionDescriptor.desc_ != hmm::Descriptor::kR2S) {
    printf("ERR: Descriptor Index 2 must be %s, but %s.\n",
           hmm::Descriptor::kR2S.c_str(),
           result.at(2).transitionDescriptor.desc_.c_str());
  }
  if (result.at(3).transitionDescriptor.desc_ != hmm::Descriptor::kS2R) {
    printf("ERR: Descriptor Index 3 must be %s, but %s.\n",
           hmm::Descriptor::kS2R.c_str(),
           result.at(3).transitionDescriptor.desc_.c_str());
  }
  // assertEquals(null,  result.get(0).transitionDescriptor);
  // assertEquals(Descriptor.R2R, result.get(1).transitionDescriptor);
  //  assertEquals(Descriptor.R2S, result.get(2).transitionDescriptor);
  //  assertEquals(Descriptor.S2R, result.get(3).transitionDescriptor);
  if (viterbi.IsBroken()) {
    printf("ERR: Viterbi was BROKEN!!!\n");
  }
  auto actualMessageHistory = viterbi.MessageHistory();
  // Check message history
  std::vector<std::map<Rain, double>> expectedMessageHistory;
  std::map<Rain, double> message1;
  message1.emplace(Rain(Rain::kRain), 0.9);
  message1.emplace(Rain(Rain::kSun), 0.2);
  expectedMessageHistory.push_back(message1);

  std::map<Rain, double> message2;
  message2.emplace(Rain(Rain::kRain), 0.567);
  message2.emplace(Rain(Rain::kSun), 0.054);
  expectedMessageHistory.push_back(message2);

  std::map<Rain, double> message3;
  message3.emplace(Rain(Rain::kRain), 0.03969);
  message3.emplace(Rain(Rain::kSun), 0.13608);
  expectedMessageHistory.push_back(message3);

  std::map<Rain, double> message4;
  message4.emplace(Rain(Rain::kRain), 0.0367416);
  message4.emplace(Rain(Rain::kSun), 0.0190512);
  expectedMessageHistory.push_back(message4);
  CheckMessageHistory(expectedMessageHistory, actualMessageHistory);
}
void TestMain::CheckMessageHistory(
    std::vector<std::map<Rain, double>> expectedMessageHistory,
    std::vector<std::map<Rain, double>> actualMessageHistory) {
  // assertEquals(expectedMessageHistory.size(), actualMessageHistory.size());
  if (expectedMessageHistory.size() == actualMessageHistory.size()) {
    printf("CheckMessageHistory() GOOD: result is the same count.\n");
  }
  for (int i = 0; i < expectedMessageHistory.size(); i++) {
    CheckMessage(expectedMessageHistory[i], actualMessageHistory[i]);
  }
}

void TestMain::CheckMessage(std::map<Rain, double> expectedMessage,
                            std::map<Rain, double> actualMessage) {
  // assertEquals(expectedMessage.size(), actualMessage.size());
  if (expectedMessage.size() == actualMessage.size()) {
    printf("CheckMessage() GOOD: result is the same count.\n");
  }

  static double DELTA = 1e-8;
  for (auto& entry : expectedMessage) {
    // assertEquals(entry.second, std::exp(actualMessage[entry.first], DELTA);
    auto f = entry.second;
    printf("CheckMessage() entry.second value is %f\n", f);
    auto b = std::exp(actualMessage[entry.first]);
    auto value = std::abs(f - b);
    if (value < DELTA) {
      printf("CheckMessage() GOOD: value is good\n");
    }
  }
}

void TestMain::TestDeterministicCandidateOrder() {
  std::vector<Rain> candidates;
  candidates.push_back(Rain(Rain::kRain));
  candidates.push_back(Rain(Rain::kSun));

  // Reverse usual order of emission and transition probabilities keys since
  // their order should not matter.
  std::map<Rain, double> emissionLogProbabilitiesForUmbrella;
  emissionLogProbabilitiesForUmbrella.emplace(Rain(Rain::kRain),
                                              (double)log(0.5));
  emissionLogProbabilitiesForUmbrella.emplace(Rain(Rain::kSun),
                                              (double)log(0.5));

  std::map<Rain, double> emissionLogProbabilitiesForNoUmbrella;
  emissionLogProbabilitiesForNoUmbrella.emplace(Rain(Rain::kRain),
                                                (double)log(0.5));
  emissionLogProbabilitiesForNoUmbrella.emplace(Rain(Rain::kSun),
                                                (double)log(0.5));

  std::map<Transition<Rain>, double> transitionLogProbabilities;
  transitionLogProbabilities.emplace(
      Transition<Rain>(Rain(Rain::kSun), Rain(Rain::kRain)), (double)log(0.5));
  transitionLogProbabilities.emplace(
      Transition<Rain>(Rain(Rain::kSun), Rain(Rain::kSun)), (double)log(0.5));
  transitionLogProbabilities.emplace(
      Transition<Rain>(Rain(Rain::kRain), Rain(Rain::kRain)), (double)log(0.5));
  transitionLogProbabilities.emplace(
      Transition<Rain>(Rain(Rain::kRain), Rain(Rain::kSun)), (double)log(0.5));

  ViterbiAlgorithm<hmm::Rain, hmm::Umbrella, hmm::Descriptor> viterbi;
  viterbi.StartWithInitialObservation(hmm::Umbrella(Umbrella::kYesUmbr),
                                      candidates,
                                      emissionLogProbabilitiesForUmbrella);
  viterbi.NextStep(hmm::Umbrella(Umbrella::kYesUmbr), candidates,
                   emissionLogProbabilitiesForUmbrella,
                   transitionLogProbabilities);
  viterbi.NextStep(hmm::Umbrella(Umbrella::kNoUmbr), candidates,
                   emissionLogProbabilitiesForNoUmbrella,
                   transitionLogProbabilities);
  viterbi.NextStep(hmm::Umbrella(Umbrella::kYesUmbr), candidates,
                   emissionLogProbabilitiesForUmbrella,
                   transitionLogProbabilities);

  std::vector<SequenceState<hmm::Rain, hmm::Umbrella, hmm::Descriptor>> result =
      viterbi.ComputeMostLikelySequence();

  // Check most likely sequence
  if (result.size() != 4) {
    printf(
        "ERR: Result count must be 4, but %d. "
        "TestDeterministicCandidateOrder()\n",
        (int)result.size());
    return;
  }

  if (result.at(0).state.weather_ != hmm::Rain::kRain) {
    printf(
        "ERR: Rain Index 0 must be RAIN, but %s. "
        "TestDeterministicCandidateOrder()\n",
        result.at(0).state.weather_.c_str());
  }
  if (result.at(1).state.weather_ != hmm::Rain::kRain) {
    printf(
        "ERR: Rain Index 1 must be RAIN, but %s. "
        "TestDeterministicCandidateOrder()\n",
        result.at(1).state.weather_.c_str());
  }
  if (result.at(2).state.weather_ != hmm::Rain::kRain) {
    printf(
        "ERR: Rain Index 2 must be RAIN, but %s. "
        "TestDeterministicCandidateOrder()\n",
        result.at(2).state.weather_.c_str());
  }
  if (result.at(3).state.weather_ != hmm::Rain::kRain) {
    printf(
        "ERR: Rain Index 3 must be RAIN, but %s. "
        "TestDeterministicCandidateOrder()\n",
        result.at(3).state.weather_.c_str());
  }
}
void TestMain::TestEmptySequence() {
  ViterbiAlgorithm<hmm::Rain, hmm::Umbrella, hmm::Descriptor> viterbi;
  std::vector<SequenceState<hmm::Rain, hmm::Umbrella, hmm::Descriptor>> result =
      viterbi.ComputeMostLikelySequence();
  if (result.empty()) {
    printf("TestEmptySequence() GOOD: result is empty!\n");
  }
  if (viterbi.IsBroken() == false) {
    printf("TestEmptySequence() GOOD: result is not BROKEN!\n");
  }
}
void TestMain::TestBreakAtInitialMessage() {
  ViterbiAlgorithm<hmm::Rain, hmm::Umbrella, hmm::Descriptor> viterbi;
  std::vector<hmm::Rain> candidates;
  candidates.push_back(Rain(Rain::kRain));
  candidates.push_back(Rain(Rain::kSun));

  std::map<Rain, double> emissionLogProbabilities;
  emissionLogProbabilities.emplace(Rain(Rain::kRain), log(0.0));
  emissionLogProbabilities.emplace(Rain(Rain::kSun), log(0.0));
  viterbi.StartWithInitialObservation(hmm::Umbrella(Umbrella::kYesUmbr),
                                      candidates, emissionLogProbabilities);
  // assertTrue(viterbi.IsBroken());
  if (viterbi.IsBroken()) {
    printf("TestBreakAtInitialMessage() GOOD: result is BROKEN!\n");
  }
  // assertEquals(Arrays.asList(), viterbi.ComputeMostLikelySequence());
  if (viterbi.ComputeMostLikelySequence().empty()) {
    printf("TestBreakAtInitialMessage() GOOD: result is empty!\n");
  }
}
void TestMain::TestEmptyInitialMessage() {
  ViterbiAlgorithm<Rain, Umbrella, Descriptor> viterbi;
  std::vector<Rain> vrain;
  std::map<Rain, double> mrain;
  viterbi.StartWithInitialObservation(hmm::Umbrella(Umbrella::kYesUmbr), vrain,
                                      mrain);
  //  assertTrue(viterbi.IsBroken());
  if (viterbi.IsBroken()) {
    printf("TestEmptyInitialMessage() GOOD: result is BROKEN!\n");
  }
  //  assertEquals(Arrays.asList(), viterbi.computeMostLikelySequence());
  if (viterbi.ComputeMostLikelySequence().empty()) {
    printf("TestEmptyInitialMessage() GOOD: result is empty!\n");
  }
}
void TestMain::TestBreakAtFirstTransition() {
  ViterbiAlgorithm<Rain, Umbrella, Descriptor> viterbi;
  std::vector<Rain> candidates;
  candidates.push_back(Rain(Rain::kRain));
  candidates.push_back(Rain(Rain::kSun));

  std::map<Rain, double> emissionLogProbabilities;
  emissionLogProbabilities.emplace(Rain(Rain::kRain), log(0.9));
  emissionLogProbabilities.emplace(Rain(Rain::kSun), log(0.2));

  viterbi.StartWithInitialObservation(hmm::Umbrella(Umbrella::kYesUmbr),
                                      candidates, emissionLogProbabilities);
  // assertFalse(viterbi.IsBroken());
  if (viterbi.IsBroken() == false) {
    printf("StartWithInitialObservation() GOOD: result is not BROKEN!\n");
  }
  std::map<Transition<Rain>, double> transitionLogProbabilities;
  transitionLogProbabilities.emplace(
      Transition<Rain>(Rain(Rain::kRain), Rain(Rain::kRain)), log(0.0));
  transitionLogProbabilities.emplace(
      Transition<Rain>(Rain(Rain::kRain), Rain(Rain::kSun)), log(0.0));
  transitionLogProbabilities.emplace(
      Transition<Rain>(Rain(Rain::kSun), Rain(Rain::kRain)), log(0.0));
  transitionLogProbabilities.emplace(
      Transition<Rain>(Rain(Rain::kSun), Rain(Rain::kSun)), log(0.0));
  viterbi.NextStep(hmm::Umbrella(Umbrella::kYesUmbr), candidates,
                   emissionLogProbabilities, transitionLogProbabilities);

  // assertTrue(viterbi.IsBroken());
  if (viterbi.IsBroken()) {
    printf("TestBreakAtFirstTransition() GOOD: result is BROKEN!\n");
  }
  // assertEquals(Arrays.asList(Rain.T),
  // states(viterbi.computeMostLikelySequence()));
  std::vector<Rain> maybe;
  maybe.push_back(Rain(Rain::kRain));

  auto result = viterbi.ComputeMostLikelySequence();
  if (result[0].state == maybe[0]) {
    printf("TestBreakAtFirstTransition() GOOD: result's state is RAIN!\n");
  }
}
void TestMain::TestBreakAtFirstTransitionWithNoCandidates() {
  ViterbiAlgorithm<Rain, Umbrella, Descriptor> viterbi;
  std::vector<Rain> candidates;
  candidates.push_back(Rain(Rain::kRain));
  candidates.push_back(Rain(Rain::kSun));

  std::map<Rain, double> emissionLogProbabilities;
  emissionLogProbabilities.emplace(Rain(Rain::kRain), log(0.9));
  emissionLogProbabilities.emplace(Rain(Rain::kSun), log(0.2));
  viterbi.StartWithInitialObservation(hmm::Umbrella(Umbrella::kYesUmbr),
                                      candidates, emissionLogProbabilities);
  // assertFalse(viterbi.IsBroken());
  if (viterbi.IsBroken() == false) {
    printf(
        "TestBreakAtFirstTransitionWithNoCandidates() GOOD: result is not "
        "BROKEN!\n");
  }

  std::vector<Rain> candidates_temp;
  std::map<Rain, double> emissionLogProbabilities_temp;
  std::map<Transition<Rain>, double> transitionLogProbabilities_temp;
  std::map<Rain, double> rain_map;
  viterbi.NextStep(hmm::Umbrella(Umbrella::kYesUmbr), candidates_temp,
                   emissionLogProbabilities_temp,
                   transitionLogProbabilities_temp);
  //  assertTrue(viterbi.isBroken());
  if (viterbi.IsBroken()) {
    printf(
        "TestBreakAtFirstTransitionWithNoCandidates() GOOD: result is "
        "BROKEN!\n");
  }
  //  assertEquals(Arrays.asList(Rain.T),
  //  states(viterbi.computeMostLikelySequence()));
  std::vector<Rain> maybe;
  maybe.push_back(Rain(Rain::kRain));
  auto result = viterbi.ComputeMostLikelySequence();
  if (result[0].state == maybe[0]) {
    printf(
        "TestBreakAtFirstTransitionWithNoCandidates() GOOD: result's state is "
        "RAIN!\n");
  }
}
void TestMain::TestBreakAtSecondTransition() {
  ViterbiAlgorithm<Rain, Umbrella, Descriptor> viterbi;
  std::vector<Rain> candidates;
  candidates.push_back(Rain(Rain::kRain));
  candidates.push_back(Rain(Rain::kSun));

  std::map<Rain, double> emissionLogProbabilities;
  emissionLogProbabilities.emplace(Rain(Rain::kRain), log(0.9));
  emissionLogProbabilities.emplace(Rain(Rain::kSun), log(0.2));
  viterbi.StartWithInitialObservation(hmm::Umbrella(Umbrella::kYesUmbr),
                                      candidates, emissionLogProbabilities);
  // assertFalse(viterbi.isBroken());
  if (viterbi.IsBroken() == false) {
    printf("TestBreakAtSecondTransition() GOOD: result is not BROKEN!\n");
  }

  std::map<Transition<Rain>, double> transitionLogProbabilities;
  transitionLogProbabilities.emplace(
      Transition<Rain>(Rain(Rain::kRain), Rain(Rain::kRain)), (double)log(0.5));
  transitionLogProbabilities.emplace(
      Transition<Rain>(Rain(Rain::kRain), Rain(Rain::kSun)), (double)log(0.5));
  transitionLogProbabilities.emplace(
      Transition<Rain>(Rain(Rain::kSun), Rain(Rain::kRain)), (double)log(0.5));
  transitionLogProbabilities.emplace(
      Transition<Rain>(Rain(Rain::kSun), Rain(Rain::kSun)), (double)log(0.5));
  viterbi.NextStep(hmm::Umbrella(Umbrella::kYesUmbr), candidates,
                   emissionLogProbabilities, transitionLogProbabilities);

  // assertFalse(viterbi.isBroken());
  if (viterbi.IsBroken() == false) {
    printf("TestBreakAtSecondTransition() GOOD: result is not BROKEN!\n");
  }

  std::map<Transition<Rain>, double> transitionLogProbabilities2;
  transitionLogProbabilities2.emplace(
      Transition<Rain>(Rain(Rain::kRain), Rain(Rain::kRain)), (double)log(0.0));
  transitionLogProbabilities2.emplace(
      Transition<Rain>(Rain(Rain::kRain), Rain(Rain::kSun)), (double)log(0.0));
  transitionLogProbabilities2.emplace(
      Transition<Rain>(Rain(Rain::kSun), Rain(Rain::kRain)), (double)log(0.0));
  transitionLogProbabilities2.emplace(
      Transition<Rain>(Rain(Rain::kSun), Rain(Rain::kSun)), (double)log(0.0));
  viterbi.NextStep(hmm::Umbrella(Umbrella::kYesUmbr), candidates,
                   emissionLogProbabilities, transitionLogProbabilities2);

  // assertTrue(viterbi.isBroken());
  if (viterbi.IsBroken()) {
    printf("TestBreakAtSecondTransition() GOOD: result is BROKEN!\n");
  }
  // assertEquals(Arrays.asList(Rain.T, Rain.T),
  // states(viterbi.computeMostLikelySequence()));
  std::vector<Rain> maybe;
  maybe.push_back(Rain(Rain::kRain));
  maybe.push_back(Rain(Rain::kRain));
  auto result = viterbi.ComputeMostLikelySequence();
  if (result[0].state == maybe[0] && result[1].state == maybe[1]) {
    printf("TestBreakAtSecondTransition() GOOD: result's state is RAIN!\n");
  }
}
}  // namespace hmm
