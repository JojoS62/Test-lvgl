#if 0
float calcTemperature(float adcValue)
{
    const float coeffTab[2][2] = {
            { 5.405e-3f,   6.65e-6f  },       // Ni
            { 3.9083e-3f, -5.775e-7f }        // Pt
        };

    float R1 = 2405.0f;
    float R0 = 1000.0f;
    float A = coeffTab[0][0];
    float B = coeffTab[0][1];
    float p = A  / B;
    float q = (1.0f - (adcValue * R1 / R0) / (1.0f - adcValue)) / B; 
    float t = -p / 2.0f + sqrt(p*p / 4.0f - q);

    return t;
}

float calcAdcValue(float temperature)
{
    const float coeffTab[2][2] = {
            { 5.405e-3f,   6.65e-6f  },       // Ni
            { 3.9083e-3f, -5.775e-7f }        // Pt
        };

    float R1 = 2405.0f;
    float R0 = 1000.0f;
    float A = coeffTab[0][0];
    float B = coeffTab[0][1];
    float RTD = R0 * (1 + A * temperature + B * temperature * temperature); 
    float adcValue = RTD / (RTD + R1);

    return adcValue;
}

void pidControlThread(void const *args) 
{
    DigitalOut myled(LED1);
    AnalogIn    adcVT(A0);
    PID         pid(pidKp, pidKi, pidKd, 1000.0f);
    PwmOut      pwmHeater(D2);

    float       actKp, actKi, actKd, actSetpoint;

    float adcSetpoint = calcAdcValue(pidSetpoint);
    pid.setSetPoint(adcSetpoint);
    pid.setOutputLimits(0.0f, 1.0f);
    pid.setMode(AUTO_MODE); 

    pwmHeater.period_ms(10);

    while (true) {
        if (actKp != pidKp || actKi != pidKi || actKd != pidKd) {
            pid.setTunings(pidKp, pidKi, pidKp);
            actKp = pidKp;
            actKi = pidKi;
            actKd = pidKd;
        }
        if (actSetpoint != pidSetpoint) {
            pid.setSetPoint(calcAdcValue(pidSetpoint));
            actSetpoint = pidSetpoint;
        }
        myled = !myled;

        int nAvg = 20;
        float sum = 0.0f;
        for (int i = 0; i < nAvg; i++) {
            sum += adcVT.read();
        }
        float adcValue = sum / nAvg;
        float vt = calcTemperature(adcValue);

        pid.setProcessValue(adcValue);
        float outControl = pid.compute();
        pwmHeater = outControl;

        message_t *message = mpool.alloc();
        message->temperature = vt;
        message->controlOut = outControl;
        measuringValues.put(message);

        wait_ms(1000);
    }
}
#endif