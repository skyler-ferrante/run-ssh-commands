#!/bin/bash

# Shouuld probably make a makefile at somepoint
# But this works for now

gcc src/ssh_run.c src/ssh_run.h src/main.c -lssh
