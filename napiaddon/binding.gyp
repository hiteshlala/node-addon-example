{
  'targets': [
    {
      'target_name': 'asyncfuncs',
      'sources': [
        './src/asyncstuffnapi.cpp'
      ],
      'include_dirs': [ 
        './node_modules/node-addon-api'
      ],
      'defines': [
        'NAPI_DISABLE_CPP_EXCEPTIONS'
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
         ['OS=="mac"', {
           "xcode_settings": {
              "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
            },
         }]
      ]
    }
  ] 
}
