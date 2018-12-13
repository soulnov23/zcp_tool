include Inc.mk

DIR_DEP=dep

DIR_MODULES=comm \
			src

install:
	@for dir in $(DIR_DEP); do make -C $$dir; echo; done
	@for dir in $(DIR_DEP); do make -C $$dir clean; echo; done

all:
	@for dir in $(DIR_MODULES); do make -C $$dir; echo; done

clean:
	@for dir in $(DIR_MODULES); do make -C $$dir clean; echo; done