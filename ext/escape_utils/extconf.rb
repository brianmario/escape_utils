require 'mkmf'

$CFLAGS << ' -Wall -funroll-loops -fvisibility=hidden'
$CFLAGS << ' -Wextra -O0 -ggdb3' if ENV['DEBUG']

have_func("rb_str_new_with_class")

create_makefile("escape_utils/escape_utils")
