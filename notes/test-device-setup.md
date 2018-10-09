# Agile LoraWAN setup

Please note: **All passwords etc. mentioned in this file are only examples and not for use in ANY production device**

## Start the LoRaWAN stack

1) Login to the **Raspberry PI**
    * Username: `pi`
    * Password: `raspberry`
2) Start the `docker-compose` stack:
    ```shell
    cd ~/lorawan
    docker-compose up -d
    ```
3) Ensure that the `packer-forwarder` didn't fail during start
    * look at the log with: `docker-compose logs -f packet-forwarder`
    * If it failed during launc, abort with `docker-compose down` and go back to 2.
4) Grab a browser, login to the **LoRaServer** @ `http://<raspberry-ip>:8081`
    * Username: `admin`
    * Password: `admin`

### Configure the LoRaWAN stack

1) Create a **Network-server**
    * Name: `network-server`
    * Server: `loraserver:8000`
2) Create a **Gateway-profile**
    * Name: `gateway-profile`
    * Channels: `0,1,2`
    * Network-server: `network-server`
3) Create a **Service-profile**
    * Name: `service-profile`
    * Network-server: `network-server`
    * ✔️ in `Add gateway meta-data`
4) Create a **Device-profile**
    * Name: `device-profile`
    * Network-server: `network-server`
    * LoRaWAN MAC version: `1.0.0`
    * LoRaWAN Regional Parameters revision: `A`
5) Create a **Gateway**
    * Name: `gateway`
    * Description: `Agile LoRaWAN Gateway`
    * Gateway-ID: `0000000000010203`
    * Network-server: `network-server`
    * Network-server: `network-server`
    * Gateway-profile: `gateway-profile`
6) Create an **Application**
    * Name: `application`
    * Description: `Agile Framework Test Application`
    * Service-profile: `service-profile`
    * Payload codec: `Cayenne LPP`
7) Click on the newly created `application`
8) On the `DEVICES` tab, create a new devide
    * Name: `device-1`
    * Description: `device 1`
    * Device EUI: `4799b26900370056`
    * Device-profile: `device-profile`
    * ✔️ in `Disable frame-counter validation`
    * Click `CREATE DEVICE`
    * Device Address: `076e8bb8`
    * Network session encryption key, Serving network session integrity key, Forwarding network session integrity key: `aef22e2f9add468adbeb7f7e5d6059b0`
    * Application session key: `dc7d6106a3318c9dc0558827ae194b44`
    * Click `(RE)ACTIVATE DEVICE`
9) On the `DEVICES` tab, create a new devide
    * Name: `device-2`
    * Description: `device 2`
    * Device EUI: `4799b26900370057`
    * Device-profile: `device-profile`
    * ✔️ in `Disable frame-counter validation`
    * Click `CREATE DEVICE`
    * Device Address: `06de3d9b`
    * Network session encryption key, Serving network session integrity key, Forwarding network session integrity key: `c5d5dfbdef9f42779587d65312e4367a`
    * Application session key: `2461fce0304f8c2a596030d2d04e4b32`
    * Click `(RE)ACTIVATE DEVICE`
10) On the `DEVICES` tab, create a new devide
    * Name: `device-3`
    * Description: `device 3`
    * Device EUI: `4799b26900370058`
    * Device-profile: `device-profile`
    * ✔️ in `Disable frame-counter validation`
    * Click `CREATE DEVICE`
    * Device Address: `07a471c3`
    * Network session encryption key, Serving network session integrity key, Forwarding network session integrity key: `e012370615e8769473a527287f146af5`
    * Application session key: `296b0a118e81c6e16beb47198fa81c6c`
    * Click `(RE)ACTIVATE DEVICE`
11) On the `DEVICES` tab, create a new devide
    * Name: `device-4`
    * Description: `device 4`
    * Device EUI: `4799b26900370059`
    * Device-profile: `device-profile`
    * ✔️ in `Disable frame-counter validation`
    * Click `CREATE DEVICE`
    * Device Address: `068d1fae`
    * Network session encryption key, Serving network session integrity key, Forwarding network session integrity key: `43c29fd8b4cfcf452d8ace17f4f05308`
    * Application session key: `da0bd58c7c28e0dadae9b52651a231ea`
    * Click `(RE)ACTIVATE DEVICE`

## Start the Agile stack

1) Start the `docker-compose` stack:
    ```shell
    cd ~/agile-stack
    source .env

    ```
