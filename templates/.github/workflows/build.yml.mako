<%!
    import pydmt.helpers.github
    import pydmt.helpers.python
%>name: build
on: [push, pull_request, workflow_dispatch]
jobs:
  build:
    runs-on: ${"${{ matrix.os }}"}
    strategy:
      matrix:
        include:
% for os, python in pydmt.helpers.github.get_workflows_platforms():
          - os: "${os}"
            python-version: "${python}"
% endfor
    steps:
    - name: checkout
      uses: actions/checkout@v4
    - name: cache-packages
      uses: actions/cache@v4
      with:
        path: |
          /var/cache/apt
          /var/lib/apt/lists
        key: ${"${{ matrix.os }}"}-apt-${"${{ hashFiles('**/package-list.txt') }}"}
        restore-keys: |
          ${"${{ matrix.os }}"}-apt-
    - name: cache-pip-packages
      uses: actions/cache@v4
      with:
        path: ~/.cache/pip
        key: ${"${{ matrix.os }}"}-pip-${"${{ hashFiles('**/requirements.txt') }}"}
        restore-keys: |
          ${"${{ matrix.os }}"}-pip-
    - name: python ${"${{ matrix.python-version }}"}
      uses: actions/setup-python@v5
      with:
        python-version: ${"${{ matrix.python-version }}"}
        cache: "pip"
    - name: bootstrap-pip
      run: python -m pip install --upgrade pip
    - name: bootstrap
      run: python -m pip install pydmt
    - name: pydmt
      run: pydmt build
