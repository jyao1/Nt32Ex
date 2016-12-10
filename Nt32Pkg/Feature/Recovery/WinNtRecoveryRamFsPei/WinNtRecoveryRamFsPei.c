/**@file

Copyright (c) 2006 - 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

//
// The package level header files this module uses
//
#include <Uefi.h>
#include <WinNtPeim.h>
#include <Ppi/NtThunk.h>

//
// The protocols, PPI and GUID defintions for this module
//
#include <Ppi/BlockIo.h>

//
// The Library classes this module consumes
//
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PeiServicesLib.h>

#include "WinNtRecoveryRamFsPei.h"

#define EFI_NT32_RECOVERY_FS  L"..\\Fv\\RecoveryDisk.img"
#define FAT_BLOCK_OFFSET  0
//#define FAT_BLOCK_OFFSET  0x22

RAM_DISK_BLOCK_IO_PRIVATE   mRamDiskBlkIoPrivateTemplate = {
  RAM_DISK_BLOCK_IO_PRIVATE_SIGNATURE,
  {
    RamDiskGetNumberOfBlockDevices,
    RamDiskGetBlockDeviceMediaInfo,
    RamDiskReadBlocks
  },
  {
    UsbMassStorage,   // DeviceType;
    TRUE,             // MediaPresent;
    0,                // LastBlock (TBD)
    0,                // BlockSize; (TBD)
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiVirtualBlockIoPpiGuid,
    &mRamDiskBlkIoPrivateTemplate.BlockIo
  },
  1, // DeviceIndex - start from 1
};

#define RAM_DISK_BLOCK_SIZE 0x200

EFI_WIN_NT_THUNK_PROTOCOL   *mWinNt;

EFI_STATUS
ReadBuffer (
  OUT VOID        **Buffer,
  OUT UINTN       *BufferSize
  )
{
  VOID   *FileBuffer;
  DWORD  FileSize;
  HANDLE NtFileHandle;
  BOOL   Result;

  NtFileHandle = mWinNt->CreateFile (
                           EFI_NT32_RECOVERY_FS,
                           GENERIC_READ | GENERIC_WRITE,
                           FILE_SHARE_READ | FILE_SHARE_WRITE,
                           NULL,
                           OPEN_EXISTING,
                           0,
                           NULL
                           );
  if (NtFileHandle == INVALID_HANDLE_VALUE) {
    DEBUG ((EFI_D_ERROR, "WinNt recovery fs %s not found!\n", EFI_NT32_RECOVERY_FS));
    return EFI_NOT_FOUND;
  }

  FileSize = mWinNt->SetFilePointer (
                       NtFileHandle,
                       0,
                       NULL,
                       FILE_END
                       );
  mWinNt->SetFilePointer (
                       NtFileHandle,
                       0,
                       NULL,
                       FILE_BEGIN
                       );

  FileBuffer = AllocatePages (EFI_SIZE_TO_PAGES(FileSize));
  Result = mWinNt->ReadFile (
                     NtFileHandle,
                     FileBuffer,
                     FileSize,
                     &FileSize,
                     NULL
                     );
  mWinNt->CloseHandle (NtFileHandle);
  if (!Result) {
    DEBUG ((EFI_D_ERROR, "WinNt recovery fs %s read fail!\n", EFI_NT32_RECOVERY_FS));
    return EFI_NOT_FOUND;
  } else {
    DEBUG ((EFI_D_ERROR, "WinNt recovery fs %s read success!\n", EFI_NT32_RECOVERY_FS));
  }

  //
  // SetPositon at 0x22 * RAM_DISK_BLOCK_SIZE ??
  //
  *BufferSize = FileSize - FAT_BLOCK_OFFSET * RAM_DISK_BLOCK_SIZE;
  *Buffer     = (UINT8 *)FileBuffer + FAT_BLOCK_OFFSET * RAM_DISK_BLOCK_SIZE;

  return EFI_SUCCESS;
}

/**
  The user Entry Point for module WinNtSimpleFileSystem. The user code starts with this function.

  @param  FileHandle              Handle of the file being invoked. Type 
                                  EFI_PEI_FILE_HANDLE is defined in  
                                  FfsFindNextFile(). 
  @param  PeiServices             Describes the list of possible PEI Services. 

  @retval EFI_SUCCESS             The entry point was executed successfully. 
  @retval EFI_OUT_OF_RESOURCES    There is no enough memory to complete the 
                                  operations.

**/
EFI_STATUS
EFIAPI
WinNtRecoveryRamFsPeimEntry (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_STATUS              Status;
  VOID                    *Buffer;
  UINTN                   BufferSize;
  PEI_NT_THUNK_PPI        *NtThunkPpi;

  DEBUG ((EFI_D_ERROR, "WinNtRecoveryRamFsPeimEntry - enter\n"));

  Status = PeiServicesLocatePpi (
              &gPeiNtThunkPpiGuid,
              0,
              NULL,
              (VOID **) &NtThunkPpi
              );
  ASSERT_EFI_ERROR (Status);

  mWinNt  = (EFI_WIN_NT_THUNK_PROTOCOL *) NtThunkPpi->NtThunk ();

  ReadBuffer (&Buffer, &BufferSize);

  mRamDiskBlkIoPrivateTemplate.RamBuffer = Buffer;
  mRamDiskBlkIoPrivateTemplate.RamBufferSize = BufferSize;

  DEBUG ((EFI_D_ERROR, "RamBufferSize - %x\n", mRamDiskBlkIoPrivateTemplate.RamBufferSize));
  DEBUG ((EFI_D_ERROR, "RamBuffer - %x\n", mRamDiskBlkIoPrivateTemplate.RamBuffer));

  mRamDiskBlkIoPrivateTemplate.Media.BlockSize = RAM_DISK_BLOCK_SIZE;
  mRamDiskBlkIoPrivateTemplate.Media.LastBlock = (mRamDiskBlkIoPrivateTemplate.RamBufferSize / mRamDiskBlkIoPrivateTemplate.Media.BlockSize) - 1;

  Status = PeiServicesInstallPpi (&mRamDiskBlkIoPrivateTemplate.PpiDescriptor);

  DEBUG ((EFI_D_ERROR, "WinNtRecoveryRamFsPeimEntry - exit (%r)\n", Status));

  return Status;
}

/**
  Gets the count of block I/O devices that one specific block driver detects.

  This function is used for getting the count of block I/O devices that one 
  specific block driver detects.  To the PEI ATAPI driver, it returns the number
  of all the detected ATAPI devices it detects during the enumeration process. 
  To the PEI legacy floppy driver, it returns the number of all the legacy 
  devices it finds during its enumeration process. If no device is detected, 
  then the function will return zero.  
  
  @param[in]  PeiServices          General-purpose services that are available 
                                   to every PEIM.
  @param[in]  This                 Indicates the EFI_PEI_RECOVERY_BLOCK_IO_PPI 
                                   instance.
  @param[out] NumberBlockDevices   The number of block I/O devices discovered.

  @retval     EFI_SUCCESS          The operation performed successfully.

**/
EFI_STATUS
EFIAPI
RamDiskGetNumberOfBlockDevices (
  IN  EFI_PEI_SERVICES               **PeiServices,
  IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI  *This,
  OUT UINTN                          *NumberBlockDevices
  )
{
  *NumberBlockDevices = 1;
  return EFI_SUCCESS;
}

/**
  Gets a block device's media information.

  This function will provide the caller with the specified block device's media 
  information. If the media changes, calling this function will update the media 
  information accordingly.

  @param[in]  PeiServices   General-purpose services that are available to every
                            PEIM
  @param[in]  This          Indicates the EFI_PEI_RECOVERY_BLOCK_IO_PPI instance.
  @param[in]  DeviceIndex   Specifies the block device to which the function wants 
                            to talk. Because the driver that implements Block I/O 
                            PPIs will manage multiple block devices, the PPIs that 
                            want to talk to a single device must specify the 
                            device index that was assigned during the enumeration
                            process. This index is a number from one to 
                            NumberBlockDevices.
  @param[out] MediaInfo     The media information of the specified block media.  
                            The caller is responsible for the ownership of this 
                            data structure.

  @par Note: 
      The MediaInfo structure describes an enumeration of possible block device 
      types.  This enumeration exists because no device paths are actually passed 
      across interfaces that describe the type or class of hardware that is publishing 
      the block I/O interface. This enumeration will allow for policy decisions
      in the Recovery PEIM, such as "Try to recover from legacy floppy first, 
      LS-120 second, CD-ROM third." If there are multiple partitions abstracted 
      by a given device type, they should be reported in ascending order; this 
      order also applies to nested partitions, such as legacy MBR, where the 
      outermost partitions would have precedence in the reporting order. The 
      same logic applies to systems such as IDE that have precedence relationships 
      like "Master/Slave" or "Primary/Secondary". The master device should be 
      reported first, the slave second.
  
  @retval EFI_SUCCESS        Media information about the specified block device 
                             was obtained successfully.
  @retval EFI_DEVICE_ERROR   Cannot get the media information due to a hardware 
                             error.

**/
EFI_STATUS
EFIAPI
RamDiskGetBlockDeviceMediaInfo (
  IN  EFI_PEI_SERVICES               **PeiServices,
  IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI  *This,
  IN  UINTN                          DeviceIndex,
  OUT EFI_PEI_BLOCK_IO_MEDIA         *MediaInfo
  )
{
  RAM_DISK_BLOCK_IO_PRIVATE      *Private;

  Private = RAM_DISK_BLOCK_IO_PRIVATE_DATA_FROM_THIS(This);

  if (DeviceIndex != Private->DeviceIndex) {
    return EFI_NO_MEDIA;
  }

  CopyMem (MediaInfo, &Private->Media, sizeof(*MediaInfo));

  return EFI_SUCCESS;
}

/**
  Reads the requested number of blocks from the specified block device.

  The function reads the requested number of blocks from the device. All the 
  blocks are read, or an error is returned. If there is no media in the device,
  the function returns EFI_NO_MEDIA.

  @param[in]  PeiServices   General-purpose services that are available to 
                            every PEIM.
  @param[in]  This          Indicates the EFI_PEI_RECOVERY_BLOCK_IO_PPI instance.
  @param[in]  DeviceIndex   Specifies the block device to which the function wants 
                            to talk. Because the driver that implements Block I/O 
                            PPIs will manage multiple block devices, PPIs that 
                            want to talk to a single device must specify the device 
                            index that was assigned during the enumeration process. 
                            This index is a number from one to NumberBlockDevices.
  @param[in]  StartLBA      The starting logical block address (LBA) to read from
                            on the device
  @param[in]  BufferSize    The size of the Buffer in bytes. This number must be
                            a multiple of the intrinsic block size of the device.
  @param[out] Buffer        A pointer to the destination buffer for the data.
                            The caller is responsible for the ownership of the 
                            buffer.
                         
  @retval EFI_SUCCESS             The data was read correctly from the device.
  @retval EFI_DEVICE_ERROR        The device reported an error while attempting 
                                  to perform the read operation.
  @retval EFI_INVALID_PARAMETER   The read request contains LBAs that are not 
                                  valid, or the buffer is not properly aligned.
  @retval EFI_NO_MEDIA            There is no media in the device.
  @retval EFI_BAD_BUFFER_SIZE     The BufferSize parameter is not a multiple of
                                  the intrinsic block size of the device.

**/
EFI_STATUS
EFIAPI
RamDiskReadBlocks (
  IN  EFI_PEI_SERVICES               **PeiServices,
  IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI  *This,
  IN  UINTN                          DeviceIndex,
  IN  EFI_PEI_LBA                    StartLBA,
  IN  UINTN                          BufferSize,
  OUT VOID                           *Buffer
  )
{
  RAM_DISK_BLOCK_IO_PRIVATE      *Private;

  DEBUG ((EFI_D_ERROR, "WinNtRamDiskBlkIoReadBlocks - enter (%lx, %x)\n", StartLBA, BufferSize));

  Private = RAM_DISK_BLOCK_IO_PRIVATE_DATA_FROM_THIS(This);

  if (DeviceIndex != Private->DeviceIndex) {
    return EFI_NO_MEDIA;
  }

  if (StartLBA > Private->Media.LastBlock) {
    return EFI_INVALID_PARAMETER;
  }

  if ((BufferSize % Private->Media.BlockSize) != 0) {
    return EFI_BAD_BUFFER_SIZE;
  }

  CopyMem (Buffer, (VOID *)(UINTN)((UINTN)Private->RamBuffer + MultU64x32 (StartLBA, (UINT32)Private->Media.BlockSize)), BufferSize);

  DEBUG ((EFI_D_ERROR, "WinNtRamDiskBlkIoReadBlocks - exit\n"));

  return EFI_SUCCESS;
}
