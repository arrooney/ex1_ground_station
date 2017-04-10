/* Copyright (C) 2016  Brendan Bruner
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * bbruner@ualberta.ca
 *
 * This test suite was inspired from the test suite at:
 * http://www.jera.com/techinfo/jtns/jtn002.html - January 2015
 */

#include "unit.h"

unsigned int unit_asserts_passed = 0, delta_unit_asserts_passed = 0;
unsigned int unit_asserts_failed = 0, delta_unit_asserts_failed = 0;
unsigned int unit_tests_run = 0;
unsigned int unit_tests_aborted = 0;
unsigned int unit_asserts_failed_pre_test = 0;
unsigned int unit_tests_passed = 0, unit_tests_failed = 0;
