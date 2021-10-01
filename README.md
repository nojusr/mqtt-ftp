# mqtt-ftp

## About

This is a small C daemon to allow for remote management of files via the MQTT protocol, or a unix domain socket.

## Why the name?

Because in terms of functionality, this is basically stripped down FTP on top of the MQTT protocol

## Usage

Run the daemon with a specified config (see `example.conf`), all input/output is formatted as json.

### Input format:
```
{
    "request_id": int,
    "command": "string",
    "file_path": "string",
    "write_input": "string" (optional)
}
```

## Output format:
```
{
    "request_id": int,
    "output": "string", (optional)
    "response": int (command return code)
}
```

### Specifics

* `request_id` will be reflected to be able to tell which message is which.

* possible `response` codes:
    - 0 is `RESP_OK`, self explanatory
    - 1 is `RESP_ERR`, used for general errors
    - 2 is `RESP_NOT_FOUND`, returned when a bad `file_path` has been provided
    - 3 is `RESP_MISSING_PERMS`, returned when the host machine doesn't have the permission to read/write a file.

* possible `command` values:
    - "read"
    - "write"
    - "create"
    - "delete"

## Dependencies

* mosquitto
* json-c
* libconfig

## Building

* `git clone` this repo
* `make`
