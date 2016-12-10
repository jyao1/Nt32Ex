/**@file

Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

 Reset.c

Abstract:

  Reset Architectural Protocol as defined in Tiano under NT Emulation

**/

#include <PiPei.h>
#include <WinNtPeim.h>

#include <Ppi/Reset.h>
#include <Ppi/NtThunk.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

EFI_STATUS
EFIAPI
WinNtResetSystem (
  IN CONST EFI_PEI_SERVICES          **PeiServices
  );

EFI_PEI_RESET_PPI                mResetPpi = { WinNtResetSystem };

EFI_PEI_PPI_DESCRIPTOR       mPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiResetPpiGuid,
  &mResetPpi
};

EFI_STATUS
EFIAPI
InitializeNtReset (
  IN       EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
{
  EFI_STATUS  Status;

  Status = (**PeiServices).InstallPpi(PeiServices, &mPpiList);
  ASSERT_EFI_ERROR(Status);

  return Status;
}

EFI_STATUS
EFIAPI
WinNtResetSystem (
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
{
  PEI_NT_THUNK_PPI          *NtThunkPpi;
  EFI_STATUS                Status;
  EFI_WIN_NT_THUNK_PROTOCOL *WinNt;

  Status = (**PeiServices).LocatePpi (
                             PeiServices,
                             &gPeiNtThunkPpiGuid,
                             0,
                             NULL,
                             (VOID **) &NtThunkPpi
                             );
  ASSERT_EFI_ERROR (Status);

  WinNt = (EFI_WIN_NT_THUNK_PROTOCOL *)NtThunkPpi->NtThunk();

  //
  // BUGBUG Need to kill all console windows later
  //
  //
  // Discard ResetType, always return 0 as exit code
  //
  WinNt->ExitProcess (0);

  //
  // Should never go here
  //
  ASSERT (FALSE);

  return EFI_UNSUPPORTED;
}
