/**@file

Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:
  WinNtFwh.c
    
Abstract:
  PEIM to abstract construction of firmware volume in a Windows NT environment.

Revision History

**/

//
// The package level header files this module uses
//
#include <PiPei.h>
#include <WinNtPeim.h>
//
// The protocols, PPI and GUID defintions for this module
//
#include <Ppi/NtFwh.h>
#include <Ppi/FirmwareVolumeInfo.h>
#include <Guid/FirmwareFileSystem2.h>
//
// The Library classes this module consumes
//
#include <Library/DebugLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>

EFI_STATUS
EFIAPI
PeimInitializeWinNtFwh (
  IN EFI_FFS_FILE_HEADER       *FfsHeader,
  IN EFI_PEI_SERVICES          **PeiServices
  )
/*++

Routine Description:
  Perform a call-back into the SEC simulator to get address of the Firmware Hub

Arguments:
  FfsHeader   - Ffs Header available to every PEIM
  PeiServices - General purpose services available to every PEIM.
    
Returns:
  None

--*/
{
  EFI_STATUS                  Status;
  EFI_PEI_PPI_DESCRIPTOR      *PpiDescriptor;
  NT_FWH_PPI                  *FwhPpi;
  EFI_PHYSICAL_ADDRESS        FdBase;
  EFI_FIRMWARE_VOLUME_HEADER  *FvHeader;
  UINT64                      FdSize;
  UINTN                       Index;
  EFI_BOOT_MODE               BootMode;

  BootMode = GetBootModeHob();

  DEBUG ((EFI_D_ERROR, "NT 32 Firmware Volume PEIM Loaded - BootMode (0x%x)\n", BootMode));

  //
  // Get the Fwh Information PPI
  //
  Status = (**PeiServices).LocatePpi (
                            (const EFI_PEI_SERVICES **)PeiServices,
                            &gNtFwhPpiGuid, // GUID
                            0,              // INSTANCE
                            &PpiDescriptor, // EFI_PEI_PPI_DESCRIPTOR
                            (VOID**)&FwhPpi         // PPI
                            );
  ASSERT_EFI_ERROR (Status);

  Index = 0;
  do {
    //
    // Get information about all the FD's in the system
    //
    Status = FwhPpi->NtFwh (Index, &FdBase, &FdSize);
    if (!EFI_ERROR (Status)) {
      //
      // Assume the FD starts with an FV header
      //
      FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *) (UINTN) FdBase;

      if (Index == 0) {
        FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *)((UINTN)FdBase + FixedPcdGet32(PcdWinNtFlashFvRecoveryBase));
        BuildFvHob((UINT64)(UINTN)FvHeader, FvHeader->FvLength);
        DEBUG((EFI_D_INFO, "BuildFvHob (FvRecovery) - 0x%lx - 0x%lx\n", (UINT64)(UINTN)FvHeader, FvHeader->FvLength));

        //
        // Do we report Variable FV? Yes, because:
        // 1) The variable FV header corruption should be handled by FVB DXE driver.
        // 2) The variable data corruption should be handled by default variable hob solution.
        // So it is safety to report Variable FV here.
        //
        FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *)((UINTN)FdBase + FixedPcdGet32(PcdWinNtFlashNvStorageVariableBase));
        BuildFvHob((UINT64)(UINTN)FvHeader, FvHeader->FvLength);
        DEBUG((EFI_D_INFO, "BuildFvHob (Variable) - 0x%lx - 0x%lx\n", (UINT64)(UINTN)FvHeader, FvHeader->FvLength));

        FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *)((UINTN)FdBase + FixedPcdGet32(PcdWinNtMicrocodeFvBase));
        BuildFvHob((UINT64)(UINTN)FvHeader, FvHeader->FvLength);
        DEBUG((EFI_D_INFO, "BuildFvHob (Microcode) - 0x%lx - 0x%lx\n", (UINT64)(UINTN)FvHeader, FvHeader->FvLength));

        if (BootMode != BOOT_IN_RECOVERY_MODE) {
          FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *)((UINTN)FdBase + FixedPcdGet32(PcdWinNtFlashFvMainBase));
          BuildFvHob((UINT64)(UINTN)FvHeader, FvHeader->FvLength);
          DEBUG((EFI_D_INFO, "BuildFvHob (FvMain) - 0x%lx - 0x%lx\n", (UINT64)(UINTN)FvHeader, FvHeader->FvLength));

          PeiServicesInstallFvInfoPpi(
            NULL,
            (VOID *)FvHeader,
            (UINT32)FvHeader->FvLength,
            NULL,
            NULL
            );
        }

        //
        // Assume the first FD was produced by the NT32.DSC
        //  All these strange offests are needed to keep in
        //  sync with the FlashMap and NT32.dsc file
        //
        BuildResourceDescriptorHob (
          EFI_RESOURCE_FIRMWARE_DEVICE,
          (EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
          FdBase,
          FdSize
          );
        ASSERT(FdSize >= PcdGet32(PcdWinNtFlashFvRecoverySize) + 
                         PcdGet32(PcdWinNtFlashFvMainSize) +
                         PcdGet32(PcdFlashNvStorageVariableSize) +
                         PcdGet32(PcdFlashNvStorageFtwWorkingSize) +
                         PcdGet32(PcdFlashNvStorageFtwSpareSize) +
                         PcdGet32(PcdWinNtFlashNvStorageEventLogSize) +
                         PcdGet32(PcdWinNtMicrocodeFvSize));
      } else {
        FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *)(UINTN)FdBase;
        do {
          if (FvHeader->Signature != EFI_FVH_SIGNATURE) {
            break;
          }
          //
          // Make an FV Hob for the FV in the FD
          //
          BuildFvHob((UINT64)(UINTN)FvHeader, FvHeader->FvLength);
          DEBUG((EFI_D_INFO, "BuildFvHob - 0x%lx - 0x%lx\n", (UINT64)(UINTN)FvHeader, FvHeader->FvLength));
          FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *)(UINTN)((UINTN)FvHeader + FvHeader->FvLength);
        } while ((UINTN)FvHeader < (UINTN)(FdBase + FdSize));

        //
        // For other FD's just map them in.
        //
        BuildResourceDescriptorHob (
          EFI_RESOURCE_FIRMWARE_DEVICE,
          (EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
          FdBase,
          FdSize
          );
      }
    }

    Index++;
  } while (!EFI_ERROR (Status));

  return Status;
}
