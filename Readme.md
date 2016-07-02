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

## Known limitation:
This package is only the sample code to show the concept.
It does not have a full validation and does not meet the production quality yet.