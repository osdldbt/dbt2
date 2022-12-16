ARG DISTRO
FROM ${DISTRO}

RUN dnf -qy update && \
    dnf -qy install bison \
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
    dnf -q clean all

RUN groupadd postgres && \
    useradd -g postgres postgres
USER postgres
RUN ssh-keygen -t rsa -N '' -f ~/.ssh/id_rsa <<< y && \
    cp ~/.ssh/id_rsa.pub ~/.ssh/authorized_keys

# Install PostgreSQL from source.

USER root
ARG DBVER
RUN curl -o /tmp/postgresql-${DBVER}.tar.gz \
         -sOL https://ftp.postgresql.org/pub/source/v${DBVER}/postgresql-${DBVER}.tar.gz && \
    tar -C /usr/local/src -xf /tmp/postgresql-${DBVER}.tar.gz
WORKDIR /usr/local/src/postgresql-${DBVER}
RUN ./configure -q --prefix /usr && \
    make -s -j$(nproc) install && \
    rm -f /tmp/postgresql-${DBVER}.tar.gz

RUN echo "/usr/lib" > /etc/ld.so.conf.d/postgresql.conf && \
    ldconfig

# Install DBT-2.

COPY . /usr/local/src/dbt2
WORKDIR /usr/local/src/dbt2
ARG PKG_CONFIG_PATH="/usr/lib/pkgconfig"
ENV DBMS="$dbms"
RUN cmake -H. -Bbuilds/release && \
    (cd builds/release && make -s install)

RUN systemctl enable sshd
CMD [ "/usr/sbin/init" ]
