// NixieCalc
// lightweight calculator lib
// designed for nixie calculators...
// uses double type, precision depends on MC
// 64-bit on ESP32
// Version 0.1.0 (alpha)

/* 
MIT license
Copyright (C) 2021 highvoltglow

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <Arduino.h>
#include <math.h>


// MIN/MAX VALUES, defined for 14-digit calculator
#define MAX_CALC_VALUE 99999999999999
#define MIN_CALC_VALUE -99999999999999

//enums
enum class operation : uint8_t
{
  none,
  addition,
  subtraction,
  division,
  multiplication,
  squareroot,
  percent,
  equals,
  memclear,
  memread,
  memstore,
  memsubtraction,
  memaddition,
  allclear,
  clear,
  switchsign,
  inv,
  pow,
  sin,
  cos,
  tan,
  log,
  ln
};

enum class operation_return_code : uint8_t
{
  success,
  overflow,
  divideByZero,
  domain,
  unknownoperation
};

enum class angle_mode : uint8_t
{
  deg, 
  rad
};

// calculator engine class
class NixieCalc
{
public:
  NixieCalc();

  // user input
  void onOperation(operation op);
  void onNumericInput(double value);

  // get return code of math operation
  operation_return_code getOperationReturnCode();

  // get input/operation result
  double getDisplayValue();

  // get/set angle mode
  angle_mode getAngleMode();
  void setAngleMode(angle_mode angleMode);

private:
  // numeric registers
  double _displayValue;
  double _leftValue;
  double _rightValue;
  double _memoryValue;

  // return code of math operations
  operation_return_code _operationReturnCode;

  // angle mode
  angle_mode _angleMode;

  // state variables
  bool _numberEntered;
  bool _equalsEntered;

  // current operation
  operation _operation;

  // functions for operation types
  void onDualValueOperation(operation op);
  void onSingleValueOperation(operation op);
  void onMemoryOperation(operation op);
  void onClearOperation(operation op);
  void onPercentOperation(operation op);
  void onEqualsOperation(operation op);

  // clean up
  void onAllClear();

  // math operations
  operation_return_code calculateValue(double *result, operation op, double leftValue, double rightValue=0.0);

};
