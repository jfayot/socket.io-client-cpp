# Socket.IO C++ Client

Fork from [socketio/socket.io-client-cpp](https://github.com/socketio/socket.io-client-cpp) which has not published any official release since 2021.

See original repo for documentation.

This version adds the following:

- CPM.cmake for dependency management
- nlohmann/json as a replacement of rapidjson (which has no official release since 2016!)
- preliminary msgpack parser support

Usage example:

```cpp
  sio::client sio;
  sio.set_parser_msgpack();

  sio.connect("http://localhost:3000/socket.io/", sio::message::from_json(
    {
      {"token", "Bearer " + std::getenv("APP_TOKEN")}
    }
  ));

  sio.socket()->on("person", sio::socket::event_listener([&](sio::event &event)
    {
      json j = event.get_message()->to_json();
      std::cout << "received: " << j.dump(2) << std::endl;
      sio.socket()->emit("echo", sio::message::from_json(j));
    }));
```
