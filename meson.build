project('chat', 'cpp')

#test_dep = dependency('gtest', fallback : ['gtest', 'gtest_dep'])

src = ['src/main/server.cpp', 'src/main/cliente.cpp', 'src/main/usuario.cpp',
      'src/main/sala.cpp', 'src/main/servidor_chat.cpp']
servidor = executable('servidor', src)

src1 = ['src/main/cliente_chat.cpp', 'src/main/cliente.cpp',
       'src/main/usuario.cpp', 'src/main/sala.cpp']
cliente = executable('cliente', src1)

#usuario_test = executable('usuario_test',
	      	'src/test/usuario_unittest.cpp',
		test_dep)
#test('Pruebas unitarias para clase usuario', usuario_test)
