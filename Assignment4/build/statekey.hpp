/****************************************************************

  Generated by Eclipse Cyclone DDS IDL to CXX Translator
  File name: /mnt/c/Users/yukev/Documents/School/4th Year/ELEC_477/Main/Assignment4/statekey.idl
  Source: /mnt/c/Users/yukev/Documents/School/4th Year/ELEC_477/Main/Assignment4/build/statekey.hpp
  Cyclone DDS: v0.11.0

*****************************************************************/
#ifndef DDSCXX__MNT_C_USERS_YUKEV_DOCUMENTS_SCHOOL_4TH_YEAR_ELEC_477_MAIN_ASSIGNMENT4_BUILD_STATEKEY_HPP_C1B094ED3FFC40FDB4430301D98EBFC5
#define DDSCXX__MNT_C_USERS_YUKEV_DOCUMENTS_SCHOOL_4TH_YEAR_ELEC_477_MAIN_ASSIGNMENT4_BUILD_STATEKEY_HPP_C1B094ED3FFC40FDB4430301D98EBFC5

#include <utility>
#include <ostream>
#include <cstdint>
#include <string>

namespace State
{
class Update
{
private:
 std::string icao24_;
 uint32_t timestamp_ = 0;
 double lat_ = 0.0;
 double lon_ = 0.0;
 float velocity_ = 0.0f;
 float heading_ = 0.0f;
 float vertrate_ = 0.0f;
 std::string callsign_;
 uint32_t squawk_ = 0;
 float baroaltitude_ = 0.0f;
 float geoaltitude_ = 0.0f;

public:
  Update() = default;

  explicit Update(
    const std::string& icao24,
    uint32_t timestamp,
    double lat,
    double lon,
    float velocity,
    float heading,
    float vertrate,
    const std::string& callsign,
    uint32_t squawk,
    float baroaltitude,
    float geoaltitude) :
    icao24_(icao24),
    timestamp_(timestamp),
    lat_(lat),
    lon_(lon),
    velocity_(velocity),
    heading_(heading),
    vertrate_(vertrate),
    callsign_(callsign),
    squawk_(squawk),
    baroaltitude_(baroaltitude),
    geoaltitude_(geoaltitude) { }

  const std::string& icao24() const { return this->icao24_; }
  std::string& icao24() { return this->icao24_; }
  void icao24(const std::string& _val_) { this->icao24_ = _val_; }
  void icao24(std::string&& _val_) { this->icao24_ = std::move(_val_); }
  uint32_t timestamp() const { return this->timestamp_; }
  uint32_t& timestamp() { return this->timestamp_; }
  void timestamp(uint32_t _val_) { this->timestamp_ = _val_; }
  double lat() const { return this->lat_; }
  double& lat() { return this->lat_; }
  void lat(double _val_) { this->lat_ = _val_; }
  double lon() const { return this->lon_; }
  double& lon() { return this->lon_; }
  void lon(double _val_) { this->lon_ = _val_; }
  float velocity() const { return this->velocity_; }
  float& velocity() { return this->velocity_; }
  void velocity(float _val_) { this->velocity_ = _val_; }
  float heading() const { return this->heading_; }
  float& heading() { return this->heading_; }
  void heading(float _val_) { this->heading_ = _val_; }
  float vertrate() const { return this->vertrate_; }
  float& vertrate() { return this->vertrate_; }
  void vertrate(float _val_) { this->vertrate_ = _val_; }
  const std::string& callsign() const { return this->callsign_; }
  std::string& callsign() { return this->callsign_; }
  void callsign(const std::string& _val_) { this->callsign_ = _val_; }
  void callsign(std::string&& _val_) { this->callsign_ = std::move(_val_); }
  uint32_t squawk() const { return this->squawk_; }
  uint32_t& squawk() { return this->squawk_; }
  void squawk(uint32_t _val_) { this->squawk_ = _val_; }
  float baroaltitude() const { return this->baroaltitude_; }
  float& baroaltitude() { return this->baroaltitude_; }
  void baroaltitude(float _val_) { this->baroaltitude_ = _val_; }
  float geoaltitude() const { return this->geoaltitude_; }
  float& geoaltitude() { return this->geoaltitude_; }
  void geoaltitude(float _val_) { this->geoaltitude_ = _val_; }

  bool operator==(const Update& _other) const
  {
    (void) _other;
    return icao24_ == _other.icao24_ &&
      timestamp_ == _other.timestamp_ &&
      lat_ == _other.lat_ &&
      lon_ == _other.lon_ &&
      velocity_ == _other.velocity_ &&
      heading_ == _other.heading_ &&
      vertrate_ == _other.vertrate_ &&
      callsign_ == _other.callsign_ &&
      squawk_ == _other.squawk_ &&
      baroaltitude_ == _other.baroaltitude_ &&
      geoaltitude_ == _other.geoaltitude_;
  }

  bool operator!=(const Update& _other) const
  {
    return !(*this == _other);
  }

};

std::ostream& operator<<(std::ostream& os, Update const& rhs);

} //namespace State

#include "dds/topic/TopicTraits.hpp"
#include "org/eclipse/cyclonedds/topic/datatopic.hpp"

namespace org {
namespace eclipse {
namespace cyclonedds {
namespace topic {

template <> constexpr const char* TopicTraits<::State::Update>::getTypeName()
{
  return "State::Update";
}

template <> constexpr bool TopicTraits<::State::Update>::isSelfContained()
{
  return false;
}

#ifdef DDSCXX_HAS_TYPELIB
template<> constexpr unsigned int TopicTraits<::State::Update>::type_map_blob_sz() { return 638; }
template<> constexpr unsigned int TopicTraits<::State::Update>::type_info_blob_sz() { return 100; }
template<> inline const uint8_t * TopicTraits<::State::Update>::type_map_blob() {
  alignas(4) static const uint8_t blob[] = {
 0xdb,  0x00,  0x00,  0x00,  0x01,  0x00,  0x00,  0x00,  0xf1,  0x83,  0xa0,  0xe6,  0x51,  0xd0,  0xe4,  0x22, 
 0x14,  0x68,  0x1f,  0x23,  0x53,  0xdb,  0x93,  0x00,  0xc3,  0x00,  0x00,  0x00,  0xf1,  0x51,  0x01,  0x00, 
 0x01,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0xb3,  0x00,  0x00,  0x00,  0x0b,  0x00,  0x00,  0x00, 
 0x0c,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x01,  0x00,  0x70,  0x00,  0xa8,  0xf1,  0x25,  0x5a, 
 0x0b,  0x00,  0x00,  0x00,  0x01,  0x00,  0x00,  0x00,  0x01,  0x00,  0x07,  0xd7,  0xe6,  0xd5,  0x5b,  0x00, 
 0x0b,  0x00,  0x00,  0x00,  0x02,  0x00,  0x00,  0x00,  0x01,  0x00,  0x0a,  0x40,  0x73,  0xd4,  0x1b,  0x00, 
 0x0b,  0x00,  0x00,  0x00,  0x03,  0x00,  0x00,  0x00,  0x01,  0x00,  0x0a,  0xa7,  0xbc,  0x1a,  0xd1,  0x00, 
 0x0b,  0x00,  0x00,  0x00,  0x04,  0x00,  0x00,  0x00,  0x01,  0x00,  0x09,  0xac,  0x1a,  0x45,  0x3d,  0x00, 
 0x0b,  0x00,  0x00,  0x00,  0x05,  0x00,  0x00,  0x00,  0x01,  0x00,  0x09,  0xf7,  0x4f,  0x53,  0x9f,  0x00, 
 0x0b,  0x00,  0x00,  0x00,  0x06,  0x00,  0x00,  0x00,  0x01,  0x00,  0x09,  0xff,  0x85,  0x37,  0xc5,  0x00, 
 0x0c,  0x00,  0x00,  0x00,  0x07,  0x00,  0x00,  0x00,  0x31,  0x00,  0x70,  0x00,  0x81,  0x3a,  0xbe,  0x31, 
 0x0b,  0x00,  0x00,  0x00,  0x08,  0x00,  0x00,  0x00,  0x01,  0x00,  0x07,  0xe1,  0xf0,  0xe4,  0xeb,  0x00, 
 0x0b,  0x00,  0x00,  0x00,  0x09,  0x00,  0x00,  0x00,  0x01,  0x00,  0x09,  0xb3,  0xcb,  0xcb,  0x36,  0x00, 
 0x0b,  0x00,  0x00,  0x00,  0x0a,  0x00,  0x00,  0x00,  0x01,  0x00,  0x09,  0xff,  0x74,  0xd2,  0x8a,  0x00, 
 0x72,  0x01,  0x00,  0x00,  0x01,  0x00,  0x00,  0x00,  0xf2,  0x64,  0x11,  0x2d,  0x8f,  0xf7,  0x5f,  0xce, 
 0x24,  0xc1,  0x19,  0x64,  0x60,  0x66,  0xe3,  0x00,  0x5a,  0x01,  0x00,  0x00,  0xf2,  0x51,  0x01,  0x00, 
 0x16,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x0e,  0x00,  0x00,  0x00,  0x53,  0x74,  0x61,  0x74, 
 0x65,  0x3a,  0x3a,  0x55,  0x70,  0x64,  0x61,  0x74,  0x65,  0x00,  0x00,  0x00,  0x36,  0x01,  0x00,  0x00, 
 0x0b,  0x00,  0x00,  0x00,  0x15,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x01,  0x00,  0x70,  0x00, 
 0x07,  0x00,  0x00,  0x00,  0x69,  0x63,  0x61,  0x6f,  0x32,  0x34,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, 
 0x18,  0x00,  0x00,  0x00,  0x01,  0x00,  0x00,  0x00,  0x01,  0x00,  0x07,  0x00,  0x0a,  0x00,  0x00,  0x00, 
 0x74,  0x69,  0x6d,  0x65,  0x73,  0x74,  0x61,  0x6d,  0x70,  0x00,  0x00,  0x00,  0x12,  0x00,  0x00,  0x00, 
 0x02,  0x00,  0x00,  0x00,  0x01,  0x00,  0x0a,  0x00,  0x04,  0x00,  0x00,  0x00,  0x6c,  0x61,  0x74,  0x00, 
 0x00,  0x00,  0x00,  0x00,  0x12,  0x00,  0x00,  0x00,  0x03,  0x00,  0x00,  0x00,  0x01,  0x00,  0x0a,  0x00, 
 0x04,  0x00,  0x00,  0x00,  0x6c,  0x6f,  0x6e,  0x00,  0x00,  0x00,  0x00,  0x00,  0x17,  0x00,  0x00,  0x00, 
 0x04,  0x00,  0x00,  0x00,  0x01,  0x00,  0x09,  0x00,  0x09,  0x00,  0x00,  0x00,  0x76,  0x65,  0x6c,  0x6f, 
 0x63,  0x69,  0x74,  0x79,  0x00,  0x00,  0x00,  0x00,  0x16,  0x00,  0x00,  0x00,  0x05,  0x00,  0x00,  0x00, 
 0x01,  0x00,  0x09,  0x00,  0x08,  0x00,  0x00,  0x00,  0x68,  0x65,  0x61,  0x64,  0x69,  0x6e,  0x67,  0x00, 
 0x00,  0x00,  0x00,  0x00,  0x17,  0x00,  0x00,  0x00,  0x06,  0x00,  0x00,  0x00,  0x01,  0x00,  0x09,  0x00, 
 0x09,  0x00,  0x00,  0x00,  0x76,  0x65,  0x72,  0x74,  0x72,  0x61,  0x74,  0x65,  0x00,  0x00,  0x00,  0x00, 
 0x17,  0x00,  0x00,  0x00,  0x07,  0x00,  0x00,  0x00,  0x31,  0x00,  0x70,  0x00,  0x09,  0x00,  0x00,  0x00, 
 0x63,  0x61,  0x6c,  0x6c,  0x73,  0x69,  0x67,  0x6e,  0x00,  0x00,  0x00,  0x00,  0x15,  0x00,  0x00,  0x00, 
 0x08,  0x00,  0x00,  0x00,  0x01,  0x00,  0x07,  0x00,  0x07,  0x00,  0x00,  0x00,  0x73,  0x71,  0x75,  0x61, 
 0x77,  0x6b,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x1b,  0x00,  0x00,  0x00,  0x09,  0x00,  0x00,  0x00, 
 0x01,  0x00,  0x09,  0x00,  0x0d,  0x00,  0x00,  0x00,  0x62,  0x61,  0x72,  0x6f,  0x61,  0x6c,  0x74,  0x69, 
 0x74,  0x75,  0x64,  0x65,  0x00,  0x00,  0x00,  0x00,  0x1a,  0x00,  0x00,  0x00,  0x0a,  0x00,  0x00,  0x00, 
 0x01,  0x00,  0x09,  0x00,  0x0c,  0x00,  0x00,  0x00,  0x67,  0x65,  0x6f,  0x61,  0x6c,  0x74,  0x69,  0x74, 
 0x75,  0x64,  0x65,  0x00,  0x00,  0x00,  0x00,  0x00,  0x22,  0x00,  0x00,  0x00,  0x01,  0x00,  0x00,  0x00, 
 0xf2,  0x64,  0x11,  0x2d,  0x8f,  0xf7,  0x5f,  0xce,  0x24,  0xc1,  0x19,  0x64,  0x60,  0x66,  0xe3,  0xf1, 
 0x83,  0xa0,  0xe6,  0x51,  0xd0,  0xe4,  0x22,  0x14,  0x68,  0x1f,  0x23,  0x53,  0xdb,  0x93, };
  return blob;
}
template<> inline const uint8_t * TopicTraits<::State::Update>::type_info_blob() {
  alignas(4) static const uint8_t blob[] = {
 0x60,  0x00,  0x00,  0x00,  0x01,  0x10,  0x00,  0x40,  0x28,  0x00,  0x00,  0x00,  0x24,  0x00,  0x00,  0x00, 
 0x14,  0x00,  0x00,  0x00,  0xf1,  0x83,  0xa0,  0xe6,  0x51,  0xd0,  0xe4,  0x22,  0x14,  0x68,  0x1f,  0x23, 
 0x53,  0xdb,  0x93,  0x00,  0xc7,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x04,  0x00,  0x00,  0x00, 
 0x00,  0x00,  0x00,  0x00,  0x02,  0x10,  0x00,  0x40,  0x28,  0x00,  0x00,  0x00,  0x24,  0x00,  0x00,  0x00, 
 0x14,  0x00,  0x00,  0x00,  0xf2,  0x64,  0x11,  0x2d,  0x8f,  0xf7,  0x5f,  0xce,  0x24,  0xc1,  0x19,  0x64, 
 0x60,  0x66,  0xe3,  0x00,  0x5e,  0x01,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x04,  0x00,  0x00,  0x00, 
 0x00,  0x00,  0x00,  0x00, };
  return blob;
}
#endif //DDSCXX_HAS_TYPELIB

} //namespace topic
} //namespace cyclonedds
} //namespace eclipse
} //namespace org

namespace dds {
namespace topic {

template <>
struct topic_type_name<::State::Update>
{
    static std::string value()
    {
      return org::eclipse::cyclonedds::topic::TopicTraits<::State::Update>::getTypeName();
    }
};

}
}

REGISTER_TOPIC_TYPE(::State::Update)

namespace org{
namespace eclipse{
namespace cyclonedds{
namespace core{
namespace cdr{

template<>
const propvec &get_type_props<::State::Update>();

namespace {
  static const volatile propvec &properties___State__Update = get_type_props<::State::Update>();
}

template<typename T, std::enable_if_t<std::is_base_of<cdr_stream, T>::value, bool> = true >
bool write(T& streamer, const ::State::Update& instance, const entity_properties_t *props) {
  (void)instance;
  member_id_set member_ids;
  if (!streamer.start_struct(*props))
    return false;
  auto prop = streamer.first_entity(props);
  while (prop) {
    switch (prop->m_id) {
      case 0:
      if (!streamer.start_member(*prop))
        return false;
      if (!write_string(streamer, instance.icao24(), 0))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 1:
      if (!streamer.start_member(*prop))
        return false;
      if (!write(streamer, instance.timestamp()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 2:
      if (!streamer.start_member(*prop))
        return false;
      if (!write(streamer, instance.lat()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 3:
      if (!streamer.start_member(*prop))
        return false;
      if (!write(streamer, instance.lon()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 4:
      if (!streamer.start_member(*prop))
        return false;
      if (!write(streamer, instance.velocity()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 5:
      if (!streamer.start_member(*prop))
        return false;
      if (!write(streamer, instance.heading()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 6:
      if (!streamer.start_member(*prop))
        return false;
      if (!write(streamer, instance.vertrate()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 7:
      if (!streamer.start_member(*prop))
        return false;
      if (!write_string(streamer, instance.callsign(), 0))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 8:
      if (!streamer.start_member(*prop))
        return false;
      if (!write(streamer, instance.squawk()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 9:
      if (!streamer.start_member(*prop))
        return false;
      if (!write(streamer, instance.baroaltitude()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 10:
      if (!streamer.start_member(*prop))
        return false;
      if (!write(streamer, instance.geoaltitude()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
    }
    prop = streamer.next_entity(prop);
  }
  return streamer.finish_struct(*props, member_ids);
}

template<typename S, std::enable_if_t<std::is_base_of<cdr_stream, S>::value, bool> = true >
bool write(S& str, const ::State::Update& instance, key_mode key) {
  const auto &props = get_type_props<::State::Update>();
  str.set_mode(cdr_stream::stream_mode::write, key);
  return write(str, instance, props.data()); 
}

template<typename T, std::enable_if_t<std::is_base_of<cdr_stream, T>::value, bool> = true >
bool read(T& streamer, ::State::Update& instance, const entity_properties_t *props) {
  (void)instance;
  member_id_set member_ids;
  if (!streamer.start_struct(*props))
    return false;
  auto prop = streamer.first_entity(props);
  while (prop) {
    switch (prop->m_id) {
      case 0:
      if (!streamer.start_member(*prop))
        return false;
      if (!read_string(streamer, instance.icao24(), 0))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 1:
      if (!streamer.start_member(*prop))
        return false;
      if (!read(streamer, instance.timestamp()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 2:
      if (!streamer.start_member(*prop))
        return false;
      if (!read(streamer, instance.lat()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 3:
      if (!streamer.start_member(*prop))
        return false;
      if (!read(streamer, instance.lon()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 4:
      if (!streamer.start_member(*prop))
        return false;
      if (!read(streamer, instance.velocity()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 5:
      if (!streamer.start_member(*prop))
        return false;
      if (!read(streamer, instance.heading()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 6:
      if (!streamer.start_member(*prop))
        return false;
      if (!read(streamer, instance.vertrate()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 7:
      if (!streamer.start_member(*prop))
        return false;
      if (!read_string(streamer, instance.callsign(), 0))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 8:
      if (!streamer.start_member(*prop))
        return false;
      if (!read(streamer, instance.squawk()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 9:
      if (!streamer.start_member(*prop))
        return false;
      if (!read(streamer, instance.baroaltitude()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 10:
      if (!streamer.start_member(*prop))
        return false;
      if (!read(streamer, instance.geoaltitude()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
    }
    prop = streamer.next_entity(prop);
  }
  return streamer.finish_struct(*props, member_ids);
}

template<typename S, std::enable_if_t<std::is_base_of<cdr_stream, S>::value, bool> = true >
bool read(S& str, ::State::Update& instance, key_mode key) {
  const auto &props = get_type_props<::State::Update>();
  str.set_mode(cdr_stream::stream_mode::read, key);
  return read(str, instance, props.data()); 
}

template<typename T, std::enable_if_t<std::is_base_of<cdr_stream, T>::value, bool> = true >
bool move(T& streamer, const ::State::Update& instance, const entity_properties_t *props) {
  (void)instance;
  member_id_set member_ids;
  if (!streamer.start_struct(*props))
    return false;
  auto prop = streamer.first_entity(props);
  while (prop) {
    switch (prop->m_id) {
      case 0:
      if (!streamer.start_member(*prop))
        return false;
      if (!move_string(streamer, instance.icao24(), 0))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 1:
      if (!streamer.start_member(*prop))
        return false;
      if (!move(streamer, instance.timestamp()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 2:
      if (!streamer.start_member(*prop))
        return false;
      if (!move(streamer, instance.lat()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 3:
      if (!streamer.start_member(*prop))
        return false;
      if (!move(streamer, instance.lon()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 4:
      if (!streamer.start_member(*prop))
        return false;
      if (!move(streamer, instance.velocity()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 5:
      if (!streamer.start_member(*prop))
        return false;
      if (!move(streamer, instance.heading()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 6:
      if (!streamer.start_member(*prop))
        return false;
      if (!move(streamer, instance.vertrate()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 7:
      if (!streamer.start_member(*prop))
        return false;
      if (!move_string(streamer, instance.callsign(), 0))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 8:
      if (!streamer.start_member(*prop))
        return false;
      if (!move(streamer, instance.squawk()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 9:
      if (!streamer.start_member(*prop))
        return false;
      if (!move(streamer, instance.baroaltitude()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 10:
      if (!streamer.start_member(*prop))
        return false;
      if (!move(streamer, instance.geoaltitude()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
    }
    prop = streamer.next_entity(prop);
  }
  return streamer.finish_struct(*props, member_ids);
}

template<typename S, std::enable_if_t<std::is_base_of<cdr_stream, S>::value, bool> = true >
bool move(S& str, const ::State::Update& instance, key_mode key) {
  const auto &props = get_type_props<::State::Update>();
  str.set_mode(cdr_stream::stream_mode::move, key);
  return move(str, instance, props.data()); 
}

template<typename T, std::enable_if_t<std::is_base_of<cdr_stream, T>::value, bool> = true >
bool max(T& streamer, const ::State::Update& instance, const entity_properties_t *props) {
  (void)instance;
  member_id_set member_ids;
  if (!streamer.start_struct(*props))
    return false;
  auto prop = streamer.first_entity(props);
  while (prop) {
    switch (prop->m_id) {
      case 0:
      if (!streamer.start_member(*prop))
        return false;
      if (!max_string(streamer, instance.icao24(), 0))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 1:
      if (!streamer.start_member(*prop))
        return false;
      if (!max(streamer, instance.timestamp()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 2:
      if (!streamer.start_member(*prop))
        return false;
      if (!max(streamer, instance.lat()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 3:
      if (!streamer.start_member(*prop))
        return false;
      if (!max(streamer, instance.lon()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 4:
      if (!streamer.start_member(*prop))
        return false;
      if (!max(streamer, instance.velocity()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 5:
      if (!streamer.start_member(*prop))
        return false;
      if (!max(streamer, instance.heading()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 6:
      if (!streamer.start_member(*prop))
        return false;
      if (!max(streamer, instance.vertrate()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 7:
      if (!streamer.start_member(*prop))
        return false;
      if (!max_string(streamer, instance.callsign(), 0))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 8:
      if (!streamer.start_member(*prop))
        return false;
      if (!max(streamer, instance.squawk()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 9:
      if (!streamer.start_member(*prop))
        return false;
      if (!max(streamer, instance.baroaltitude()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 10:
      if (!streamer.start_member(*prop))
        return false;
      if (!max(streamer, instance.geoaltitude()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
    }
    prop = streamer.next_entity(prop);
  }
  return streamer.finish_struct(*props, member_ids);
}

template<typename S, std::enable_if_t<std::is_base_of<cdr_stream, S>::value, bool> = true >
bool max(S& str, const ::State::Update& instance, key_mode key) {
  const auto &props = get_type_props<::State::Update>();
  str.set_mode(cdr_stream::stream_mode::max, key);
  return max(str, instance, props.data()); 
}

} //namespace cdr
} //namespace core
} //namespace cyclonedds
} //namespace eclipse
} //namespace org

#endif // DDSCXX__MNT_C_USERS_YUKEV_DOCUMENTS_SCHOOL_4TH_YEAR_ELEC_477_MAIN_ASSIGNMENT4_BUILD_STATEKEY_HPP_C1B094ED3FFC40FDB4430301D98EBFC5
