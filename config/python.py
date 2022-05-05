console_scripts = []
setup_requires = []
run_requires = []
test_requires = []
dev_requires = [
    'pymakehelper',  # for the makefile
    'pydmt',  # for building
]
install_requires = list(setup_requires)
install_requires.extend(run_requires)

extras_require = {}

python_requires = ">=3.9"
test_python=["3.9"]
test_container=['ubuntu:21.10']
