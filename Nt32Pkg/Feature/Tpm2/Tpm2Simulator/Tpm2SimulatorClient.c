/**@file

Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#include <Uefi.h>
#include <PiPei.h>
#include <WinNtPeim.h>
#include <Ppi/NtThunk.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/WinNtLib.h>

#include "Tpm2Simulator.h"

#define IP_ADDRESS "127.0.0.1"

struct  in_addr mIpAddress = {127, 0, 0, 1};
unsigned char   mReceiveBuffer[1024];

VOID
DumpData (
  IN UINT8 *Buffer,
  IN UINTN BufferSize
  )
{
  UINTN Index;

  for (Index = 0; Index < BufferSize; Index++) {
    DEBUG ((DEBUG_INFO, "%02x ", Buffer[Index]));
  }
  DEBUG ((DEBUG_INFO, "\n"));
}

/**
  Read number of bytes data in blocking mode.

  If there is no enough data in socket, this function will wait.
  This function will return if enough data is read, or socket error.
**/
BOOLEAN
ReadBytes (
  IN  SOCKET          Socket,
  OUT UINT8           *Buffer,
  IN  UINT32          NumberOfBytes
  )
{
  INT32                 Result;
  UINT32                NumberReceived;

  NumberReceived = 0;
  while (NumberReceived < NumberOfBytes) {
    Result = gWinNtSocket->recv (Socket, Buffer + NumberReceived, NumberOfBytes - NumberReceived, 0);
    if (Result == -1) {
      DEBUG ((DEBUG_ERROR, "Receive error - 0x%x\n", gWinNtSocket->WSAGetLastError()));
      return FALSE;
    }
    if (Result == 0) {
      return FALSE;
    }
    NumberReceived += Result;
  }
  return TRUE;
}

BOOLEAN
ReadData32 (
  IN SOCKET           Socket,
  OUT UINT32          *Data
  )
{
  BOOLEAN  Result;

  Result = ReadBytes (Socket, (UINT8 *)Data, sizeof(UINT32));
  if (!Result) {
    return Result;
  }
  *Data = SwapBytes32 (*Data);
  return TRUE;
}

/**
  Write number of bytes data in blocking mode.

  This function will return if data is written, or socket error.
**/
BOOLEAN
WriteBytes(
  IN  SOCKET           Socket,
  IN  UINT8            *Buffer,
  IN  UINT32           NumberOfBytes
  )
{
  INT32                Result;
  UINT32               NumberSent;

  NumberSent = 0;
  while (NumberSent < NumberOfBytes) {
    Result = gWinNtSocket->send (Socket, Buffer + NumberSent, NumberOfBytes - NumberSent, 0);
    if (Result == -1) {
      if (gWinNtSocket->WSAGetLastError() == 0x2745) {
        DEBUG ((DEBUG_ERROR, "Client disconnected\n"));
      } else {
        DEBUG ((DEBUG_ERROR, "Send error - 0x%x\n", gWinNtSocket->WSAGetLastError()));
      }
      return FALSE;
    }
    NumberSent += Result;
  }
  return TRUE;
}

BOOLEAN
WriteData32 (
  IN SOCKET           Socket,
  IN UINT32           Data
  )
{
  Data = SwapBytes32(Data);
  return WriteBytes (Socket, (UINT8 *)&Data, sizeof(UINT32));
}

/**
  Read multiple bytes in blocking mode.

  The length is presented as first 4 bytes in big endian.
  The data follows the length.

  If there is no enough data in socket, this function will wait.
  This function will return if enough data is read, or socket error.
**/
BOOLEAN
ReadMultipleBytes (
  IN SOCKET           Socket,
  OUT UINT8           *Buffer,
  OUT UINT32          *BytesReceived,
  IN UINT32           MaxBufferLength
  )
{
  UINT32               Length;
  BOOLEAN              Result;

  Result = ReadData32 (Socket, &Length);
  if (!Result) {
    return Result;
  }
  *BytesReceived = Length;
  if (Length > MaxBufferLength) {
    DEBUG ((DEBUG_ERROR, "Buffer too small. Expected - 0x%x\n", Length));
    return FALSE;
  }
  if (Length == 0) {
    return TRUE;
  }
  Result = ReadBytes (Socket, Buffer, Length);
  if (!Result) {
    return Result;
  }
  return TRUE;
}

/**
  Write multiple bytes.

  The length is presented as first 4 bytes in big endian.
  The data follows the length.
**/
BOOLEAN
WriteMultipleBytes (
  IN SOCKET           Socket,
  IN UINT8            *Buffer,
  IN UINT32           BytesToSend
  )
{
  BOOLEAN  Result;

  Result = WriteData32 (Socket, BytesToSend);
  if (!Result) {
    return Result;
  }
  Result = WriteBytes (Socket, Buffer, BytesToSend);
  if (!Result) {
    return Result;
  }
  return TRUE;
}

BOOLEAN
SendTpmCommand (
  IN SOCKET           Socket,
  IN UINT8            *Buffer,
  IN UINT32           BytesToSend
  )
{
  UINT32   Command;
  UINT8    Locality;
  BOOLEAN  Result;

  Command = TPM_SEND_COMMAND;
  Result = WriteData32 (Socket, Command);
  if (!Result) {
    return Result;
  }
  DEBUG((DEBUG_ERROR, "Command Port Transmit:\n"));
  Command = SwapBytes32(Command);
  DumpData ((UINT8 *)&Command, sizeof(UINT32));

  Locality = 0;
  Result = WriteBytes (Socket, &Locality, sizeof(UINT8));
  if (!Result) {
    return Result;
  }
  DEBUG((DEBUG_ERROR, "Command Port Transmit:\n"));
  DumpData (&Locality, 1);

  Result = WriteMultipleBytes (Socket, Buffer, BytesToSend);
  if (!Result) {
    return Result;
  }
  DEBUG((DEBUG_ERROR, "Command Port Transmit:\n"));
  DumpData (Buffer, BytesToSend);
  return TRUE;
}

BOOLEAN
ReceiveTpmCommand (
  IN  SOCKET           Socket,
  OUT UINT8            *Buffer,
  OUT UINT32           *BytesReceived,
  IN  UINT32           MaxBufferLength
  )
{
  BOOLEAN  Result;
  UINT32   Status;

  Result = ReadMultipleBytes (Socket, Buffer, BytesReceived, MaxBufferLength);
  if (!Result) {
    return Result;
  }
  DEBUG((DEBUG_ERROR, "Command Port Receive:\n"));
  DumpData (Buffer, *BytesReceived);

  Result = ReadData32 (Socket, &Status);
  if (!Result) {
    return Result;
  }
  DEBUG((DEBUG_ERROR, "Command Port Receive:\n"));
  Status = SwapBytes32(Status);
  DumpData ((UINT8 *)&Status, sizeof(UINT32));
  return Result;
}

BOOLEAN
CommunicateTpmCommandData (
  IN VOID             *Socket,
  IN UINT8            *SendBuffer,
  IN UINT32           BytesToSend,
  OUT UINT8           *ReceiveBuffer,
  IN OUT UINT32       *BytesToReceive
  )
{
  SOCKET  CommandSocket;
  BOOLEAN Result;

  CommandSocket = (SOCKET)Socket;
  Result = SendTpmCommand (CommandSocket, SendBuffer, BytesToSend);
  if (!Result) {
    DEBUG((DEBUG_ERROR, "SendTpmCommand Error - %x\n", gWinNtSocket->WSAGetLastError()));
    return Result;
  }
  Result = ReceiveTpmCommand (CommandSocket, ReceiveBuffer, BytesToReceive, *BytesToReceive);
  if (!Result) {
    DEBUG((DEBUG_ERROR, "ReceiveTpmCommand Error - %x\n", gWinNtSocket->WSAGetLastError()));
    return Result;
  }
  return Result;
}

BOOLEAN
SendTpmPlatformData (
  IN SOCKET           Socket,
  IN UINT32           Command,
  IN UINT8            *SendBuffer,
  IN UINT32           BytesToSend
  )
{
  BOOLEAN  Result;

  Result = WriteData32 (Socket, Command);
  if (!Result) {
    return Result;
  }
  DEBUG((DEBUG_ERROR, "Platform Port Transmit:\n"));
  Command = SwapBytes32(Command);
  DumpData ((UINT8 *)&Command, sizeof(UINT32));

  if (BytesToSend != 0) {
    Result = WriteBytes (Socket, SendBuffer, BytesToSend);
    if (!Result) {
      return Result;
    }
    DEBUG((DEBUG_ERROR, "Platform Port Transmit:\n"));
    DumpData (SendBuffer, BytesToSend);
  }

  return TRUE;
}

BOOLEAN
ReceiveTpmPlatformData (
  IN  SOCKET           Socket,
  OUT UINT8            *ReceiveBuffer,
  IN OUT UINT32        *BytesToReceive
  )
{
  BOOLEAN  Result;
  UINT32   Status;

  if (*BytesToReceive != 0) {
    Result = ReadBytes (Socket, ReceiveBuffer, *BytesToReceive);
    if (!Result) {
      return Result;
    }
    DEBUG((DEBUG_ERROR, "Platform Port Receive:\n"));
    DumpData (ReceiveBuffer, *BytesToReceive);
  }
  Result = ReadData32 (Socket, &Status);
  if (!Result) {
    return Result;
  }
  DEBUG((DEBUG_ERROR, "Platform Port Receive:\n"));
  Status = SwapBytes32(Status);
  DumpData ((UINT8 *)&Status, sizeof(UINT32));
  return Result;
}

BOOLEAN
CommunicateTpmPlatformData (
  IN SOCKET           Socket,
  IN UINT32           Command,
  IN UINT8            *SendBuffer,
  IN UINT32           BytesToSend,
  OUT UINT8           *ReceiveBuffer,
  IN OUT UINT32       *BytesToReceive
  )
{
  BOOLEAN Result;

  Result = SendTpmPlatformData (Socket, Command, SendBuffer, BytesToSend);
  if (!Result) {
    DEBUG((DEBUG_ERROR, "SendTpmPlatformData Error - %x\n", gWinNtSocket->WSAGetLastError()));
    return Result;
  }

  Result = ReceiveTpmPlatformData (Socket, ReceiveBuffer, BytesToReceive);
  if (!Result) {
    DEBUG((DEBUG_ERROR, "ReceiveTpmPlatformData Error - %x\n", gWinNtSocket->WSAGetLastError()));
    return Result;
  }
  return Result;
}

BOOLEAN
InitTpmClient (
  OUT SOCKET  *Socket,
  IN  UINT16  Port
  )
{
  SOCKET             ClientSocket;
  struct sockaddr_in ServerAddr;
  INT32              Return;

  ClientSocket = gWinNtSocket->socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (ClientSocket == INVALID_SOCKET) {
    DEBUG ((DEBUG_ERROR, "Create Socket Failed - %x\n", gWinNtSocket->WSAGetLastError()));
    return FALSE;
  }

  ServerAddr.sin_family = AF_INET;
  CopyMem (&ServerAddr.sin_addr.s_addr, &mIpAddress, sizeof(struct in_addr));
  ServerAddr.sin_port = SwapBytes16(Port);
  ZeroMem (ServerAddr.sin_zero, sizeof(ServerAddr.sin_zero));

  Return = gWinNtSocket->connect (ClientSocket, (struct sockaddr *)&ServerAddr, sizeof(ServerAddr));
  if (Return == SOCKET_ERROR) {
    DEBUG ((DEBUG_ERROR, "Connect Error - %x\n", gWinNtSocket->WSAGetLastError()));
    return FALSE;
  }

  DEBUG((DEBUG_ERROR, "connect success!\n"));

  *Socket = ClientSocket;
  return TRUE;
}

BOOLEAN
DeinitTpmClient (
  IN SOCKET  Socket
  )
{
  gWinNtSocket->closesocket (Socket);
  return TRUE;
}

VOID *
CreateTpmSocket (
  OUT VOID **CommandSocket,
  OUT VOID **PlatformSocket
  )
{
  SOCKET  TpmCommandSocket;
  SOCKET  TpmPlatformSocket;
  BOOLEAN Result;
  WSADATA Ws;
  UINT32  ResponseSize;

  if (gWinNtSocket->WSAStartup(MAKEWORD(2,2), &Ws) != 0) {
    DEBUG((DEBUG_ERROR, "Init Windows Socket Failed - %x\n", gWinNtSocket->WSAGetLastError()));
    return NULL;
  }

  Result = InitTpmClient (&TpmPlatformSocket, DEFAULT_TPM_PLATFORM_PORT);
  if (!Result) {
    return NULL;
  }
  Result = InitTpmClient (&TpmCommandSocket, DEFAULT_TPM_COMMAND_PORT);
  if (!Result) {
    return NULL;
  }

  ResponseSize = 0;
  Result = CommunicateTpmPlatformData (TpmPlatformSocket, TPM_SIGNAL_POWER_ON, NULL, 0, NULL, &ResponseSize);
  if (!Result) {
    return NULL;
  }
  ResponseSize = 0;
  Result = CommunicateTpmPlatformData (TpmPlatformSocket, TPM_SIGNAL_NV_ON, NULL, 0, NULL, &ResponseSize);
  if (!Result) {
    return NULL;
  }

  if (CommandSocket != NULL) {
    *CommandSocket = (VOID *)TpmCommandSocket;
  }
  if (PlatformSocket != NULL) {
    *PlatformSocket = (VOID *)TpmPlatformSocket;
  }
  return (VOID *)TpmCommandSocket;
}

VOID
DestroyTpmSocket (
  IN VOID *CommandSocket,
  IN VOID *PlatformSocket
  )
{
  SOCKET  TpmCommandSocket;
  SOCKET  TpmPlatformSocket;
  UINT32  ResponseSize;

  TpmCommandSocket = (SOCKET)CommandSocket;
  TpmPlatformSocket = (SOCKET)PlatformSocket;
  ASSERT (PlatformSocket != NULL);

  ResponseSize = 0;
  CommunicateTpmPlatformData (TpmPlatformSocket, TPM_SIGNAL_NV_OFF, NULL, 0, NULL, &ResponseSize);
  ResponseSize = 0;
  CommunicateTpmPlatformData (TpmPlatformSocket, TPM_SIGNAL_POWER_OFF, NULL, 0, NULL, &ResponseSize);

  DeinitTpmClient (TpmCommandSocket);
  DeinitTpmClient (TpmPlatformSocket);
  gWinNtSocket->WSACleanup();
}
