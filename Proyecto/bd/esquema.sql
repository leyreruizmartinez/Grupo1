CREATE TABLE LibrosBd (ID INTEGER PRIMARY KEY AUTOINCREMENT,ISBN TEXT UNIQUE,Titulo TEXT,Autor TEXT,Anio INTEGER,Copias INTEGER,Disponible INTEGER);
CREATE TABLE sqlite_sequence(name,seq);
CREATE TABLE Prestamos (ID INTEGER PRIMARY KEY AUTOINCREMENT,UsuarioID INTEGER,ISBN TEXT);
CREATE TABLE usuarios (id INTEGER PRIMARY KEY,nombre TEXT,apellido TEXT,correo TEXT UNIQUE,telefono TEXT,contrasena TEXT,sancionado TEXT);
CREATE TABLE historial (usuario INTEGER,isbn TEXT,titulo TEXT,autor TEXT,fecha_prestamo TEXT,fecha_devolucion TEXT,estado INTEGER,FOREIGN KEY (usuario) REFERENCES usuarios(id) ON DELETE CASCADE,PRIMARY KEY (usuario, isbn, fecha_prestamo));
