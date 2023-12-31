
test:
	clib install --dev
	@$(CC) test.c src/geohash.c -std=c99 -I src -I deps -o $@
	@./$@

.PHONY: test
