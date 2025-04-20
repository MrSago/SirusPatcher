#include "QtGui/ladyspiritschanger.h"

#include <stdexcept>
#include <string>

#include "DBC/dbchandler.h"
#include "DBC/record.h"

const std::string LadySpiritsChanger::kDbcPath = "./CreatureDisplayInfo.dbc";
const std::string LadySpiritsChanger::kDbcPathSave =
    "./CreatureDisplayInfo.dbc.save";

struct CreatureDisplayInfo {
  uint32_t id;                   // 1
  irefid display_id;             // 2
  irefid sound_id;               // 3
  irefid extra;                  // 4
  float scale;                   // 5
  uint32_t opacity;              // 6
  std::string texture1;          // 7
  std::string texture2;          // 8
  std::string texture3;          // 9
  std::string portrait_texture;  // 10
  irefid blood_level;            // 11
  irefid blood_color;            // 12
  irefid npc_sounds;             // 13
  irefid particles;              // 14
  uint32_t creature_geoset;      // 15
  irefid object_effect;          // 16
};

void LadySpiritsChanger::Change() {
  DBCHandler handler;
  if (handler.Load(kDbcPathSave) != DBCError::kSuccess) {
    throw std::runtime_error("Error loading DBC file:\n" + kDbcPathSave);
  }

  Record fire_elemental = handler.GetRecordById(31320);
  CreatureDisplayInfo fire_elemental_info{
      fire_elemental.GetUInt32(1),  fire_elemental.GetUInt32(2),
      fire_elemental.GetUInt32(3),  fire_elemental.GetUInt32(4),
      fire_elemental.GetFloat(5),   fire_elemental.GetUInt32(6),
      fire_elemental.GetString(7),  fire_elemental.GetString(8),
      fire_elemental.GetString(9),  fire_elemental.GetString(10),
      fire_elemental.GetUInt32(11), fire_elemental.GetUInt32(12),
      fire_elemental.GetUInt32(13), fire_elemental.GetUInt32(14),
      fire_elemental.GetUInt32(15), fire_elemental.GetUInt32(16)};

  Record lady_spirit = handler.GetRecordById(31553);
  lady_spirit.SetUInt32(2, fire_elemental_info.display_id);
  lady_spirit.SetUInt32(3, fire_elemental_info.sound_id);
  lady_spirit.SetUInt32(4, fire_elemental_info.extra);
  lady_spirit.SetFloat(5, fire_elemental_info.scale);
  lady_spirit.SetUInt32(6, fire_elemental_info.opacity);
  lady_spirit.AddStringRef(7, fire_elemental_info.texture1);
  lady_spirit.AddStringRef(8, fire_elemental_info.texture2);
  lady_spirit.AddStringRef(9, fire_elemental_info.texture3);
  lady_spirit.AddStringRef(10, fire_elemental_info.portrait_texture);
  lady_spirit.SetUInt32(11, fire_elemental_info.blood_level);
  lady_spirit.SetUInt32(12, fire_elemental_info.blood_color);
  lady_spirit.SetUInt32(13, fire_elemental_info.npc_sounds);
  lady_spirit.SetUInt32(14, fire_elemental_info.particles);
  lady_spirit.SetUInt32(15, fire_elemental_info.creature_geoset);
  lady_spirit.SetUInt32(16, fire_elemental_info.object_effect);

  lady_spirit.SetFloat(5, 2.0f);

  if (handler.Save(kDbcPath) != DBCError::kSuccess) {
    throw std::runtime_error("wp");
  }
}
