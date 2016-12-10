/**@file

Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  BootMode.c
   
Abstract:

  Tiano PEIM to provide the platform support functionality within Windows

**/



//
// The package level header files this module uses
//
#include <PiPei.h>
//
// The protocols, PPI and GUID defintions for this module
//
#include <Ppi/MasterBootMode.h>
#include <Ppi/BootInRecoveryMode.h>
#include <Ppi/Capsule.h>
#include <Ppi/NtRecovery.h>

//
// The Library classes this module consumes
//
#include <Library/DebugLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>

//
// Module globals
//
EFI_PEI_PPI_DESCRIPTOR  mPpiListBootMode = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiMasterBootModePpiGuid,
  NULL
};

EFI_PEI_PPI_DESCRIPTOR  mPpiListRecoveryBootMode = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiBootInRecoveryModePpiGuid,
  NULL
};

/**
  Determine if we're in capsule update boot mode.
  It's done by calling Capsule PPI.

  @param  PeiServices General purpose services available to every PEIM.

  @retval TRUE  If it's Capsule boot path.

  @retval FALSE If it's not Capsule boot path.
**/
BOOLEAN
CheckUpdateCapsule (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS              Status;
  PEI_CAPSULE_PPI         *Capsule;

  Status = (**PeiServices).LocatePpi (
             PeiServices,
             &gPeiCapsulePpiGuid,
             0,
             NULL,
             (VOID **) &Capsule
             );
  if (!EFI_ERROR(Status)) {
    Status = Capsule->CheckCapsuleUpdate ((EFI_PEI_SERVICES **) PeiServices);
    if (!EFI_ERROR(Status)) {
      return TRUE;
    }
  }

  return FALSE;
}

BOOLEAN
CheckRecovery (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS              Status;
  PEI_NT_RECOVERY_PPI     *NtRecovery;
  BOOLEAN                 IsRecovery;

  Status = (**PeiServices).LocatePpi (
             PeiServices,
             &gPeiNtRecoveryPpiGuid,
             0,
             NULL,
             (VOID **) &NtRecovery
             );
  if (EFI_ERROR(Status)) {
    return FALSE;
  }

  Status = NtRecovery->IsRecoveryMode (&IsRecovery);
  if (EFI_ERROR(Status)) {
    return FALSE;
  }

  return IsRecovery;
}

EFI_STATUS
EFIAPI
InitializeBootMode (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
/*++

Routine Description:

  Peform the boot mode determination logic

Arguments:

  FileHandle  - Handle of the file being invoked.
  PeiServices - Describes the list of possible PEI Services.
    
Returns:

  Status -  EFI_SUCCESS if the boot mode could be set

--*/
{
  EFI_STATUS  Status;
  UINTN       BootMode;

  DEBUG ((EFI_D_ERROR, "NT32 Boot Mode PEIM Loaded\n"));

  //
  // Let's assume things are OK if not told otherwise
  // Should we read an environment variable in order to easily change this?
  //
  BootMode  = BOOT_WITH_FULL_CONFIGURATION;
  if (FeaturePcdGet (PcdWinNtCapsuleEnable)) {
    if (CheckUpdateCapsule (PeiServices)) {
      BootMode = BOOT_ON_FLASH_UPDATE;
    }
  }
  if (FeaturePcdGet (PcdWinNtRecoveryEnable)) {
    if (CheckRecovery (PeiServices)) {
      BootMode = BOOT_IN_RECOVERY_MODE;
    }
  }

  DEBUG ((EFI_D_ERROR, "NT32 Boot Mode - %x\n", BootMode));
  Status    = (**PeiServices).SetBootMode (PeiServices, (UINT8) BootMode);
  ASSERT_EFI_ERROR (Status);

  Status = (**PeiServices).InstallPpi (PeiServices, &mPpiListBootMode);
  ASSERT_EFI_ERROR (Status);

  if (BootMode == BOOT_IN_RECOVERY_MODE) {
    Status = (**PeiServices).InstallPpi (PeiServices, &mPpiListRecoveryBootMode);
    ASSERT_EFI_ERROR (Status);
  }

  return Status;
}
