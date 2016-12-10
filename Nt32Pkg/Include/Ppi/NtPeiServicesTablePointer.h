/**@file

Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#ifndef __NT_PEI_SERVICES_TABLE_POINTER_H__
#define __NT_PEI_SERVICES_TABLE_POINTER_H__

#define PEI_NT_PEI_SERVICES_TABLE_POINTER_PPI_GUID \
  { 0x42d9ea9, 0x582d, 0x4317, {0xb6, 0xdc, 0xcc, 0xe9, 0x94, 0x7a, 0x96, 0x64} }

typedef
VOID
(EFIAPI *SET_PEI_SERVICES_TABLE_POINTER) (
  IN CONST EFI_PEI_SERVICES ** PeiServicesTablePointer
  );

typedef
CONST EFI_PEI_SERVICES **
(EFIAPI *GET_PEI_SERVICES_TABLE_POINTER) (
  VOID
  );

typedef struct {
  SET_PEI_SERVICES_TABLE_POINTER SetPeiServicesTablePointer;
  GET_PEI_SERVICES_TABLE_POINTER GetPeiServicesTablePointer;
} PEI_NT_PEI_SERVICES_TABLE_POINTER_PPI;

extern EFI_GUID gPeiNtServicesTablePointerPpiGuid;

#endif
