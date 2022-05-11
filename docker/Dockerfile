FROM debian:stable-slim

RUN apt-get update
RUN apt-get install -y build-essential \
                       ca-certificates \
                       cmake \
                       curl \
                       gnupg \
                       libev-dev \
                       locales \
                       lsb-release \
                       pkg-config
RUN rm -rf /var/lib/apt/lists/* && \
    localedef -i en_US -c -f UTF-8 -A /usr/share/locale/locale.alias en_US.UTF-8

# Set up PostgreSQL community repository.
RUN sh -c "curl https://www.postgresql.org/media/keys/ACCC4CF8.asc | gpg --dearmor | tee /etc/apt/trusted.gpg.d/apt.postgresql.org.gpg > /dev/null"
RUN sh -c 'echo "deb http://apt.postgresql.org/pub/repos/apt $(lsb_release -cs)-pgdg main" > /etc/apt/sources.list.d/pgdg.list'
RUN apt-get update

ARG PGVERSION
RUN apt-get install -y postgresql-server-dev-${PGVERSION}
