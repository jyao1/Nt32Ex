/**@file

Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  WinNtLib.c

Abstract:

  WinNt Library 

**/

#include <PiPei.h>
#include <WinNtPeim.h>
#include <Ppi/NtThunk.h>
#include <Library/PeiServicesLib.h>
#include <Library/WinNtLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>

EFI_WIN_NT_THUNK_PROTOCOL *gWinNt;
EFI_WIN_NT_SOCKET_THUNK_PROTOCOL *gWinNtSocket;

EFI_STATUS
EFIAPI
WinNtLibConstructor (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
/*++

Routine Description:

Arguments:

Returns:

--*/
{
  EFI_STATUS               Status;
  PEI_NT_THUNK_PPI         *NtThunkPpi;
  PEI_NT_SOCKET_THUNK_PPI  *NtSocketThunkPpi;

  Status = PeiServicesLocatePpi (
              &gPeiNtThunkPpiGuid,
              0,
              NULL,
              (VOID **) &NtThunkPpi
              );
  ASSERT_EFI_ERROR (Status);

  gWinNt  = (EFI_WIN_NT_THUNK_PROTOCOL *) NtThunkPpi->NtThunk ();

  Status = PeiServicesLocatePpi (
              &gPeiNtSocketThunkPpiGuid,
              0,
              NULL,
              (VOID **) &NtSocketThunkPpi
              );
  ASSERT_EFI_ERROR (Status);

  gWinNtSocket  = (EFI_WIN_NT_SOCKET_THUNK_PROTOCOL *) NtSocketThunkPpi->NtSocketThunk ();

  return EFI_SUCCESS;
}
