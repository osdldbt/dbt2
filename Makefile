default:
	@echo "targets: clean, debug, package, release"
	@echo "Don't forget to set DBMS in the environment:"
	@echo "    make release DBMS=pgsql"

UNAME_S := $(shell uname -s)

clean:
	-rm -rf builds

debug:
	cmake -H. -Bbuilds/debug -DCMAKE_BUILD_TYPE=Debug -DDBMS=$(DBMS)
	cd builds/debug && make

package:
	git checkout-index --prefix=builds/source/ -a
	cmake -Hbuilds/source -Bbuilds/source -DDBMS="cpack"
	cd builds/source && make package_source

release:
	cmake -H. -Bbuilds/release -DDBMS=$(DBMS)
	cd builds/release && make
