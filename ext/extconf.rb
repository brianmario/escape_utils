# encoding: UTF-8
require 'mkmf'
require 'rbconfig'

$CFLAGS << ' -Wall -funroll-loops'
$CFLAGS << ' -Wextra' if ENV['DEBUG']
# $CFLAGS << ' -O0 -ggdb'

create_makefile("escape_utils_ext")
