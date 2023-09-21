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

int main()
{
   //Initialization of GPIO Module
   u32 a = 2000000,i=0;//65000~2e16
    char step = 0;
    // Define a structure of the type XGpio. Mainly used for indicate the device base address.(see below [1]). We only defined one GPIO module, so there is only one initialization.
    XGpio GPIO;
    // With function XGpio_Initialize, we load the real base-address of the GPIO AXI module to the element in address &GPIO_I of the structure GPIO_I, which is of the type XGpio. (see below [2])
    // Also this functiona calls the function XGpio_LookupConfig, which loads the configuration odf the AXI-GPIO Module into the structure ConfigPtr. The configuration come already set from vivado and only
    // includes : XPAR_AXI_GPIO_0_DEVICE_ID, XPAR_AXI_GPIO_0_BASEADDR, XPAR_AXI_GPIO_0_INTERRUPT_PRESENT, XPAR_AXI_GPIO_0_IS_DUAL. It means, devide ID, Base address, if interrup was enabled and if both channels are enabled.
    XGpio_Initialize(&GPIO, XPAR_AXI_GPIO_0_BASEADDR);

    //Code For Counting with LEDS and dummy delay
   while(1){
      for (i=0;i<a;i++){
      }
   // We send the content of variable "step", to the channel 2 of the only GPIO module incorporated in VIVADO. 
      XGpio_DiscreteWrite(&GPIO, 2, step);
   // Alternatively, we can sen the data using xil_out32 function. Doing this , we are not using the "xgpio.h" lirary and none of the XGpio definition or XGpio_Initialize function is needed. HOwever, this is more error prone and 
   // the address where you are writting must be calculated by your own. 
   //   Xil_Out32(XPAR_AXI_GPIO_0_BASEADDR+8,step);
      step++;
      if(step>0b1111){
         step=0;
                  }
         }
// Cierra main
    return 0;
}



   //  [1]
   //  Structure associated to GPIO, i.e. XGpio:
   //  typedef struct {
   //  UINTPTR BaseAddress; /* Device base address */
   //  u32 IsReady;      /* Device is initialized and ready */
   //  int InterruptPresent;   /* Are interrupts supported in h/w */
   //  int IsDual;    /* Are 2 channels supported in h/w */
   //             } XGpio;

   //[2]

   // In this first function, we define the ConfigPtr with the DeviceId, then we invoke the function "XGpio_CfgInitialize". (see below details of this function)
   // int XGpio_Initialize(XGpio * InstancePtr, u16 DeviceId)
   // {
   //    XGpio_Config *ConfigPtr;

   //    /*
   //     * Assert arguments
   //     */
   //    Xil_AssertNonvoid(InstancePtr != NULL);

   //    /*
   //     * Lookup configuration data in the device configuration table.
   //     * Use this configuration info down below when initializing this
   //     * driver.
   //     */
   //    ConfigPtr = XGpio_LookupConfig(DeviceId);
   //    if (ConfigPtr == (XGpio_Config *) NULL) {
   //       InstancePtr->IsReady = 0;
   //       return (XST_DEVICE_NOT_FOUND);
   //    }
   //    return XGpio_CfgInitialize(InstancePtr, ConfigPtr,
   //                ConfigPtr->BaseAddress);
   // }
   //---------------
   // In this function, we load in InstancePtr, the base address of the AXI GPIO Module.
   // int XGpio_CfgInitialize(XGpio * InstancePtr, XGpio_Config * Config,
   //          UINTPTR EffectiveAddr)
   // {
   //    /* Assert arguments */
   //    Xil_AssertNonvoid(InstancePtr != NULL);

   //    /* Set some default values. */
   //    InstancePtr->BaseAddress = EffectiveAddr;

   //    InstancePtr->InterruptPresent = Config->InterruptPresent;
   //    InstancePtr->IsDual = Config->IsDual;

   //    /*
   //     * Indicate the instance is now ready to use, initialized without error
   //     */
   //    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;
   //    return (XST_SUCCESS);
   // }
   //     