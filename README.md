# arduino-openhab-sonos-controller
A touchable (hardware) interface to control sonos speakers via openhab2 smart home system.


# Run Openhab2 REST API Mock Server

The local runnable python3 based mock server emulated the /items resources of the Openhab2 REST API. This allows you to test your arduino code without having accdess to your OH2 server.

Python3 must be installed

```
> python OpenHabRestApiMockServer.py
```

On linux use Ctrl-z and kill %1 to shutdown server.

Test your mock server with curl:

```
# request presets
> curl http://127.0.0.1:8080/rest/items/SonosPresets/state

# select playlist
> curl -X PUT -H "Content-Type: text/plain" --data-binary "Playlist 1"  http://127.0.0.1:8080/rest/items/ROOM_SelectedSonosPreset/state

# check selected playlist
> curl http://127.0.0.1:8080/rest/items/ROOM_SelectedSonosPreset/state
```

# OpenHAB 2 Setup

Configure your Sonos binding in OH2 according to the documentation: https://docs.openhab.org/addons/bindings/sonos/readme.html

Define the following items e.g. in file __sonos.items__

```
String SonosPresets
String ROOM_SelectedSonosPreset
```

**ROOM_SelectedSonosPreset** contains the selected preset set by an Arduino call to the OH2 REST API. Replace ROOM with your room name.

Define these rules e.g. in file __sonos.rules__

```
rule "Setup SonosPresets"
when
    System started
then
    SonosPresets.postUpdate("RadioEins, Deutschlandfunk, Gute Musik, Blues")
end

rule "React on Sonos Preset Selection - ROOM"
when
    ROOM_SelectedSonosPreset changed
then
    SonosPlay1Play.sendCommand(ROOM_SelectedSonosPreset.state)
end
```

