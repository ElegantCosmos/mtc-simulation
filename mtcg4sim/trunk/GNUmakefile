# --------------------------------------------------------------
# GNUmakefile for mtc simulation.  Michinari Sakai 2013/05/16.
# --------------------------------------------------------------

name := mtcg4sim
G4TARGET := $(name)
G4EXLIB := true
G4WORKDIR := .

ifndef G4INSTALL
  G4INSTALL = ../../../..
endif

.PHONY: all
all: CPPFLAGS += -O3
all: lib bin

.PHONY: debug
debug: CPPFLAGS += -g
debug: lib bin

CPPFLAGS += -DNOTET=1 -DNOVCGLIB=1 \
			`root-config --cflags` \
			-I$(CADMESH_INSTALL_DIR)/include \
			-I$(ASSIMP_INSTALL_DIR)/include \
			-I$(MTCSDS_INSTALL_DIR)/include

EXTRALIBS += `root-config --new --glibs` \
			 -L$(CADMESH_INSTALL_DIR)/lib -lcadmesh \
			 -L$(ASSIMP_INSTALL_DIR)/lib -lassimp \
			 -L$(MTCSDS_INSTALL_DIR)/lib -lmtc-sds \
			 -Wl,-rpath,$(ASSIMP_INSTALL_DIR)/lib \
			 -Wl,-rpath,$(CADMESH_INSTALL_DIR)/lib \
			 -Wl,-rpath,$(MTCSDS_INSTALL_DIR)/lib

include $(G4INSTALL)/config/architecture.gmk

include $(G4INSTALL)/config/binmake.gmk
