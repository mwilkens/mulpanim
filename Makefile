OBJ = mulpanim.o

ALLEGRO_FLAGS = $(shell pkg-config allegro-5 allegro_font-5 allegro_image-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5 --libs --cflags)

ifeq ($(shell echo $(notdir $(CXX)) | head -c3),g++)
IS_GCC = 1
endif
ifeq ($(shell echo $(notdir $(CXX)) | head -c7),clang++)
IS_CLANG = 1
endif
ifeq ($(shell echo $(notdir $(CXX)) | head -c4),icpc)
IS_ICPC = 1
endif

CXXVER = $(CXX)
CFLAGS = -I.. -Wall -Wextra -Wfatal-errors
LIBS = -lm
ifdef IS_GCC
CXXVER = $(shell $(CXX) -v 2>&1 | tail -n 1)
endif
ifdef IS_CLANG
CXXVER = $(shell $(CXX) -v 2>&1 | head -n 1)
endif
ifdef IS_ICPC
CXXVER = $(shell $(CXX) -v 2>&1)
CFLAGS = -I./include/
LIBS =
endif

#--------------------------------------------------
# Set compilation flags allowing to customize CImg
#--------------------------------------------------

# Flags to enable strict code standards
ifeq ($(notdir $(CXX)),icpc)
ANSI_CFLAGS = -std=c++11
else
ANSI_CFLAGS = -std=c++11 -pedantic
endif

# Flags to enable code debugging.
DEBUG_CFLAGS = -Dcimg_verbosity=3 -Dcimg_strict_warnings -g -fsanitize=address

# Flags to enable color output messages.
# (requires a VT100 compatible terminal)
VT100_CFLAGS = -Dcimg_use_vt100

# Flags to enable code optimization by the compiler.
OPT_CFLAGS = -Ofast
ifdef IS_GCC
OPT_CFLAGS = -Ofast -mtune=generic
endif
ifdef IS_ICPC
OPT_CFLAGS = -fast
endif

#-------------------------
# Define Makefile entries
#-------------------------
%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(ALLEGRO_FLAGS) $(CFLAGS) $(CONF_CFLAGS) $(LIBS) $(CONF_LIBS)

mulpanim: $(OBJ)
	@echo
	@echo "** Compiling '$* ($(SVERSION))' with '$(CXXVER)'"
	@echo
	$(CXX) -o $@ $^ $(ALLEGRO_FLAGS) $(CFLAGS) $(CONF_CFLAGS) $(LIBS) $(CONF_LIBS)

clean:
	rm -rf *.exe *.o *.obj *~ \#* $(FILES) $(EXTRA_FILES)
ifneq ($(EXE_PRE),)
	rm -f $(EXE_PRE)*
endif
