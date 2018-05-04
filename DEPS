vars = {
  'android_git': 'https://android.googlesource.com',
  'chromium_git': 'https://chromium.googlesource.com',

  # This variable is set on the Chrome infra for compatiblity with gclient.
  'root': '.',
}

deps = {

  '{root}/build':
    '{chromium_git}/chromium/src/build.git@2f3b6e8ce9e783b2a09496d70eef2974506a41c8',

  '{root}/buildtools':
    '{chromium_git}/chromium/buildtools.git@461b345a815c1c745ac0534a6a4bd52d123abe68',

  '{root}/testing':
    '{chromium_git}/chromium/src/testing@6dfa36ab2e5143fa2f7353e3af5d2935af2e61f7',

  # Cherry is a dEQP management GUI written in Go. We use it for viewing test results.
  '{root}/third_party/cherry':
    '{android_git}/platform/external/cherry@4f8fb08d33ca5ff05a1c638f04c85bbb8d8b52cc',

  '{root}/third_party/deqp/src':
    '{android_git}/platform/external/deqp@5bc346ba2d5465a2e6094e254f12b1586fd0097f',

  '{root}/third_party/glslang/src':
    '{android_git}/platform/external/shaderc/glslang@2edde6665d9a56ead5ea0e55b4e64d9a803e6164',

  '{root}/third_party/googletest/src':
    '{chromium_git}/external/github.com/google/googletest.git@d175c8bf823e709d570772b038757fadf63bc632',

  '{root}/third_party/libpng/src':
    '{android_git}/platform/external/libpng@094e181e79a3d6c23fd005679025058b7df1ad6c',

  '{root}/third_party/spirv-headers/src':
    '{android_git}/platform/external/shaderc/spirv-headers@98b01515724c428d0f0a5d01deffcce0f5f5e61c',

  '{root}/third_party/spirv-tools/src':
    '{android_git}/platform/external/shaderc/spirv-tools@9996173f363729b3a97309685dbd4d78547a63a7',

  '{root}/third_party/vulkan-validation-layers/src':
    '{android_git}/platform/external/vulkan-validation-layers@88e43c8f9a750839a5d6d3f90098505bad3a98a1',

  '{root}/third_party/zlib':
    '{chromium_git}/chromium/src/third_party/zlib@24ab14872e8e068ba08cc31cc3d43bcc6d5cb832',

  '{root}/tools/clang':
    '{chromium_git}/chromium/src/tools/clang.git@e70074db10b27867e6c873adc3ac7e5f9ee0ff6e',

  '{root}/tools/gyp':
    '{chromium_git}/external/gyp@5e2b3ddde7cda5eb6bc09a5546a76b00e49d888f',
}

hooks = [
  # Pull clang-format binaries using checked-in hashes.
  {
    'name': 'clang_format_win',
    'pattern': '.',
    'condition': 'host_os == "win"',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--platform=win32',
                '--no_auth',
                '--bucket', 'chromium-clang-format',
                '-s', '{root}/buildtools/win/clang-format.exe.sha1',
    ],
  },
  {
    'name': 'clang_format_mac',
    'pattern': '.',
    'condition': 'host_os == "mac"',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--platform=darwin',
                '--no_auth',
                '--bucket', 'chromium-clang-format',
                '-s', '{root}/buildtools/mac/clang-format.sha1',
    ],
  },
  {
    'name': 'clang_format_linux',
    'pattern': '.',
    'condition': 'host_os == "linux"',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--platform=linux*',
                '--no_auth',
                '--bucket', 'chromium-clang-format',
                '-s', '{root}/buildtools/linux64/clang-format.sha1',
    ],
  },
  # Pull GN binaries using checked-in hashes.
  {
    'name': 'gn_win',
    'pattern': '.',
    'condition': 'host_os == "win"',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--platform=win32',
                '--no_auth',
                '--bucket', 'chromium-gn',
                '-s', '{root}/buildtools/win/gn.exe.sha1',
    ],
  },
  {
    'name': 'gn_mac',
    'pattern': '.',
    'condition': 'host_os == "mac"',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--platform=darwin',
                '--no_auth',
                '--bucket', 'chromium-gn',
                '-s', '{root}/buildtools/mac/gn.sha1',
    ],
  },
  {
    'name': 'gn_linux64',
    'pattern': '.',
    'condition': 'host_os == "linux"',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--platform=linux*',
                '--no_auth',
                '--bucket', 'chromium-gn',
                '-s', '{root}/buildtools/linux64/gn.sha1',
    ],
  },

  {
    # Update the Windows toolchain if necessary.  Must run before 'clang' below.
    'name': 'win_toolchain',
    'pattern': '.',
    'condition': 'checkout_win',
    'action': ['python', '{root}/build/vs_toolchain.py', 'update', '--force'],
  },

  {
    # Note: On Win, this should run after win_toolchain, as it may use it.
    'name': 'clang',
    'pattern': '.',
    'action': ['python', '{root}/tools/clang/scripts/update.py', '--if-needed'],
  },

  # Pull rc binaries using checked-in hashes.
  {
    'name': 'rc_win',
    'pattern': '.',
    'condition': 'checkout_win and host_os == "win"',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-browser-clang/rc',
                '-s', '{root}/build/toolchain/win/rc/win/rc.exe.sha1',
    ],
  },

  {
    # A change to a .gyp, .gypi, or to GYP itself should run the generator.
    'pattern': '.',
    'action': ['python', '{root}/gyp/gyp_angle'],
  },
]

recursedeps = [
  # buildtools provides clang_format.
  '{root}/buildtools',
]
