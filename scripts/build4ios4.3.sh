#!/bin/bash

export IPHONEOS_DEPLOYMENT_TARGET="4.3"

export CC="/Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/llvm-g++-4.2" 

export CFLAGS="-arch i386 -pipe -Os -gdwarf-2 -isysroot /Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator4.3.sdk/"

export LDFLAGS="-arch i386 -pipe -Os -gdwarf-2 -isysroot /Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator4.3.sdk"

