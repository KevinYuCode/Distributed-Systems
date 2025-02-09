/****************************************************************

  Generated by Eclipse Cyclone DDS IDL to CXX Translator
  File name: /mnt/c/Users/yukev/Documents/School/4th Year/ELEC_477/Main/Assignment4/track_id_queryInstance/track.idl
  Source: /mnt/c/Users/yukev/Documents/School/4th Year/ELEC_477/Main/Assignment4/track_id_queryInstance/build/track.hpp
  Cyclone DDS: v0.11.0

*****************************************************************/
#ifndef DDSCXX__MNT_C_USERS_YUKEV_DOCUMENTS_SCHOOL_4TH_YEAR_ELEC_477_MAIN_ASSIGNMENT4_TRACK_ID_QUERYINSTANCE_BUILD_TRACK_HPP_37FAD95855A143C1636E67205C99F80E
#define DDSCXX__MNT_C_USERS_YUKEV_DOCUMENTS_SCHOOL_4TH_YEAR_ELEC_477_MAIN_ASSIGNMENT4_TRACK_ID_QUERYINSTANCE_BUILD_TRACK_HPP_37FAD95855A143C1636E67205C99F80E

#include <utility>
#include <ostream>
#include <cstdint>

namespace Radar
{
class Track
{
private:
 uint32_t id_ = 0;
 uint64_t timestamp_ = 0;
 double strt_angle_ = 0.0;
 double end_angle_ = 0.0;
 double distance_ = 0.0;

public:
  Track() = default;

  explicit Track(
    uint32_t id,
    uint64_t timestamp,
    double strt_angle,
    double end_angle,
    double distance) :
    id_(id),
    timestamp_(timestamp),
    strt_angle_(strt_angle),
    end_angle_(end_angle),
    distance_(distance) { }

  uint32_t id() const { return this->id_; }
  uint32_t& id() { return this->id_; }
  void id(uint32_t _val_) { this->id_ = _val_; }
  uint64_t timestamp() const { return this->timestamp_; }
  uint64_t& timestamp() { return this->timestamp_; }
  void timestamp(uint64_t _val_) { this->timestamp_ = _val_; }
  double strt_angle() const { return this->strt_angle_; }
  double& strt_angle() { return this->strt_angle_; }
  void strt_angle(double _val_) { this->strt_angle_ = _val_; }
  double end_angle() const { return this->end_angle_; }
  double& end_angle() { return this->end_angle_; }
  void end_angle(double _val_) { this->end_angle_ = _val_; }
  double distance() const { return this->distance_; }
  double& distance() { return this->distance_; }
  void distance(double _val_) { this->distance_ = _val_; }

  bool operator==(const Track& _other) const
  {
    (void) _other;
    return id_ == _other.id_ &&
      timestamp_ == _other.timestamp_ &&
      strt_angle_ == _other.strt_angle_ &&
      end_angle_ == _other.end_angle_ &&
      distance_ == _other.distance_;
  }

  bool operator!=(const Track& _other) const
  {
    return !(*this == _other);
  }

};

std::ostream& operator<<(std::ostream& os, Track const& rhs);

} //namespace Radar

#include "dds/topic/TopicTraits.hpp"
#include "org/eclipse/cyclonedds/topic/datatopic.hpp"

namespace org {
namespace eclipse {
namespace cyclonedds {
namespace topic {

template <> constexpr const char* TopicTraits<::Radar::Track>::getTypeName()
{
  return "Radar::Track";
}

#ifdef DDSCXX_HAS_TYPELIB
template<> constexpr unsigned int TopicTraits<::Radar::Track>::type_map_blob_sz() { return 374; }
template<> constexpr unsigned int TopicTraits<::Radar::Track>::type_info_blob_sz() { return 100; }
template<> inline const uint8_t * TopicTraits<::Radar::Track>::type_map_blob() {
  alignas(4) static const uint8_t blob[] = {
 0x7b,  0x00,  0x00,  0x00,  0x01,  0x00,  0x00,  0x00,  0xf1,  0x65,  0x81,  0x82,  0x48,  0x1b,  0xc8,  0x23, 
 0xa1,  0xa8,  0x5d,  0xd3,  0xf6,  0x6c,  0x20,  0x00,  0x63,  0x00,  0x00,  0x00,  0xf1,  0x51,  0x01,  0x00, 
 0x01,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x53,  0x00,  0x00,  0x00,  0x05,  0x00,  0x00,  0x00, 
 0x0b,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x31,  0x00,  0x07,  0xb8,  0x0b,  0xb7,  0x74,  0x00, 
 0x0b,  0x00,  0x00,  0x00,  0x01,  0x00,  0x00,  0x00,  0x01,  0x00,  0x08,  0xd7,  0xe6,  0xd5,  0x5b,  0x00, 
 0x0b,  0x00,  0x00,  0x00,  0x02,  0x00,  0x00,  0x00,  0x01,  0x00,  0x0a,  0xc2,  0xa5,  0xbf,  0x81,  0x00, 
 0x0b,  0x00,  0x00,  0x00,  0x03,  0x00,  0x00,  0x00,  0x01,  0x00,  0x0a,  0x10,  0x48,  0xd8,  0x25,  0x00, 
 0x0b,  0x00,  0x00,  0x00,  0x04,  0x00,  0x00,  0x00,  0x01,  0x00,  0x0a,  0xa7,  0x4e,  0xc9,  0xc5,  0x00, 
 0xcb,  0x00,  0x00,  0x00,  0x01,  0x00,  0x00,  0x00,  0xf2,  0x4c,  0x28,  0x68,  0x79,  0xfa,  0x68,  0x2d, 
 0x9a,  0xdc,  0x4e,  0x54,  0x69,  0xd0,  0x26,  0x00,  0xb3,  0x00,  0x00,  0x00,  0xf2,  0x51,  0x01,  0x00, 
 0x15,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x0d,  0x00,  0x00,  0x00,  0x52,  0x61,  0x64,  0x61, 
 0x72,  0x3a,  0x3a,  0x54,  0x72,  0x61,  0x63,  0x6b,  0x00,  0x00,  0x00,  0x00,  0x8f,  0x00,  0x00,  0x00, 
 0x05,  0x00,  0x00,  0x00,  0x11,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x31,  0x00,  0x07,  0x00, 
 0x03,  0x00,  0x00,  0x00,  0x69,  0x64,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x18,  0x00,  0x00,  0x00, 
 0x01,  0x00,  0x00,  0x00,  0x01,  0x00,  0x08,  0x00,  0x0a,  0x00,  0x00,  0x00,  0x74,  0x69,  0x6d,  0x65, 
 0x73,  0x74,  0x61,  0x6d,  0x70,  0x00,  0x00,  0x00,  0x19,  0x00,  0x00,  0x00,  0x02,  0x00,  0x00,  0x00, 
 0x01,  0x00,  0x0a,  0x00,  0x0b,  0x00,  0x00,  0x00,  0x73,  0x74,  0x72,  0x74,  0x5f,  0x61,  0x6e,  0x67, 
 0x6c,  0x65,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x18,  0x00,  0x00,  0x00,  0x03,  0x00,  0x00,  0x00, 
 0x01,  0x00,  0x0a,  0x00,  0x0a,  0x00,  0x00,  0x00,  0x65,  0x6e,  0x64,  0x5f,  0x61,  0x6e,  0x67,  0x6c, 
 0x65,  0x00,  0x00,  0x00,  0x17,  0x00,  0x00,  0x00,  0x04,  0x00,  0x00,  0x00,  0x01,  0x00,  0x0a,  0x00, 
 0x09,  0x00,  0x00,  0x00,  0x64,  0x69,  0x73,  0x74,  0x61,  0x6e,  0x63,  0x65,  0x00,  0x00,  0x00,  0x00, 
 0x22,  0x00,  0x00,  0x00,  0x01,  0x00,  0x00,  0x00,  0xf2,  0x4c,  0x28,  0x68,  0x79,  0xfa,  0x68,  0x2d, 
 0x9a,  0xdc,  0x4e,  0x54,  0x69,  0xd0,  0x26,  0xf1,  0x65,  0x81,  0x82,  0x48,  0x1b,  0xc8,  0x23,  0xa1, 
 0xa8,  0x5d,  0xd3,  0xf6,  0x6c,  0x20, };
  return blob;
}
template<> inline const uint8_t * TopicTraits<::Radar::Track>::type_info_blob() {
  alignas(4) static const uint8_t blob[] = {
 0x60,  0x00,  0x00,  0x00,  0x01,  0x10,  0x00,  0x40,  0x28,  0x00,  0x00,  0x00,  0x24,  0x00,  0x00,  0x00, 
 0x14,  0x00,  0x00,  0x00,  0xf1,  0x65,  0x81,  0x82,  0x48,  0x1b,  0xc8,  0x23,  0xa1,  0xa8,  0x5d,  0xd3, 
 0xf6,  0x6c,  0x20,  0x00,  0x67,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x04,  0x00,  0x00,  0x00, 
 0x00,  0x00,  0x00,  0x00,  0x02,  0x10,  0x00,  0x40,  0x28,  0x00,  0x00,  0x00,  0x24,  0x00,  0x00,  0x00, 
 0x14,  0x00,  0x00,  0x00,  0xf2,  0x4c,  0x28,  0x68,  0x79,  0xfa,  0x68,  0x2d,  0x9a,  0xdc,  0x4e,  0x54, 
 0x69,  0xd0,  0x26,  0x00,  0xb7,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x04,  0x00,  0x00,  0x00, 
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
struct topic_type_name<::Radar::Track>
{
    static std::string value()
    {
      return org::eclipse::cyclonedds::topic::TopicTraits<::Radar::Track>::getTypeName();
    }
};

}
}

REGISTER_TOPIC_TYPE(::Radar::Track)

namespace org{
namespace eclipse{
namespace cyclonedds{
namespace core{
namespace cdr{

template<>
const propvec &get_type_props<::Radar::Track>();

namespace {
  static const volatile propvec &properties___Radar__Track = get_type_props<::Radar::Track>();
}

template<typename T, std::enable_if_t<std::is_base_of<cdr_stream, T>::value, bool> = true >
bool write(T& streamer, const ::Radar::Track& instance, const entity_properties_t *props) {
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
      if (!write(streamer, instance.id()))
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
      if (!write(streamer, instance.strt_angle()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 3:
      if (!streamer.start_member(*prop))
        return false;
      if (!write(streamer, instance.end_angle()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 4:
      if (!streamer.start_member(*prop))
        return false;
      if (!write(streamer, instance.distance()))
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
bool write(S& str, const ::Radar::Track& instance, key_mode key) {
  const auto &props = get_type_props<::Radar::Track>();
  str.set_mode(cdr_stream::stream_mode::write, key);
  return write(str, instance, props.data()); 
}

template<typename T, std::enable_if_t<std::is_base_of<cdr_stream, T>::value, bool> = true >
bool read(T& streamer, ::Radar::Track& instance, const entity_properties_t *props) {
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
      if (!read(streamer, instance.id()))
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
      if (!read(streamer, instance.strt_angle()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 3:
      if (!streamer.start_member(*prop))
        return false;
      if (!read(streamer, instance.end_angle()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 4:
      if (!streamer.start_member(*prop))
        return false;
      if (!read(streamer, instance.distance()))
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
bool read(S& str, ::Radar::Track& instance, key_mode key) {
  const auto &props = get_type_props<::Radar::Track>();
  str.set_mode(cdr_stream::stream_mode::read, key);
  return read(str, instance, props.data()); 
}

template<typename T, std::enable_if_t<std::is_base_of<cdr_stream, T>::value, bool> = true >
bool move(T& streamer, const ::Radar::Track& instance, const entity_properties_t *props) {
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
      if (!move(streamer, instance.id()))
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
      if (!move(streamer, instance.strt_angle()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 3:
      if (!streamer.start_member(*prop))
        return false;
      if (!move(streamer, instance.end_angle()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 4:
      if (!streamer.start_member(*prop))
        return false;
      if (!move(streamer, instance.distance()))
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
bool move(S& str, const ::Radar::Track& instance, key_mode key) {
  const auto &props = get_type_props<::Radar::Track>();
  str.set_mode(cdr_stream::stream_mode::move, key);
  return move(str, instance, props.data()); 
}

template<typename T, std::enable_if_t<std::is_base_of<cdr_stream, T>::value, bool> = true >
bool max(T& streamer, const ::Radar::Track& instance, const entity_properties_t *props) {
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
      if (!max(streamer, instance.id()))
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
      if (!max(streamer, instance.strt_angle()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 3:
      if (!streamer.start_member(*prop))
        return false;
      if (!max(streamer, instance.end_angle()))
        return false;
      if (!streamer.finish_member(*prop, member_ids))
        return false;
      break;
      case 4:
      if (!streamer.start_member(*prop))
        return false;
      if (!max(streamer, instance.distance()))
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
bool max(S& str, const ::Radar::Track& instance, key_mode key) {
  const auto &props = get_type_props<::Radar::Track>();
  str.set_mode(cdr_stream::stream_mode::max, key);
  return max(str, instance, props.data()); 
}

} //namespace cdr
} //namespace core
} //namespace cyclonedds
} //namespace eclipse
} //namespace org

#endif // DDSCXX__MNT_C_USERS_YUKEV_DOCUMENTS_SCHOOL_4TH_YEAR_ELEC_477_MAIN_ASSIGNMENT4_TRACK_ID_QUERYINSTANCE_BUILD_TRACK_HPP_37FAD95855A143C1636E67205C99F80E
