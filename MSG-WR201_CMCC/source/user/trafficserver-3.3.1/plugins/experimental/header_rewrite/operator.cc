/*
  Licensed to the Apache Software Foundation (ASF) under one
  or more contributor license agreements.  See the NOTICE file
  distributed with this work for additional information
  regarding copyright ownership.  The ASF licenses this file
  to you under the Apache License, Version 2.0 (the
  "License"); you may not use this file except in compliance
  with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/
//////////////////////////////////////////////////////////////////////////////////////////////
// operator.cc: Implementation of the operator base class
//
//

#define UNUSED __attribute__ ((unused))
static char UNUSED rcsId__operator_cc[] = "@(#) $Id: operator.cc,v 1.2 2013/06/03 15:59:26 chen.bin Exp $ built on " __DATE__ " " __TIME__;

#include <ts/ts.h>
#include "operator.h"

const OperModifiers
Operator::get_oper_modifiers() const {
  if (_next)
    return static_cast<OperModifiers>(_mods | static_cast<Operator*>(_next)->get_oper_modifiers());

  return _mods;
}

void
Operator::initialize(Parser& p) {
  Statement::initialize(p);

  if (p.mod_exist("L")) {
    _mods = static_cast<OperModifiers>(_mods | OPER_LAST);
  }

  if (p.mod_exist("QSA")) {
    _mods = static_cast<OperModifiers>(_mods | OPER_QSA);
  }

}
