#!/usr/bin/ruby
IN_FILE     = "boot.s.pre"
OUT_FILE    = "boot.s"
PREFIX      = "src/arch/x86/"

`cat #{PREFIX}#{IN_FILE} > #{PREFIX}#{OUT_FILE}`
