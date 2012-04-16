{
  'variables': {
    'strict_aliasing%': 'false',     # turn on/off -fstrict-aliasing
    'visibility%': 'hidden',         # V8's visibility setting
    'v8_target_arch%': 'x64',        # set v8's target architecture
    'target_arch%': 'x64',           	# set v8's target architecture
    'host_arch%': 'x64',            # set v8's host architecture
    'want_separate_host_toolset': 0, # V8 should not build target and host
    'library%': 'static_library',    # allow override to 'shared_library' for DLL/.so builds
    'component%': 'static_library',  # NB. these names match with what V8 expects
    'msvs_multi_core_compile': '0',  # we do enable multicore compiles, but not using the V8 way
  },

}
