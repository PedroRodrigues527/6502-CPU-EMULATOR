FROM debian:latest

ARG USER_ID=1000
ARG GROUP_ID=1000

RUN groupadd -g $GROUP_ID appuser && useradd -m -u $USER_ID -g $GROUP_ID -s /bin/bash appuser

WORKDIR /app

COPY . /app

RUN apt-get update && apt-get install -y \
    g++ \
    make \
    gdb \
    && rm -rf /var/lib/apt/lists/*

RUN chown -R appuser:appuser /app
USER appuser

CMD ["/bin/bash"]