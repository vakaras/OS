TESTS=pyemu

test:
	bin/test ${TESTS} \
		--with-coverage --cover-package="${TESTS}" \
		--cover-erase --cover-html --cover-html-dir=var/html

buildout:
	bin/buildout -v

bootstrap:
	python bootstrap.py

gui:
	bin/pyemu gui apps/pyemu/pyemu/test/test_program_1
