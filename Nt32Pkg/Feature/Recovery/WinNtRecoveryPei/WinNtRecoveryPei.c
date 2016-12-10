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

#include <WinNtPeim.h>
#include <Ppi/NtThunk.h>

#include <Ppi/DeviceRecoveryModule.h>
#include <Ppi/NtRecovery.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>

CHAR16 *mNt32RecoveryFileName;

EFI_STATUS
EFIAPI
GetNumberRecoveryCapsules (
  IN  EFI_PEI_SERVICES                      **PeiServices,
  IN  EFI_PEI_DEVICE_RECOVERY_MODULE_PPI    *This,
  OUT UINTN                                 *NumberRecoveryCapsules
);

EFI_STATUS
EFIAPI
GetRecoveryCapsuleInfo (
  IN  EFI_PEI_SERVICES                      **PeiServices,
  IN  EFI_PEI_DEVICE_RECOVERY_MODULE_PPI    *This,
  IN  UINTN                                 CapsuleInstance,
  OUT UINTN                                 *Size,
  OUT EFI_GUID                              *CapsuleType
);

EFI_STATUS
EFIAPI
LoadRecoveryCapsule (
  IN  OUT EFI_PEI_SERVICES                  **PeiServices,
  IN  EFI_PEI_DEVICE_RECOVERY_MODULE_PPI    *This,
  IN  UINTN                                 CapsuleInstance,
  OUT VOID                                  *Buffer
);

EFI_PEI_DEVICE_RECOVERY_MODULE_PPI mWinNtRecoveryModule = {
  GetNumberRecoveryCapsules,
  GetRecoveryCapsuleInfo,
  LoadRecoveryCapsule
};

//
// PPI to be installed
//
EFI_PEI_PPI_DESCRIPTOR mWinNtRecoveryPpiList[] = { 
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiDeviceRecoveryModulePpiGuid,
    &mWinNtRecoveryModule
  }
};

EFI_WIN_NT_THUNK_PROTOCOL   *mWinNt;
  
/**
  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval EFI_SUCCESS            PPI successfully installed.

**/  
EFI_STATUS
EFIAPI
WinNtRecoveryPeimEntry (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  EFI_STATUS        Status;
  PEI_NT_THUNK_PPI  *NtThunkPpi;

  Status = PeiServicesLocatePpi (
              &gPeiNtThunkPpiGuid,
              0,
              NULL,
              (VOID **) &NtThunkPpi
              );
  ASSERT_EFI_ERROR (Status);

  mWinNt  = (EFI_WIN_NT_THUNK_PROTOCOL *) NtThunkPpi->NtThunk ();

  Status = PeiServicesInstallPpi (mWinNtRecoveryPpiList);
  ASSERT_EFI_ERROR (Status);

  mNt32RecoveryFileName = (CHAR16 *)PcdGetPtr(PcdRecoveryFileName);

  return EFI_SUCCESS;
}

/**
  Returns the number of DXE capsules residing on the device.

  This function searches for DXE capsules from the associated device and returns
  the number and maximum size in bytes of the capsules discovered. Entry 1 is 
  assumed to be the highest load priority and entry N is assumed to be the lowest 
  priority.

  @param[in]  PeiServices              General-purpose services that are available 
                                       to every PEIM
  @param[in]  This                     Indicates the EFI_PEI_DEVICE_RECOVERY_MODULE_PPI
                                       instance.
  @param[out] NumberRecoveryCapsules   Pointer to a caller-allocated UINTN. On 
                                       output, *NumberRecoveryCapsules contains 
                                       the number of recovery capsule images 
                                       available for retrieval from this PEIM 
                                       instance.

  @retval EFI_SUCCESS        One or more capsules were discovered.
  @retval EFI_DEVICE_ERROR   A device error occurred.
  @retval EFI_NOT_FOUND      A recovery DXE capsule cannot be found.

**/
EFI_STATUS
EFIAPI
GetNumberRecoveryCapsules (
  IN  EFI_PEI_SERVICES                        **PeiServices,
  IN  EFI_PEI_DEVICE_RECOVERY_MODULE_PPI      *This,
  OUT UINTN                                   *NumberRecoveryCapsules
  )
{
  *NumberRecoveryCapsules = 1;
  return EFI_SUCCESS;
}

/**
  Returns the size and type of the requested recovery capsule.

  This function gets the size and type of the capsule specified by CapsuleInstance.

  @param[in]  PeiServices       General-purpose services that are available to every PEIM
  @param[in]  This              Indicates the EFI_PEI_DEVICE_RECOVERY_MODULE_PPI 
                                instance.
  @param[in]  CapsuleInstance   Specifies for which capsule instance to retrieve 
                                the information.  This parameter must be between 
                                one and the value returned by GetNumberRecoveryCapsules() 
                                in NumberRecoveryCapsules.
  @param[out] Size              A pointer to a caller-allocated UINTN in which 
                                the size of the requested recovery module is 
                                returned.
  @param[out] CapsuleType       A pointer to a caller-allocated EFI_GUID in which 
                                the type of the requested recovery capsule is 
                                returned.  The semantic meaning of the value 
                                returned is defined by the implementation.

  @retval EFI_SUCCESS        One or more capsules were discovered.
  @retval EFI_DEVICE_ERROR   A device error occurred.
  @retval EFI_NOT_FOUND      A recovery DXE capsule cannot be found.

**/
EFI_STATUS
EFIAPI
GetRecoveryCapsuleInfo (
  IN  EFI_PEI_SERVICES                      **PeiServices,
  IN  EFI_PEI_DEVICE_RECOVERY_MODULE_PPI    *This,
  IN  UINTN                                 CapsuleInstance,
  OUT UINTN                                 *Size,
  OUT EFI_GUID                              *CapsuleType
  )
{
  DWORD  FileSize;
  HANDLE NtFileHandle;

  if (CapsuleInstance != 1) {
    return EFI_NOT_FOUND;
  }

  NtFileHandle = mWinNt->CreateFile (
                           mNt32RecoveryFileName,
                           GENERIC_READ | GENERIC_WRITE,
                           FILE_SHARE_READ | FILE_SHARE_WRITE,
                           NULL,
                           OPEN_EXISTING,
                           0,
                           NULL
                           );
  if (NtFileHandle == INVALID_HANDLE_VALUE) {
    DEBUG ((EFI_D_ERROR, "WinNt recovery file %s not found!\n", mNt32RecoveryFileName));
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
  mWinNt->CloseHandle(NtFileHandle);

  *Size = FileSize;
  CopyMem (CapsuleType, &gPeiNtRecoveryPpiGuid, sizeof(gPeiNtRecoveryPpiGuid));
  return EFI_SUCCESS;
}

/**
  Loads a DXE capsule from some media into memory.

  This function, by whatever mechanism, retrieves a DXE capsule from some device
  and loads it into memory. Note that the published interface is device neutral.

  @param[in]     PeiServices       General-purpose services that are available 
                                   to every PEIM
  @param[in]     This              Indicates the EFI_PEI_DEVICE_RECOVERY_MODULE_PPI
                                   instance.
  @param[in]     CapsuleInstance   Specifies which capsule instance to retrieve.
  @param[out]    Buffer            Specifies a caller-allocated buffer in which 
                                   the requested recovery capsule will be returned.

  @retval EFI_SUCCESS        The capsule was loaded correctly.
  @retval EFI_DEVICE_ERROR   A device error occurred.
  @retval EFI_NOT_FOUND      A requested recovery DXE capsule cannot be found.

**/
EFI_STATUS
EFIAPI
LoadRecoveryCapsule (
  IN  EFI_PEI_SERVICES                      **PeiServices,
  IN  EFI_PEI_DEVICE_RECOVERY_MODULE_PPI    *This,
  IN  UINTN                                 CapsuleInstance,
  OUT VOID                                  *Buffer
  )
{
  DWORD      FileSize;
  BOOL       Result;
  HANDLE     NtFileHandle;

  if (CapsuleInstance != 1) {
    return EFI_NOT_FOUND;
  }

  DEBUG ((EFI_D_ERROR, "WinNt LoadRecoveryCapsule ...\n"));

  NtFileHandle = mWinNt->CreateFile (
                           mNt32RecoveryFileName,
                           GENERIC_READ | GENERIC_WRITE,
                           FILE_SHARE_READ | FILE_SHARE_WRITE,
                           NULL,
                           OPEN_EXISTING,
                           0,
                           NULL
                           );
  if (NtFileHandle == INVALID_HANDLE_VALUE) {
    DEBUG ((EFI_D_ERROR, "WinNt recovery file %s not found!\n", mNt32RecoveryFileName));
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

  Result = mWinNt->ReadFile (
                     NtFileHandle,
                     Buffer,
                     FileSize,
                     &FileSize,
                     NULL
                     );
  mWinNt->CloseHandle (NtFileHandle);
  if (!Result) {
    DEBUG ((EFI_D_ERROR, "WinNt recovery file %s read fail!\n", mNt32RecoveryFileName));
    return EFI_NOT_FOUND;
  } else {
    DEBUG ((EFI_D_ERROR, "WinNt recovery file %s read success!\n", mNt32RecoveryFileName));
  }

  return EFI_SUCCESS;
}
