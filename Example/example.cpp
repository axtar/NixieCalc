// NixieCalc example

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

#include <Arduino.h>
#include <NixieCalc.h>

// the nixie calculator has 14 digits, 64-bit doubles have a precision of near 16 digits, should be good enough
#define DIGIT_COUNT 14 

// some macros
#define NUM(x) nixieCalc.onNumericInput(x)
#define ONOP(x) nixieCalc.onOperation(x)
#define SHOW printResult(nixieCalc.getDisplayValue(), nixieCalc.getOperationReturnCode())
#define OP operation

// forward declarations
String formatNumber(double number);
void printResult(double value, operation_return_code retCode);

// globals
NixieCalc nixieCalc;

void setup() 
{
  Serial.begin(115200);
  Serial.println();
  nixieCalc.setAngleMode(angle_mode::deg); // set to degrees for trigonometric functions
}

void loop() 
{
  // make some calculations...
  // call onNumericInput() to process a numeric value, for example 5.25
  // call onOperation to process an operation (see operation enum for supported operations)
  // call getDisplayValue after every input or operation to refresh the nixie display
  Serial.print("00: [1.5] [+] [3] [=] --> " );
  nixieCalc.onNumericInput(1.5);              Serial.printf("[%s]", formatNumber(nixieCalc.getDisplayValue()).c_str());
  nixieCalc.onOperation(operation::addition); Serial.printf("[%s]", formatNumber(nixieCalc.getDisplayValue()).c_str());
  nixieCalc.onNumericInput(3);                Serial.printf("[%s]", formatNumber(nixieCalc.getDisplayValue()).c_str());
  nixieCalc.onOperation(operation::equals);  
  Serial.printf("     "); 
  printResult(nixieCalc.getDisplayValue(), nixieCalc.getOperationReturnCode());

  // now calling getDisplayValue only for the result and using macros
  Serial.print("01: [1] [+] [3] [=] [=]                     "); NUM(1); ONOP(OP::addition); NUM(3); ONOP(OP::equals); ONOP(OP::equals); SHOW; // repeating equals repeats last operation
  Serial.print("02: [1] [-] [3] [=]                         "); NUM(1); ONOP(OP::subtraction); NUM(3); ONOP(OP::equals); SHOW;
  Serial.print("03: [2] [*] [4] [=]                         "); NUM(2); ONOP(OP::multiplication); NUM(4); ONOP(OP::equals); SHOW;
  Serial.print("04: [3] [/] [4] [=]                         "); NUM(3); ONOP(OP::division); NUM(4); ONOP(OP::equals); SHOW;
  Serial.print("05: [3] [inv]                               "); NUM(3); ONOP(OP::inv); SHOW;
  Serial.print("06: [9] [sqrr]                              "); NUM(9); ONOP(OP::squareroot); SHOW;
  Serial.print("07: [6] [+] [10] [%] [=]                    "); NUM(6); ONOP(OP::addition); NUM(10); ONOP(OP::percent); ONOP(OP::equals); SHOW;
  Serial.print("08: [4] [pow] [3] [=]                       "); NUM(4); ONOP(OP::pow); NUM(3); ONOP(OP::equals); SHOW;
  Serial.print("09: [45] [sin]                              "); NUM(45); ONOP(OP::sin); SHOW;
  Serial.print("10: [30] [cos]                              "); NUM(30); ONOP(OP::cos); SHOW;
  Serial.print("11: [45] [tan]                              "); NUM(45); ONOP(OP::tan); SHOW;
  Serial.print("12: [20] [log]                              "); NUM(20); ONOP(OP::log); SHOW;
  Serial.print("13: [20] [ln]                               "); NUM(20); ONOP(OP::ln); SHOW;
  Serial.print("14: [50] [+/-]                              "); NUM(50); ONOP(OP::switchsign); SHOW;
  
  // little chain
  Serial.print("15: [5.2] [+] [30] [sin] [-] [81] [sqrr] [=]"); NUM(5.2); ONOP(OP::addition); NUM(30); ONOP(OP::sin); 
                ONOP(OP::subtraction); NUM(81); ONOP(OP::squareroot); ONOP(OP::equals); SHOW;

  // produce some errors, after each error we have to press AC
  Serial.print("16: [6] [/] [0] [=]                         "); NUM(6); ONOP(OP::division); NUM(0); ONOP(OP::equals); SHOW; ONOP(OP::allclear); 
  Serial.print("17: [90] [tan]                              "); NUM(90); ONOP(OP::tan); SHOW; ONOP(OP::allclear);
  Serial.print("18: [0] [ln]                                "); NUM(0); ONOP(OP::ln); SHOW; ONOP(OP::allclear);
  Serial.print("19: [1] [+/-] [sqrr]                        "); NUM(1); ONOP(OP::switchsign); ONOP(OP::squareroot); SHOW; ONOP(OP::allclear);
  Serial.print("20: [1] [+] [99999999999999] [=]            "); NUM(1); ONOP(OP::addition); NUM(99999999999999); ONOP(OP::equals); SHOW; ONOP(OP::allclear);

  // output:
  /*
  00: [1.5] [+] [3] [=] --> [1.5][1.5][3]      --> 4.5
  01: [1] [+] [3] [=] [=]                      --> 7
  02: [1] [-] [3] [=]                          --> -2
  03: [2] [*] [4] [=]                          --> 8
  04: [3] [/] [4] [=]                          --> 0.75
  05: [3] [inv]                                --> 0.3333333333333
  06: [9] [sqrr]                               --> 3
  07: [6] [+] [10] [%] [=]                     --> 6.6
  08: [4] [pow] [3] [=]                        --> 64
  09: [45] [sin]                               --> 0.7071067811865
  10: [30] [cos]                               --> 0.8660254037844
  11: [45] [tan]                               --> 1
  12: [20] [log]                               --> 1.301029995664
  13: [20] [ln]                                --> 2.995732273554
  14: [50] [+/-]                               --> -50
  15: [5.2] [+] [30] [sin] [-] [81] [sqrr] [=] --> -3.3
  16: [6] [/] [0] [=]                          --> divide by zero
  17: [90] [tan]                               --> invalid domain
  18: [0] [ln]                                 --> invalid domain
  19: [1] [+/-] [sqrr]                         --> invalid domain
  20: [1] [+] [99999999999999] [=]             --> overflow
  */

  while(true);
}

void printResult(double value, operation_return_code retCode)
{
  String s;
  switch(retCode)
  {
    case operation_return_code::success:
      s = formatNumber(value);
      break;

    case operation_return_code::divideByZero:
      s = "divide by zero";
      break;

    case operation_return_code::domain:
      s = "invalid domain";
      break;

    case operation_return_code::overflow:
      s = "overflow";
      break;

    case operation_return_code::unknownoperation:
      s = "unknown operation";
      break;
  }

  Serial.printf(" --> %s\n", s.c_str());
}

// funny function to format a double as follow:
// - no scientific notation
// - floating decimals
// - no decimal point if no decimals
// - total output length limited to DIGIT_COUNT
// haven't found a standard format for this?!
String formatNumber(double number)
{
  char buffer[50];
  String result;
  
  // stupid thing, but it avoids displaying negative zeros 
  if(number == 0)
  {
    number = 0;
  }
  // format with maximum number of decimals
  sprintf(buffer, "%-.*f", DIGIT_COUNT, number);
  result = buffer;
  // get number of digits before the decimal point
  int index = result.indexOf('.');
  if(number < 0)
  {
    index--;
  }
  // get number of decimals to display
  int decimals = DIGIT_COUNT - index;
  // format again with the correct number of decimals
  sprintf(buffer, "%-.*f", decimals, number);
  result = buffer;
  index = result.indexOf('.');
  if(index != -1)
  {
    // remove trailing zeroes after decimal point
    while(result[result.length()-1] == '0')
    {
      result.remove(result.length()-1);
    }
    // remove decimal point if no decimals left
    if(result[result.length()-1] == '.')
    {
      result.remove(result.length()-1);
    }
  }
  return(result);
}
