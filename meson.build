project('snake', 'c',
	license : 'Zlib',
	default_options : ['c_std=c89', 'warning_level=3', 'werror=true']
)

if meson.get_compiler('c').get_id() == 'emscripten'
	use_sdl2_args = ['-s', 'USE_SDL=2']
	sdl2_dep = declare_dependency(
		compile_args : use_sdl2_args,
		link_args : use_sdl2_args
	)
	exe_suffix = 'html'
else
	sdl2_dep = dependency('sdl2')
	exe_suffix = []
endif

executable('snake', ['src/main.c', 'src/snake.c'],
	c_args : ['-ansi', '-Wc++-compat'],
	dependencies : sdl2_dep,
	name_suffix : exe_suffix
)
