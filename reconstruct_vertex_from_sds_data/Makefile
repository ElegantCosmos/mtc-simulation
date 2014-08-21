TARGET=reconstruct_event
#DEBUG =
DEBUG = -g

all:
	g++ $(TARGET).cpp $(DEBUG) -o $(TARGET) `root-config --cflags` \
	-I$(MTCSDS_INSTALL_DIR)/include \
	`root-config --new --libs` \
	-L$(MTCSDS_INSTALL_DIR)/lib -lmtc-sds \
	-Wl,-rpath,$(MTCSDS_INSTALL_DIR)/lib

clean:
	rm $(TARGET)
