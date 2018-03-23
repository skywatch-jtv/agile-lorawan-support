# TTN Images for the RPI

Scripts for creating docker images with the entire TTN stack on the Raspberry Pi.


# Know Issues

* docker-compose file not working ans still incomplete!
* configurations are baked into the images by the scripts in order to avoid tedious and errorprone manual steps - especially at this early point


## Usage

### Compiling the images

```bash
cd ttn
./build.sh [TTN git tag]
```

If the `TTN git tag` is not supplied, the scripts will checkout and compile the `master`branch.

Generates multiple TTN docker images, intended to work together in order to provide a complete TTN experience.


### Startup

TBD...

