CC = gcc

PRGNAME = wl_RaMen
IDIR = include -I/usr/include/freetype2
BUILDDIR = build
TESTDIR = src/tests

LIB_DIRS = lib
SOURCE_DIRS = src
HEADER_DIR = src/headers
WLXML_DIR = wl_XML
TESTS = 

CFLAGS = -Wall -Wextra -fstack-protector -I $(IDIR) -I $(HEADER_DIR) -L $(LIB_DIRS) -lwayland-client  -lrt -lm -lfreetype
LDFLAGS = -I $(IDIR) -L$(BUILDDIR) -L$(LIB_DIRS) -lwayland-client -lrt -lm -lfreetype
OPTIMIZATION = -Og

TESTPATH = $(BUILDDIR)/$(TESTDIR)
SOURCES = $(foreach dir, $(SOURCE_DIRS), $(wildcard $(dir)/*.c))
OBJS = $(patsubst %.c, $(BUILDDIR)/%.o, $(SOURCES))

all: run

run: tree WLXML exec
	@./$(BUILDDIR)/$(PRGNAME)

tree:
	@printf "Building directory tree\n"
	@mkdir -p $(TESTPATH)/$$dir;
	@for dir in $(SOURCE_DIRS); do \
		mkdir -p $(BUILDDIR)/$$dir; \
	done

release: OPTIMIZATION=-O3
release: tree WLXML exec
test: tree $(TESTS)

exec: $(OBJS)
	@printf "Building Executable\n"
	@printf "\e[1;35m(LD)\e[m $(BUILDDIR)/$(PRGNAME)"
	@$(CC) -o $(BUILDDIR)/$(PRGNAME) $^ $(OPIMIZATION) $(LDFLAGS)
	@printf " [DONE]\n"


WLXML:
	@printf "Generating boilerplate and header from XMLs\n"
	@wayland-scanner private-code \
	  < $(WLXML_DIR)/wlr-layer-shell-unstable-v1.xml \
	  > $(SOURCE_DIRS)/wlr-shell-protocol.c
	@wayland-scanner client-header \
	  < $(WLXML_DIR)/wlr-layer-shell-unstable-v1.xml \
	  > $(HEADER_DIR)/wlr-shell-client-protocol.h
	@wayland-scanner client-header \
	  < /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml \
	  > $(HEADER_DIR)/xdg-shell-client-protocol.h
	@wayland-scanner private-code \
	  < /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml \
	  > $(SOURCE_DIRS)/xdg-shell-protocol.c


$(TESTS): $(OBJS)
	@printf "(CC) $(TESTPATH)/$@.o\n"
	@$(CC) -c -o $(TESTPATH)/$@.o $(TESTDIR)/$@.c $(OPTIMIZATION) $(CFLAGS)
	@printf "\e[1;32m(LD)\e[m $(TESTPATH)/$@\n"
	@$(CC) -o $(TESTPATH)/$@ $(TESTPATH)/$@.o $(OPIMIZATION) $(LDFLAGS)

$(BUILDDIR)/%.o: %.c
	@printf "(CC) $@"
	@$(CC) -c -o $@ $^ $(OPTIMIZATION) $(CFLAGS)
	@printf " [DONE]\n"


.PHONY: clean tree library static dynamic

clean:
	rm -rf $(BUILDDIR)
