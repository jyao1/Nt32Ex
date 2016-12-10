/**@file
  Setup Variable data structure for NT32 platform.

Copyright (c) 2009, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             


**/

#ifndef __WIN_NT_MEMORY_LAYOUT_H__
#define __WIN_NT_MEMORY_LAYOUT_H__

#define EFI_WIN_NT_MEMORY_LAYOUT_GUID  \
  { 0xf53cf8a1, 0xa3cc, 0x4d59, { 0x9b, 0x15, 0xed, 0xa, 0x6b, 0x2f, 0x78, 0x12 }}

typedef struct {
  EFI_PHYSICAL_ADDRESS  Base;
  UINT64                Size;
} EFI_WIN_NT_MEMORY_DESCRIPTOR;

typedef struct {
  UINT32                        NumberOfRegions;
  EFI_WIN_NT_MEMORY_DESCRIPTOR  Descriptor[1];
} EFI_WIN_NT_MEMORY_LAYOUT;

extern EFI_GUID   gEfiWinNtMemoryLayoutGuid;

#endif
