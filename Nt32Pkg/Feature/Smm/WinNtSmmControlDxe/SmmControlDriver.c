/** @file

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>

#include <Protocol/SmmControl2.h>
#include <Protocol/WinNtSmmTrigger.h>

#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>

///
/// SMM Control 2 Protocol function prototypes
///
EFI_STATUS
EFIAPI
Activate (
  IN CONST EFI_SMM_CONTROL2_PROTOCOL     *This,
  IN OUT  UINT8                          *CommandPort       OPTIONAL,
  IN OUT  UINT8                          *DataPort          OPTIONAL,
  IN      BOOLEAN                        Periodic           OPTIONAL,
  IN      EFI_SMM_PERIOD                 ActivationInterval OPTIONAL
  );

EFI_STATUS
EFIAPI
Deactivate (
  IN CONST     EFI_SMM_CONTROL2_PROTOCOL  *This,
  IN      BOOLEAN                         Periodic OPTIONAL
  );

///
/// Handle for the SMM Control2 Protocol
///
EFI_HANDLE  mSmmControl2Handle = NULL;

///
/// SMM COntrol2 Protocol instance
///
EFI_SMM_CONTROL2_PROTOCOL mSmmControl2 = {
  Activate,
  Deactivate,
  0
};

EFI_WIN_NT_SMM_TRIGGER_PROTOCOL *mSmmTrigger;

/**
  Generates an SMI using the parameters passed in.

  @param  This                A pointer to an instance of
                              EFI_SMM_CONTROL_PROTOCOL
  @param  ArgumentBuffer      The argument buffer
  @param  ArgumentBufferSize  The size of the argument buffer
  @param  Periodic            TRUE to indicate a periodical SMI
  @param  ActivationInterval  Interval of the periodical SMI

  @retval EFI_INVALID_PARAMETER  Periodic is TRUE or ArgumentBufferSize > 1
  @retval EFI_SUCCESS            SMI generated

**/
EFI_STATUS
EFIAPI
Activate (
  IN CONST EFI_SMM_CONTROL2_PROTOCOL     *This,
  IN OUT  UINT8                          *CommandPort       OPTIONAL,
  IN OUT  UINT8                          *DataPort          OPTIONAL,
  IN      BOOLEAN                        Periodic           OPTIONAL,
  IN      EFI_SMM_PERIOD                 ActivationInterval OPTIONAL
  )
{
  EFI_STATUS   Status;

  if (Periodic) {
    return EFI_INVALID_PARAMETER;
  }

  if (mSmmTrigger == NULL) {
    Status = gBS->LocateProtocol (&gEfiWinNtSmmTriggerProtocolGuid, NULL, &mSmmTrigger);
  }
  if (mSmmTrigger == NULL) {
    return EFI_NOT_READY;
  }

  Status = mSmmTrigger->Trigger (mSmmTrigger, CommandPort, DataPort);

  return Status;
}

/**
  Clears an SMI.

  @param  This      Pointer to an instance of EFI_SMM_CONTROL_PROTOCOL
  @param  Periodic  TRUE to indicate a periodical SMI

  @return Return value from SmmClear ()

**/
EFI_STATUS
EFIAPI
Deactivate (
  IN CONST EFI_SMM_CONTROL2_PROTOCOL     *This,
  IN       BOOLEAN                       Periodic
  )
{
  if (Periodic) {
    return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}

/**
  This is the constructor for the SMM Control protocol.

  This function installs EFI_SMM_CONTROL2_PROTOCOL.

  @param  ImageHandle Handle for the image of this driver
  @param  SystemTable Pointer to the EFI System Table

  @retval EFI_UNSUPPORTED There's no Intel ICH on this platform
  @return The status returned from InstallProtocolInterface().

--*/
EFI_STATUS
EFIAPI
SmmControl2Init (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  //
  // Install our protocol interfaces on the device's handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mSmmControl2Handle,
                  &gEfiSmmControl2ProtocolGuid,  &mSmmControl2,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
