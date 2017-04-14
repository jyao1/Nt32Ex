## @file
# EFI/Framework Emulation Platform with UEFI HII interface supported.
#
# The Emulation Platform can be used to debug individual modules, prior to creating
#    a real platform. This also provides an example for how an DSC is created.
#
# Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
# Copyright (c) 2015, Hewlett-Packard Development Company, L.P.<BR>
# (C) Copyright 2016 Hewlett Packard Enterprise Development LP<BR>
#
#    This program and the accompanying materials
#    are licensed and made available under the terms and conditions of the BSD License
#    which accompanies this distribution. The full text of the license may be found at
#    http://opensource.org/licenses/bsd-license.php
#
#    THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#    WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = NT32
  PLATFORM_GUID                  = EB216561-961F-47EE-9EF9-CA426EF547C2
  PLATFORM_VERSION               = 0.4
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/NT32$(ARCH)
  SUPPORTED_ARCHITECTURES        = IA32|X64
  BUILD_TARGETS                  = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER               = ALL
  FLASH_DEFINITION               = Nt32Pkg/Nt32Pkg.fdf
  #
  # This flag is to control tool to generate PCD info for dynamic(ex) PCD,
  # then enable or disable PCD info feature. TRUE is enable, and FLASE is disable.
  # If the flag is absent, it will be same as FALSE.
  #
  PCD_INFO_GENERATION            = TRUE

  #
  # Defines for default states.  These can be changed on the command line.
  # -D FLAG=VALUE
  #
  # Note: Secure Boot feature highly depends on the OpenSSL building. To enable this 
  #       feature, please follow the instructions found in the file "Patch-HOWTO.txt" 
  #       located in CryptoPkg\Library\OpensslLib to enable the OpenSSL building first.
  #
  DEFINE SECURE_BOOT_ENABLE      = FALSE
  
  #
  # This flag is to enable or disable TLS feature.  
  # These can be changed on the command line.
  # -D FLAG=VALUE
  #
  # Note: TLS feature highly depends on the OpenSSL building. To enable this 
  #       feature, please follow the instructions found in the file "Patch-HOWTO.txt" 
  #       located in CryptoPkg\Library\OpensslLib to enable the OpenSSL building first.
  #
  DEFINE TLS_ENABLE = FALSE
  
  #
  # Indicates whether HTTP connections (i.e., unsecured) are permitted or not.
  # -D FLAG=VALUE
  #
  # Note: If ALLOW_HTTP_CONNECTIONS is TRUE, HTTP connections are allowed. Both 
  #       the "https://" and "http://" URI schemes are permitted. Otherwise, HTTP 
  #       connections are denied. Only the "https://" URI scheme is permitted.
  #
  DEFINE ALLOW_HTTP_CONNECTIONS = TRUE

  #
  # This flag is to enable or disable IPv6 network stack.
  # These can be changed on the command line.
  # -D FLAG=VALUE
  #
  DEFINE NETWORK_IP6_ENABLE = FALSE

  DEFINE SMM_ENABLE              = TRUE
  DEFINE CAPSULE_ENABLE          = TRUE
  DEFINE RECOVERY_ENABLE         = TRUE
  DEFINE MICROCODE_UPDATE_ENABLE = TRUE
  DEFINE TPM2_ENABLE             = TRUE
  DEFINE NETWORK_ENABLE          = FALSE
  DEFINE PERF_ENABLE             = FALSE
  DEFINE PROFILE_ENABLE          = FALSE
  DEFINE SMI_HANDLER_PROFILE_ENABLE = FALSE
  DEFINE SHELL_BUILD_ENABLE      = FALSE

#  DEFINE DSC_GLOBAL_BUILD_OPTIONS = /D EFI_SPECIFICATION_VERSION=0x00020000  /D PI_SPECIFICATION_VERSION=0x00000009  /D TIANO_RELEASE_VERSION=0x00080006 /D EFI_DEBUG /D EFI_FIRMWARE_VENDOR="L\"INTEL\"" /D EFI_BUILD_VERSION="L\"EDKII\""

################################################################################
#
# SKU Identification section - list of all SKU IDs supported by this
#                              Platform.
#
################################################################################
[SkuIds]
  0|DEFAULT              # The entry: 0|DEFAULT is reserved and always required.

################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################
[LibraryClasses]
  #
  # Entry point
  #
  PeiCoreEntryPoint|MdePkg/Library/PeiCoreEntryPoint/PeiCoreEntryPoint.inf
  PeimEntryPoint|MdePkg/Library/PeimEntryPoint/PeimEntryPoint.inf
  DxeCoreEntryPoint|MdePkg/Library/DxeCoreEntryPoint/DxeCoreEntryPoint.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  #
  # Basic
  #
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  CpuLib|MdePkg/Library/BaseCpuLib/BaseCpuLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
  PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf
  PciExpressLib|MdePkg/Library/BasePciExpressLib/BasePciExpressLib.inf
  CacheMaintenanceLib|MdePkg/Library/BaseCacheMaintenanceLib/BaseCacheMaintenanceLib.inf
  PeCoffLib|MdePkg/Library/BasePeCoffLib/BasePeCoffLib.inf
  PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf
  #
  # UEFI & PI
  #
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLibDevicePathProtocol/UefiDevicePathLibDevicePathProtocol.inf
  UefiDecompressLib|IntelFrameworkModulePkg/Library/BaseUefiTianoCustomDecompressLib/BaseUefiTianoCustomDecompressLib.inf
  PeiServicesTablePointerLib|Nt32Pkg/Library/PeiServicesTablePointerLibNt32/PeiServicesTablePointerLibNt32.inf
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  UefiBootManagerLib|MdeModulePkg/Library/UefiBootManagerLib/UefiBootManagerLib.inf
  FileExplorerLib|MdeModulePkg/Library/FileExplorerLib/FileExplorerLib.inf

  #
  # Generic Modules
  #
  UefiUsbLib|MdePkg/Library/UefiUsbLib/UefiUsbLib.inf
  UefiScsiLib|MdePkg/Library/UefiScsiLib/UefiScsiLib.inf
  NetLib|MdeModulePkg/Library/DxeNetLib/DxeNetLib.inf
  IpIoLib|MdeModulePkg/Library/DxeIpIoLib/DxeIpIoLib.inf
  UdpIoLib|MdeModulePkg/Library/DxeUdpIoLib/DxeUdpIoLib.inf
  TcpIoLib|MdeModulePkg/Library/DxeTcpIoLib/DxeTcpIoLib.inf
  HttpLib|MdeModulePkg/Library/DxeHttpLib/DxeHttpLib.inf
  DpcLib|MdeModulePkg/Library/DxeDpcLib/DxeDpcLib.inf
  OemHookStatusCodeLib|MdeModulePkg/Library/OemHookStatusCodeLibNull/OemHookStatusCodeLibNull.inf
  CustomizedDisplayLib|MdeModulePkg/Library/CustomizedDisplayLib/CustomizedDisplayLib.inf
  SecurityManagementLib|MdeModulePkg/Library/DxeSecurityManagementLib/DxeSecurityManagementLib.inf
!if $(PERF_ENABLE) == TRUE
  TimerLib|Nt32Pkg/Library/TscTimerLib/TscTimerLib.inf
!else
  TimerLib|MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
!endif

!if $(SMI_HANDLER_PROFILE_ENABLE) == TRUE
  SmiHandlerProfileLib|MdeModulePkg/Library/SmmSmiHandlerProfileLib/SmmSmiHandlerProfileLib.inf
!else
  SmiHandlerProfileLib|MdePkg/Library/SmiHandlerProfileLibNull/SmiHandlerProfileLibNull.inf
!endif

  SerialPortLib|MdePkg/Library/BaseSerialPortLibNull/BaseSerialPortLibNull.inf
!if $(CAPSULE_ENABLE)
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibFmp/DxeCapsuleLib.inf
!else
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
!endif
  BootLogoLib|MdeModulePkg/Library/BootLogoLib/BootLogoLib.inf
  #
  # Platform
  #
  PlatformBootManagerLib|Nt32Pkg/Library/PlatformBootManagerLib/PlatformBootManagerLib.inf
  #
  # Misc
  #
  DebugLib|MdeModulePkg/Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  DebugPrintErrorLevelLib|MdeModulePkg/Library/DxeDebugPrintErrorLevelLib/DxeDebugPrintErrorLevelLib.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf
  CpuExceptionHandlerLib|MdeModulePkg/Library/CpuExceptionHandlerLibNull/CpuExceptionHandlerLibNull.inf
  LockBoxLib|MdeModulePkg/Library/LockBoxNullLib/LockBoxNullLib.inf

  IntrinsicLib|CryptoPkg/Library/IntrinsicLib/IntrinsicLib.inf
!if $(TLS_ENABLE) == TRUE
  OpensslLib|CryptoPkg/Library/OpensslLib/OpensslLib.inf
!else
  OpensslLib|CryptoPkg/Library/OpensslLib/OpensslLibCrypto.inf
!endif
  RngLib|MdePkg/Library/BaseRngLib/BaseRngLib.inf
  
!if $(SECURE_BOOT_ENABLE) == TRUE
  PlatformSecureLib|Nt32Pkg/Library/PlatformSecureLib/PlatformSecureLib.inf
  TpmMeasurementLib|SecurityPkg/Library/DxeTpmMeasurementLib/DxeTpmMeasurementLib.inf
  AuthVariableLib|SecurityPkg/Library/AuthVariableLib/AuthVariableLib.inf
!else
  TpmMeasurementLib|MdeModulePkg/Library/TpmMeasurementLibNull/TpmMeasurementLibNull.inf
  AuthVariableLib|MdeModulePkg/Library/AuthVariableLibNull/AuthVariableLibNull.inf
!endif
  VarCheckLib|MdeModulePkg/Library/VarCheckLib/VarCheckLib.inf

!if $(CAPSULE_ENABLE) || $(RECOVERY_ENABLE)
  IniParsingLib|SignedCapsulePkg/Library/IniParsingLib/IniParsingLib.inf
  EdkiiSystemCapsuleLib|SignedCapsulePkg/Library/EdkiiSystemCapsuleLib/EdkiiSystemCapsuleLib.inf
!endif

!if $(CAPSULE_ENABLE)
  PlatformFlashAccessLib|Nt32Pkg/Feature/Capsule/Library/PlatformFlashAccessLib/PlatformFlashAccessLib.inf
  MicrocodeFlashAccessLib|Nt32Pkg/Feature/Capsule/Library/PlatformFlashAccessLib/PlatformFlashAccessLib.inf
!endif

  Tpm2CommandLib|SecurityPkg/Library/Tpm2CommandLib/Tpm2CommandLib.inf
  Tcg2PpVendorLib|SecurityPkg/Library/Tcg2PpVendorLibNull/Tcg2PpVendorLibNull.inf
  ResetSystemLib|Nt32Pkg/Library/ResetSystemLibWinNt/ResetSystemLibWinNt.inf

[LibraryClasses.common.USER_DEFINED]
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  PeCoffExtraActionLib|MdePkg/Library/BasePeCoffExtraActionLibNull/BasePeCoffExtraActionLibNull.inf
  ReportStatusCodeLib|MdeModulePkg/Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf
  OemHookStatusCodeLib|Nt32Pkg/Library/PeiNt32OemHookStatusCodeLib/PeiNt32OemHookStatusCodeLib.inf
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf

[LibraryClasses.common.PEIM,LibraryClasses.common.PEI_CORE]
  #
  # PEI phase common
  #
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/PeiExtractGuidedSectionLib/PeiExtractGuidedSectionLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibOptPei/BaseMemoryLibOptPei.inf
  IoLib|MdePkg/Library/PeiIoLibCpuIo/PeiIoLibCpuIo.inf
  PeCoffGetEntryPointLib|Nt32Pkg/Library/Nt32PeiPeCoffGetEntryPointLib/Nt32PeiPeCoffGetEntryPointLib.inf
  PeCoffExtraActionLib|Nt32Pkg/Library/PeiNt32PeCoffExtraActionLib/PeiNt32PeCoffExtraActionLib.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
!if $(PERF_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/PeiPerformanceLib/PeiPerformanceLib.inf
!endif

[LibraryClasses.common.PEI_CORE]
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  OemHookStatusCodeLib|MdeModulePkg/Library/OemHookStatusCodeLibNull/OemHookStatusCodeLibNull.inf

[LibraryClasses.common.PEIM]
  PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  OemHookStatusCodeLib|Nt32Pkg/Library/PeiNt32OemHookStatusCodeLib/PeiNt32OemHookStatusCodeLib.inf
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/PeiCryptLib.inf
  WinNtLib|Nt32Pkg/Library/PeiWinNtLib/PeiWinNtLib.inf
  HashLib|SecurityPkg/Library/HashLibBaseCryptoRouter/HashLibBaseCryptoRouterPei.inf
  Tcg2PhysicalPresenceLib|SecurityPkg/Library/PeiTcg2PhysicalPresenceLib/PeiTcg2PhysicalPresenceLib.inf

[LibraryClasses.common]
  #
  # DXE phase common
  #
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibOptDxe/BaseMemoryLibOptDxe.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  OemHookStatusCodeLib|Nt32Pkg/Library/DxeNt32OemHookStatusCodeLib/DxeNt32OemHookStatusCodeLib.inf
  PeCoffExtraActionLib|Nt32Pkg/Library/DxeNt32PeCoffExtraActionLib/DxeNt32PeCoffExtraActionLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
  WinNtLib|Nt32Pkg/Library/DxeWinNtLib/DxeWinNtLib.inf
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf
  TlsLib|CryptoPkg/Library/TlsLib/TlsLib.inf

!if $(PERF_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
!endif

  HashLib|SecurityPkg/Library/HashLibBaseCryptoRouter/HashLibBaseCryptoRouterDxe.inf
  Tcg2PhysicalPresenceLib|SecurityPkg/Library/DxeTcg2PhysicalPresenceLib/DxeTcg2PhysicalPresenceLib.inf
  Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibTcg2/Tpm2DeviceLibTcg2.inf
  Tpm2SimulatorLib|Nt32Pkg/Feature/Tpm2/Tpm2Simulator/Tpm2DeviceLibSimulator.inf

[LibraryClasses.common.DXE_CORE]
  HobLib|MdePkg/Library/DxeCoreHobLib/DxeCoreHobLib.inf
  MemoryAllocationLib|MdeModulePkg/Library/DxeCoreMemoryAllocationLib/DxeCoreMemoryAllocationLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
!if $(PERF_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/DxeCorePerformanceLib/DxeCorePerformanceLib.inf
!endif

[LibraryClasses.common.SMM_CORE]
  SmmCorePlatformHookLib|MdeModulePkg/Library/SmmCorePlatformHookLibNull/SmmCorePlatformHookLibNull.inf
  MemoryAllocationLib|MdeModulePkg/Library/PiSmmCoreMemoryAllocationLib/PiSmmCoreMemoryAllocationLib.inf
  SmmServicesTableLib|MdeModulePkg/Library/PiSmmCoreSmmServicesTableLib/PiSmmCoreSmmServicesTableLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/SmmReportStatusCodeLib/SmmReportStatusCodeLib.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  SmmMemLib|Nt32Pkg/Feature/Smm/Library/SmmMemLibWinNt/SmmMemLib.inf
!if $(PERF_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/SmmCorePerformanceLib/SmmCorePerformanceLib.inf
!endif

[LibraryClasses.common.DXE_SMM_DRIVER]
#  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  SmmServicesTableLib|MdePkg/Library/SmmServicesTableLib/SmmServicesTableLib.inf
  ReportStatusCodeLib|MdeModulePkg\Library\SmmReportStatusCodeLib/SmmReportStatusCodeLib.inf
  MemoryAllocationLib|MdePkg/Library/SmmMemoryAllocationLib/SmmMemoryAllocationLib.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/SmmCryptLib.inf
  SmmMemLib|Nt32Pkg/Feature/Smm/Library/SmmMemLibWinNt/SmmMemLib.inf
!if $(PERF_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/SmmPerformanceLib/SmmPerformanceLib.inf
!endif

[LibraryClasses.common.UEFI_DRIVER]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
#  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf

[LibraryClasses.common.UEFI_APPLICATION]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  PrintLib|MdeModulePkg/Library/DxePrintLibPrint2Protocol/DxePrintLibPrint2Protocol.inf
  ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf
  ShellCEntryLib|ShellPkg/Library/UefiShellCEntryLib/UefiShellCEntryLib.inf
  
[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  #
  # Runtime
  #
  ReportStatusCodeLib|MdeModulePkg/Library/RuntimeDxeReportStatusCodeLib/RuntimeDxeReportStatusCodeLib.inf
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/RuntimeCryptLib.inf
!if $(CAPSULE_ENABLE)
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibFmp/DxeRuntimeCapsuleLib.inf
!endif

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsFeatureFlag]
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdDxeIplSwitchToLongMode|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdDxeIplBuildPageTables|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdPeiCoreImageLoaderSearchTeSectionFirst|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdVariableCollectStatistics|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdInstallAcpiSdtProtocol|TRUE
!if $(SMM_ENABLE)
  gEfiNt32PkgTokenSpaceGuid.PcdWinNtSmmEnable|TRUE
!endif
!if $(CAPSULE_ENABLE)
  gEfiNt32PkgTokenSpaceGuid.PcdWinNtCapsuleEnable|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdSupportUpdateCapsuleReset|TRUE
!endif
!if $(RECOVERY_ENABLE)
  gEfiNt32PkgTokenSpaceGuid.PcdWinNtRecoveryEnable|TRUE
!endif
!if $(TPM2_ENABLE)
  gEfiNt32PkgTokenSpaceGuid.PcdWinNtTpm2Enable|TRUE
!endif

[PcdsFixedAtBuild]
!if $(PROFILE_ENABLE)
  gEfiMdeModulePkgTokenSpaceGuid.PcdMemoryProfilePropertyMask|0x0
  gEfiMdeModulePkgTokenSpaceGuid.PcdMemoryProfileMemoryType|0x67F
!endif
!if $(SMI_HANDLER_PROFILE_ENABLE)
  gEfiMdeModulePkgTokenSpaceGuid.PcdSmiHandlerProfilePropertyMask|1
!endif

!if $(CAPSULE_ENABLE) || $(RECOVERY_ENABLE)
#  gEfiNt32PkgTokenSpaceGuid.PcdWinNtMemorySizeForSecMain|L"128!64"
!else
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxSizeNonPopulateCapsule|0x0
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxSizePopulateCapsule|0x0
!endif

!if $(RECOVERY_ENABLE)
  gEfiMdeModulePkgTokenSpaceGuid.PcdRecoveryFileName|L"..\\Fv\\Nt32Rec.Cap"
!endif

  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000040
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x1f
  gEfiNt32PkgTokenSpaceGuid.PcdWinNtFirmwareVolume|L"..\\Fv\\Nt32.fd"
  gEfiNt32PkgTokenSpaceGuid.PcdWinNtFirmwareBlockSize|0x10000
  gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x0f
  gEfiMdeModulePkgTokenSpaceGuid.PcdResetOnMemoryTypeInformationChange|FALSE
!if $(SECURE_BOOT_ENABLE) == TRUE || $(TLS_ENABLE) == TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize|0x2000
!endif

!if $(NETWORK_ENABLE) == TRUE
!if $(ALLOW_HTTP_CONNECTIONS) == TRUE
  gEfiNetworkPkgTokenSpaceGuid.PcdAllowHttpConnections|TRUE
!endif
!endif

!ifndef $(USE_OLD_SHELL)
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdShellFile|{ 0x83, 0xA5, 0x04, 0x7C, 0x3E, 0x9E, 0x1C, 0x4F, 0xAD, 0x65, 0xE0, 0x52, 0x68, 0xD0, 0xB4, 0xD1 }
!endif

!if $(SECURE_BOOT_ENABLE) == TRUE
  # override the default values from SecurityPkg to ensure images from all sources are verified in secure boot
  gEfiSecurityPkgTokenSpaceGuid.PcdOptionRomImageVerificationPolicy|0x04
  gEfiSecurityPkgTokenSpaceGuid.PcdFixedMediaImageVerificationPolicy|0x04
  gEfiSecurityPkgTokenSpaceGuid.PcdRemovableMediaImageVerificationPolicy|0x04
!endif
                        
  gEfiMdeModulePkgTokenSpaceGuid.PcdBootManagerMenuFile|{ 0x21, 0xaa, 0x2c, 0x46, 0x14, 0x76, 0x03, 0x45, 0x83, 0x6e, 0x8a, 0xb6, 0xf4, 0x66, 0x23, 0x31 }

!if $(PERF_ENABLE) == TRUE
  gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|0x1
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxPeiPerformanceLogEntries|140
!endif

################################################################################
#
# Pcd Dynamic Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsDynamicDefault.common.DEFAULT]
  gEfiNt32PkgTokenSpaceGuid.PcdWinNtSerialPort|L"COM1!COM2"|VOID*|20
  gEfiNt32PkgTokenSpaceGuid.PcdWinNtGop|L"UGA Window 1!UGA Window 2"|VOID*|52
  gEfiNt32PkgTokenSpaceGuid.PcdWinNtConsole|L"Bus Driver Console Window"|VOID*|52
  gEfiNt32PkgTokenSpaceGuid.PcdWinNtVirtualDisk|L"FW;40960;512"|VOID*|26
  gEfiNt32PkgTokenSpaceGuid.PcdWinNtMemorySize|L"256!256"|VOID*|20
  gEfiNt32PkgTokenSpaceGuid.PcdWinNtPhysicalDisk|L"a:RW;2880;512!d:RO;307200;2048!j:RW;262144;512"|VOID*|100
  gEfiNt32PkgTokenSpaceGuid.PcdWinNtUga|L"UGA Window 1!UGA Window 2"|VOID*|52

  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareBase|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingBase|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableBase|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareBase64|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingBase64|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableBase64|0
!if $(MICROCODE_UPDATE_ENABLE)
  gUefiCpuPkgTokenSpaceGuid.PcdCpuMicrocodePatchAddress|0
  gUefiCpuPkgTokenSpaceGuid.PcdCpuMicrocodePatchRegionSize|0
!endif

  # gEfiTpmDeviceInstanceTpm2SimulatorGuid     = { 0x6480adca, 0xa594, 0x41b4, { 0xb0, 0x6b, 0xdf, 0xff, 0xd7, 0x5d, 0x69, 0x45 } }
  gEfiSecurityPkgTokenSpaceGuid.PcdTpmInstanceGuid|{0xca, 0xad, 0x80, 0x64, 0x94, 0xa5, 0xb4, 0x41, 0xb0, 0x6b, 0xdf, 0xff, 0xd7, 0x5d, 0x69, 0x45}

[PcdsDynamicDefault.Ia32]
#  gEfiNt32PkgTokenSpaceGuid.PcdWinNtFileSystem|L".!..\..\..\..\EdkShellBinPkg\Bin\Ia32\Apps"|VOID*|106
  gEfiNt32PkgTokenSpaceGuid.PcdWinNtFileSystem|L"."|VOID*|106

[PcdsDynamicDefault.x64]
#  gEfiNt32PkgTokenSpaceGuid.PcdWinNtFileSystem|L".!..\..\..\..\EdkShellBinPkg\Bin\X64\Apps"|VOID*|106
  gEfiNt32PkgTokenSpaceGuid.PcdWinNtFileSystem|L"."|VOID*|106

[PcdsDynamicHii.common.DEFAULT]
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupConOutColumn|L"SetupConsoleConfig"|gEfiNt32PkgTokenSpaceGuid|0x0|80
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupConOutRow|L"SetupConsoleConfig"|gEfiNt32PkgTokenSpaceGuid|0x4|25
  gEfiMdePkgTokenSpaceGuid.PcdPlatformBootTimeOut|L"Timeout"|gEfiGlobalVariableGuid|0x0|10
  gEfiMdePkgTokenSpaceGuid.PcdHardwareErrorRecordLevel|L"HwErrRecSupport"|gEfiGlobalVariableGuid|0x0|1

[PcdsDynamicExDefault]
!if $(CAPSULE_ENABLE) || $(RECOVERY_ENABLE)
  gEfiSignedCapsulePkgTokenSpaceGuid.PcdEdkiiSystemFirmwareImageDescriptor|{0x0}|VOID*|0x100
  gEfiMdeModulePkgTokenSpaceGuid.PcdSystemFmpCapsuleImageTypeIdGuid|{0xD3, 0xEE, 0x7E, 0x3F, 0x91, 0xF4, 0x1E, 0x40, 0x9E, 0xCE, 0x74, 0x31, 0x32, 0x2E, 0xAD, 0xF6}
  gEfiSignedCapsulePkgTokenSpaceGuid.PcdEdkiiSystemFirmwareFileGuid|{0xB2, 0x9E, 0x9C, 0xAF, 0xAD, 0x12, 0x3E, 0x4D, 0xA4, 0xD4, 0x96, 0xF6, 0xC9, 0x96, 0x62, 0x15}
!endif

###################################################################################################
#
# Components Section - list of the modules and components that will be processed by compilation
#                      tools and the EDK II tools to generate PE32/PE32+/Coff image files.
#
# Note: The EDK II DSC file is not used to specify how compiled binary images get placed
#       into firmware volume images. This section is just a list of modules to compile from
#       source into UEFI-compliant binaries.
#       It is the FDF file that contains information on combining binary files into firmware
#       volume images, whose concept is beyond UEFI and is described in PI specification.
#       Binary modules do not need to be listed in this section, as they should be
#       specified in the FDF file. For example: Shell binary (Shell_Full.efi), FAT binary (Fat.efi),
#       Logo (Logo.bmp), and etc.
#       There may also be modules listed in this section that are not required in the FDF file,
#       When a module listed here is excluded from FDF file, then UEFI-compliant binary will be
#       generated for it, but the binary will not be put into any firmware volume.
#
###################################################################################################
[Components]
  ##
  #  SEC Phase modules
  ##
  Nt32Pkg/Sec/SecMain.inf {
  <BuildOptions>
    # Add override here, because default X64_CC_FLAGS is already overriden in DSC
    MSFT:*_*_X64_CC_FLAGS == /nologo /W4 /WX /Gy /c /D UNICODE /Od /FIAutoGen.h /EHs-c- /GF /Gs8192 /Zi /Gm /D _CRT_SECURE_NO_WARNINGS /D _CRT_SECURE_NO_DEPRECATE
  }

!if $(CAPSULE_ENABLE) || $(RECOVERY_ENABLE)
  # FMP image decriptor
  Nt32Pkg/Feature/Capsule/SystemFirmwareDescriptor/SystemFirmwareDescriptor.inf
!endif

  ##
  #  PEI Phase modules
  ##
  MdeModulePkg/Core/Pei/PeiMain.inf
  MdeModulePkg/Universal/PCD/Pei/Pcd.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  }
  MdeModulePkg/Universal/ReportStatusCodeRouter/Pei/ReportStatusCodeRouterPei.inf
  MdeModulePkg/Universal/StatusCodeHandler/Pei/StatusCodeHandlerPei.inf
  Nt32Pkg/WinNtOemHookStatusCodeHandlerPei/WinNtOemHookStatusCodeHandlerPei.inf
  Nt32Pkg/BootModePei/BootModePei.inf
  Nt32Pkg/StallPei/StallPei.inf
  Nt32Pkg/WinNtFlashMapPei/WinNtFlashMapPei.inf
  
  MdeModulePkg/Universal/FaultTolerantWritePei/FaultTolerantWritePei.inf
  MdeModulePkg/Universal/Variable/Pei/VariablePei.inf

  Nt32Pkg/WinNtAutoScanPei/WinNtAutoScanPei.inf
  Nt32Pkg/WinNtFirmwareVolumePei/WinNtFirmwareVolumePei.inf
  Nt32Pkg/WinNtThunkPPIToProtocolPei/WinNtThunkPPIToProtocolPei.inf
!if $(CAPSULE_ENABLE)
  MdeModulePkg/Universal/CapsulePei/CapsulePei.inf
!endif

!if $(RECOVERY_ENABLE)
  FatPkg/FatPei/FatPei.inf
  MdeModulePkg/Universal/Disk/CdExpressPei/CdExpressPei.inf
  Nt32Pkg/Feature/Recovery/WinNtRecoveryPei/WinNtRecoveryPei.inf
  Nt32Pkg/Feature/Recovery/WinNtRecoveryRamFsPei/WinNtRecoveryRamFsPei.inf
  SignedCapsulePkg/Universal/RecoveryModuleLoadPei/RecoveryModuleLoadPei.inf {
    <LibraryClasses>
      FmpAuthenticationLib|SecurityPkg/Library/FmpAuthenticationLibRsa2048Sha256/FmpAuthenticationLibRsa2048Sha256.inf
      PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  }
!endif

  MdeModulePkg/Core/DxeIplPeim/DxeIpl.inf

  Nt32Pkg/ResetPei/ResetPei.inf

!if $(TPM2_ENABLE)
  SecurityPkg/Tcg/Tcg2Pei/Tcg2Pei.inf {
    <LibraryClasses>
      Tpm2DeviceLib|Nt32Pkg/Feature/Tpm2/Tpm2Simulator/Tpm2DeviceLibSimulator.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha1/HashInstanceLibSha1.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha256/HashInstanceLibSha256.inf
  }
!endif

  ##
  #  DXE Phase modules
  ##
  MdeModulePkg/Core/Dxe/DxeMain.inf {
    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000040
    <LibraryClasses>
      NULL|MdeModulePkg/Library/DxeCrc32GuidedSectionExtractLib/DxeCrc32GuidedSectionExtractLib.inf
      DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
      DebugLib|Nt32Pkg/Library/DxeDebugLibWinNt/DxeDebugLibWinNt.inf
    <BuildOptions>
      *_*_*_CC_FLAGS =
  }

  MdeModulePkg/Universal/PCD/Dxe/Pcd.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  }
  Nt32Pkg/MetronomeDxe/MetronomeDxe.inf
  Nt32Pkg/RealTimeClockRuntimeDxe/RealTimeClockRuntimeDxe.inf
  Nt32Pkg/ResetRuntimeDxe/ResetRuntimeDxe.inf
  MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf
!if $(SMM_ENABLE)
  Nt32Pkg/FvbServicesRuntimeDxe/FvbServicesRuntimeSmm.inf
!else
  Nt32Pkg/FvbServicesRuntimeDxe/FvbServicesRuntimeDxe.inf
!endif
  MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf  {
    <LibraryClasses>
!if $(SECURE_BOOT_ENABLE) == TRUE
      NULL|SecurityPkg/Library/DxeImageVerificationLib/DxeImageVerificationLib.inf
!endif 
  }
  MdeModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf
  MdeModulePkg/Universal/EbcDxe/EbcDxe.inf
  MdeModulePkg/Universal/MemoryTest/NullMemoryTestDxe/NullMemoryTestDxe.inf
  Nt32Pkg/WinNtThunkDxe/WinNtThunkDxe.inf
  Nt32Pkg/CpuRuntimeDxe/CpuRuntimeDxe.inf
  Nt32Pkg/MpServicesDxe/MpServicesDxe.inf
!if $(SMM_ENABLE)
  MdeModulePkg/Universal/FaultTolerantWriteDxe/FaultTolerantWriteSmm.inf
  MdeModulePkg/Universal/FaultTolerantWriteDxe/FaultTolerantWriteSmmDxe.inf
!else
  MdeModulePkg/Universal/FaultTolerantWriteDxe/FaultTolerantWriteDxe.inf
!endif
  Nt32Pkg/MiscSubClassPlatformDxe/MiscSubClassPlatformDxe.inf
  Nt32Pkg/TimerDxe/TimerDxe.inf
  MdeModulePkg/Universal/ReportStatusCodeRouter/RuntimeDxe/ReportStatusCodeRouterRuntimeDxe.inf
  MdeModulePkg/Universal/StatusCodeHandler/RuntimeDxe/StatusCodeHandlerRuntimeDxe.inf
  Nt32Pkg/WinNtOemHookStatusCodeHandlerDxe/WinNtOemHookStatusCodeHandlerDxe.inf
!if $(SMM_ENABLE)
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableSmmRuntimeDxe.inf
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableSmm.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/VarCheckUefiLib/VarCheckUefiLib.inf
  }
!else
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableRuntimeDxe.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/VarCheckUefiLib/VarCheckUefiLib.inf
  }
!endif
!if $(SECURE_BOOT_ENABLE) == TRUE
  SecurityPkg/VariableAuthenticated/SecureBootConfigDxe/SecureBootConfigDxe.inf
!endif
  MdeModulePkg/Universal/WatchdogTimerDxe/WatchdogTimer.inf
  MdeModulePkg/Universal/MonotonicCounterRuntimeDxe/MonotonicCounterRuntimeDxe.inf
  MdeModulePkg/Universal/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf
  MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  MdeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  }
  MdeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  }
  MdeModulePkg/Universal/Console/TerminalDxe/TerminalDxe.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  }
  MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf {
    <LibraryClasses>
      DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  }
  MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  MdeModulePkg/Universal/Disk/PartitionDxe/PartitionDxe.inf
  MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf
  FatPkg/EnhancedFatDxe/Fat.inf
  MdeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf
  MdeModulePkg/Bus/Scsi/ScsiBusDxe/ScsiBusDxe.inf     ##This driver follows UEFI specification definition
  MdeModulePkg/Bus/Scsi/ScsiDiskDxe/ScsiDiskDxe.inf    ##This driver follows UEFI specification definition
  IntelFrameworkModulePkg/Bus/Pci/IdeBusDxe/IdeBusDxe.inf
  Nt32Pkg/WinNtBusDriverDxe/WinNtBusDriverDxe.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  }
  Nt32Pkg/WinNtBlockIoDxe/WinNtBlockIoDxe.inf
  Nt32Pkg/WinNtSerialIoDxe/WinNtSerialIoDxe.inf
  Nt32Pkg/WinNtGopDxe/WinNtGopDxe.inf
  Nt32Pkg/WinNtSimpleFileSystemDxe/WinNtSimpleFileSystemDxe.inf
  MdeModulePkg/Application/HelloWorld/HelloWorld.inf

!if $(SMM_ENABLE)
  Nt32Pkg/Feature/Smm/WinNtSmmAccessDxe/SmmAccessDxe.inf
  Nt32Pkg/Feature/Smm/WinNtSmmControlDxe/SmmControlDxe.inf
  MdeModulePkg/Core/PiSmmCore/PiSmmIpl.inf
  MdeModulePkg/Core/PiSmmCore/PiSmmCore.inf {
    <PcdsFixedAtBuild>
      gEfiMdeModulePkgTokenSpaceGuid.PcdPropertiesTableEnable|TRUE
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80400040
    <LibraryClasses>
      DebugLib|Nt32Pkg/Library/SmmDebugLibWinNt/SmmDebugLibWinNt.inf
    <BuildOptions>
      DEBUG_*_*_CC_FLAGS = /Od
  }
  MdeModulePkg/Universal/ReportStatusCodeRouter/Smm/ReportStatusCodeRouterSmm.inf
  MdeModulePkg/Universal/StatusCodeHandler/Smm/StatusCodeHandlerSmm.inf
  Nt32Pkg/Feature/Smm/WinNtOemHookStatusCodeHandlerSmm/WinNtOemHookStatusCodeHandlerSmm.inf
  UefiCpuPkg/CpuIo2Smm/CpuIo2Smm.inf
  Nt32Pkg/Feature/Smm/WinNtSmmCpu/WinNtSmmCpu.inf
  Nt32Pkg/Feature/Smm/WinNtSmmChildDispatcherSmm/WinNtSmmChildDispatcherSmm.inf
  UefiCpuPkg/PiSmmCommunication/PiSmmCommunicationSmm.inf 
  MdeModulePkg/Universal/SmmCommunicationBufferDxe/SmmCommunicationBufferDxe.inf
!endif

  #
  # Network stack drivers
  # To test network drivers, need network Io driver(SnpNt32Io.dll), please refer to NETWORK-IO Subproject.
  #
!if $(NETWORK_ENABLE) == TRUE
  MdeModulePkg/Universal/Network/DpcDxe/DpcDxe.inf
  MdeModulePkg/Universal/Network/ArpDxe/ArpDxe.inf
  MdeModulePkg/Universal/Network/Dhcp4Dxe/Dhcp4Dxe.inf
  MdeModulePkg/Universal/Network/Ip4Dxe/Ip4Dxe.inf
  MdeModulePkg/Universal/Network/MnpDxe/MnpDxe.inf
  MdeModulePkg/Universal/Network/VlanConfigDxe/VlanConfigDxe.inf
  MdeModulePkg/Universal/Network/Mtftp4Dxe/Mtftp4Dxe.inf
  MdeModulePkg/Universal/Network/Udp4Dxe/Udp4Dxe.inf
  Nt32Pkg/SnpNt32Dxe/SnpNt32Dxe.inf

!if $(NETWORK_IP6_ENABLE) == TRUE
  NetworkPkg/Ip6Dxe/Ip6Dxe.inf
  NetworkPkg/Dhcp6Dxe/Dhcp6Dxe.inf
  NetworkPkg/TcpDxe/TcpDxe.inf
  NetworkPkg/Udp6Dxe/Udp6Dxe.inf
  NetworkPkg/Mtftp6Dxe/Mtftp6Dxe.inf
  NetworkPkg/UefiPxeBcDxe/UefiPxeBcDxe.inf
  NetworkPkg/IScsiDxe/IScsiDxe.inf
!else
  MdeModulePkg/Universal/Network/Tcp4Dxe/Tcp4Dxe.inf
  MdeModulePkg/Universal/Network/UefiPxeBcDxe/UefiPxeBcDxe.inf
  MdeModulePkg/Universal/Network/IScsiDxe/IScsiDxe.inf
!endif

  NetworkPkg/HttpBootDxe/HttpBootDxe.inf
  NetworkPkg/DnsDxe/DnsDxe.inf
  NetworkPkg/HttpDxe/HttpDxe.inf
  NetworkPkg/HttpUtilitiesDxe/HttpUtilitiesDxe.inf
!endif
   
!if $(TLS_ENABLE) == TRUE
  NetworkPkg/TlsDxe/TlsDxe.inf
  NetworkPkg/TlsAuthConfigDxe/TlsAuthConfigDxe.inf
!endif

  MdeModulePkg/Universal/BdsDxe/BdsDxe.inf {
    <PcdsFixedAtBuild>
      gEfiMdeModulePkgTokenSpaceGuid.PcdPropertiesTableEnable|TRUE
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80400040
    <LibraryClasses>
      DebugLib|Nt32Pkg/Library/DxeDebugLibWinNt/DxeDebugLibWinNt.inf
  }
  MdeModulePkg/Application/UiApp/UiApp.inf{
    <LibraryClasses>
      NULL|MdeModulePkg/Library/DeviceManagerUiLib/DeviceManagerUiLib.inf
      NULL|MdeModulePkg/Library/BootManagerUiLib/BootManagerUiLib.inf
      NULL|MdeModulePkg/Library/BootMaintenanceManagerUiLib/BootMaintenanceManagerUiLib.inf
  }
  MdeModulePkg/Universal/DriverHealthManagerDxe/DriverHealthManagerDxe.inf
  MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf
  MdeModulePkg/Universal/DisplayEngineDxe/DisplayEngineDxe.inf
  MdeModulePkg/Universal/SetupBrowserDxe/SetupBrowserDxe.inf
  MdeModulePkg/Universal/PrintDxe/PrintDxe.inf
  MdeModulePkg/Universal/DriverSampleDxe/DriverSampleDxe.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  }
  MdeModulePkg/Application/VariableInfo/VariableInfo.inf
  
  MdeModulePkg/Universal/Acpi/AcpiTableDxe/AcpiTableDxe.inf

  MdeModulePkg/Universal/PlatformDriOverrideDxe/PlatformDriOverrideDxe.inf
  MdeModulePkg/Universal/LoadFileOnFv2/LoadFileOnFv2.inf
  MdeModulePkg/Application/BootManagerMenuApp/BootManagerMenuApp.inf {
    <LibraryClasses>
      NULL|IntelFrameworkModulePkg/Library/LegacyBootManagerLib/LegacyBootManagerLib.inf
  }
  MdeModulePkg/Logo/LogoDxe.inf

!if $(SHELL_BUILD_ENABLE) == TRUE
  ShellPkg/Application/Shell/Shell.inf {
    <PcdsFixedAtBuild>
      gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE
    <LibraryClasses>
      NULL|ShellPkg/Library/UefiShellLevel2CommandsLib/UefiShellLevel2CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel1CommandsLib/UefiShellLevel1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel3CommandsLib/UefiShellLevel3CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellDriver1CommandsLib/UefiShellDriver1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellInstall1CommandsLib/UefiShellInstall1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellNetwork1CommandsLib/UefiShellNetwork1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellNetwork2CommandsLib/UefiShellNetwork2CommandsLib.inf
      ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
      ShellCommandLib|ShellPkg/Library/UefiShellCommandLib/UefiShellCommandLib.inf
      HandleParsingLib|ShellPkg/Library/UefiHandleParsingLib/UefiHandleParsingLib.inf
      BcfgCommandLib|ShellPkg/Library/UefiShellBcfgCommandLib/UefiShellBcfgCommandLib.inf
      FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  }
!endif

!if $(TPM2_ENABLE)
  SecurityPkg/Tcg/Tcg2Dxe/Tcg2Dxe.inf {
    <LibraryClasses>
      Tpm2DeviceLib|Nt32Pkg/Feature/Tpm2/Tpm2Simulator/Tpm2DeviceLibSimulator.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha1/HashInstanceLibSha1.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha256/HashInstanceLibSha256.inf
  }
  Nt32Pkg/Feature/Tpm2/Tcg2Config/Tcg2ConfigDxe.inf
!endif

!if $(CAPSULE_ENABLE)
  MdeModulePkg/Universal/EsrtDxe/EsrtDxe.inf
  
  SignedCapsulePkg/Universal/SystemFirmwareUpdate/SystemFirmwareReportDxe.inf {
    <LibraryClasses>
      FmpAuthenticationLib|SecurityPkg/Library/FmpAuthenticationLibPkcs7/FmpAuthenticationLibPkcs7.inf
  }
  SignedCapsulePkg/Universal/SystemFirmwareUpdate/SystemFirmwareUpdateDxe.inf {
    <LibraryClasses>
      FmpAuthenticationLib|SecurityPkg/Library/FmpAuthenticationLibPkcs7/FmpAuthenticationLibPkcs7.inf
  }

  MdeModulePkg/Application/CapsuleApp/CapsuleApp.inf
!endif

!if $(MICROCODE_UPDATE_ENABLE)
  Nt32Pkg/Feature/Microcode/MicrocodeUpdateDxe/MicrocodeUpdateDxe.inf

  Nt32Pkg/Feature/Microcode/MicrocodeDummy/MicrocodeUpdates.inf {
    <BuildOptions>
      # *_*_*_GENFW_FLAGS = -a 0x800 -p 0xFF
  }
!endif

!if $(PROFILE_ENABLE)
  MdeModulePkg/Application/MemoryProfileInfo/MemoryProfileInfo.inf {
    <LibraryClasses>
      ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
      FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
      SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf
  }
!endif

!if $(SMI_HANDLER_PROFILE_ENABLE) == TRUE
  MdeModulePkg/Application/SmiHandlerProfileInfo/SmiHandlerProfileInfo.inf
!endif

!if $(PERF_ENABLE) == TRUE
  PerformancePkg/Dp_App/Dp.inf {
    <LibraryClasses>
      PerformanceLib|MdeModulePkg/Library/DxeSmmPerformanceLib/DxeSmmPerformanceLib.inf
    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|0x1
  }
!endif

###################################################################################################
#
# BuildOptions Section - Define the module specific tool chain flags that should be used as
#                        the default flags for a module. These flags are appended to any 
#                        standard flags that are defined by the build process. They can be 
#                        applied for any modules or only those modules with the specific 
#                        module style (EDK or EDKII) specified in [Components] section.
#
###################################################################################################
[BuildOptions]
  *_*_*_CC_FLAGS = /FAcs
  DEBUG_*_*_CC_FLAGS = /Od /GL-
  DEBUG_*_*_DLINK_FLAGS = /EXPORT:InitializeDriver=$(IMAGE_ENTRY_POINT) /BASE:0x10000 /ALIGN:4096 /FILEALIGN:4096 /SUBSYSTEM:CONSOLE
  NOOPT_*_*_DLINK_FLAGS = /EXPORT:InitializeDriver=$(IMAGE_ENTRY_POINT) /BASE:0x10000 /ALIGN:4096 /FILEALIGN:4096 /SUBSYSTEM:CONSOLE
  RELEASE_*_*_DLINK_FLAGS = /ALIGN:4096 /FILEALIGN:4096

#############################################################################################################
# NOTE:
# The following [Libraries] section is for building EDK module under the EDKII tool chain.
# If you want build EDK module for Nt32 platform, please uncomment [Libraries] section and
# libraries used by that EDK module.
# Currently, Nt32 platform do not has any EDK style module
#
#
#[Libraries]
  #
  # Libraries common to PEI and DXE
  #
  #  EdkCompatibilityPkg/Foundation/Efi/Guid/EfiGuidLib.inf
  #  EdkCompatibilityPkg/Foundation/Framework/Guid/EdkFrameworkGuidLib.inf
  #  EdkCompatibilityPkg/Foundation/Guid/EdkGuidLib.inf
  #  EdkCompatibilityPkg/Foundation/Library/EfiCommonLib/EfiCommonLib.inf
  #  EdkCompatibilityPkg/Foundation/Cpu/Pentium/CpuIA32Lib/CpuIA32Lib.inf
  #  EdkCompatibilityPkg/Foundation/Cpu/Itanium/CpuIa64Lib/CpuIA64Lib.inf
  #  EdkCompatibilityPkg/Foundation/Library/CustomizedDecompress/CustomizedDecompress.inf
  #  EdkCompatibilityPkg/Foundation/Library/CompilerStub/CompilerStubLib.inf
  #  EdkCompatibilityPkg/Foundation/Library/Dxe/Hob/HobLib.inf

  #
  # PEI libraries
  #
  #  EdkCompatibilityPkg/Foundation/Framework/Ppi/EdkFrameworkPpiLib.inf
  #  EdkCompatibilityPkg/Foundation/Ppi/EdkPpiLib.inf
  #  EdkCompatibilityPkg/Foundation/Library/Pei/PeiLib/PeiLib.inf
  #  EdkCompatibilityPkg/Foundation/Library/Pei/Hob/PeiHobLib.inf

  #
  # DXE libraries
  #
  #  EdkCompatibilityPkg/Foundation/Core/Dxe/ArchProtocol/ArchProtocolLib.inf
  #  EdkCompatibilityPkg/Foundation/Efi/Protocol/EfiProtocolLib.inf
  #  EdkCompatibilityPkg/Foundation/Framework/Protocol/EdkFrameworkProtocolLib.inf
  #  EdkCompatibilityPkg/Foundation/Protocol/EdkProtocolLib.inf
  #  EdkCompatibilityPkg/Foundation/Library/Dxe/EfiDriverLib/EfiDriverLib.inf
  #  EdkCompatibilityPkg/Foundation/Library/RuntimeDxe/EfiRuntimeLib/EfiRuntimeLib.inf
  #  EdkCompatibilityPkg/Foundation/Library/Dxe/Graphics/Graphics.inf
  #  EdkCompatibilityPkg/Foundation/Library/Dxe/EfiIfrSupportLib/EfiIfrSupportLib.inf
  #  EdkCompatibilityPkg/Foundation/Library/Dxe/Print/PrintLib.inf
  #  EdkCompatibilityPkg/Foundation/Library/Dxe/EfiScriptLib/EfiScriptLib.inf
  #  EdkCompatibilityPkg/Foundation/Library/Dxe/EfiUiLib/EfiUiLib.inf

  #
  # Print/Graphics Library consume SetupBrowser Print Protocol
  #
  #  EdkCompatibilityPkg/Foundation/Library/Dxe/PrintLite/PrintLib.inf
  #  EdkCompatibilityPkg/Foundation/Library/Dxe/GraphicsLite/Graphics.inf

