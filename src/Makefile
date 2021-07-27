DIR_MODULES = base \
			  app \
              test

all:
	@for dir in $(DIR_MODULES); \
	do \
		$(MAKE) -C $$dir; \
		echo; \
	done

clean:
	@for dir in $(DIR_MODULES); \
	do \
		$(MAKE) -C $$dir clean; \
		echo; \
	done

format:
	@for dir in $(DIR_MODULES); \
	do \
		$(MAKE) -C $$dir format; \
		echo; \
	done

.PHONY: all clean format

.DEFAULT_GOAL: all