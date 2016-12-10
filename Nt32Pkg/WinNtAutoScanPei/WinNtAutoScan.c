/**@file

Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
(C) Copyright 2016 Hewlett Packard Enterprise Development LP<BR>
This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:
  WinNtAutoscan.c

Abstract:
  This PEIM to abstract memory auto-scan in a Windows NT environment.

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
#include <Ppi/NtAutoscan.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/Capsule.h>

#include <Guid/MemoryTypeInformation.h>
#include <Guid/SmramMemoryReserve.h>
#include <Guid/WinNtMemoryLayout.h>

//
// The Library classes this module consumes
//
#include <Library/DebugLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/HobLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcdLib.h>

EFI_MEMORY_TYPE_INFORMATION mDefaultMemoryTypeInformation[] = {
  { EfiReservedMemoryType,  0x0010 },
  { EfiRuntimeServicesCode, 0x0060 },
  { EfiRuntimeServicesData, 0x00B0 },
  { EfiACPIMemoryNVS,       0x0008 },
  { EfiACPIReclaimMemory,   0x0008 },
  { EfiMaxMemoryType,       0      }
};

/**
   Validate variable data for the MemoryTypeInformation. 

   @param MemoryData       Variable data.
   @param MemoryDataSize   Variable data length.
   
   @return TRUE            The variable data is valid.
   @return FALSE           The variable data is invalid.

**/
BOOLEAN
ValidateMemoryTypeInfoVariable (
  IN EFI_MEMORY_TYPE_INFORMATION      *MemoryData,
  IN UINTN                            MemoryDataSize
  )
{
  UINTN                       Count;
  UINTN                       Index;

  // Check the input parameter.
  if (MemoryData == NULL) {
    return FALSE;
  }

  // Get Count
  Count = MemoryDataSize / sizeof (*MemoryData);

  // Check Size
  if (Count * sizeof(*MemoryData) != MemoryDataSize) {
    return FALSE;
  }

  // Check last entry type filed.
  if (MemoryData[Count - 1].Type != EfiMaxMemoryType) {
    return FALSE;
  }

  // Check the type filed.
  for (Index = 0; Index < Count - 1; Index++) {
    if (MemoryData[Index].Type >= EfiMaxMemoryType) {
      return FALSE;
    }
  }

  return TRUE;
}

UINTN
CountSeperatorsInString (
  IN  CONST CHAR16   *String,
  IN  CHAR16         Seperator
  )
/*++

Routine Description:
  Count the number of seperators in String

Arguments:
  String    - String to process
  Seperator - Item to count

Returns:
  Number of Seperator in String

--*/
{
  UINTN Count;

  for (Count = 0; *String != '\0'; String++) {
    if (*String == Seperator) {
      Count++;
    }
  }

  return Count;
}

UINTN
GetMaxSystemMemoryCount (
  VOID
  )
{
  UINTN  SystemMemoryCount;
  
  SystemMemoryCount = CountSeperatorsInString(PcdGetPtr(PcdWinNtMemorySizeForSecMain), '!') + 1;
  return SystemMemoryCount;
}


EFI_STATUS
EFIAPI
PeimInitializeWinNtAutoScan (
  IN       EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
/*++

Routine Description:
  Perform a call-back into the SEC simulator to get a memory value

Arguments:
  FfsHeader   - General purpose data available to every PEIM
  PeiServices - General purpose services available to every PEIM.
    
Returns:
  None

--*/
{
  EFI_STATUS                            Status;
  EFI_PEI_PPI_DESCRIPTOR                *PpiDescriptor;
  PEI_NT_AUTOSCAN_PPI                   *PeiNtService;
  UINT64                                MemorySize;
  EFI_PHYSICAL_ADDRESS                  MemoryBase;
  UINTN                                 Index;
  EFI_RESOURCE_ATTRIBUTE_TYPE           Attributes;
  UINT64                                PeiMemorySize;
  EFI_PHYSICAL_ADDRESS                  PeiMemoryBase;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI       *Variable;
  UINTN                                 DataSize;
  EFI_MEMORY_TYPE_INFORMATION           MemoryData [EfiMaxMemoryType + 1];
  UINT64                                SmramMemorySize;
  EFI_PHYSICAL_ADDRESS                  SmramMemoryBase;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK        *SmramHobDescriptorBlock;
  PEI_CAPSULE_PPI                       *Capsule;
  VOID                                  *CapsuleBuffer;
  UINTN                                 CapsuleBufferLength;
  EFI_BOOT_MODE                         BootMode;
  EFI_WIN_NT_MEMORY_LAYOUT              *MemoryLayout;
  UINTN                                 MaxSystemMemoryCount;

  DEBUG ((EFI_D_ERROR, "NT 32 Autoscan PEIM Loaded\n"));

  //
  // Get the PEI NT Autoscan PPI
  //
  Status = PeiServicesLocatePpi (
             &gPeiNtAutoScanPpiGuid, // GUID
             0,                      // INSTANCE
             &PpiDescriptor,         // EFI_PEI_PPI_DESCRIPTOR
             (VOID**)&PeiNtService           // PPI
             );
  ASSERT_EFI_ERROR (Status);

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_ERROR, "BootMode - %x\n", BootMode));

  if (FeaturePcdGet (PcdWinNtCapsuleEnable)) {
    MaxSystemMemoryCount = GetMaxSystemMemoryCount();
    MemoryLayout = BuildGuidHob (
                     &gEfiWinNtMemoryLayoutGuid,
                     sizeof (EFI_WIN_NT_MEMORY_LAYOUT) + sizeof (EFI_WIN_NT_MEMORY_DESCRIPTOR) * (MaxSystemMemoryCount - 1)
                     );
    ASSERT (MemoryLayout != NULL);
    MemoryLayout->NumberOfRegions = 0;
  
    Capsule = NULL;
    CapsuleBuffer = NULL;
    CapsuleBufferLength = 0;
    if (BootMode == BOOT_ON_FLASH_UPDATE) {
      Status = PeiServicesLocatePpi (&gPeiCapsulePpiGuid, 0, NULL, (VOID**) &Capsule);
      ASSERT_EFI_ERROR (Status);
    }
  }

  Index = 0;
  SmramMemorySize = 0;
  SmramMemoryBase = 0;
  do {
    Status = PeiNtService->NtAutoScan (Index, &MemoryBase, &MemorySize);
    DEBUG ((EFI_D_ERROR, "NtAutoScan(%d) Status - %r\n", Index, Status));
    if (!EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "NtAutoScan(%d) Base - 0x%lx\n", Index, MemoryBase));
      DEBUG ((EFI_D_ERROR, "NtAutoScan(%d) Size - 0x%lx\n", Index, MemorySize));

      if (FeaturePcdGet (PcdWinNtCapsuleEnable)) {
        if (MemoryLayout->NumberOfRegions < MaxSystemMemoryCount) {
          MemoryLayout->Descriptor[MemoryLayout->NumberOfRegions].Base = MemoryBase;
          MemoryLayout->Descriptor[MemoryLayout->NumberOfRegions].Size = MemorySize;
          MemoryLayout->NumberOfRegions ++;
        }
      }

      Attributes =
        (
          EFI_RESOURCE_ATTRIBUTE_PRESENT |
          EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
          EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
          EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
          EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
          EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
        );

      if (Index == 0) {
        //
        // Register the memory with the PEI Core
        //

        if (FeaturePcdGet(PcdWinNtSmmEnable)) {
          //
          // SMRAM
          //
          SmramMemorySize = PcdGet64(PcdWinNtSmramSize);
          SmramMemoryBase = MemoryBase + MemorySize - SmramMemorySize;
          DEBUG ((EFI_D_ERROR, "SmramMemoryBase - 0x%lx\n", SmramMemoryBase));
          DEBUG ((EFI_D_ERROR, "SmramMemorySize - 0x%lx\n", SmramMemorySize));

          MemorySize      = MemorySize - SmramMemorySize;
        }

        PeiMemoryBase = MemoryBase;
        PeiMemorySize = MemorySize;

        if (FeaturePcdGet(PcdWinNtCapsuleEnable)) {
          //
          // Capsule
          //
          if (Capsule != NULL) {
            CapsuleBufferLength = ((UINTN) PeiMemorySize / 2);
            PeiMemorySize       = CapsuleBufferLength;
            CapsuleBuffer = (VOID*) (UINTN) (PeiMemoryBase + CapsuleBufferLength);
          }
        }

        Attributes |= EFI_RESOURCE_ATTRIBUTE_TESTED;
      }
      
      BuildResourceDescriptorHob (
        EFI_RESOURCE_SYSTEM_MEMORY,
        Attributes,
        MemoryBase,
        MemorySize
        );
      DEBUG ((EFI_D_ERROR, "ResourceHob - 0x%lx - 0x%lx\n", MemoryBase, MemorySize));
    }
    Index++;
  } while (!EFI_ERROR (Status));

  if (FeaturePcdGet(PcdWinNtCapsuleEnable)) {
    if (Capsule != NULL) {
      //
      // Call the Capsule PPI Coalesce function to coalesce the capsule data.
      //
      Status = Capsule->Coalesce (
                          (EFI_PEI_SERVICES**) PeiServices,
                          &CapsuleBuffer,
                          &CapsuleBufferLength
                          );
      DEBUG ((EFI_D_ERROR, "CoalesceStatus - %r\n", Status));
      DEBUG ((EFI_D_ERROR, "CapsuleBuffer - %x\n", CapsuleBuffer));
      DEBUG ((EFI_D_ERROR, "CapsuleBufferLength - %x\n", CapsuleBufferLength));
      
      //
      // If it failed, then NULL out our capsule PPI pointer so that the capsule
      // HOB does not get created below.
      //
      if (Status != EFI_SUCCESS) {
        Capsule = NULL;
      }
    }
  }

  Status = PeiServicesInstallPeiMemory (PeiMemoryBase, PeiMemorySize);
  ASSERT_EFI_ERROR (Status);

  if (FeaturePcdGet(PcdWinNtCapsuleEnable)) {
    //
    // If we found the capsule PPI (and we didn't have errors), then
    // call the capsule PEIM to allocate memory for the capsule.
    //
    if (Capsule != NULL) {
      Status = Capsule->CreateState((EFI_PEI_SERVICES **)PeiServices, CapsuleBuffer, CapsuleBufferLength);
    }
  }

  //
  // Build the CPU hob with 52-bit addressing and 16-bits of IO space.
  //
  BuildCpuHob (52, 16);

  //
  // Build GUIDed Hob that contains the Memory Type Information array
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **)&Variable
             );
  ASSERT_EFI_ERROR (Status);

  DataSize = sizeof (MemoryData);
  Status = Variable->GetVariable (
                       Variable,
                       EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME,
                       &gEfiMemoryTypeInformationGuid,
                       NULL,
                       &DataSize,
                       &MemoryData
                       );
  if (EFI_ERROR (Status) || !ValidateMemoryTypeInfoVariable(MemoryData, DataSize)) {
    //
    // Create Memory Type Information HOB
    //
    BuildGuidDataHob (
      &gEfiMemoryTypeInformationGuid,
      mDefaultMemoryTypeInformation,
      sizeof(mDefaultMemoryTypeInformation)
      );
  } else {
    //
    // Create Memory Type Information HOB
    //
    BuildGuidDataHob (
      &gEfiMemoryTypeInformationGuid,
      MemoryData,
      DataSize
      );
  }

  if (FeaturePcdGet(PcdWinNtSmmEnable)) {
    //
    // BuildSmramHob
    //
    if ((SmramMemoryBase != 0) && (SmramMemorySize != 0)) {
      SmramHobDescriptorBlock = BuildGuidHob (
                                  &gEfiSmmPeiSmramMemoryReserveGuid,
                                  sizeof (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK) + sizeof (EFI_SMRAM_DESCRIPTOR)
                                  );
      ASSERT (SmramHobDescriptorBlock != NULL);
      SmramHobDescriptorBlock->NumberOfSmmReservedRegions = 1;

      SmramHobDescriptorBlock->Descriptor[0].PhysicalStart = SmramMemoryBase;
      SmramHobDescriptorBlock->Descriptor[0].CpuStart      = SmramMemoryBase;
      SmramHobDescriptorBlock->Descriptor[0].PhysicalSize  = SmramMemorySize;
      SmramHobDescriptorBlock->Descriptor[0].RegionState   = EFI_SMRAM_CLOSED;
    }
  }

  return Status;
}
