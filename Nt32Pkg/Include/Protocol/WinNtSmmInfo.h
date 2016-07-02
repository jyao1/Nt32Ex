/**@file

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __WIN_NT_SMM_INFO_H__
#define __WIN_NT_SMM_INFO_H__

//
// This protocal is installed in SMM database
//

#define EFI_WIN_NT_SMM_INFO_GUID \
  { 0x3be064aa, 0x1dba, 0x49fb, { 0xbc, 0x6, 0x78, 0x46, 0xf9, 0x67, 0xd3, 0xaa } }

extern EFI_GUID gEfiWinNtSmmInfoProtocolGuid;

typedef struct _EFI_WIN_NT_SMM_INFO_PROTOCOL  EFI_WIN_NT_SMM_INFO_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_WIN_NT_SMM_GET_INFO)(
  IN CONST EFI_WIN_NT_SMM_INFO_PROTOCOL  *This,
  OUT UINT8                              *CommandPort       OPTIONAL,
  OUT UINT8                              *DataPort          OPTIONAL
  );

typedef
EFI_STATUS
(EFIAPI *EFI_WIN_NT_SMM_SET_INFO)(
  IN CONST EFI_WIN_NT_SMM_INFO_PROTOCOL  *This,
  IN UINT8                               *CommandPort       OPTIONAL,
  IN UINT8                               *DataPort          OPTIONAL
  );

struct _EFI_WIN_NT_SMM_INFO_PROTOCOL {
  EFI_WIN_NT_SMM_GET_INFO    GetInfo;
  EFI_WIN_NT_SMM_SET_INFO    SetInfo;
};

#endif
