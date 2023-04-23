{
    "targets": [{
        "target_name": "virtual_display",
        "sources": [],
        "conditions": [
            ['OS=="win"', {
                "sources": [
                    "src/virtual_display.cc"
                ],
                 "defines": [
                    "_HAS_EXCEPTIONS=1"
                ],
                "msvs_settings": {
                    "VCCLCompilerTool": {
                    "ExceptionHandling": 1
                }},
            }]
        ],
        'include_dirs': [
            "<!@(node -p \"require('node-addon-api').include\")"
        ],
        'libraries': [],
        'dependencies': [
            "<!(node -p \"require('node-addon-api').gyp\")"
        ],
        'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS'],
        "xcode_settings": {
            "MACOSX_DEPLOYMENT_TARGET": "10.14",
            "SYSTEM_VERSION_COMPAT": 1,
            "OTHER_CPLUSPLUSFLAGS": ["-std=c++14", "-stdlib=libc++"],
            "OTHER_LDFLAGS": [
                "-framework Storekit"
            ]
        }
    }]
}
