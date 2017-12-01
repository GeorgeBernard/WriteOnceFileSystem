#!/bin/bash

sudo sh -c "free && sync && echo 3 >'/proc/sys/vm/drop_caches' && free" root
