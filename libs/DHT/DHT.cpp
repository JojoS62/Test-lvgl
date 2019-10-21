/*
 *  DHT Library for  Digital-output Humidity and Temperature sensors
 *
 *  Works with DHT11, DHT22
 *             SEN11301P,  Grove - Temperature&Humidity Sensor     (Seeed Studio)
 *             SEN51035P,  Grove - Temperature&Humidity Sensor Pro (Seeed Studio)
 *             AM2302   ,  temperature-humidity sensor
 *             HM2303   ,  Digital-output humidity and temperature sensor
 *
 *  Copyright (C) Wim De Roeve
 *                based on DHT22 sensor library by HO WING KIT
 *                Arduino DHT11 library 
 *
  *                Modiefied by JojoS
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "DHT.h"

DHT::DHT(PinName pin, int DHTtype) :
        _dataPin(pin, PIN_INPUT, PullUp, 0) {
    _DHTtype = DHTtype;
}

DHT::~DHT() {
}


int DHT::readData() {
    uint8_t cnt = 7;
    uint8_t idx = 0;

    _tmr.stop();
    _tmr.reset();

    // EMPTY BUFFER
    for (int i = 0; i < 5; i++)
        _bits[i] = 0;

    // REQUEST SAMPLE
    _dataPin.output();
    _dataPin.write(0);
    wait_ms(18);
    _dataPin.write(1);
    wait_us(40);
    _dataPin.input();

    // ACKNOWLEDGE or TIMEOUT
    unsigned int loopCnt = 10000;
    while (!_dataPin.read())
        if (!loopCnt--)
            return ERROR_TIMEOUT;

    loopCnt = 10000;
    while (_dataPin.read())
        if (!loopCnt--)
            return ERROR_TIMEOUT;

    // READ OUTPUT - 40 BITS => 5 BYTES or TIMEOUT
    for (int i = 0; i < 40; i++) {

        loopCnt = 10000;
        while (!_dataPin.read())
            if (loopCnt-- == 0)
                return ERROR_TIMEOUT;

        _tmr.start();

        loopCnt = 10000;
        while (_dataPin.read())
            if (!loopCnt--)
                return ERROR_TIMEOUT;

        if (_tmr.read_us() > 40)
            _bits[idx] |= (1 << cnt);

        _tmr.stop();
        _tmr.reset();

        if (cnt == 0) {   // next byte?

            cnt = 7;    // restart at MSB
            idx++;      // next byte!

        } else
            cnt--;

    }

    uint8_t sum = _bits[0] + _bits[1] + _bits[2] + _bits[3];

    if (_bits[4] != sum)
        return ERROR_CHECKSUM;

    _lastTemperature = CalcTemperature();
    _lastHumidity = CalcHumidity();

    return ERROR_NONE;
}

float DHT::CalcTemperature() {
    int v;

    switch (_DHTtype) {
        case DHT11:
            v = _bits[2];
            return float(v);
        case DHT22:
            v = _bits[2] & 0x7F;
            v *= 256;
            v += _bits[3];
            if (_bits[2] & 0x80)
                v *= -1;
            return v / 10.0f;
    }
    return 0;
}

float DHT::ReadHumidity() {
    return _lastHumidity;
}

float DHT::ConvertCelciustoFarenheit(float celsius) {
    return celsius * 9 / 5 + 32;
}

float DHT::ConvertCelciustoKelvin(float celsius) {
    return celsius + 273.15;
}

// dewPoint function NOAA
// reference: http://wahiduddin.net/calc/density_algorithms.htm
float DHT::CalcdewPoint(float celsius, float humidity) {
    float A0 = 373.15 / (273.15 + celsius);
    float SUM = -7.90298 * (A0 - 1);
    SUM += 5.02808 * log10(A0);
    SUM += -1.3816e-7 * (pow(10, (11.344 * (1 - 1 / A0))) - 1);
    SUM += 8.1328e-3 * (pow(10, (-3.49149 * (A0 - 1))) - 1);
    SUM += log10(1013.246);
    float VP = pow(10, SUM - 3) * humidity;
    float T = log(VP / 0.61078);   // temp var
    return (241.88 * T) / (17.558 - T);
}

// delta max = 0.6544 wrt dewPoint()
// 5x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
float DHT::CalcdewPointFast(float celsius, float humidity) {
    float a = 17.271;
    float b = 237.7;
    float temp = (a * celsius) / (b + celsius) + log(humidity / 100);
    float Td = (b * temp) / (a - temp);
    return Td;
}

float DHT::ReadTemperature(eScale Scale) {
    if (Scale == FARENHEIT)
        return ConvertCelciustoFarenheit(_lastTemperature);
    else if (Scale == KELVIN)
        return ConvertCelciustoKelvin(_lastTemperature);
    else
        return _lastTemperature;
}

float DHT::CalcHumidity() {
    int v;

    switch (_DHTtype) {
        case DHT11:
            v = _bits[0];
            return float(v);
        case DHT22:
            v = _bits[0];
            v *= 256;
            v += _bits[1];
            return v / 10.0f;
    }
    return 0;
}

