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

#include <Protocol/SmmAccess2.h>

#include <Guid/SmramMemoryReserve.h>

#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/HobLib.h>

///
/// SMM Access Protocol function prototypes
///
EFI_STATUS
EFIAPI
Open (
  IN EFI_SMM_ACCESS2_PROTOCOL  *This
  );

EFI_STATUS
EFIAPI
Close (
  IN EFI_SMM_ACCESS2_PROTOCOL  *This
  );

EFI_STATUS
EFIAPI
Lock (
  IN EFI_SMM_ACCESS2_PROTOCOL  *This
  );

EFI_STATUS
EFIAPI
GetCapabilities (
  IN CONST EFI_SMM_ACCESS2_PROTOCOL  *This,
  IN OUT UINTN                       *SmramMapSize, 
  IN OUT EFI_SMRAM_DESCRIPTOR        *SmramMap
  );

///
/// Handle for the SMM Access 2 Protocol
///
EFI_HANDLE                mHandle = NULL;

//
// SMM Access 2 Protocol instance
//
EFI_SMM_ACCESS2_PROTOCOL  mSmmAccess = {
  Open,
  Close,
  Lock,
  GetCapabilities,
  FALSE,
  FALSE
};

///
/// Number of SMRAM Regions managed by the SMM Access 2 Protocol
///
UINTN                 mNumberRegions = 0;

///
/// Pointer to an allocated buffer of SMRAM descriptors
///
EFI_SMRAM_DESCRIPTOR  *mSmramDesc = NULL;
  
/**
   The entry function for Clarkdale Uncore SMM Access 2 Protocol driver.

   @param ImageHandle   The driver image handle for Clarkdale Uncore SMM 
                        Access 2 Protocol driver
   @param SystemTable   The pointer to System Table

   @retval EFI_SUCCESS  Success to initialize the Clarkdale Uncore SMM 
                        Access 2 Protocol driver.
   @return EFI_STATUS   The status of initialization work.

**/
EFI_STATUS
EFIAPI
SmmAccessDriverEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                       Status;
  UINTN                            Index;
  VOID                             *GuidHob;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK   *DescriptorBlock;

  //
  // Retrieve the GUIDed HOB of SMRAM region descriptors published in PEI
  //
  GuidHob = GetFirstGuidHob (&gEfiSmmPeiSmramMemoryReserveGuid);
  if (GuidHob == NULL) {
    DEBUG ((EFI_D_ERROR, "SmramMemoryReserve HOB not found\n"));
    return EFI_NOT_FOUND;
  }
  DescriptorBlock = GET_GUID_HOB_DATA (GuidHob);

  //
  // Allocate buffer for all the SMRAM descriptors
  //
  mNumberRegions = DescriptorBlock->NumberOfSmmReservedRegions;
  mSmramDesc = (EFI_SMRAM_DESCRIPTOR *)AllocatePool (mNumberRegions * sizeof (EFI_SMRAM_DESCRIPTOR));
  ASSERT (mSmramDesc != NULL);

  //
  // Use the HOB to initialize the allocated buffer of SMRAM region descriptors
  // 
  for (Index = 0; Index < mNumberRegions; Index++) {
    mSmramDesc[Index].PhysicalStart = DescriptorBlock->Descriptor[Index].PhysicalStart;
    mSmramDesc[Index].CpuStart      = DescriptorBlock->Descriptor[Index].CpuStart;
    mSmramDesc[Index].PhysicalSize  = DescriptorBlock->Descriptor[Index].PhysicalSize;
    mSmramDesc[Index].RegionState   = DescriptorBlock->Descriptor[Index].RegionState;
  }

  //
  // Install SMM Access 2 Protocol into a new handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces(
                  &mHandle,
                  &gEfiSmmAccess2ProtocolGuid, &mSmmAccess,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);                  
  
  return EFI_SUCCESS;
}

/**
   This routine accepts a request to "open" a region of SMRAM.  The
   region could be legacy ABSEG, HSEG, or TSEG near top of physical memory.
   The use of "open" means that the memory is visible from all boot-service
   and SMM agents.

   @param This                    Pointer to the SMM Access Interface.
   
   @retval EFI_SUCCESS            The region was successfully opened.
   @retval EFI_DEVICE_ERROR       The region could not be opened because locked by chipset.

**/
EFI_STATUS
EFIAPI
Open (
  IN EFI_SMM_ACCESS2_PROTOCOL  *This
  )
{
  if (mSmmAccess.LockState) {
    return EFI_DEVICE_ERROR;
  }
  //
  // Open all SMRAM regions 
  //
  mSmmAccess.OpenState = TRUE;
  
  return EFI_SUCCESS;
}

/**
   This routine accepts a request to "close" a region of SMRAM.  The
   region could be legacy AB or TSEG near top of physical memory.
   The use of "close" means that the memory is only visible from SMM agents,
   not from BS or RT code.

   @param This                      Pointer to the SMM Access Interface.
   
   @retval EFI_SUCCESS              The region was successfully closed.
   @retval EFI_DEVICE_ERROR         The region could not be closed because locked by
                                    chipset.
**/
EFI_STATUS
EFIAPI
Close (
  IN EFI_SMM_ACCESS2_PROTOCOL *This
  )
{
  if (mSmmAccess.LockState) {
    return EFI_DEVICE_ERROR;
  }
  //
  // Close all SMRAM regions
  //
  mSmmAccess.OpenState = FALSE;
  
  return EFI_SUCCESS;
}

/**
   This routine accepts a request to "lock" SMRAM.  The
   region could be legacy AB or TSEG near top of physical memory.
   The use of "lock" means that the memory can no longer be opened
   to BS state.

   @param This                     Pointer to the SMM Access Interface.
   
   @retval EFI_SUCCESS             The region was successfully locked.
   @retval EFI_DEVICE_ERROR        The region could not be locked because at least
                                   one range is still open.
**/
EFI_STATUS
EFIAPI
Lock (
  IN EFI_SMM_ACCESS2_PROTOCOL  *This
  )
{
  if (mSmmAccess.OpenState) {
    return EFI_DEVICE_ERROR;
  }
  //
  // Lock the chipset
  //
  mSmmAccess.LockState = TRUE;

  return EFI_SUCCESS;
}

/**
   This routine services a user request to discover the SMRAM
   capabilities of this platform.  This will report the possible
   ranges that are possible for SMRAM access, based upon the
   memory controller capabilities.

   @param This            Pointer to the SMRAM Access Interface.
   @param SmramMapSize    Pointer to the variable containing size of the
                          buffer to contain the description information.
   @param SmramMap        Buffer containing the data describing the Smram
                          region descriptors.
   
   @retval EFI_BUFFER_TOO_SMALL  The user did not provide a sufficient buffer.
   @retval EFI_SUCCESS           The user provided a sufficiently-sized buffer.

**/
EFI_STATUS
EFIAPI
GetCapabilities (
  IN CONST EFI_SMM_ACCESS2_PROTOCOL  *This,
  IN OUT UINTN                       *SmramMapSize, 
  IN OUT EFI_SMRAM_DESCRIPTOR        *SmramMap
  )
{
  UINTN  NecessaryBufferSize;

  NecessaryBufferSize = mNumberRegions * sizeof(EFI_SMRAM_DESCRIPTOR);
  if (*SmramMapSize < NecessaryBufferSize) {
    *SmramMapSize = NecessaryBufferSize;
    return EFI_BUFFER_TOO_SMALL;
  }
  CopyMem (SmramMap, mSmramDesc, NecessaryBufferSize);
  *SmramMapSize = NecessaryBufferSize;
  return EFI_SUCCESS;
}
