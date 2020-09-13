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

# test_os="[ubuntu-16.04, ubuntu-18.04, ubuntu-20.04]"
test_os="[ubuntu-20.04]"
# test_python="[3.6, 3.7, 3.8]"
test_python="[3.8]"

install_requires = list(setup_requires)
install_requires.extend(run_requires)

python_requires = ">=3.6"

extras_require = {
    # ':python_version == "2.7"': ['futures'],  # for python2.7 backport of concurrent.futures
}
