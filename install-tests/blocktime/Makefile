ifndef TUXDIR
$(error TUXDIR is not set)
endif

export PATH:=$(TUXDIR)/bin:$(PATH)
export LD_LIBRARY_PATH:=$(TUXDIR)/lib:$(LD_LIBRARY_PATH)
export TUXCONFIG:=$(CURDIR)/tuxconfig

check: server client tuxconfig
	-rm -f ULOG.*
	-tmipcrm -y
	tmboot -y
	./client
	tmshutdown -y
	grep -q ':TEST: SLEEP called' ULOG.*
	grep -q ':TEST: SERVICE called' ULOG.*

ubbconfig: ubbconfig.in
	cat $< \
          | sed s:@TUXDIR@:$(TUXDIR):g \
          | sed s:@UNAME@:`uname -n`:g \
          | sed s:@CURDIR@:$(CURDIR):g > $@

tuxconfig: ubbconfig
	tmloadcf -y $<

server: server.c
	buildserver -o $@ -f $< -s SERVICE -s SLEEP -v -f "-g -Wl,--no-as-needed"

client: client.c
	buildclient -o $@ -f $< -v -f "-g -Wl,--no-as-needed"

clean:
	-rm -f *.o ubbconfig tuxconfig client server ULOG.* stdout stderr access.*
