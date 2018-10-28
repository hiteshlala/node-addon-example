{
  'targets': [
    {
      'target_name': 'asyncfuncs',
      'sources': [
        './src/asyncstuffneat.cpp'
      ],
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
         }]
      ]
    }
  ] 
}
