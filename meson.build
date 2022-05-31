project('Ren demo', 'cpp', 
  default_options: ['cpp_std=c++17', 'default_library=static'],
  version: '0.1')

conf = configuration_data()
conf.set('SRC_DIR', meson.project_source_root())
configure_file(
  input : 'config.h.in',
  output : 'config.h',
  configuration : conf)
conf_inc = include_directories('.')

subdir('src')
