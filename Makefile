default:
	@echo "targets: clean, debug, package, release"
	@echo "Don't forget to set DBMS in the environment:"
	@echo "    make release DBMS=pgsql"

UNAME_S := $(shell uname -s)

clean:
	-rm -rf _builds

debug:
	cmake -H. -B_builds/debug -DCMAKE_BUILD_TYPE=Debug -DDBMS=$(DBMS)
	cd _builds/debug && make

package:
	git checkout-index --prefix=_builds/source/ -a
	cmake -H_builds/source -B_builds/source -DDBMS="cpack"
	cd _builds/source && make package_source

release:
	cmake -H. -B_builds/release -DDBMS=$(DBMS)
	cd _builds/release && make
