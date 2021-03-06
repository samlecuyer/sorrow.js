{
	'variables': {
		'v8_use_snapshot%': 'true',
		'target_arch': 'x64',
		
		'werror': '',
		'library_files': [
			'src/sorrow.js',
			'lib/assert.js',
			'lib/binary.js',
			'lib/console.js',
			'lib/event.js',
			'lib/fs.js',
			'lib/io.js',
			'lib/module.js',
			'lib/plugins.js',
			'lib/system.js',
			'lib/test.js',
			'lib/util.js',
		],
	},
	
	'targets': [
		{
			'target_name': 'sorrow',
			'type': 'executable',
			
			'dependencies': [
				'sorrow_js2c',
				'deps/v8/tools/gyp/v8.gyp:v8',
			],
			
			'include_dirs': [
				'src',
				'<(SHARED_INTERMEDIATE_DIR)',
				'deps/v8/include',
			],
	  
			'sources': [
				'src/sorrow.h',
				'src/sorrow.cpp',
				'src/sorrow_binary.cpp',
				'src/sorrow_ext.cpp',
				'src/sorrow_fs.cpp',
				'src/sorrow_io.cpp',
				'src/v8_arrays.cpp',
				'src/sorrow_bytes.h',
				'src/sorrow_bytes.cpp',
				'<(SHARED_INTERMEDIATE_DIR)/sorrow_natives.h',
				'<@(library_files)',
				'common.gypi',
			],
			
			'defines': [
				'ARCH="<(target_arch)"',
				'PLATFORM="<(OS)"',
			],

			'link_settings': {
            			'libraries': [
             				 '-liconv',
            			],
          		},

		}, #target sorrow
		
		{
			'target_name': 'sorrow_js2c',
			'type': 'none',
			'actions': [
				{
					'action_name': 'sorrow_js2c',
					'inputs': [
						'<@(library_files)',
						'./config.gypi',
					],
					
					'outputs': [
						'<(SHARED_INTERMEDIATE_DIR)/sorrow_natives.h',
					],
					
					'action': [
						'python',
						'tools/js2c.py',
						'<@(_outputs)',
						'<@(library_files)',
					],
				},
			], #actions
		}, #target sorrow_js2c
	], #targets
}
