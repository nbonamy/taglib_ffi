
build:
	cd src && cmake . && make

tests: build
	cd test && dart main.dart
