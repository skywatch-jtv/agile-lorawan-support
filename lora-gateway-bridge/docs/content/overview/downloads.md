---
title: Downloads
menu:
    main:
        parent: overview
        weight: 2
---

# Downloads

## Precompiled binaries

| File name                                                                                                                                             | OS      | Arch  |
| ----------------------------------------------------------------------------------------------------------------------------------------------------- | ------- | ----- |
| [lora-gateway-bridge_{{< version >}}_darwin_amd64.tar.gz](https://dl.loraserver.io/tar/lora-gateway-bridge_{{< version >}}_darwin_amd64.tar.gz)       | OS X    | amd64 |
| [lora-gateway-bridge_{{< version >}}_linux_386.tar.gz](https://dl.loraserver.io/tar/lora-gateway-bridge_{{< version >}}_linux_386.tar.gz)             | Linux   | 386   |
| [lora-gateway-bridge_{{< version >}}_linux_amd64.tar.gz](https://dl.loraserver.io/tar/lora-gateway-bridge_{{< version >}}_linux_amd64.tar.gz)         | Linux   | amd64 |
| [lora-gateway-bridge_{{< version >}}_linux_armv5.tar.gz](https://dl.loraserver.io/tar/lora-gateway-bridge_{{< version >}}_linux_armv5.tar.gz)         | Linux   | armv5 |
| [lora-gateway-bridge_{{< version >}}_linux_armv6.tar.gz](https://dl.loraserver.io/tar/lora-gateway-bridge_{{< version >}}_linux_armv6.tar.gz)         | Linux   | armv6 |
| [lora-gateway-bridge_{{< version >}}_linux_armv7.tar.gz](https://dl.loraserver.io/tar/lora-gateway-bridge_{{< version >}}_linux_armv7.tar.gz)         | Linux   | armv7 |
| [lora-gateway-bridge_{{< version >}}_linux_arm64.tar.gz](https://dl.loraserver.io/tar/lora-gateway-bridge_{{< version >}}_linux_arm64.tar.gz)         | Linux   | arm64 |
| [lora-gateway-bridge_{{< version >}}_windows_386.tar.gz](https://dl.loraserver.io/tar/lora-gateway-bridge_{{< version >}}_windows_386.tar.gz)         | Windows | 386   |
| [lora-gateway-bridge_{{< version >}}_windows_amd64.tar.gz](https://dl.loraserver.io/tar/lora-gateway-bridge_{{< version >}}_windows_amd64.tar.gz)     | Windows | amd64 |

## Debian / Ubuntu packages

| File name                                                                                                                   | OS      | Arch  |
| ----------------------------------------------------------------------------------------------------------------------------| ------- | ----- |
| [lora-gateway-bridge_{{< version >}}_i386.deb](https://dl.loraserver.io/deb/lora-gateway-bridge_{{< version >}}_i386.deb)   | Linux   | 386   |
| [lora-gateway-bridge_{{< version >}}_amd64.deb](https://dl.loraserver.io/deb/lora-gateway-bridge_{{< version >}}_amd64.deb) | Linux   | amd64 |
| [lora-gateway-bridge_{{< version >}}_armhf.deb](https://dl.loraserver.io/deb/lora-gateway-bridge_{{< version >}}_armhf.deb) | Linux   | arm   |
| [lora-gateway-bridge_{{< version >}}_arm64.deb](https://dl.loraserver.io/deb/lora-gateway-bridge_{{< version >}}_arm64.deb) | Linux   | arm64 |

## Debian Ubuntu repository

```bash
sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 1CE2AFD36DBCCA00

export DISTRIB_ID=`lsb_release -si`
export DISTRIB_CODENAME=`lsb_release -sc`
sudo echo "deb https://repos.loraserver.io/${DISTRIB_ID,,} ${DISTRIB_CODENAME} testing" | sudo tee /etc/apt/sources.list.d/loraserver.list
sudo apt-get update
```

## Docker images

For Docker images, please refer to https://hub.docker.com/r/loraserver/lora-gateway-bridge/.
