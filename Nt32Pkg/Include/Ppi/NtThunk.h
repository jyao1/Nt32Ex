/**@file

Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  NtThunk.h

Abstract:

  WinNt Thunk interface PPI

**/

#ifndef __NT_PEI_WIN_NT_THUNK_H__
#define __NT_PEI_WIN_NT_THUNK_H__

#include <WinNtDxe.h>

#define PEI_NT_THUNK_PPI_GUID \
  { \
    0x98c281e5, 0xf906, 0x43dd, {0xa9, 0x2b, 0xb0, 0x3, 0xbf, 0x27, 0x65, 0xda } \
  }

typedef
VOID *
(EFIAPI *PEI_NT_THUNK_INTERFACE) (
  VOID
  );

/*++

Routine Description:
  Export of EFI_WIN_NT_THUNK_PROTOCOL from the Windows SEC.

Arguments:
  InterfaceBase - Address of the EFI_WIN_NT_THUNK_PROTOCOL

Returns:
  EFI_SUCCESS - Data returned

--*/
typedef struct {
  PEI_NT_THUNK_INTERFACE  NtThunk;
} PEI_NT_THUNK_PPI;

extern EFI_GUID gPeiNtThunkPpiGuid;



#define PEI_NT_SOCKET_THUNK_PPI_GUID \
  { \
    0x7793141d, 0xda74, 0x4733, { 0x9a, 0xcb, 0x76, 0xc7, 0x17, 0x55, 0x8, 0x6a } \
  }

typedef
VOID *
(EFIAPI *PEI_NT_SOCKET_THUNK_INTERFACE) (
  VOID
  );

/*++

Routine Description:
  Export of EFI_WIN_NT_SOCKET_THUNK_PROTOCOL from the Windows SEC.

Arguments:
  InterfaceBase - Address of the EFI_WIN_NT_SOCKET_THUNK_PROTOCOL

Returns:
  EFI_SUCCESS - Data returned

--*/
typedef struct {
  PEI_NT_SOCKET_THUNK_INTERFACE  NtSocketThunk;
} PEI_NT_SOCKET_THUNK_PPI;

extern EFI_GUID gPeiNtSocketThunkPpiGuid;

#endif
