/**@file

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __WIN_NT_SMM_TRIGGER_H__
#define __WIN_NT_SMM_TRIGGER_H__

//
// This protocal is installed in DXE database
//

#define EFI_WIN_NT_SMM_TRIGGER_GUID \
  { 0x1b365c66, 0xc61c, 0x4ca3, { 0xa9, 0x44, 0x41, 0x78, 0xe7, 0x39, 0x1f, 0x20 } }

extern EFI_GUID gEfiWinNtSmmTriggerProtocolGuid;

typedef struct _EFI_WIN_NT_SMM_TRIGGER_PROTOCOL  EFI_WIN_NT_SMM_TRIGGER_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_WIN_NT_SMM_TRIGGER)(
  IN CONST EFI_WIN_NT_SMM_TRIGGER_PROTOCOL  *This,
  IN OUT UINT8                              *CommandPort       OPTIONAL,
  IN OUT UINT8                              *DataPort          OPTIONAL
  );

struct _EFI_WIN_NT_SMM_TRIGGER_PROTOCOL {
  EFI_WIN_NT_SMM_TRIGGER    Trigger;
};

#endif
