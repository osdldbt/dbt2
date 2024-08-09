# vim: set ft=make :

.PHONY: appimage clean debug package release test

default:
	@echo "targets: appimage (Linux only), clean, debug, package, release, test"

appimage:
	cmake -H. -Bbuilds/appimage -DCMAKE_INSTALL_PREFIX=/usr
	cd builds/appimage && make -s
	cd builds/appimage/src && sed -i -e 's#/usr#././#g' dbt2-client
	cd builds/appimage/src && sed -i -e 's#/usr#././#g' dbt2-client2
	cd builds/appimage/src && sed -i -e 's#/usr#././#g' dbt2-datagen
	cd builds/appimage/src && sed -i -e 's#/usr#././#g' dbt2-driver
	cd builds/appimage/src && sed -i -e 's#/usr#././#g' dbt2-driver2
	cd builds/appimage/src && sed -i -e 's#/usr#././#g' dbt2-driver3
	cd builds/appimage/src && sed -i -e 's#/usr#././#g' dbt2-rand
	cd builds/appimage/src && sed -i -e 's#/usr#././#g' dbt2-transaction-test
	cd builds/appimage/src && sed -i -e 's#/usr#././#g' dbt2-transaction-test
	cd builds/appimage && make -s install DESTDIR=AppDir
	cd builds/appimage && make -s appimage-podman

clean:
	-rm -rf builds

debug:
	cmake -H. -Bbuilds/debug -DCMAKE_BUILD_TYPE=Debug
	cd builds/debug && make

package:
	git checkout-index --prefix=builds/source/ -a
	cmake -Hbuilds/source -Bbuilds/source
	cd builds/source && make package_source

release:
	cmake -H. -Bbuilds/release
	cd builds/release && make

test: debug
	cd builds/debug && make test
