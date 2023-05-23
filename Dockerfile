FROM ubuntu:22.04

RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install -y \
      sudo \
      bash-completion \
      apt-utils \
      build-essential \
      make \
      git
ENV USER=erad USER_ID=1000 USER_GID=1000

RUN groupadd --gid "${USER_GID}" "${USER}" && \
    useradd \
      --uid ${USER_ID} \
      --gid ${USER_GID} \
      --create-home \
      --shell /bin/bash \
      ${USER} && \
      echo ${USER}:${USER} | chpasswd &&  \
      adduser ${USER} sudo

COPY user-mapping.sh /
RUN  chmod u+x user-mapping.sh

RUN apt-get install -y \
      libpmem1 libpmem-dev libpmem1-debug \
      libpmemobj1 libpmemobj-dev libpmemobj1-debug \
      libpmemobj-cpp-dev libpmemobj-cpp-doc \
      libpmemblk1 libpmemblk-dev libpmemblk1-debug \
      libpmemlog1 libpmemlog-dev libpmemlog1-debug \
      libpmempool1 libpmempool-dev libpmempool1-debug \
      libpmemkv1 libpmemkv-dev \
      pmdk-tools

RUN mkdir /daxfs
RUN chmod a+rwx /daxfs

ENTRYPOINT ["/user-mapping.sh"]