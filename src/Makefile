DIR_MODULES = base \
			  app \
              test

all:
	@for dir in $(DIR_MODULES); \
	do \
		$(MAKE) -C $$dir || exit $$?; \
		echo; \
	done

clean:
	@for dir in $(DIR_MODULES); \
	do \
		$(MAKE) -C $$dir clean || exit $$?; \
		echo; \
	done

format:
	@for dir in $(DIR_MODULES); \
	do \
		$(MAKE) -C $$dir format || exit $$?; \
		echo; \
	done

.PHONY: all clean format

.DEFAULT_GOAL: all