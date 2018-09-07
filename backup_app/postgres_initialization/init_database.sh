#!/bin/bash
set -e

psql -v ON_ERROR_STOP=1 -U "postgres" -d "postgres" <<-EOSQL
CREATE TABLE sensordata (
    deviceName  varchar(32) not null,
    devEUI      varchar(32) not null,
    time        timestamp not null,
    rssi        integer,
    loRaSNR     integer,
    temperatureSensor   real,
    humiditySensor      real,
    latitude            real,
    longitude           real,
    altitude            real
);
CREATE INDEX sensordata_time ON sensordata(time);
CREATE INDEX sensordata_time_deveuid ON sensordata(time,devEUI);
EOSQL
