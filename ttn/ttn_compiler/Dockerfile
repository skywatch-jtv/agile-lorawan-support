FROM ubuntu

# update image, install build tools, set locale
RUN apt-get update -qq && apt-get install -yq \
  locales \
  build-essential \
  gcc \
  wget \
  git \
  autoconf \
  automake \
  libtool \
  python-dev \
  bsdtar; \
locale-gen en_DK.UTF-8; \
update-locale LC_ALL=en_DK.UTF-8 LANG=en_DK.UTF-8; \
wget -qO- https://bootstrap.pypa.io/get-pip.py | python -; \
python -m pip install --upgrade pip setuptools wheel; \
rm -rf /var/lib/apt/lists/* /tmp/*;
ENV LANG=en_DK.UTF-8 LANGUAGE=en_DK:en LC_ALL=en_DK.UTF-8 GOPATH="/work" PATH="$PATH:/usr/local/go/bin:/work/bin"

# install go1.10
RUN wget -qO- https://dl.google.com/go/go1.10.linux-armv6l.tar.gz | tar -C /usr/local -xzf-;

# install protobuf
RUN cd /tmp; \
wget -qO- https://github.com/google/googletest/archive/release-1.8.0.tar.gz | tar -xzf-; \
wget -qO- https://github.com/google/protobuf/archive/master.zip | bsdtar -xvf-; \
cd protobuf-master; \
ln -s ../googletest-release-1.8.0 gtest; \
./autogen.sh; \
./configure; \
make -j4; \
make install; \
ldconfig; \
cd python; \
python setup.py build; \
python setup.py install; \
rm -rf /tmp/*;

# install redis
RUN apt-get update -qq && apt-get install -yq \
  redis-server; \
rm -rf /var/lib/apt/lists/* /tmp/*;

# install RabbitMQ, include MQTT
RUN apt-get update -qq && apt 2>/dev/null install –y \
  erlang \
  logrotate; \
wget -qO /tmp/rabbitmq-server.deb https://github.com/rabbitmq/rabbitmq-server/releases/download/rabbitmq_v3_6_10/rabbitmq-server_3.6.10-1_all.deb && dpkg -i /tmp/rabbitmq-server.deb; \
apt 2>/dev/null -fyq install; \
wget -rqO /usr/local/bin/rabbitmqadmin https://raw.githubusercontent.com/rabbitmq/rabbitmq-management/v3.7.4/bin/rabbitmqadmin && chmod +x /usr/local/bin/rabbitmqadmin; \
rabbitmq-plugins enable rabbitmq_management; \
rabbitmq-plugins enable rabbitmq_mqtt; \
rm -rf /var/lib/apt/lists/* /tmp/*;

VOLUME ["/work"]
WORKDIR /work

COPY entrypoint.sh /bin
ENTRYPOINT ["/bin/entrypoint.sh"]
CMD ["/bin/bash"]
