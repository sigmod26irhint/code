CC      = g++
CFLAGS  = -O3 -mavx -march=native -std=c++14 -w
LDFLAGS =



SOURCES = \
	utils.cpp \
	containers/relations.cpp \
	containers/offsets.cpp \
	indices/inverted_file.cpp \
	indices/1dgrid.cpp \
	indices/sharded_postinglist.cpp \
	indices/temporal_inverted_file.cpp \
	indices/hierarchical_index.cpp \
	indices/hint_m_subs+sort+cm.cpp \
	indices/hint_m_subs+sort+ss+cm.cpp \
	indices/temporal_inverted_file_slicing.cpp \
	indices/temporal_inverted_file_sharding.cpp \
	indices/temporal_inverted_file_hint_alpha.cpp \
	indices/temporal_inverted_file_hint_beta.cpp \
	indices/temporal_inverted_file_hint_gamma.cpp \
	indices/temporal_inverted_file_hint_gammah.cpp \
	indices/temporal_inverted_file_hint_delta.cpp \
	indices/temporal_inverted_file_hint_slicing.cpp \
	indices/irhint.cpp
OBJECTS = $(SOURCES:.cpp=.o)


all: lscan tif irhint tif_u irhint_u


lscan: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) \
	utils.o \
	containers/relations.o \
	main_lscan.cpp \
	-o query_lscan.exec $(LDADD)
	
tif: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) \
	utils.o \
	containers/relations.o \
	containers/offsets.o \
	indices/inverted_file.o \
	indices/1dgrid.o \
	indices/sharded_postinglist.o \
	indices/temporal_inverted_file.o \
	indices/hierarchical_index.o \
	indices/hint_m_subs+sort+cm.o \
	indices/hint_m_subs+sort+ss+cm.o \
	indices/temporal_inverted_file_slicing.o \
	indices/temporal_inverted_file_sharding.o \
	indices/temporal_inverted_file_hint_alpha.o \
	indices/temporal_inverted_file_hint_beta.o \
	indices/temporal_inverted_file_hint_gamma.o \
	indices/temporal_inverted_file_hint_gammah.o \
	indices/temporal_inverted_file_hint_delta.o \
	indices/temporal_inverted_file_hint_slicing.o \
	main_tif.cpp \
	-o query_tif.exec $(LDADD)

irhint: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) \
	utils.o \
	containers/relations.o \
	containers/offsets.o \
	indices/inverted_file.o \
	indices/temporal_inverted_file.o \
	indices/hierarchical_index.o \
	indices/irhint.o \
	main_irhint.cpp \
	-o query_irhint.exec $(LDADD)

tif_u: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) \
	utils.o \
	containers/relations.o \
	containers/offsets.o \
	indices/inverted_file.o \
	indices/1dgrid.o \
	indices/sharded_postinglist.o \
	indices/temporal_inverted_file.o \
	indices/hierarchical_index.o \
	indices/hint_m_subs+sort+cm.o \
	indices/hint_m_subs+sort+ss+cm.o \
	indices/temporal_inverted_file_slicing.o \
	indices/temporal_inverted_file_sharding.o \
	indices/temporal_inverted_file_hint_alpha.o \
	indices/temporal_inverted_file_hint_beta.o \
	indices/temporal_inverted_file_hint_gamma.o \
	indices/temporal_inverted_file_hint_gammah.o \
	indices/temporal_inverted_file_hint_delta.o \
	indices/temporal_inverted_file_hint_slicing.o \
	main_tif_update.cpp \
	-o update_tif.exec $(LDADD)

irhint_u: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) \
	utils.o \
	containers/relations.o \
	containers/offsets.o \
	indices/inverted_file.o \
	indices/temporal_inverted_file.o \
	indices/hierarchical_index.o \
	indices/irhint.o \
	main_irhint_update.cpp \
	-o update_irhint.exec $(LDADD)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf utils.o
	rm -rf containers/*.o
	rm -rf indices/*.o
	rm -rf query_lscan.exec
	rm -rf query_tif.exec
	rm -rf query_irhint.exec
	rm -rf update_tif.exec
	rm -rf update_irhint.exec
