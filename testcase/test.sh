#!/bin/bash

make
rm testimage
cp diskBackup testimage
./project testimage  <commands.txt



