FROM rockylinux:8

# Set up PostgreSQL community repository.
RUN dnf -y install https://download.postgresql.org/pub/repos/yum/reporpms/EL-8-x86_64/pgdg-redhat-repo-latest.noarch.rpm
RUN dnf -qy module disable postgresql

ARG PGVERSION

RUN dnf -y update
RUN dnf -y install cmake \
                   gcc \
                   glibc-langpack-en \
                   libev-devel \
                   openssl-devel \
                   make \
                   postgresql${PGVERSION}-devel \
                   postgresql${PGVERSION}-libs \
                   redhat-rpm-config
