FROM centos:8

# Set up PostgreSQL community repository.
RUN dnf -y install https://download.postgresql.org/pub/repos/yum/reporpms/EL-8-x86_64/pgdg-redhat-repo-latest.noarch.rpm
RUN dnf -qy module disable postgresql

RUN dnf -y update
RUN dnf -y install cmake \
                   gcc \
                   make \
                   postgresql13-devel \
                   postgresql13-libs \
                   redhat-rpm-config
