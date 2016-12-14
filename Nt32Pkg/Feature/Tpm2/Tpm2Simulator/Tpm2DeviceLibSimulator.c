/** @file

Copyright (c) 2016, Intel Corporation. All rights reserved. <BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/Tpm2DeviceLib.h>
#include <IndustryStandard/Tpm20.h>

#define TPM_COMMAND_SOCKET_HOB_GUID \
  { 0xf592834d, 0xabf4, 0x4f2b, { 0x81, 0x83, 0x75, 0xd6, 0x6b, 0x64, 0x1d, 0x52 }}
#define TPM_PLATFORM_SOCKET_HOB_GUID \
  { 0x28eb86a7, 0x62bb, 0x43db, { 0x83, 0xfc, 0x8a, 0xfe, 0xb8, 0xdf, 0x2d, 0xf6 }}

EFI_GUID  mTpmCommandSocketHobGuid  = TPM_COMMAND_SOCKET_HOB_GUID;
EFI_GUID  mTpmPlatformSocketHobGuid = TPM_PLATFORM_SOCKET_HOB_GUID;

VOID *mTpmCommandSocket;
VOID *mTpmPlatformSocket;

VOID *
CreateTpmSocket (
  OUT VOID **CommandSocket,
  OUT VOID **PlatformSocket
  );

VOID
DestroyTpmSocket (
  IN VOID *CommandSocket,
  IN VOID *PlatformSocket
  );

BOOLEAN
CommunicateTpmCommandData (
  IN VOID             *Socket,
  IN UINT8            *SendBuffer,
  IN UINT32           BytesToSend,
  OUT UINT8           *ReceiveBuffer,
  IN OUT UINT32       *BytesToReceive
  );

VOID
GetTpmCommandSocket (
  VOID
  )
{
  EFI_HOB_GUID_TYPE        *GuidHob;

  GuidHob = GetFirstGuidHob (&mTpmCommandSocketHobGuid);
  if (GuidHob == NULL) {
    return ;
  }
  mTpmCommandSocket = (VOID *)(*(UINTN *)(GET_GUID_HOB_DATA (GuidHob)));
  ASSERT (mTpmCommandSocket != NULL);

  GuidHob = GetFirstGuidHob (&mTpmPlatformSocketHobGuid);
  ASSERT (GuidHob != NULL);
  mTpmPlatformSocket = (VOID *)(*(UINTN *)(GET_GUID_HOB_DATA (GuidHob)));
  ASSERT (mTpmPlatformSocket != NULL);
}

RETURN_STATUS
EFIAPI
Tpm2DeviceLibSimulatorConstructor (
  VOID
  )
{
  GetTpmCommandSocket ();
  if (mTpmCommandSocket == NULL) {
    mTpmCommandSocket = CreateTpmSocket (&mTpmCommandSocket, &mTpmPlatformSocket);
    if (mTpmCommandSocket != NULL) {
        BuildGuidDataHob (
          &mTpmCommandSocketHobGuid,
          &mTpmCommandSocket,
          sizeof (VOID *)
          );
        BuildGuidDataHob (
          &mTpmPlatformSocketHobGuid,
          &mTpmPlatformSocket,
          sizeof (VOID *)
          );
    }
  }
  return RETURN_SUCCESS;
}

/**
  This service enables the sending of commands to the TPM2.

  @param[in]      InputParameterBlockSize  Size of the TPM2 input parameter block.
  @param[in]      InputParameterBlock      Pointer to the TPM2 input parameter block.
  @param[in,out]  OutputParameterBlockSize Size of the TPM2 output parameter block.
  @param[in]      OutputParameterBlock     Pointer to the TPM2 output parameter block.

  @retval EFI_SUCCESS            The command byte stream was successfully sent to the device and a response was successfully received.
  @retval EFI_DEVICE_ERROR       The command was not successfully sent to the device or a response was not successfully received from the device.
  @retval EFI_BUFFER_TOO_SMALL   The output parameter block is too small. 
**/
EFI_STATUS
EFIAPI
Tpm2SubmitCommand (
  IN UINT32            InputParameterBlockSize,
  IN UINT8             *InputParameterBlock,
  IN OUT UINT32        *OutputParameterBlockSize,
  IN UINT8             *OutputParameterBlock
  )
{
  TPM2_RESPONSE_HEADER      *Header;
  BOOLEAN                   Result;

  if (mTpmCommandSocket == NULL) {
    GetTpmCommandSocket ();
    if (mTpmCommandSocket == NULL) {
      DEBUG ((EFI_D_ERROR, "TpmCommandSocket - %r\n", EFI_NOT_FOUND));
      return EFI_NOT_FOUND;
    }
  }
  Result = CommunicateTpmCommandData (mTpmCommandSocket, InputParameterBlock, InputParameterBlockSize, OutputParameterBlock, OutputParameterBlockSize);
  if (!Result) {
    return EFI_DEVICE_ERROR;
  }
  if (*OutputParameterBlockSize < sizeof(TPM2_RESPONSE_HEADER)) {
    return EFI_DEVICE_ERROR;
  }
  Header = (TPM2_RESPONSE_HEADER *)OutputParameterBlock;
  *OutputParameterBlockSize = SwapBytes32 (Header->paramSize);

  return EFI_SUCCESS;
}

/**
  This service requests use TPM2.

  @retval EFI_SUCCESS      Get the control of TPM2 chip.
  @retval EFI_NOT_FOUND    TPM2 not found.
  @retval EFI_DEVICE_ERROR Unexpected device behavior.
**/
EFI_STATUS
EFIAPI
Tpm2RequestUseTpm (
  VOID
  )
{
  if (mTpmCommandSocket == NULL) {
    GetTpmCommandSocket ();
    if (mTpmCommandSocket == NULL) {
      DEBUG ((EFI_D_ERROR, "TpmCommandSocket - %r\n", EFI_NOT_FOUND));
      return EFI_NOT_FOUND;
    }
  }
  return EFI_SUCCESS;
}

/**
  This service register TPM2 device.

  @param Tpm2Device  TPM2 device

  @retval EFI_SUCCESS          This TPM2 device is registered successfully.
  @retval EFI_UNSUPPORTED      System does not support register this TPM2 device.
  @retval EFI_ALREADY_STARTED  System already register this TPM2 device.
**/
EFI_STATUS
EFIAPI
Tpm2RegisterTpm2DeviceLib (
  IN TPM2_DEVICE_INTERFACE   *Tpm2Device
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Shutdown TPM2 simulator.
**/
VOID
EFIAPI
Tpm2SimulatorShutdown (
  VOID
  )
{
  if (mTpmCommandSocket == NULL) {
    GetTpmCommandSocket ();
    if (mTpmCommandSocket == NULL) {
      DEBUG ((EFI_D_ERROR, "TpmCommandSocket - %r\n", EFI_NOT_FOUND));
      return ;
    }
  }
  DestroyTpmSocket (mTpmCommandSocket, mTpmPlatformSocket);
}