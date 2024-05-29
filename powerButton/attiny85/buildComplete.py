#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# pylint: disable=undefined-variable
#
# This script is executed by PlatformIO before compiling the code, it's some sort of hook executed
# before everything in order to manually patch few things
#
import os
import shutil
Import("env", "projenv")


# Move back the original file
def buildPostAction(source, target, env):
    shutil.copyfile(".pio/build/digispark-tiny/firmware.hex", "firmware.hex")


# Before compilation (BEGIN)
# print("----------------------------------------------- Compilation [BEGIN]")
print("----------------------------------------------- Compilation [ END ]")
env.AddPostAction("buildprog", buildPostAction)
