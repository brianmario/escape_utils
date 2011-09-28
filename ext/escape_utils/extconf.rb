# encoding: UTF-8
require 'mkmf'
require 'rbconfig'

$CFLAGS << ' -Wall -funroll-loops'
$CFLAGS << ' -Wextra -O0 -ggdb3' if ENV['DEBUG']

if try_compile(<<SRC)
#include <ruby.h>
int main(void) { rb_cvar_set(Qnil, Qnil, Qnil); return 0; }
SRC
  $CFLAGS << " -DRB_CVAR_SET_ARITY=3 "
else
  $CFLAGS << " -DRB_CVAR_SET_ARITY=4 "
end

create_makefile("escape_utils/escape_utils")
