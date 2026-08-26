FROM ubuntu:26.04

LABEL org.opencontainers.image.source=https://github.com/veltzer/demos-os-linux

ENV DEBIAN_FRONTEND=noninteractive

# gcc-15/g++-15 are pinned explicitly: the C/C++ processors compile with gcc
# and g++, and the clang processors resolve their libstdc++ headers out of the
# gcc tree via --gcc-install-dir=/usr/lib/gcc/x86_64-linux-gnu/15 (see
# rsconstruct.toml). Ubuntu 26.04 also ships a prerelease gcc-16 that carries
# no C++ headers, so the version is pinned rather than left to `g++`.
RUN apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates \
        curl \
        git \
        gh \
        gcc-15 \
        g++-15 \
        make \
        pkg-config \
        python3 \
        python3-venv \
        software-properties-common \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-15 100 \
        --slave /usr/bin/g++ g++ /usr/bin/g++-15 \
    && add-apt-repository universe \
    && apt-get update

RUN python3 -m venv /opt/venv
ENV PATH="/opt/venv/bin:${PATH}"

# CACHEBUST is set per build to ensure the curl below always refetches
# `latest` (otherwise the buildx layer cache would serve a stale binary
# even when a new rsconstruct release exists).
ARG CACHEBUST=1
RUN echo "cachebust=${CACHEBUST}" \
    && curl -fsSL "https://github.com/veltzer/rsconstruct/releases/latest/download/rsconstruct-linux-x86_64" \
        -o /usr/local/bin/rsconstruct \
    && chmod +x /usr/local/bin/rsconstruct \
    && rsconstruct version

WORKDIR /build
COPY rsconstruct.toml ./
# install-deps installs the [dependencies] section; install installs the
# external tools the enabled processors need (clang, ruff, mypy, rumdl, ...).
# Both are required: baking the tools into the image keeps the build job from
# having to install them on every run, and means the job no longer depends on
# the runtime binary's prompting behaviour.
RUN rsconstruct tools install-deps \
    && rsconstruct tools install
