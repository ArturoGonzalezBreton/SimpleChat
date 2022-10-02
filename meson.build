project('chat', 'cpp')

gtk_dep = dependency('gtkmm-4.0')
json_dep = dependency('jsoncpp')

deps = [dependency('gtkmm-4.0'), dependency('jsoncpp')]

src = ['src/main/server.cpp', 'src/main/cliente.cpp', 'src/main/usuario.cpp',
      'src/main/sala.cpp', 'src/main/servidor_chat.cpp', 'src/main/estado.cpp',
      'src/main/mensaje.cpp']
servidor = executable('servidor', src, dependencies : deps)

src1 = ['src/main/cliente_chat.cpp', 'src/main/cliente.cpp',
       'src/main/usuario.cpp', 'src/main/sala.cpp']
cliente = executable('cliente', src1, dependencies : deps)

#usuario_test = executable('usuario_test',
#	      	'src/test/usuario_unittest.cpp',
#		test_dep)
#test('Pruebas unitarias para clase usuario', usuario_test)
