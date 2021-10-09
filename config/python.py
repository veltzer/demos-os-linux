console_scripts = [
]

setup_requires = [
]

run_requires = [
]

test_requires = [
]

dev_requires = [
    'pymakehelper',  # for the makefile
    'pydmt',  # for building
]

test_container="[ubuntu-20.04]"
test_python="[3.8]"

install_requires = list(setup_requires)
install_requires.extend(run_requires)

python_requires = ">=3.8"

extras_require = {
}
