console_scripts = [
]

setup_requires = [
]

run_requires = [
]

test_requires = [
]

dev_requires = [
    'setuptools', # for installing non wheel packages
    'pymakehelper',  # for the makefile
    'pydmt',  # for building
]

test_container="[ 'ubuntu:21.10' ]"
test_python="[3.9]"

install_requires = list(setup_requires)
install_requires.extend(run_requires)

python_requires = ">=3.9"

extras_require = {
}
