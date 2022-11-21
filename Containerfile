ARG DISTRO
FROM ${DISTRO}

RUN dnf -y update
RUN dnf -y install bison \
                   cmake \
                   flex \
                   gcc \
                   glibc-devel \
                   glibc-langpack-en \
                   hostname \
                   iproute \
                   libev-devel \
                   make \
                   openssh-clients \
                   openssh-server \
                   openssl-devel \
                   patch \
                   perf \
                   perl \
                   readline-devel \
                   rsync \
                   sysstat \
                   tar \
                   which \
                   zlib-devel && \
    dnf clean all

# Install PostgreSQL from source.

ARG DBVER
WORKDIR /tmp
RUN curl -OL https://ftp.postgresql.org/pub/source/v${DBVER}/postgresql-${DBVER}.tar.gz && \
    tar -C /usr/local/src -xvf postgresql-${DBVER}.tar.gz
WORKDIR /usr/local/src/postgresql-${DBVER}
RUN ./configure --prefix /usr && \
    make -j$(nproc) install && \
    rm /tmp/postgresql-${DBVER}.tar.gz

RUN echo "/usr/lib" > /etc/ld.so.conf.d/postgresql.conf && \
    ldconfig

# Install DBT-2.

COPY . /usr/local/src/dbt2
WORKDIR /usr/local/src/dbt2
ARG PKG_CONFIG_PATH="/usr/lib/pkgconfig"
ENV DBMS="$dbms"
RUN cmake -H. -Bbuilds/release && \
    (cd builds/release && make install)

RUN groupadd postgres && \
    useradd -g postgres postgres
USER postgres
RUN ssh-keygen -t rsa -N '' -f ~/.ssh/id_rsa <<< y && \
    cp ~/.ssh/id_rsa.pub ~/.ssh/authorized_keys

USER root
RUN systemctl enable sshd
CMD [ "/usr/sbin/init" ]
