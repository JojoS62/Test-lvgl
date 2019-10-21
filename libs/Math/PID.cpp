/**
 * @author Aaron Berk
 *
 * @section LICENSE
 *
 * Copyright (c) 2010 ARM Limited
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @section DESCRIPTION
 * 
 * A PID controller is a widely used feedback controller commonly found in
 * industry.
 *
 * This library is a port of Brett Beauregard's Arduino PID library:
 *
 *  http://www.arduino.cc/playground/Code/PIDLibrary
 *
 * The wikipedia article on PID controllers is a good place to start on
 * understanding how they work:
 *
 *  http://en.wikipedia.org/wiki/PID_controller
 *
 * For a clear and elegant explanation of how to implement and tune a
 * controller, the controlguru website by Douglas J. Cooper (who also happened
 * to be Brett's controls professor) is an excellent reference:
 *
 *  http://www.controlguru.com/
 */

/**
 * Includes
 */
#include "PID.h"

PID::PID(float Kc, float tauI, float tauD, float interval) {

    _usingFeedForward = false;
    _inAuto           = false;

    //Default the limits to the full range of analogIn 0-100%
    //Make sure to set these to more appropriate limits for
    //your application.
    setInputLimits(0.0f, 1.0f);
    setOutputLimits(0.0f, 1.0f);

    _tSample = interval;

    setTunings(Kc, tauI, tauD);

    _setPoint             = 0.0f;
    _processVariable      = 0.0f;
    _prevProcessVariable  = 0.0f;
    _controllerOutput     = 0.0f;
    _prevControllerOutput = 0.0f;

    _accError = 0.0f;
    _bias     = 0.0f;
    
    _realOutput = 0.0f;

}

void PID::setInputLimits(float inMin, float inMax) {

    //Make sure we haven't been given impossible values.
    if (inMin >= inMax) {
        return;
    }

    //Rescale the working variables to reflect the changes.
    _prevProcessVariable *= (inMax - inMin) / _inSpan;
    _accError            *= (inMax - inMin) / _inSpan;

    //Make sure the working variables are within the new limits.
    if (_prevProcessVariable > 1.0f) {
        _prevProcessVariable = 1.0f;
    } else if (_prevProcessVariable < 0.0f) {
        _prevProcessVariable = 0.0f;
    }

    _inMin  = inMin;
    _inMax  = inMax;
    _inSpan = inMax - inMin;

}

void PID::setOutputLimits(float outMin, float outMax) {

    //Make sure we haven't been given impossible values.
    if (outMin >= outMax) {
        return;
    }

    //Rescale the working variables to reflect the changes.
    _prevControllerOutput *= (outMax - outMin) / _outSpan;

    //Make sure the working variables are within the new limits.
    if (_prevControllerOutput > 1.0f) {
        _prevControllerOutput = 1.0f;
    } else if (_prevControllerOutput < 0.0f) {
        _prevControllerOutput = 0.0f;
    }

    _outMin  = outMin;
    _outMax  = outMax;
    _outSpan = outMax - outMin;

}

void PID::setTunings(float Kc, float tauI, float tauD) {

    //Verify that the tunings make sense.
    if (Kc == 0.0f || tauI < 0.0f || tauD < 0.0f) {
        return;
    }

    //Store raw values to hand back to user on request.
    _pParam = Kc;
    _iParam = tauI;
    _dParam = tauD;

    float tempTauR;

    if (tauI == 0.0f) {
        tempTauR = 0.0f;
    } else {
        tempTauR = (1.0f / tauI) * _tSample;
    }

    //For "bumpless transfer" we need to rescale the accumulated error.
    if (_inAuto) {
        if (tempTauR == 0.0f) {
            _accError = 0.0f;
        } else {
            _accError *= (_Kc * _tauR) / (Kc * tempTauR);
        }
    }

    _Kc   = Kc;
    _tauR = tempTauR;
    _tauD = tauD / _tSample;

}

void PID::reset(void) {

    float scaledBias = 0.0f;

    if (_usingFeedForward) {
        scaledBias = (_bias - _outMin) / _outSpan;
    } else {
        scaledBias = (_realOutput - _outMin) / _outSpan;
    }

    _prevControllerOutput = scaledBias;
    _prevProcessVariable  = (_processVariable - _inMin) / _inSpan;

    //Clear any error in the integral.
    _accError = 0.0f;
}

void PID::setMode(int mode) {

    //We were in manual, and we just got set to auto.
    //Reset the controller internals.
    if (mode != 0 && !_inAuto) {
        reset();
    }

    _inAuto = (mode != 0);
}

void PID::setInterval(float interval) {

    if (interval > 0.0f) {
        //Convert the time-based tunings to reflect this change.
        _tauR     *= (interval / _tSample);
        _accError *= (_tSample / interval);
        _tauD     *= (interval / _tSample);
        _tSample   = interval;
    }
}

void PID::setSetPoint(float sp) {

    _setPoint = sp;

}

void PID::setProcessValue(float pv) {

    _processVariable = pv;

}

void PID::setBias(float bias){

    _bias = bias;
    _usingFeedForward = 1;

}

float PID::compute() {

    //Pull in the input and setpoint, and scale them into percent span.
    float scaledPV = (_processVariable - _inMin) / _inSpan;

    if (scaledPV > 1.0f) {
        scaledPV = 1.0f;
    } else if (scaledPV < 0.0f) {
        scaledPV = 0.0f;
    }

    float scaledSP = (_setPoint - _inMin) / _inSpan;
    if (scaledSP > 1.0f) {
        scaledSP = 1.0f;
    } else if (scaledSP < 0.0f) {
        scaledSP = 0.0f;
    }

    float error = scaledSP - scaledPV;

    //Check and see if the output is pegged at a limit and only
    //integrate if it is not. This is to prevent reset-windup.
    if (!(_prevControllerOutput >= 1.0f && error > 0.0f) && !(_prevControllerOutput <= 0.0f && error < 0.0f)) {
        _accError += error;
    }

    //Compute the current slope of the input signal.
    float dMeas = (scaledPV - _prevProcessVariable) / _tSample;

    float scaledBias = 0.0f;

    if (_usingFeedForward) {
        scaledBias = (_bias - _outMin) / _outSpan;
    }

    //Perform the PID calculation.
    _controllerOutput = scaledBias + _Kc * (error + (_tauR * _accError) - (_tauD * dMeas));

    //Make sure the computed output is within output constraints.
    if (_controllerOutput < 0.0f) {
        _controllerOutput = 0.0f;
    } else if (_controllerOutput > 1.0f) {
        _controllerOutput = 1.0f;
    }

    //Remember this output for the windup check next time.
    _prevControllerOutput = _controllerOutput;
    //Remember the input for the derivative calculation next time.
    _prevProcessVariable  = scaledPV;

    //Scale the output from percent span back out to a real world number.
    return ((_controllerOutput * _outSpan) + _outMin);

}

float PID::getInMin() {

    return _inMin;

}

float PID::getInMax() {

    return _inMax;

}

float PID::getOutMin() {

    return _outMin;

}

float PID::getOutMax() {

    return _outMax;

}

float PID::getInterval() {

    return _tSample;

}

float PID::getPParam() {

    return _pParam;

}

float PID::getIParam() {

    return _iParam;

}

float PID::getDParam() {

    return _dParam;

}
