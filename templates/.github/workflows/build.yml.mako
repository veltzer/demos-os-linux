<%!
    import config.python
%>name: build
on: [push, pull_request]
jobs:
  build:
    name: Build on ${"${{ matrix.container }}"}
    runs-on: ubuntu-latest
    container: ${"${{ matrix.container }}"}
    strategy:
      matrix:
        container: [ 'ubuntu:20.10' ]
        python-version: [3.8]
    steps:
    - uses: actions/checkout@v2
    - name: Set up Python ${"${{ matrix.python-version }}"}
      uses: actions/setup-python@v2
      with:
        python-version: ${"${{ matrix.python-version }}"}
    - name: Install python dependencies
      run: |
        python -m pip install --user --upgrade pip
        pip install --user -r requirements.txt
    - name: Install OS dependencies
      run: |
        sudo apt-get -y update
        python -m scripts.install
    - name: Build
      run: make
