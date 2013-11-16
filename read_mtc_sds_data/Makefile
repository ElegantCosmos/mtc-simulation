all:
	g++ read_mtc_sds_data.cpp -o read_mtc_sds_data `root-config --cflags` \
	-I$(MTCSDS_INSTALL_DIR)/include \
	`root-config --new --libs` \
	-L$(MTCSDS_INSTALL_DIR)/lib -lmtc-sds \
	-Wl,-rpath,$(MTCSDS_INSTALL_DIR)/lib

clean:
	rm read_mtc_sds_data
