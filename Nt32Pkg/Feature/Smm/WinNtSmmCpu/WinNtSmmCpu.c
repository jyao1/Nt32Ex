/** @file

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include "WinNtSmmCpu.h"

//
// SMM CPU Private Data structure that contains SMM Configuration Protocol 
// along its supporting fields.
//
SMM_CPU_PRIVATE_DATA  mSmmCpuPrivateData = {
  SMM_CPU_PRIVATE_DATA_SIGNATURE,               // Signature
  NULL,                                         // SmmCpuHandle

  { 0 },                                        // CpuSaveStateSize array
  { NULL },                                     // CpuSaveState array
  { 0 },                                        // SmmReservedSmramRegion
  {
    SmmStartupThisAp,                           // SmmCoreEntryContext.SmmStartupThisAp
    0,                                          // SmmCoreEntryContext.CurrentlyExecutingCpu
    0,                                          // SmmCoreEntryContext.NumberOfCpus
    &mSmmCpuPrivateData.CpuSaveStateSize[0],    // SmmCoreEntryContext.CpuSaveStateSize
    &mSmmCpuPrivateData.CpuSaveState[0]         // SmmCoreEntryContext.CpuSaveState
  },
  NULL,                                         // SmmCoreEntry
  {
    mSmmCpuPrivateData.SmmReservedSmramRegion,  // SmmConfiguration.SmramReservedRegions
    RegisterSmmEntry                            // SmmConfiguration.RegisterSmmEntry
  },

  MAXIMUM_SWI_VALUE,                            // CommandPort
  0xFF,                                         // DataPort
  {
    WinNtSmmTrigger                             // SmmTrigger.Trigger
  },
  { 0 },                                        // WinNtSmmSaveState
  {
    WinNtSmmReadSaveState,                      // WinNtSmmCpu.ReadSaveState
    WinNtSmmWriteSaveState                      // WinNtSmmCpu.WriteSaveState
  },
};

//
// Global pointer used to access mSmmCpuPrivateData from outside and inside SMM
//
SMM_CPU_PRIVATE_DATA  *gSmmCpuPrivate = &mSmmCpuPrivateData;

///
/// Handle for the SMM CPU Protocol
///
EFI_HANDLE  mSmmCpuHandle = NULL;

///
/// SMM CPU Protocol instance
///
EFI_SMM_CPU_PROTOCOL  mSmmCpu  = {
  SmmReadSaveState,
  SmmWriteSaveState
};

EFI_WIN_NT_SMM_INFO_PROTOCOL  mSmmInfo = {
  WinNtSmmGetInfo,
  WinNtSmmSetInfo
};

EFI_SMM_SAVE_STATE_REGISTER  mSupportedSaveStateRegisters[] = {
                               0,  // Reserved
  EFI_SMM_SAVE_STATE_REGISTER_ES,  // = 20,
  EFI_SMM_SAVE_STATE_REGISTER_DS,  // = 23,
  EFI_SMM_SAVE_STATE_REGISTER_FS,  // = 24,
  EFI_SMM_SAVE_STATE_REGISTER_GS,  // = 25,
  EFI_SMM_SAVE_STATE_REGISTER_R8,  // = 30,
  EFI_SMM_SAVE_STATE_REGISTER_R9,  // = 31,
  EFI_SMM_SAVE_STATE_REGISTER_R10, // = 32,
  EFI_SMM_SAVE_STATE_REGISTER_R11, // = 33,
  EFI_SMM_SAVE_STATE_REGISTER_R12, // = 34,
  EFI_SMM_SAVE_STATE_REGISTER_R13, // = 35,
  EFI_SMM_SAVE_STATE_REGISTER_R14, // = 36,
  EFI_SMM_SAVE_STATE_REGISTER_R15, // = 37,
  EFI_SMM_SAVE_STATE_REGISTER_RAX, // = 38,
  EFI_SMM_SAVE_STATE_REGISTER_RBX, // = 39,
  EFI_SMM_SAVE_STATE_REGISTER_RCX, // = 40,
  EFI_SMM_SAVE_STATE_REGISTER_RDX, // = 41,
  EFI_SMM_SAVE_STATE_REGISTER_RBP, // = 43,
  EFI_SMM_SAVE_STATE_REGISTER_RSI, // = 44,
  EFI_SMM_SAVE_STATE_REGISTER_RDI, // = 45,
  EFI_SMM_SAVE_STATE_REGISTER_RFLAGS, // = 51,
};

CPU_SMM_SAVE_STATE_LOOKUP_ENTRY mSmmCpuWidthOffset[] = {
  {0, 0,                   0,                   0,                       0}, // Reserved
  {4, 4,  SMM_CPU_OFFSET(Es),  SMM_CPU_OFFSET(Es),                       0}, // EFI_SMM_SAVE_STATE_REGISTER_ES = 20,
  {4, 4,  SMM_CPU_OFFSET(Ds),  SMM_CPU_OFFSET(Ds),                       0}, // EFI_SMM_SAVE_STATE_REGISTER_DS = 23,
  {4, 4,  SMM_CPU_OFFSET(Fs),  SMM_CPU_OFFSET(Fs),                       0}, // EFI_SMM_SAVE_STATE_REGISTER_FS = 24,
  {4, 4,  SMM_CPU_OFFSET(Gs),  SMM_CPU_OFFSET(Gs),                       0}, // EFI_SMM_SAVE_STATE_REGISTER_GS = 25,
  {0, 8,                  0,   SMM_CPU_OFFSET(R8),  SMM_CPU_OFFSET(R8) + 4}, // EFI_SMM_SAVE_STATE_REGISTER_R8 = 30,
  {0, 8,                  0,   SMM_CPU_OFFSET(R9),  SMM_CPU_OFFSET(R9) + 4}, // EFI_SMM_SAVE_STATE_REGISTER_R9 = 31,
  {0, 8,                  0,  SMM_CPU_OFFSET(R10), SMM_CPU_OFFSET(R10) + 4}, // EFI_SMM_SAVE_STATE_REGISTER_R10 = 32,
  {0, 8,                  0,  SMM_CPU_OFFSET(R11), SMM_CPU_OFFSET(R11) + 4}, // EFI_SMM_SAVE_STATE_REGISTER_R11 = 33,
  {0, 8,                  0,  SMM_CPU_OFFSET(R12), SMM_CPU_OFFSET(R12) + 4}, // EFI_SMM_SAVE_STATE_REGISTER_R12 = 34,
  {0, 8,                  0,  SMM_CPU_OFFSET(R13), SMM_CPU_OFFSET(R13) + 4}, // EFI_SMM_SAVE_STATE_REGISTER_R13 = 35,
  {0, 8,                  0,  SMM_CPU_OFFSET(R14), SMM_CPU_OFFSET(R14) + 4}, // EFI_SMM_SAVE_STATE_REGISTER_R14 = 36,
  {0, 8,                  0,  SMM_CPU_OFFSET(R15), SMM_CPU_OFFSET(R15) + 4}, // EFI_SMM_SAVE_STATE_REGISTER_R15 = 37,
  {4, 8, SMM_CPU_OFFSET(Rax), SMM_CPU_OFFSET(Rax), SMM_CPU_OFFSET(Rax) + 4}, // EFI_SMM_SAVE_STATE_REGISTER_RAX = 38,
  {4, 8, SMM_CPU_OFFSET(Rbx), SMM_CPU_OFFSET(Rbx), SMM_CPU_OFFSET(Rbx) + 4}, // EFI_SMM_SAVE_STATE_REGISTER_RBX = 39,
  {4, 8, SMM_CPU_OFFSET(Rcx), SMM_CPU_OFFSET(Rcx), SMM_CPU_OFFSET(Rcx) + 4}, // EFI_SMM_SAVE_STATE_REGISTER_RCX = 40,
  {4, 8, SMM_CPU_OFFSET(Rdx), SMM_CPU_OFFSET(Rdx), SMM_CPU_OFFSET(Rdx) + 4}, // EFI_SMM_SAVE_STATE_REGISTER_RDX = 41,
  {4, 8, SMM_CPU_OFFSET(Rbp), SMM_CPU_OFFSET(Rbp), SMM_CPU_OFFSET(Rbp) + 4}, // EFI_SMM_SAVE_STATE_REGISTER_RBP = 43,
  {4, 8, SMM_CPU_OFFSET(Rsi), SMM_CPU_OFFSET(Rsi), SMM_CPU_OFFSET(Rsi) + 4}, // EFI_SMM_SAVE_STATE_REGISTER_RSI = 44,
  {4, 8, SMM_CPU_OFFSET(Rdi), SMM_CPU_OFFSET(Rdi), SMM_CPU_OFFSET(Rdi) + 4}, // EFI_SMM_SAVE_STATE_REGISTER_RDI = 45,
  {4, 8, SMM_CPU_OFFSET(Rflags), SMM_CPU_OFFSET(Rflags), SMM_CPU_OFFSET(Rflags) + 4}, // EFI_SMM_SAVE_STATE_REGISTER_RFLAGS = 51,
};

/**
  Read information from the CPU save state.

  @param  Register  Specifies the CPU register to read form the save state.

  @retval 0   Register is not valid
  @retval >0  Index into mSmmCpuWidthOffset[] associated with Register

**/
UINTN
GetRegisterIndex (
  IN EFI_SMM_SAVE_STATE_REGISTER  Register
)
{
  UINTN  Index;
  
  for (Index = 0; Index < sizeof(mSupportedSaveStateRegisters)/sizeof(mSupportedSaveStateRegisters[0]); Index++) {
    if (Register == mSupportedSaveStateRegisters[Index]) {
      return Index;
    }
  }
  return 0;
}


/**
  SMM Ready To Lock event notification handler.

  The CPU S3 data is copied to SMRAM for security and mSmmReadyToLock is set to
  perform additional lock actions that must be performed from SMM on the next SMI.

  @param[in] Protocol   Points to the protocol's unique identifier.
  @param[in] Interface  Points to the interface instance.
  @param[in] Handle     The handle on which the interface was installed.

  @retval EFI_SUCCESS   Notification handler runs successfully.
 **/
EFI_STATUS
EFIAPI
SmmReadyToLockEventNotify (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  return EFI_SUCCESS;
}

/**
  The user Entry Point for module WinNtGop. The user code starts with this function.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
InitializeWinNtSmmCpu (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS   Status;
  VOID         *Registration;

  //
  // compute CPU Save State address, and CPU Save state 
  //
  gSmmCpuPrivate->CpuSaveStateSize[0] = sizeof(WIN_NT_SMM_SAVE_STATE);
  gSmmCpuPrivate->CpuSaveState[0]     = &gSmmCpuPrivate->WinNtSmmSaveState;

  //
  // Fill in SMM Reserved Regions
  //
  gSmmCpuPrivate->SmmReservedSmramRegion[0].SmramReservedStart = 0;
  gSmmCpuPrivate->SmmReservedSmramRegion[0].SmramReservedSize  = 0;

  //
  // Install the SMM Configuration Protocol onto a new handle on the handle database.
  // The entire SMM Configuration Protocol is allocated from SMRAM, so only a pointer
  // to an SMRAM address will be present in the handle database 
  //
  Status = SystemTable->BootServices->InstallMultipleProtocolInterfaces (
                                        &gSmmCpuPrivate->SmmCpuHandle,
                                        &gEfiSmmConfigurationProtocolGuid, &gSmmCpuPrivate->SmmConfiguration,
                                        NULL
                                        );
  ASSERT_EFI_ERROR (Status);

  Status = SystemTable->BootServices->InstallMultipleProtocolInterfaces (
                                        &gSmmCpuPrivate->SmmCpuHandle,
                                        &gEfiWinNtSmmTriggerProtocolGuid, &gSmmCpuPrivate->SmmTrigger,
                                        NULL
                                        );
  ASSERT_EFI_ERROR (Status);

  Status = SystemTable->BootServices->InstallMultipleProtocolInterfaces (
                                        &gSmmCpuPrivate->SmmCpuHandle,
                                        &gEfiWinNtSmmCpuProtocolGuid, &gSmmCpuPrivate->WinNtSmmCpu,
                                        NULL
                                        );
  ASSERT_EFI_ERROR (Status);

  //
  // Install the SMM CPU Protocol into SMM protocol database
  //
  Status = gSmst->SmmInstallProtocolInterface (
                    &mSmmCpuHandle,
                    &gEfiSmmCpuProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mSmmCpu
                    );
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmInstallProtocolInterface (
                    &mSmmCpuHandle,
                    &gEfiWinNtSmmInfoProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mSmmInfo
                    );
  ASSERT_EFI_ERROR (Status);
  
  //
  // register SMM Ready To Lock Protocol notification
  //
  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEfiSmmReadyToLockProtocolGuid,
                    SmmReadyToLockEventNotify,
                    &Registration
                    );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

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
  )
{
  WIN_NT_SMM_SAVE_STATE                *StateCopy;
  EFI_SMM_SAVE_STATE_IO_INFO           *IoInfo;
  UINTN                                RegisterIndex;

  if (CpuIndex != 0) {
    return EFI_INVALID_PARAMETER;
  }
  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  StateCopy = gSmmCpuPrivate->CpuSaveState[CpuIndex];

  //
  // Check for special EFI_SMM_SAVE_STATE_REGISTER_LMA
  //
  if (Register == EFI_SMM_SAVE_STATE_REGISTER_LMA) {
    //
    // Only byte access is supported for this register
    //
    if (Width != 1) {
      return EFI_INVALID_PARAMETER;          
    }

    if (sizeof(UINTN) == sizeof(UINT32)) {
      *(UINT8 *)Buffer = EFI_SMM_SAVE_STATE_REGISTER_LMA_32BIT;
    } else {
      *(UINT8 *)Buffer = EFI_SMM_SAVE_STATE_REGISTER_LMA_64BIT;
    }
    
    return EFI_SUCCESS;
  }

  //
  // Check for special EFI_SMM_SAVE_STATE_REGISTER_IO
  //
  if (Register == EFI_SMM_SAVE_STATE_REGISTER_IO) {
    if (Width != sizeof(EFI_SMM_SAVE_STATE_IO_INFO)) {
      return EFI_INVALID_PARAMETER;
    }

    if (gSmmCpuPrivate->CommandPort == MAXIMUM_SWI_VALUE) {
      return EFI_NOT_FOUND;
    }

    //
    // Zero the IoInfo structure that will be returned in Buffer
    //
    IoInfo = (EFI_SMM_SAVE_STATE_IO_INFO *)Buffer;
    ZeroMem (IoInfo, sizeof (EFI_SMM_SAVE_STATE_IO_INFO));

    IoInfo->IoPort  = 0xB2;
    IoInfo->IoWidth = sizeof(gSmmCpuPrivate->CommandPort);
    IoInfo->IoType  = EFI_SMM_SAVE_STATE_IO_TYPE_OUTPUT;
    IoInfo->IoData  = gSmmCpuPrivate->CommandPort;
  }

  //
  // Convert Register to a register lookup table index
  //
  RegisterIndex = GetRegisterIndex (Register);
  if (RegisterIndex == 0) {
    return EFI_NOT_FOUND;
  }

  if (sizeof(UINTN) == sizeof(UINT32)) {
    //
    // If 32-bit mode width is zero, then the specified register can not be accessed
    //
    if (mSmmCpuWidthOffset[RegisterIndex].Width32 == 0) {
      return EFI_NOT_FOUND;
    }
    
    //
    // If Width is bigger than the 32-bit mode width, then the specified register can not be accessed
    //
    if (Width > mSmmCpuWidthOffset[RegisterIndex].Width32) {
      return EFI_INVALID_PARAMETER;          
    }

    //
    // Write return buffer
    //
    CopyMem(Buffer, (UINT8 *)StateCopy + mSmmCpuWidthOffset[RegisterIndex].Offset32, Width);
  } else {
    //
    // If 64-bit mode width is zero, then the specified register can not be accessed
    //
    if (mSmmCpuWidthOffset[RegisterIndex].Width64 == 0) {
      return EFI_NOT_FOUND;
    }
    
    //
    // If Width is bigger than the 64-bit mode width, then the specified register can not be accessed
    //
    if (Width > mSmmCpuWidthOffset[RegisterIndex].Width64) {
      return EFI_INVALID_PARAMETER;          
    }
    
    //
    // Write lower 32-bits of return buffer
    //
    CopyMem(Buffer, (UINT8 *)StateCopy + mSmmCpuWidthOffset[RegisterIndex].Offset64Lo, MIN (4, Width));
    if (Width >= 4) {
      //
      // Write upper 32-bits of return buffer
      //
      CopyMem((UINT8 *)Buffer + 4, (UINT8 *)StateCopy + mSmmCpuWidthOffset[RegisterIndex].Offset64Hi, Width - 4);
    }
  }

  return EFI_SUCCESS;
}

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
  )
{
  WIN_NT_SMM_SAVE_STATE                *StateCopy;
  UINTN                                RegisterIndex;

  if (CpuIndex != 0) {
    return EFI_INVALID_PARAMETER;
  }
  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  StateCopy = gSmmCpuPrivate->CpuSaveState[CpuIndex];

  //
  // Writes to EFI_SMM_SAVE_STATE_REGISTER_LMA are ignored
  //
  if (Register == EFI_SMM_SAVE_STATE_REGISTER_LMA) {
    return EFI_SUCCESS;
  }

  //
  // Writes to EFI_SMM_SAVE_STATE_REGISTER_IO are not supported
  //
  if (Register == EFI_SMM_SAVE_STATE_REGISTER_IO) {
    return EFI_NOT_FOUND;          
  }

  //
  // Convert Register to a register lookup table index
  //
  RegisterIndex = GetRegisterIndex (Register);
  if (RegisterIndex == 0) {
    //
    // If Register is not valie, then return EFI_NOT_FOUND
    //
    return EFI_NOT_FOUND;
  }

  if (sizeof(UINTN) == sizeof(UINT32)) {
    //
    // If 32-bit mode width is zero, then the specified register can not be accessed
    //
    if (mSmmCpuWidthOffset[RegisterIndex].Width32 == 0) {
      return EFI_NOT_FOUND;
    }
    
    //
    // If Width is bigger than the 32-bit mode width, then the specified register can not be accessed
    //
    if (Width > mSmmCpuWidthOffset[RegisterIndex].Width32) {
      return EFI_INVALID_PARAMETER;          
    }

    //
    // Write SMM State register
    //
    CopyMem((UINT8 *)StateCopy + mSmmCpuWidthOffset[RegisterIndex].Offset32, Buffer, Width);
  } else {
    //
    // If 64-bit mode width is zero, then the specified register can not be accessed
    //
    if (mSmmCpuWidthOffset[RegisterIndex].Width64 == 0) {
      return EFI_NOT_FOUND;
    }
    
    //
    // If Width is bigger than the 64-bit mode width, then the specified register can not be accessed
    //
    if (Width > mSmmCpuWidthOffset[RegisterIndex].Width64) {
      return EFI_INVALID_PARAMETER;          
    }
    
    //
    // Write lower 32-bits of SMM State register
    //
    CopyMem((UINT8 *)StateCopy + mSmmCpuWidthOffset[RegisterIndex].Offset64Lo, Buffer, MIN (4, Width));
    if (Width >= 4) {
      //
      // Write upper 32-bits of SMM State register
      //
      CopyMem((UINT8 *)StateCopy + mSmmCpuWidthOffset[RegisterIndex].Offset64Hi, (UINT8 *)Buffer + 4, Width - 4);
    }
  }

  return EFI_SUCCESS;
}

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
  )
{
  //
  // Record SMM Foundation EntryPoint, later invoke it on SMI entry vector.
  //
  gSmmCpuPrivate->SmmCoreEntry = SmmEntryPoint;
  return EFI_SUCCESS;
}

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
  )
{
  return EFI_INVALID_PARAMETER;
}

EFI_STATUS
EFIAPI
WinNtSmmGetInfo (
  IN CONST EFI_WIN_NT_SMM_INFO_PROTOCOL  *This,
  OUT UINT8                              *CommandPort       OPTIONAL,
  OUT UINT8                              *DataPort          OPTIONAL
  )
{
  if (CommandPort != NULL) {
    *CommandPort = gSmmCpuPrivate->CommandPort;
  }
  if (DataPort != NULL) {
    *DataPort = gSmmCpuPrivate->DataPort;
  }
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
WinNtSmmSetInfo (
  IN CONST EFI_WIN_NT_SMM_INFO_PROTOCOL  *This,
  IN UINT8                               *CommandPort       OPTIONAL,
  IN UINT8                               *DataPort          OPTIONAL
  )
{
  if (CommandPort != NULL) {
    gSmmCpuPrivate->CommandPort = *CommandPort;
  } else {
    gSmmCpuPrivate->CommandPort = MAXIMUM_SWI_VALUE;
  }
  if (DataPort != NULL) {
    gSmmCpuPrivate->DataPort = *DataPort;
  }
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
WinNtSmmTrigger (
  IN CONST EFI_WIN_NT_SMM_TRIGGER_PROTOCOL  *This,
  IN OUT UINT8                              *CommandPort       OPTIONAL,
  IN OUT UINT8                              *DataPort          OPTIONAL
  )
{
  WinNtSmmSetInfo (NULL, CommandPort, DataPort);

  //
  // Invoke SMM Foundation EntryPoint with the processor information context.
  //
  gSmmCpuPrivate->SmmCoreEntry (&gSmmCpuPrivate->SmmCoreEntryContext);

  WinNtSmmGetInfo (NULL, CommandPort, DataPort);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
WinNtSmmReadSaveState (
  IN CONST EFI_SMM_CPU_PROTOCOL   *This,
  IN UINTN                        Width,
  IN EFI_SMM_SAVE_STATE_REGISTER  Register,
  IN UINTN                        CpuIndex,
  OUT VOID                        *Buffer
  )
{
  return SmmReadSaveState (This, Width, Register, CpuIndex, Buffer);
}

EFI_STATUS
EFIAPI
WinNtSmmWriteSaveState (
  IN CONST EFI_SMM_CPU_PROTOCOL   *This,
  IN UINTN                        Width, 
  IN EFI_SMM_SAVE_STATE_REGISTER  Register,
  IN UINTN                        CpuIndex,
  IN CONST VOID                   *Buffer
  )
{
  return SmmWriteSaveState (This, Width, Register, CpuIndex, Buffer);
}
