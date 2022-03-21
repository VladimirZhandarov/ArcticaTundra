# �������� ������ OC (������ ���� Windows7 SP1)
!contains(QMAKE_HOST.version, ^144) {
    message("Cannot build the project with Windows $${QMAKE_HOST.version}")
    error("Use Windows 7 SP 1")
}


# ������ ������� ������ ����������� ������ �� 64-������ ��
!contains(QMAKE_HOST.arch, x86_64) {
  error("Assembling the project should proceed on a 64-bit OS")
}


# �������� ������ Qt
!contains(QT_VERSION, ^4.8.6) {
    message("Cannot build the project with Qt version $${QT_VERSION}")
    error("Use Qt 4.8.6")
}

# �������� ����� ��������� ���������� OpenCV ������ 2.4.8
#!system(set OPENCV248) : error("Not find environment variable OPENCV248")
# �������� ����� ��������� ���������� CUDA TOOL ������ 5.5
#!system(set CUDA_TOOL55) : error("Not find environment variable CUDA_TOOL55")

# �������� ����� ��������� ���������� OpenSceneGraph ������ 2.8.5
#!system(set OPENSCENEGRAPH285) : error("Not find environment variable OPENSCENEGRAPH285")
# �������� ����� ��������� ���������� CodeSynthesis XSD ������ 3.3.0
#!system(set CODESYN_XSD330) : error("Not find environment variable CODESYN_XSD330")
# �������� ����� ��������� XMLRPC lib
#!system(set XML_RPC) : error("Not find environment variable XML_RPC")
# �������� ����� ��������� JRTPLIB
#!system(set JRTPLIB) : error("Not find environment variable JRTPLIB")
# �������� ����� ��������� QWT6
#!system(set QWT6DIR) : error("Not find environment variable QWT6DIR")
# �������� ����� ��������� Ogre 1.7
#!system(set OGRE17) : error("Not find environment variable OGRE17")
# �������� ����� ��������� Qt Solutions
#!system(set QT_SOLUTIONS) : error("Not find environment variable QT_SOLUTIONS")

# �������� ����� ��������� ���������� Boost ������ 1.47.0
#!system(set BOOST1470) : error("Not find environment variable BOOST1470")

# �������� ����� ��������� ���� PostgreSQL ������ 9.3.3-1
#!system(set PGSQL9331) : error("Not find environment variable PGSQL9331")

# �������� ����� ��������� ����������� ODB ������ 2.3.0
#!system(set CODESYN_ODB_BIN230) : error("Not find environment variable CODESYN_ODB_BIN230")

# �������� ����� ��������� Common Runtime Library ODB ������ 2.3.0
#!system(set CODESYN_ODB_COMMON230) : error("Not find environment variable CODESYN_ODB_COMMON230")

# �������� ����� ��������� Database Runtime Library ODB ������ 2.3.1 ��� PostgreSQL
#!system(set CODESYN_ODB_PGSQL231) : error("Not find environment variable CODESYN_ODB_PGSQL231")

# �������� ����� ��������� Database Runtime Library ODB ������ 2.3.1 ��� Qt
#!system(set CODESYN_ODB_QT231) : error("Not find environment variable CODESYN_ODB_QT231")

# �������� ����� ��������� QJSON
#!system(set QJSON) : error("Not find environment QJSON")

# �������� ����� ��������� QWT 6.1.0
#!system(set QWT610) : error("Not find environment QWT610")

# ������ �������������� ��� 64-��������� ��
PLATFORM = x64
CONFIG += PLATFORM
QMAKE_LFLAGS *= /MACHINE:X64


# �������, ���� ���������� ��������� �������� �����
DESTDIR = ../..


# ����� ������������� ���������� warning-�� (-W4)
# Warning-� ������� �� ������ (-WX)
QMAKE_CXXFLAGS_WARN_ON = -W4 # -WX # ��������� ������������� �������������� ��� ������������� Ogre3D v1.7

# ������� ��� wchar_t ���������� ����� 
QMAKE_CXXFLAGS -= -Zc:wchar_t-


# ��� ������� ������� ������������ ����� �������� � ����� � ��������� ������� � ����� ����� �������
INCLUDEPATH = $$PWD/$$TARGET/$$TARGET
HEADERS = $$files($$PWD/$$TARGET/$$TARGET/*.h)
# CPP-����� ��������� � ����� ����� �������
SOURCES = $$files($$PWD/$$TARGET/*.cpp)
# $$PWD - ������ ���� � �������� pri-�����; �� ������ ����� �������� TARGET 
# ��� �� �������������� � ����� �������� �������� ������� ���� ��� pri-���� �������
# ���� � �������������
# ��� lib-����� ���� ������� ������� ��������� � ���� �����
LIBS += -L$$PWD/..


# � ������ ������� � ����������� ������� ������� ����������� �������� clean
# ��������� �������������� �������� ������ � ����� ����� �������������� ���� projectclean
QMAKE_DEL_FILE = del /s /q /f
QMAKE_DEL_DIR = rmdir /s /q
QMAKE_DISTCLEAN += *vcproj* *~ *.swp Makefile* 
projectclean.commands = $${QMAKE_DEL_DIR} $$PLATFORM
distclean.depends += projectclean
QMAKE_EXTRA_TARGETS += distclean projectclean


# ��� ������ � ������ debug
CONFIG(debug) {
  # ��������� �������� ����� ������ ����� ������� _d
  TARGET_SUFFIX = _d

  OBJECTS_DIR = $$_PRO_FILE_PWD_/$$PLATFORM/debug/obj
  MOC_DIR = $$_PRO_FILE_PWD_/$$PLATFORM/debug/moc

  # ���������������� ����� ���� ������ ���������, ������������� Visual Studio
  QMAKE_CXXFLAGS_DEBUG += -Fd"$$OBJECTS_DIR/../"
}


# ��� ������ � ������ release
CONFIG(release) {
  OBJECTS_DIR = $$_PRO_FILE_PWD_/$$PLATFORM/release/obj
  MOC_DIR = $$_PRO_FILE_PWD_/$$PLATFORM/release/moc
  
  # ���������������� ����� ���� ������ ���������, ������������� Visual Studio
  QMAKE_CXXFLAGS_RELEASE += -Fd"$$OBJECTS_DIR/../"
}
TARGET = $$join(TARGET,,,$$TARGET_SUFFIX) 
