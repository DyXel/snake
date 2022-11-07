project('snake', 'c',
	license : 'Zlib',
	default_options : ['c_std=c89', 'warning_level=3', 'werror=true']
)

executable('snake', ['src/main.c', 'src/snake.c'],
	c_args : ['-ansi'],
	dependencies : dependency('sdl2')
)
