/* HWTimer class for Mbed
 * Copyright (c) 2019 Johannes Stratmann
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
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "HWTimer.h"

//
// resources for HW timeout
// 

#ifndef MAX_HWTIMERS
#define MAX_HWTIMERS    4
#endif

TIM_HandleTypeDef mTimUserHandles[MAX_HWTIMERS];

// register tables for timers
TIM_TypeDef* TimBaseAddress[] = {TIM1, TIM2, TIM3, TIM4, TIM5, TIM6, TIM7, TIM8, TIM9, TIM10, TIM11, TIM12, TIM13, TIM14};
IRQn_Type TimIRQTab[] = {TIM1_UP_TIM10_IRQn, TIM2_IRQn, TIM3_IRQn, TIM4_IRQn, TIM5_IRQn, TIM6_DAC_IRQn, TIM7_IRQn, TIM8_UP_TIM13_IRQn, 
                         TIM1_BRK_TIM9_IRQn, TIM1_UP_TIM10_IRQn, TIM1_TRG_COM_TIM11_IRQn, TIM8_BRK_TIM12_IRQn, TIM8_UP_TIM13_IRQn, TIM8_TRG_COM_TIM14_IRQn};
volatile uint32_t * apbClkEnableTab[] = { &RCC->APB2ENR, &RCC->APB1ENR, &RCC->APB1ENR, &RCC->APB1ENR, &RCC->APB1ENR, &RCC->APB1ENR, &RCC->APB1ENR, &RCC->APB2ENR, 
                                          &RCC->APB2ENR, &RCC->APB2ENR, &RCC->APB2ENR, &RCC->APB1ENR, &RCC->APB1ENR, &RCC->APB1ENR  };
uint32_t apbClkMaskTab[] = { RCC_APB2ENR_TIM1EN,  RCC_APB1ENR_TIM2EN,  RCC_APB1ENR_TIM3EN,  RCC_APB1ENR_TIM4EN, 
                             RCC_APB1ENR_TIM5EN,  RCC_APB1ENR_TIM6EN,  RCC_APB1ENR_TIM7EN,  RCC_APB2ENR_TIM8EN,
                             RCC_APB2ENR_TIM9EN,  RCC_APB2ENR_TIM10EN, RCC_APB2ENR_TIM11EN, RCC_APB1ENR_TIM12EN,
                             RCC_APB1ENR_TIM13EN, RCC_APB1ENR_TIM14EN };

// helper functions
TIM_HandleTypeDef* findFirstFreeHandle()
{
    int i = 0;
    while((i < MAX_HWTIMERS) && (mTimUserHandles[i].Instance != 0)) {
        i++;
    }

    if (i == MAX_HWTIMERS) {
        return nullptr;
    }
    else {
        return &mTimUserHandles[i];
    }
}


HWTimer::HWTimer(int id, uint32_t us_period, Callback<void()> cb, bool onePulseMode) :
    _id(id),
    _onePulseMode(onePulseMode),
    _cb(cb),
    _irq(this)
{
    _pConfig = findFirstFreeHandle();
    if (_pConfig == nullptr) {
        MBED_ERROR(-1, "no handle available");
    }
    TIM_HandleTypeDef* pTIM = (TIM_HandleTypeDef*)_pConfig;

    // set IRQ ISR, use CThunk to handle member function call from static
     _irq.callback(&HWTimer::callTimerFn);

    // enable time clock
    *(apbClkEnableTab[id]) |= apbClkMaskTab[id];

    // clk for APB1/2 is different
    uint32_t clkDiv =  (apbClkEnableTab[id] == &RCC->APB2ENR) ? 1 : 2;

    // init basic timer settings
    pTIM->Instance               = TimBaseAddress[id];
    pTIM->Init.Prescaler         = (SystemCoreClock / 1000000 / clkDiv) - 1; // 1 us tick
    pTIM->Init.CounterMode       = TIM_COUNTERMODE_UP;
    pTIM->Init.Period            = us_period;
    pTIM->Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;

    if (onePulseMode) {
        HAL_TIM_OnePulse_Init(pTIM, TIM_OPMODE_SINGLE);      // timer basic init + set one pulse mode
    } else {
        HAL_TIM_Base_Init(pTIM);                             // timer basic init
    }

    __HAL_TIM_CLEAR_FLAG(pTIM, TIM_FLAG_UPDATE);         // reset update flag
    __HAL_TIM_ENABLE_IT(pTIM, TIM_IT_UPDATE);            // interrupt on update
    
    // activate IRQ in NVIC
    NVIC_SetVector(TimIRQTab[id], (uint32_t)_irq);
    NVIC_EnableIRQ(TimIRQTab[id]);
}


HWTimer::~HWTimer()
{
    // disable time clock
    *(apbClkEnableTab[_id]) &= ~apbClkMaskTab[_id];
    NVIC_DisableIRQ(TimIRQTab[_id]);
    ((TIM_HandleTypeDef*)_pConfig)->Instance = 0;
}
    
void HWTimer::start(uint32_t us_period)
{
    TIM_HandleTypeDef* pTIM = (TIM_HandleTypeDef*)_pConfig;

    if (_onePulseMode) {
        __HAL_TIM_CLEAR_FLAG(pTIM, TIM_FLAG_UPDATE);         // reset update flag
        if (us_period > 0) {
            __HAL_TIM_SET_AUTORELOAD(pTIM, us_period-1);     // set reload value for overflow and update
        }
    }

    __HAL_TIM_ENABLE(pTIM);                                  // start timer
}

void HWTimer::stop()
{
    __HAL_TIM_DISABLE((TIM_HandleTypeDef*)_pConfig);        // stop timer
}

void HWTimer::callTimerFn()
{
    if (__HAL_TIM_GET_FLAG((TIM_HandleTypeDef*)_pConfig, TIM_FLAG_UPDATE) == SET) {
        __HAL_TIM_CLEAR_FLAG((TIM_HandleTypeDef*)_pConfig, TIM_FLAG_UPDATE);

        _cb();
    }
}

