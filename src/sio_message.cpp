#include "sioclient/sio_message.h"

namespace sio
{
  message::ptr message::from_json(const json &j)
  {
    if (j.is_number_integer())
    {
      return int_message::create(j.get<int64_t>());
    }
    else if (j.is_number_float())
    {
      return double_message::create(j.get<double>());
    }
    else if (j.is_string())
    {
      return string_message::create(j.get<std::string>());
    }
    else if (j.is_array())
    {
      auto array_message = sio::array_message::create();
      for (const auto &item : j)
      {
        array_message->get_vector().push_back(from_json(item));
      }
      return array_message;
    }
    else if (j.is_object())
    {
      auto obj_message = sio::object_message::create();
      for (auto it = j.begin(); it != j.end(); ++it)
      {
        obj_message->get_map()[it.key()] = from_json(it.value());
      }
      return obj_message;
    }
    else if (j.is_boolean())
    {
      return bool_message::create(j.get<bool>());
    }
    else if (j.is_null())
    {
      return null_message::create();
    }
    return message::ptr();
  }

  message::ptr message::from_json(const json &j, const std::vector<std::shared_ptr<const std::string>> &buffers)
  {
    if (j.is_number_integer())
    {
      return int_message::create(j.get<int64_t>());
    }
    else if (j.is_number_float())
    {
      return double_message::create(j.get<double>());
    }
    else if (j.is_string())
    {
      return string_message::create(j.get<std::string>());
    }
    else if (j.is_array())
    {
      auto array_message = sio::array_message::create();
      for (const auto &item : j)
      {
        auto val = from_json(item, buffers);
        array_message->get_vector().push_back(val);
      }
      return array_message;
    }
    else if (j.is_object())
    {
      if (j.contains("_placeholder") && j["_placeholder"] == true)
      {
        if (j.contains("num"))
        {
          auto num = j["num"];
          if (num.is_number() && num >= 0)
          {
            return binary_message::create(buffers[num]);
          }
          return message::ptr();
        }
        return message::ptr();
      }
      auto obj_message = sio::object_message::create();
      for (auto it = j.begin(); it != j.end(); ++it)
      {
        obj_message->get_map()[it.key()] = from_json(it.value(), buffers);
      }
      return obj_message;
    }
    else if (j.is_boolean())
    {
      return bool_message::create(j.get<bool>());
    }
    else if (j.is_null())
    {
      return null_message::create();
    }
    return message::ptr();
  }
} // namespace sio
