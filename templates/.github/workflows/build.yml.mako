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
    - name: Install OS packages
      run: python -m scripts.install
# This is needed for the python stuff to complie (stupid 20.10!)
    - name: Install libffi.so.7
      run: |
        apt install wget
        wget https://mirrors.edge.kernel.org/ubuntu/pool/main/libf/libffi/libffi7_3.3-4_amd64.deb
        dpkg --install libffi7_3.3-4_amd64.deb
    - name: Upgrade pip
      run: python -m pip install --upgrade pip
    - name: Install python requirements
      run: python -m pip install -r requirements.txt
    - name: System upgrade
      run: sudo apt-get -y update
    - name: Build
      run: make DO_STP=0
