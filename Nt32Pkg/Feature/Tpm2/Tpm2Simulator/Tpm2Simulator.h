/**@file

Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#ifndef __TPM2_SIMULATOR_H__
#define __TPM2_SIMULATOR_H__

#define DEFAULT_TPM_COMMAND_PORT  2321
#define DEFAULT_TPM_PLATFORM_PORT 2322

//
// TPM Commands.
//

//
// Command format:
//      UINT32   Command;
//      UINT8    Data[]; // Command Specific data
// Response format: (except where noted)
//      UINT32   ReturnCode
//
#define TPM_SIGNAL_POWER_ON             1
#define TPM_SIGNAL_POWER_OFF            2
#define TPM_SIGNAL_PHYS_PRES_ON         3
#define TPM_SIGNAL_PHYS_PRES_OFF        4
#define TPM_SIGNAL_HASH_START           5
//
// TPM_SIGNAL_HASH_DATA command:
//   UINT32  BufferSize
//   UINT8   Buffer[BufferSize]
//
#define TPM_SIGNAL_HASH_DATA            6
#define TPM_SIGNAL_HASH_END             7
//
// TPM_SEND_COMMAND command:
//   UINT8   Locality
//   UINT32  InputBufferSize
//   UINT8   InputBufferSize[InputBufferSize]
// TPM_SEND_COMMAND response:
//   UINT32  OutputBufferSize
//   UINT8   OutputBufferSize[OutputBufferSize]
//
#define TPM_SEND_COMMAND                8
#define TPM_SIGNAL_CANCEL_ON            9
#define TPM_SIGNAL_CANCEL_OFF           10
#define TPM_SIGNAL_NV_ON                11
#define TPM_SIGNAL_NV_OFF               12
#define TPM_SIGNAL_KEY_CACHE_ON         13
#define TPM_SIGNAL_KEY_CACHE_OFF        14
#define TPM_REMOTE_HANDSHAKE            15
#define TPM_SET_ALTERNATIVE_RESULT      16
#define TPM_SIGNAL_RESET                17
#define TPM_SIGNAL_RESTART              18
#define TPM_SESSION_END                 20
#define TPM_STOP                        21
#define TPM_GET_COMMAND_RESPONSE_SIZES  25
#define TPM_TEST_FAILURE_MODE           30

#endif