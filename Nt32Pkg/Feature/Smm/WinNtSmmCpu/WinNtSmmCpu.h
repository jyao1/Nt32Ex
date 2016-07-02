/** @file

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _WIN_NT_SMM_CPU_H_
#define _WIN_NT_SMM_CPU_H_


#include <Uefi.h>
#include <WinNtDxe.h>

#include <Protocol/SmmConfiguration.h>
#include <Protocol/SmmCpu.h>
#include <Protocol/SmmReadyToLock.h>
#include <Protocol/WinNtSmmTrigger.h>
#include <Protocol/WinNtSmmInfo.h>
#include <Protocol/WinNtSmmCpu.h>

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>

#define  MAXIMUM_SWI_VALUE 0xFF

/*
  EFI_SMM_SAVE_STATE_REGISTER_ES = 20,
  EFI_SMM_SAVE_STATE_REGISTER_DS = 23,
  EFI_SMM_SAVE_STATE_REGISTER_FS = 24,
  EFI_SMM_SAVE_STATE_REGISTER_GS = 25,
  EFI_SMM_SAVE_STATE_REGISTER_R8 = 30,
  EFI_SMM_SAVE_STATE_REGISTER_R9 = 31,
  EFI_SMM_SAVE_STATE_REGISTER_R10 = 32,
  EFI_SMM_SAVE_STATE_REGISTER_R11 = 33,
  EFI_SMM_SAVE_STATE_REGISTER_R12 = 34,
  EFI_SMM_SAVE_STATE_REGISTER_R13 = 35,
  EFI_SMM_SAVE_STATE_REGISTER_R14 = 36,
  EFI_SMM_SAVE_STATE_REGISTER_R15 = 37,
  EFI_SMM_SAVE_STATE_REGISTER_RAX = 38,
  EFI_SMM_SAVE_STATE_REGISTER_RBX = 39,
  EFI_SMM_SAVE_STATE_REGISTER_RCX = 40,
  EFI_SMM_SAVE_STATE_REGISTER_RDX = 41,
  EFI_SMM_SAVE_STATE_REGISTER_RBP = 43,
  EFI_SMM_SAVE_STATE_REGISTER_RSI = 44,
  EFI_SMM_SAVE_STATE_REGISTER_RDI = 45,
  EFI_SMM_SAVE_STATE_REGISTER_RFLAGS = 51,
*/
typedef struct {
  //
  // Only support limited reegister
  //
  UINT32          Es;
  UINT32          Ds;
  UINT32          Fs;
  UINT32          Gs;
  UINT64          R8;
  UINT64          R9;
  UINT64          R10;
  UINT64          R11;
  UINT64          R12;
  UINT64          R13;
  UINT64          R14;
  UINT64          R15;
  UINT64          Rax;
  UINT64          Rbx;
  UINT64          Rcx;
  UINT64          Rdx;
  UINT64          Rbp;
  UINT64          Rsi;
  UINT64          Rdi;
  UINT64          Rflags;
} WIN_NT_SMM_SAVE_STATE;

typedef struct {
  UINT8   Width32;
  UINT8   Width64;
  UINT16  Offset32;
  UINT16  Offset64Lo;
  UINT16  Offset64Hi;
} CPU_SMM_SAVE_STATE_LOOKUP_ENTRY;

#define SMM_CPU_OFFSET(Field) OFFSET_OF (WIN_NT_SMM_SAVE_STATE, Field)

//
// Private structure for the SMM CPU module that is stored in DXE Runtime memory
// Contains the SMM Configuration Protocols that is produced.
// Contains a mix of DXE and SMM contents.  All the fields must be used properly.
//
#define SMM_CPU_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('s', 'c', 'p', 'u')

typedef struct {
  UINTN                           Signature;

  EFI_HANDLE                      SmmCpuHandle;

  UINTN                           CpuSaveStateSize [1];
  VOID                            *CpuSaveState    [1];

  EFI_SMM_RESERVED_SMRAM_REGION   SmmReservedSmramRegion[1];
  EFI_SMM_ENTRY_CONTEXT           SmmCoreEntryContext;
  EFI_SMM_ENTRY_POINT             SmmCoreEntry;

  EFI_SMM_CONFIGURATION_PROTOCOL  SmmConfiguration;

  //
  // Below protocols are installed in DXE database - it should be done by another thread.
  //
  UINT8                           CommandPort;
  UINT8                           DataPort;
  EFI_WIN_NT_SMM_TRIGGER_PROTOCOL SmmTrigger;

  WIN_NT_SMM_SAVE_STATE           WinNtSmmSaveState;
  EFI_WIN_NT_SMM_CPU_PROTOCOL     WinNtSmmCpu;
} SMM_CPU_PRIVATE_DATA;

extern SMM_CPU_PRIVATE_DATA  *gSmmCpuPrivate;

//
// SMM CPU Protocol function prototypes.
//

/**
  Read information from the CPU save state.

  @param  This      EFI_SMM_CPU_PROTOCOL instance
  @param  Widthe    The number of bytes to read from the CPU save state.
  @param  Register  Specifies the CPU register to read form the save state.
  @param  CpuIndex  Specifies the zero-based index of the CPU save state
  @param  Buffer    Upon return, this holds the CPU register value read from the save state.

  @retval EFI_SUCCESS   The register was read from Save State
  @retval EFI_NOT_FOUND The register is not defined for the Save State of Processor  
  @retval EFI_INVALID_PARAMTER   This or Buffer is NULL.

**/
EFI_STATUS
EFIAPI
SmmReadSaveState (
  IN CONST EFI_SMM_CPU_PROTOCOL         *This,
  IN UINTN                              Width,
  IN EFI_SMM_SAVE_STATE_REGISTER        Register,
  IN UINTN                              CpuIndex,
  OUT VOID                              *Buffer
  );

/**
  Write data to the CPU save state.

  @param  This      EFI_SMM_CPU_PROTOCOL instance
  @param  Widthe    The number of bytes to read from the CPU save state.
  @param  Register  Specifies the CPU register to write to the save state.
  @param  CpuIndex  Specifies the zero-based index of the CPU save state
  @param  Buffer    Upon entry, this holds the new CPU register value.

  @retval EFI_SUCCESS   The register was written from Save State
  @retval EFI_NOT_FOUND The register is not defined for the Save State of Processor  
  @retval EFI_INVALID_PARAMTER   ProcessorIndex or Width is not correct

**/
EFI_STATUS
EFIAPI
SmmWriteSaveState (
  IN CONST EFI_SMM_CPU_PROTOCOL         *This,
  IN UINTN                              Width,
  IN EFI_SMM_SAVE_STATE_REGISTER        Register,
  IN UINTN                              CpuIndex,
  IN CONST VOID                         *Buffer
  );

/**

  Register the SMM Foundation entry point.

  @param          This              Pointer to EFI_SMM_CONFIGURATION_PROTOCOL instance
  @param          SmmEntryPoint     SMM Foundation EntryPoint

  @retval         EFI_SUCCESS       Successfully to register SMM foundation entry point

**/
EFI_STATUS
EFIAPI
RegisterSmmEntry (
  IN CONST EFI_SMM_CONFIGURATION_PROTOCOL  *This,
  IN EFI_SMM_ENTRY_POINT                   SmmEntryPoint
  );

/**
  Schedule a procedure to run on the specified CPU.

  @param   Procedure        The address of the procedure to run
  @param   CpuIndex         Target CPU number
  @param   ProcArguments    The parameter to pass to the procedure

  @retval   EFI_INVALID_PARAMETER    CpuNumber not valid
  @retval   EFI_INVALID_PARAMETER    CpuNumber specifying BSP
  @retval   EFI_INVALID_PARAMETER    The AP specified by CpuNumber did not enter SMM
  @retval   EFI_INVALID_PARAMETER    The AP specified by CpuNumber is busy
  @retval   EFI_SUCCESS - The procedure has been successfully scheduled

**/
EFI_STATUS
EFIAPI
SmmStartupThisAp (
  IN      EFI_AP_PROCEDURE          Procedure,
  IN      UINTN                     CpuIndex,
  IN OUT  VOID                      *ProcArguments OPTIONAL
  );

EFI_STATUS
EFIAPI
WinNtSmmGetInfo (
  IN CONST EFI_WIN_NT_SMM_INFO_PROTOCOL  *This,
  OUT UINT8                              *CommandPort       OPTIONAL,
  OUT UINT8                              *DataPort          OPTIONAL
  );

EFI_STATUS
EFIAPI
WinNtSmmSetInfo (
  IN CONST EFI_WIN_NT_SMM_INFO_PROTOCOL  *This,
  IN UINT8                               *CommandPort       OPTIONAL,
  IN UINT8                               *DataPort          OPTIONAL
  );

EFI_STATUS
EFIAPI
WinNtSmmTrigger (
  IN CONST EFI_WIN_NT_SMM_TRIGGER_PROTOCOL  *This,
  IN OUT UINT8                              *CommandPort       OPTIONAL,
  IN OUT UINT8                              *DataPort          OPTIONAL
  );

EFI_STATUS
EFIAPI
WinNtSmmReadSaveState (
  IN CONST EFI_SMM_CPU_PROTOCOL   *This,
  IN UINTN                        Width,
  IN EFI_SMM_SAVE_STATE_REGISTER  Register,
  IN UINTN                        CpuIndex,
  OUT VOID                        *Buffer
  );

EFI_STATUS
EFIAPI
WinNtSmmWriteSaveState (
  IN CONST EFI_SMM_CPU_PROTOCOL   *This,
  IN UINTN                        Width, 
  IN EFI_SMM_SAVE_STATE_REGISTER  Register,
  IN UINTN                        CpuIndex,
  IN CONST VOID                   *Buffer
  );

#endif
