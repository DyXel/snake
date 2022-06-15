project('snake', 'c', license : 'Zlib', default_options : ['c_std=c89', 'warning_level=3', 'werror=true'])

sdl2_dep = dependency('sdl2')

executable('snake', 'src/snake.c', c_args : ['-ansi'], dependencies : sdl2_dep)
