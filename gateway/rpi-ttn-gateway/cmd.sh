#!/usr/bin/env bash

# change reset pin assignment if configured and not default
defaultResetPin=7
if [ -z "$PI_RESET_PIN" ]; then
  echo "PI_RESET_PIN env variable not set, reset pin assignment remains as-is (pin $defaultResetPin)"
else
  if [ "$PI_RESET_PIN" == $defaultResetPin ]; then
    echo "PI_RESET_PIN env variable is the same as the default pin assignment -> no changes"
  else
    echo "Re-assigning reset pin from $defaultResetPin to ${PI_RESET_PIN}"
    sed -i -e "s/SX1301_RESET_BCM_PIN=25/SX1301_RESET_BCM_PIN=${PI_RESET_PIN}/g" ./start.sh
  fi
fi

# inject the values provided by the user into the template
envsubst < local_conf_template.json > local_conf.json

echo "using global conf:"
cat global_conf.json

echo "using local configuration:"
cat local_conf.json

# start script from https://github.com/ttn-zh/ic880a-gateway
./start.sh
