/**@file

Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#ifndef __NT_PEI_RECOVERY_H__
#define __NT_PEI_RECOVERY_H__

#define PEI_NT_RECOVERY_PPI_GUID \
  { 0x708900bb, 0xf44d, 0x49a5, {0x95, 0x61, 0x38, 0x57, 0x90, 0x16, 0x19, 0x2e} }

typedef
EFI_STATUS
(EFIAPI *PEI_NT_IS_RECOVERY_MODE) (
  OUT BOOLEAN  *RecoveryMode
  );

typedef struct {
  PEI_NT_IS_RECOVERY_MODE IsRecoveryMode;
} PEI_NT_RECOVERY_PPI;

extern EFI_GUID gPeiNtRecoveryPpiGuid;

#endif
