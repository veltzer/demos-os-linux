FROM ubuntu:24.04

LABEL org.opencontainers.image.source=https://github.com/veltzer/demos-os-linux

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates \
        curl \
        git \
        gh \
        python3 \
        python3-venv \
        software-properties-common \
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
RUN rsconstruct tools install-deps --yes
