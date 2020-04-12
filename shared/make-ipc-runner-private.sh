#!/bin/sh

if [ "$1" != "shared/ipc-runner-public.c" ]; then
  exit 1
fi

if [ "$2" != "shared/ipc-runner-public.h" ]; then
  exit 1
fi

sed -i -e s/ipc-runner-public.h/ipc-runner-private.h/ -e s/SECTION:IpcRunner/PRIVATE:IpcRunner/ $1
mv $1 shared/ipc-runner.c
mv $2 shared/ipc-runner-private.h
