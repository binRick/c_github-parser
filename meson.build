#//***********************************************************/
project(
    'c_github-parser',          ['c'],         version : '0.0.1',         
    default_options : ['warning_level=2','c_std=gnu99'],
)
#//***********************************************************/
host_system = host_machine.system()

cc = meson.get_compiler('c')
sh = find_program('sh', required : true)
passh = find_program('passh', required : true)

#//***********************************************************/
libObjectively_dep = dependency(
  'Objectively',
  fallback: ['Objectively', 'Objectively_dep']
)
libconfig_dep = dependency(
  'libconfuse',
  fallback: ['libconfuse', 'libconfuse_dep'],
  version: '==3.3',
)
#//***********************************************************/
config_deps = [libconfig_dep,libObjectively_dep]
#//***********************************************************/
incdir = include_directories(
  './include', 
  './src', 
  './deps', 
  '../',
)
#//***********************************************************/
test1 = executable('test1', [ 
    'tests/test1.c',  
  ], include_directories : [incdir], 
  dependencies : [config_deps],
)
#//***********************************************************/
tests = [
  [
   'test 1', 
   'test-1.log',
    test1
  ]
]
#//***********************************************************/

test_env = environment()
test_env.set('__SH', sh.full_path())
test_env.set('__BD', meson.current_build_dir())
test_env.set('__SD', meson.current_source_dir())
#//***********************************************************/
#//***********************************************************/
foreach i : tests
test(i[0], passh, args: ['-L', i[1], i[2], '--help' ], env: test_env)
test(i[0], passh, args: ['-L', i[1], i[2], '-u binRick' ], env: test_env)
test(i[0], passh, args: ['-L', i[1], i[2], '-u binRick', '-v' ], env: test_env)
endforeach
#//***********************************************************/


