project('Ren demo', 'cpp', 
  default_options: ['cpp_std=c++17', 'default_library=static'],
  version: '0.1')

# Get global options
exename = get_option('exename')

# Project configuration data
conf = configuration_data()
conf.set('SRC_DIR', meson.project_source_root())
configure_file(
  input : 'config.h.in',
  output : 'config.h',
  configuration : conf)
conf_inc = include_directories('.')

# Add source files and executable
subdir('src')

# VSCode configuration data
if build_machine.system() == 'linux' and get_option('vscode_configure') == true
  run_command(files('.vscode/configure'), env : ['EXENAME=' + exename], check : false)
endif
