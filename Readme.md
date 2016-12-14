# This package is to demonstrate how to enable advanced BIOS features on NT32 emulation environment.

## How to build?
Just build it as a normal EDKII package.

  0) Download EDKII from github https://github.com/tianocore/edk2

  1) Override original Nt32Pkg with this package.

  2) Build Nt32Pkg as normal way.

## Feature:
- SMM

A user can enable fake SMM feature on NT32.

In build command line, a user may use `-D SMM_ENABLE= TRUE` to enable SMM.
This macro is defined in Nt32Pkg.dsc file.

The purpose of Nt32Smm is to debug or test the CPU independent SMM infrastructure, such as PiSmmIpl, PiSmmCore, SmmVariable driver, etc.

Nt32SMM cannot be used to validate CPU dependent driver such as PiSmmCpu, or chipsec dependent driver, such as SmmAccess, SmmControl, SmmChildDispatch. Nt32SMM uses the fake version of these drivers in Nt32Pkg\Feature\Smm directory.

- Capsule Update

A user can enable fake capsule based BIOS Update feature on NT32.

In build command line, a user may use `-D CAPSULE_ENABLE= TRUE` to enable capsule update.
This macro is defined in Nt32Pkg.dsc file.

The purpose of Nt32 capsule support is to debug or test the hardware independent capsule update infrastructure, such as DxeCapsuleLibFmp, FmpAuthenticationLibPkcs7, SystemFirmwareUpdate driver, etc.

After build, a user need copy NT32FIRMWAREUPDATECAPSULEFMPPKCS7.Cap from FV dir to $(ARCH) dir, and boot to shell and run CapsuleApp NT32FIRMWAREUPDATECAPSULEFMPPKCS7.Cap. Then the next boot will be capsule update boot mode.

A known limitation is that, capsule boot mode may not be always successfully enabled in every boot. The reason is that OS may allocate different memory for NT32 environment. One recommendation is to close other OS application, it can help increase the propability of the success.

- Recovery

A user can enable fake recovery feature on NT32.

In build command line, a user may use `-D RECOVERY_ENABLE= TRUE` to enable recovery.
This macro is defined in Nt32Pkg.dsc file.

The purpose of Nt32 recovery support is to debug or test the hardware independent recovery infrastructure, such as DxeCapsuleLibFmp, FmpAuthenticationLibRsa2048Sha256, RecoveryModuleLoadPei driver, etc.

After build, a user can create a dummy file named "Nt32RecoveryMode" under $(ARCH) dir. Then the next boot will be recovery boot mode.
A user must delete the dummy file named "Nt32RecoveryMode" under $(ARCH) dir to turn system back to normal boot mode. 

- Microcode Update

A user can enable fake Microcode Update feature on NT32.

In build command line, a user may use `-D MICROCODE_UPDATE_ENABLE= TRUE` to enable Microcode update.
This macro is defined in Nt32Pkg.dsc file.

The purpose of Nt32 Microcode update support is to debug or test the hardware independent Microcode update infrastructure, such as MicrocodeUpdateDxe driver.

After build, a user need copy MICROCODECAPSULE.Cap from FV dir to $(ARCH) dir, and boot to shell and run CapsuleApp MICROCODECAPSULE.Cap. Then the next boot will be capsule update boot mode for Microcode update.

Since Microcode update uses same way as capsule update, the limitation mentioned in Capsule update is also applied here.
Besides that, NT32 fake Microcode update uses dummy empty Microcode, not real Microcode. NT32 fake Microcode update driver (Nt32Pkg\Feature\Microcode\MicrocodeUpdateDxe) is copied from UefiCpuPkg\Feature\Capsule\MicrocodeUpdateDxe. The difference is that the NT32 fake version does not touch real CPU MSR register.

- TPM2.0

A user can enable TPM2 simulator support on NT32.

In build command line, a user may use `-D TPM2_ENABLE= TRUE` to enable TPM2.
This macro is defined in Nt32Pkg.dsc file.

The purpose of NT32 TPM2 support is to debug or test the hardware independent TPM2 feature, such as Tcg2Pei, Tcg2Dxe.

Nt32TPM2 cannot be used to validate TPM2 hardware dependent modules such as Tpm2DeviceLibDTpm. Nt32TPM2 uses the simulatore version in Nt32Pkg\Feature\Tpm2 directory.

Step:

1) A user need download the TPM2 simulator from https://www.microsoft.com/en-us/download/details.aspx?id=52507

2) A user runs "TSS.MSR v2.0 TPM2 Simulator\Simulator.exe" in the windows OS.

3) Now the user can run SecMain.exe and the NT32 emulator will communicate with TPM2.0 simulator.

## Known limitation:
This package is only the sample code to show the concept.
It does not have a full validation and does not meet the production quality yet.