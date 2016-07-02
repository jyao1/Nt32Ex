/**@file

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __WIN_NT_SMM_CPU_H__
#define __WIN_NT_SMM_CPU_H__

#include <Protocol/SmmCpu.h>

//
// This protocal is installed in DXE database
//

#define EFI_WIN_NT_SMM_CPU_GUID \
  { 0xa2f5d524, 0xdc38, 0x482c, { 0xbb, 0xfc, 0x27, 0xcf, 0xf3, 0xb4, 0x0, 0xae } }

extern EFI_GUID gEfiWinNtSmmCpuProtocolGuid;

typedef EFI_SMM_CPU_PROTOCOL  EFI_WIN_NT_SMM_CPU_PROTOCOL;

#endif
