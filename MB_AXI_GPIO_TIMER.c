/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
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
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

#include <stdio.h>
#include "xil_io.h"
#include "xil_printf.h"
#include "xparameters.h"
// We add this ligrary to manage the AXI GPIO Module
#include "xgpio.h"
// We add this ligrary to manage the AXI TIMER Module
#include "xtmrctr.h"


int main()
{	 
    int toggle=0;
    u32 Timer_Count;
    //Initialization of GPIO Module
    XGpio GPIO;
    XGpio_Initialize(&GPIO, XPAR_GPIO_0_DEVICE_ID);
    //Initialization of Timer Module
    XTmrCtr TMRInst;
    XTmrCtr_Initialize(&TMRInst, XPAR_AXI_TIMER_0_DEVICE_ID);
    // We set a reset value for the timer. It will start in this value when finish tha count. With 0xFA0A1EFF timer counts 1 sec. See [1] to check how to calculate the sime counted by the timer.
    XTmrCtr_SetResetValue(&TMRInst,0,0xFA0A1EFF);
    // We set configuration options fot the timer, predefined in xtmrctr.h. In this case, we enable the auto reload. See [2]
    XTmrCtr_SetOptions(&TMRInst,0,XTC_AUTO_RELOAD_OPTION);
    // With this function we can read the actual counter value of the timer.
    Timer_Count = XTmrCtr_GetValue(&TMRInst,0);
    // We initialize the counter.
    XTmrCtr_Start(&TMRInst,0);
   
   while(1){
      // We get the counter value of the timer
     Timer_Count = XTmrCtr_GetValue(&TMRInst,0);
      // We compare if the counter is greater that 4244967295 (0xFD050F7F), which is the half way from 4194967295 (0xFA0A1EFF) to 4294967295 (0xFFFFFFFF).
     // based on this, we toggle all LEDs, which should blink as 0.5s. ON  -->0.5s
      if(Timer_Count>= 4244967295) toggle=1;
      else toggle=0;

      if(toggle==1)      XGpio_DiscreteWrite(&GPIO, 2, 0xFFFFFFFF);
      else XGpio_DiscreteWrite(&GPIO, 2, 0x00000000);
  }

// Cierra main
  return 0;
}

//[1] The timer counts based on its clock (same as AXI clock), that in this example is 100MHZ, i.e. 10ns.
// Timer can count from 0x0 to 0xFFFFFFFF. As it is 32bits.
//0xFFFFFFFF (32bits) =  4.294.967.295 in decimal value.
// clk_timer= 100MHZ =>  10ns. Para contar 500ms, se requiren 500ms/10ns=50.000.000 de muestras. En hexa son 0x02FAF080.
// El timer contara en sentido UP. Por lo tanto debe iniciar en 4.294.967.295-50.000.000=4244967295 o 0xFD050F7F.
// Usando 0xF8000000 (4160749568). Contaré hasta: 4294967295- 4160749568 = 134.217.727 => 1,3 segundos.
// Usando 0xFA0A1EFF (4194967295 decimal), contaré 4294967295-4194967295 = 100.000.000 => 1 segundo. 

// [2]  The configuration mode options are:
// #define XTC_CASCADE_MODE_OPTION     0x00000080UL
// #define XTC_ENABLE_ALL_OPTION    0x00000040UL
// #define XTC_DOWN_COUNT_OPTION    0x00000020UL
// #define XTC_CAPTURE_MODE_OPTION     0x00000010UL
// #define XTC_INT_MODE_OPTION         0x00000008UL
// #define XTC_AUTO_RELOAD_OPTION      0x00000004UL
// #define XTC_EXT_COMPARE_OPTION      0x00000002UL

//Note the you can configure your timer with one or more of these configurations using | , example: XTC_CASCADE_MODE_OPTION|XTC_DOWN_COUNT_OPTION|XTC_AUTO_RELOAD_OPTION

//  Notice that the values of the configuration does not match with the position of the TCSR register of the AXI-TIMER IP-Core. This is because we only use this information lolocate the real value that we send to the 
// TCSR register (this is known as mapping). You can track this easily using cntrl+click in Vitis, tracking the functions used for the timer within the code and comparing with what is given in the guide of the timer. 

// static Mapping OptionsTable[] = {
//    {XTC_CASCADE_MODE_OPTION, XTC_CSR_CASC_MASK},
//    {XTC_ENABLE_ALL_OPTION, XTC_CSR_ENABLE_ALL_MASK},
//    {XTC_DOWN_COUNT_OPTION, XTC_CSR_DOWN_COUNT_MASK},
//    {XTC_CAPTURE_MODE_OPTION, XTC_CSR_CAPTURE_MODE_MASK |
//     XTC_CSR_EXT_CAPTURE_MASK},
//    {XTC_INT_MODE_OPTION, XTC_CSR_ENABLE_INT_MASK},
//    {XTC_AUTO_RELOAD_OPTION, XTC_CSR_AUTO_RELOAD_MASK},
//    {XTC_EXT_COMPARE_OPTION, XTC_CSR_EXT_GENERATE_MASK}
// };
// #define XTC_CSR_CASC_MASK     0x00000800 /**< Cascade Mode */ //00000000000000000000 100000000000 (bit11 TCSR Register)
// #define XTC_CSR_ENABLE_ALL_MASK     0x00000400 /**< Enables all timer counters  00000000000000000000 010000000000 (bit10 TCSR Register) */
// #define XTC_CSR_ENABLE_PWM_MASK     0x00000200 /**< Enables the Pulse Width Modulation 00000000000000000000 001000000000 (bit10 TCSR Register) */
// #define XTC_CSR_INT_OCCURED_MASK 0x00000100 /**< If bit is set, an
//                      interrupt has occured.
//                      If set and '1' is
//                      written to this bit
//                      position, bit is
//                      cleared. */
// //00000000000000000000 000100000000 (bit8 TCSR Register)
// #define XTC_CSR_ENABLE_TMR_MASK     0x00000080 /**< Enables only the
//                      specific timer */
// //00000000000000000000 000010000000  (bit7 TCSR Register)
// #define XTC_CSR_ENABLE_INT_MASK     0x00000040 /**< Enables the interrupt
//                      output. */
// //0000000000000000000 0000001000000 (bit6 TCSR Register)
// #define XTC_CSR_LOAD_MASK     0x00000020 /**< Loads the timer using
//                      the load value provided
//                      earlier in the Load
//                      Register,
//                      XTC_TLR_OFFSET. */
// //000000000000000000000 00000100000 (bit5 TCSR Register)
// #define XTC_CSR_AUTO_RELOAD_MASK 0x00000010 /**< In compare mode,
//                      configures
//                      the timer counter to
//                      reload  from the
//                      Load Register. The
//                      default  mode
//                      causes the timer counter
//                      to hold when the compare
//                      value is hit. In capture
//                      mode, configures  the
//                      timer counter to not
//                      hold the previous
//                      capture value if a new
//                      event occurs. The
//                      default mode cause the
//                      timer counter to hold
//                      the capture value until
//                      recognized. */
// //00000000000000000000 000000010000 (bit4 TCSR Register)
// #define XTC_CSR_EXT_CAPTURE_MASK 0x00000008 /**< Enables the
//                      external input
//                      to the timer counter. */
// //00000000000000000000 000000001000 (bit3 TCSR Register)
// #define XTC_CSR_EXT_GENERATE_MASK   0x00000004 /**< Enables the
//                      external generate output
//                      for the timer. */
// //00000000000000000000 000000000100 (bit2 TCSR Register)
// #define XTC_CSR_DOWN_COUNT_MASK     0x00000002 /**< Configures the timer
//                      counter to count down
//                      from start value, the
//                      default is to count
//                      up.*/
// //00000000000000000000 000000000010 (bit1 TCSR Register)
// #define XTC_CSR_CAPTURE_MODE_MASK   0x00000001 /**< Enables the timer to
//                      capture the timer
//                      counter value when the
//                      external capture line is
//                      asserted. The default
//                      mode is compare mode.*/
// /* @} */
// //00000000000000000000 000000000001 (bit0 TCSR Register)


