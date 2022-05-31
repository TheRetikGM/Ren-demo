src = files(
  'main.cpp')
  
inc = include_directories('include')
ren_dep = subproject('Ren').get_variable('ren_dep')

executable('main', 
  src, 
  include_directories : [inc, conf_inc],
  dependencies : ren_dep)
