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

#include <Uefi.h>
#include <WinNtDxe.h>
#include <Protocol/Reset.h>
#include <Guid/WinNtMemoryLayout.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/WinNtLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/Tpm2CommandLib.h>
#include <Library/Tpm2SimulatorLib.h>

#define EFI_NT32_MEMORY_FILE_NAME  L"MemoryBin"

EFI_STATUS
EFIAPI
InitializeNtReset (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  );

VOID
EFIAPI
WinNtResetSystem (
  IN EFI_RESET_TYPE   ResetType,
  IN EFI_STATUS       ResetStatus,
  IN UINTN            DataSize,
  IN VOID             *ResetData OPTIONAL
  );

EFI_WIN_NT_MEMORY_LAYOUT  *mWinNtMemoryLayout;

EFI_STATUS
EFIAPI
InitializeNtReset (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
/*++

Routine Description:


Arguments:

  ImageHandle of the loaded driver
  Pointer to the System Table

Returns:

  Status
--*/
// TODO:    SystemTable - add argument and description to function comment
{
  EFI_STATUS  Status;
  EFI_HANDLE  Handle;
  EFI_HOB_GUID_TYPE   *GuidHob;

  SystemTable->RuntimeServices->ResetSystem = WinNtResetSystem;

  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiResetArchProtocolGuid,
                  NULL,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  if (FeaturePcdGet (PcdWinNtCapsuleEnable)) {
    //
    // Retrieve WinNtThunkProtocol from GUID'ed HOB
    //
    GuidHob = GetFirstGuidHob (&gEfiWinNtMemoryLayoutGuid);
    ASSERT (GuidHob != NULL);
    mWinNtMemoryLayout = GET_GUID_HOB_DATA (GuidHob);
    ASSERT (mWinNtMemoryLayout != NULL);
  }

  return Status;
}

VOID
EFIAPI
WinNtResetSystem (
  IN EFI_RESET_TYPE   ResetType,
  IN EFI_STATUS       ResetStatus,
  IN UINTN            DataSize,
  IN VOID             *ResetData OPTIONAL
  )
/*++

Routine Description:

  TODO: Add function description

Arguments:

  ResetType   - TODO: add argument description
  ResetStatus - TODO: add argument description
  DataSize    - TODO: add argument description
  ResetData   - TODO: add argument description

Returns:

  EFI_SUCCESS - TODO: Add description for return value

--*/
{
  UINTN   Index;
  CHAR16  FileName[30];
  CHAR16  *TempName;
  HANDLE  NtFileHandle;
  DWORD   BufferSize;
  VOID    *Buffer;
  BOOL    Result;
  EFI_TPL  OldTpl;

  if (FeaturePcdGet (PcdWinNtCapsuleEnable)) {
    OldTpl = gBS->RaiseTPL (TPL_CALLBACK);

    StrCpy (FileName, EFI_NT32_MEMORY_FILE_NAME);
    TempName = FileName + StrLen (FileName);
    for (Index = 0; Index < mWinNtMemoryLayout->NumberOfRegions; Index++) {
      UnicodeValueToString (TempName, 0, Index, 0);

      NtFileHandle = gWinNt->CreateFile (
                               FileName,
                               GENERIC_READ | GENERIC_WRITE,
                               FILE_SHARE_READ | FILE_SHARE_WRITE,
                               NULL,
                               OPEN_ALWAYS,
                               0,
                               NULL
                               );
      if (NtFileHandle == INVALID_HANDLE_VALUE) {
        DEBUG ((EFI_D_ERROR, "ResetSystem - open %s fail\n", FileName));
        continue;
      }

      if (ResetType == EfiResetWarm) {
        //
        // WarmReset, save memory to file
        //
        Buffer     = (VOID *)(UINTN)mWinNtMemoryLayout->Descriptor[Index].Base;
        BufferSize = (DWORD)mWinNtMemoryLayout->Descriptor[Index].Size;
        Result = gWinNt->WriteFile (
                           NtFileHandle,
                           Buffer,
                           BufferSize,
                           &BufferSize,
                           NULL
                           );

        gWinNt->CloseHandle (NtFileHandle);
        if (!Result) {
          DEBUG ((EFI_D_ERROR, "ResetSystem Warm - write %s fail!\n", FileName));
        } else {
          DEBUG ((EFI_D_ERROR, "ResetSystem Warm - write %s success!\n", FileName));
        }
      } else {
        //
        // CodeReset, delete file
        //
        gWinNt->CloseHandle (NtFileHandle);

        Result = gWinNt->DeleteFile (FileName);
        if (!Result) {
          DEBUG ((EFI_D_ERROR, "ResetSystem Cold - delete %s fail!\n", FileName));
        } else {
          DEBUG ((EFI_D_ERROR, "ResetSystem Cold - delete %s success!\n", FileName));
        }
      }
    }
  }

  if (FeaturePcdGet (PcdWinNtTpm2Enable)) {
    Tpm2Shutdown (TPM_SU_CLEAR);
    Tpm2SimulatorShutdown ();
  }

  //
  // BUGBUG Need to kill all console windows later
  //
  //
  // Discard ResetType, always return 0 as exit code
  //
  gWinNt->ExitProcess (0);

  //
  // Should never go here
  //
  ASSERT (FALSE);
}
