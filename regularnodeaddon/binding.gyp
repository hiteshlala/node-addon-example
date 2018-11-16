{
  'targets': [
    {
      'target_name': 'asyncfuncs',
      'sources': [
        './src/asyncstuffneat.cpp'
      ],
      #  None of these seem to work, still wont compile with try catch
      # 'cflags!': [ '-fno-exceptions' ],
      # 'cflags_cc!': [ '-fno-exceptions' ],
      # 'cflags': [ '-fexceptions' ],
      # 'cflags_cc': [ '-fexceptions' ],
      # "defines": [
      #   "_HAS_EXCEPTIONS=1"
      # ],
      'include_dirs': [ 
      ],
      "dependencies": [
	    ],
      'conditions': [
        ['OS=="win"', {
            'defines': [],
            'libraries': [],
            'configurations': {
              'Release': {
                'msvs_settings': {
                  'VCCLCompilerTool': {
                    'RuntimeLibrary': 2, # 3-MultiThreadedDebugDLL (/MDd), 1-MultiThreadedDebug (/MTd), 2-MultiThreadedDLL (/MD), 0-MultiThreaded (/MT), 4-
                    }
                  }
              }
            }
         }],
        #  ['OS=="mac"', {
        #    'cflags!': [ '-fno-exceptions' ],
        #    'cflags_cc!': [ '-fno-exceptions' ]
        #  }]
      ]
    }
  ] 
}
