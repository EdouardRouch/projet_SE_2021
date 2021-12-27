all:
		$(MAKE) -C client
		$(MAKE) -C daemon

clean:
	$(MAKE) -C client clean
	$(MAKE) -C daemon clean
