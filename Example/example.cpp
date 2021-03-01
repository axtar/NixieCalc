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
  // make some calculations, as entered by user with the keyboard

  // we can call getDisplayValue after every input or operation to refresh the nixie display
  Serial.print("00: [1+3=] --> " );
  nixieCalc.onNumericInput(1);                Serial.printf("[%s]", formatNumber(nixieCalc.getDisplayValue()).c_str());
  nixieCalc.onOperation(operation::addition); Serial.printf("[%s]", formatNumber(nixieCalc.getDisplayValue()).c_str());
  nixieCalc.onNumericInput(3);                Serial.printf("[%s]", formatNumber(nixieCalc.getDisplayValue()).c_str());
  nixieCalc.onOperation(operation::equals);   
  printResult(nixieCalc.getDisplayValue(), nixieCalc.getOperationReturnCode());

  // now calling getDisplayValue only for the result and using macros
  Serial.print("01: [1+3==]"); NUM(1); ONOP(OP::addition); NUM(3); ONOP(OP::equals); ONOP(OP::equals); SHOW; // repeating equals repeats last operation
  Serial.print("02: [1-3=]"); NUM(1); ONOP(OP::subtraction); NUM(3); ONOP(OP::equals); SHOW;
  Serial.print("03: [2*4=]"); NUM(2); ONOP(OP::multiplication); NUM(4); ONOP(OP::equals); SHOW;
  Serial.print("04: [3/4)]"); NUM(3); ONOP(OP::division); NUM(4); ONOP(OP::equals); SHOW;
  Serial.print("05: [3inv]"); NUM(3); ONOP(OP::inv); SHOW;
  Serial.print("06: [9sqr]"); NUM(9); ONOP(OP::squareroot); SHOW;
  Serial.print("07: [6+10%=]"); NUM(6); ONOP(OP::addition); NUM(10); ONOP(OP::percent); ONOP(OP::equals); SHOW;
  Serial.print("08: [4pow3=]"); NUM(4); ONOP(OP::pow); NUM(3); ONOP(OP::equals); SHOW;
  Serial.print("09: [45sin]"); NUM(45); ONOP(OP::sin); SHOW;
  Serial.print("10: [30cos]"); NUM(30); ONOP(OP::cos); SHOW;
  Serial.print("11: [45tan]"); NUM(45); ONOP(OP::tan); SHOW;
  Serial.print("12: [20log]"); NUM(20); ONOP(OP::log); SHOW;
  Serial.print("13: [20ln]"); NUM(20); ONOP(OP::ln); SHOW;
  Serial.print("14: [50<+/->]"); NUM(50); ONOP(OP::switchsign); SHOW;
  
  // little chain
  Serial.print("15: [5.2+30sin-81sqr=]"); NUM(5.2); ONOP(OP::addition); NUM(30); ONOP(OP::sin); 
                ONOP(OP::subtraction); NUM(81); ONOP(OP::squareroot); ONOP(OP::equals); SHOW;

  // produce some errors, after each error we have to press AC
  Serial.print("16: [6/0=]"); NUM(6); ONOP(OP::division); NUM(0); ONOP(OP::equals); SHOW; ONOP(OP::allclear); 
  Serial.print("17: [90tan]"); NUM(90); ONOP(OP::tan); SHOW; ONOP(OP::allclear);
  Serial.print("18: [0ln]"); NUM(0); ONOP(OP::ln); SHOW; ONOP(OP::allclear);
  Serial.print("19: [1<+/->sqr]"); NUM(1); ONOP(OP::switchsign); ONOP(OP::squareroot); SHOW; ONOP(OP::allclear);
  Serial.print("20: [1+99999999999999=]"); NUM(1); ONOP(OP::addition); NUM(99999999999999); ONOP(OP::equals); SHOW; ONOP(OP::allclear);

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

  Serial.printf(" --> <%s>\n", s.c_str());
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

  // format with maximum number of decimals
  sprintf(buffer, "%-.*f", DIGIT_COUNT, number);
  result = buffer;
  // get number of digits before the decimal point
  int index = result.indexOf('.');
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
