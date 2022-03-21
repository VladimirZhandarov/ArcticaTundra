# Проверка версии OC (должна быть Windows7 SP1)
!contains(QMAKE_HOST.version, ^144) {
    message("Cannot build the project with Windows $${QMAKE_HOST.version}")
    error("Use Windows 7 SP 1")
}


# Сборка проекта должна происходить только на 64-битной ОС
!contains(QMAKE_HOST.arch, x86_64) {
  error("Assembling the project should proceed on a 64-bit OS")
}


# Проверка версии Qt
!contains(QT_VERSION, ^4.8.6) {
    message("Cannot build the project with Qt version $${QT_VERSION}")
    error("Use Qt 4.8.6")
}

# Проверка факта установки библиотеки OpenCV версии 2.4.8
#!system(set OPENCV248) : error("Not find environment variable OPENCV248")
# Проверка факта установки библиотеки CUDA TOOL версии 5.5
#!system(set CUDA_TOOL55) : error("Not find environment variable CUDA_TOOL55")

# Проверка факта установки библиотеки OpenSceneGraph версии 2.8.5
#!system(set OPENSCENEGRAPH285) : error("Not find environment variable OPENSCENEGRAPH285")
# Проверка факта установки библиотеки CodeSynthesis XSD версии 3.3.0
#!system(set CODESYN_XSD330) : error("Not find environment variable CODESYN_XSD330")
# Проверка факта установки XMLRPC lib
#!system(set XML_RPC) : error("Not find environment variable XML_RPC")
# Проверка факта установки JRTPLIB
#!system(set JRTPLIB) : error("Not find environment variable JRTPLIB")
# Проверка факта установки QWT6
#!system(set QWT6DIR) : error("Not find environment variable QWT6DIR")
# Проверка факта установки Ogre 1.7
#!system(set OGRE17) : error("Not find environment variable OGRE17")
# Проверка факта установки Qt Solutions
#!system(set QT_SOLUTIONS) : error("Not find environment variable QT_SOLUTIONS")

# Проверка факта установки библиотеки Boost версии 1.47.0
#!system(set BOOST1470) : error("Not find environment variable BOOST1470")

# Проверка факта установки СУБД PostgreSQL версии 9.3.3-1
#!system(set PGSQL9331) : error("Not find environment variable PGSQL9331")

# Проверка факта установки компилятора ODB версии 2.3.0
#!system(set CODESYN_ODB_BIN230) : error("Not find environment variable CODESYN_ODB_BIN230")

# Проверка факта установки Common Runtime Library ODB версии 2.3.0
#!system(set CODESYN_ODB_COMMON230) : error("Not find environment variable CODESYN_ODB_COMMON230")

# Проверка факта установки Database Runtime Library ODB версии 2.3.1 для PostgreSQL
#!system(set CODESYN_ODB_PGSQL231) : error("Not find environment variable CODESYN_ODB_PGSQL231")

# Проверка факта установки Database Runtime Library ODB версии 2.3.1 для Qt
#!system(set CODESYN_ODB_QT231) : error("Not find environment variable CODESYN_ODB_QT231")

# Проверка факта установки QJSON
#!system(set QJSON) : error("Not find environment QJSON")

# Проверка факта установки QWT 6.1.0
#!system(set QWT610) : error("Not find environment QWT610")

# Сборка осуществляется для 64-разрядной ОС
PLATFORM = x64
CONFIG += PLATFORM
QMAKE_LFLAGS *= /MACHINE:X64


# Каталог, куда помещаются собранные бинарные файлы
DESTDIR = ../..


# Вывод максимального количество warning-ов (-W4)
# Warning-и считаем за ошибки (-WX)
QMAKE_CXXFLAGS_WARN_ON = -W4 # -WX # Возникают неустраниемые предупреждения при использовании Ogre3D v1.7

# Считаем тип wchar_t встроенным типом 
QMAKE_CXXFLAGS -= -Zc:wchar_t-


# Для каждого проекта заголовочные файла хранятся в папке с названием проекта в самой папке проекта
INCLUDEPATH = $$PWD/$$TARGET/$$TARGET
HEADERS = $$files($$PWD/$$TARGET/$$TARGET/*.h)
# CPP-файлы храняться в самой папке проекта
SOURCES = $$files($$PWD/$$TARGET/*.cpp)
# $$PWD - вернет путь к текущему pri-файлу; на данном этапе значение TARGET 
# еще не переопределено и равно названию текущего проекта куда это pri-файл включен
# этим и воспользуемся
# Все lib-файлы всех модулей проекта находятся в этой папке
LIBS += -L$$PWD/..


# В каждом проекте к стандартной очистки проекта выполняемой командой clean
# добавляем дополнительное удаление файлов и папок через дополнительную цель projectclean
QMAKE_DEL_FILE = del /s /q /f
QMAKE_DEL_DIR = rmdir /s /q
QMAKE_DISTCLEAN += *vcproj* *~ *.swp Makefile* 
projectclean.commands = $${QMAKE_DEL_DIR} $$PLATFORM
distclean.depends += projectclean
QMAKE_EXTRA_TARGETS += distclean projectclean


# При сборки в режиме debug
CONFIG(debug) {
  # собранные бинарные файлы должны иметь суффикс _d
  TARGET_SUFFIX = _d

  OBJECTS_DIR = $$_PRO_FILE_PWD_/$$PLATFORM/debug/obj
  MOC_DIR = $$_PRO_FILE_PWD_/$$PLATFORM/debug/moc

  # Местораположение файла базы данных программы, генерируемого Visual Studio
  QMAKE_CXXFLAGS_DEBUG += -Fd"$$OBJECTS_DIR/../"
}


# При сборки в режиме release
CONFIG(release) {
  OBJECTS_DIR = $$_PRO_FILE_PWD_/$$PLATFORM/release/obj
  MOC_DIR = $$_PRO_FILE_PWD_/$$PLATFORM/release/moc
  
  # Местораположение файла базы данных программы, генерируемого Visual Studio
  QMAKE_CXXFLAGS_RELEASE += -Fd"$$OBJECTS_DIR/../"
}
TARGET = $$join(TARGET,,,$$TARGET_SUFFIX) 
