DIR_MODULES = comm \
			  src \
              example

all:
	@for dir in $(DIR_MODULES); \
	do \
		make -C $$dir; \
		echo; \
	done

clean:
	@for dir in $(DIR_MODULES); \
	do \
		make -C $$dir clean; \
		echo; \
	done

format:
	@for dir in $(DIR_MODULES); \
	do \
		make -C $$dir format; \
		echo; \
	done