#include "QtGui/yserachanger.h"

#include <stdexcept>
#include <string>

#include "DBC/dbchandler.h"
#include "DBC/record.h"

void YseraChanger::Change() {
  const std::string kVisualEffectNameDbcPath = "./SpellVisualEffectName.dbc";
  const std::string kVisualEffectNameDbcPathSave =
      "./SpellVisualEffectName.dbc.save";

  const std::string kVisualDbcPath = "./SpellVisual.dbc";
  const std::string kVisualDbcPathSave = "./SpellVisual.dbc.save";

  const std::string kVisualKitDbcPath = "./SpellVisualKit.dbc";
  const std::string kVisualKitDbcPathSave = "./SpellVisualKit.dbc.save";

  DBCHandler visual_effect_name, visual, visual_kit;
  if (visual_effect_name.Load(kVisualEffectNameDbcPathSave) !=
          DBCError::kSuccess ||
      visual.Load(kVisualDbcPathSave) != DBCError::kSuccess ||
      visual_kit.Load(kVisualKitDbcPathSave) != DBCError::kSuccess) {
    throw std::runtime_error("gg");
  }

  Record ysera = visual_effect_name.GetRecordById(8797);
  ysera.AddStringRef(
      3, "Spells\\Stranglethorn_diamondmines_circle_effect_solid_green.mdx");
  ysera.SetFloat(4, 4.15f);
  ysera.SetFloat(5, 4.15f);
  ysera.SetFloat(6, 4.15f);
  ysera.SetFloat(7, 4.15f);

  Record ysera_visual = visual.GetRecordById(19155);
  unsigned int tmp = ysera_visual.GetUInt32(26);
  ysera_visual.SetUInt32(26, 0);
  ysera_visual.SetUInt32(5, tmp);

  Record ysera_kit = visual_kit.GetRecordById(18001);
  tmp = ysera_kit.GetUInt32(15);
  ysera_kit.SetUInt32(15, 0);
  ysera_kit.SetUInt32(6, tmp);

  if (visual_effect_name.Save(kVisualEffectNameDbcPath) != DBCError::kSuccess ||
      visual.Save(kVisualDbcPath) != DBCError::kSuccess ||
      visual_kit.Save(kVisualKitDbcPath) != DBCError::kSuccess) {
    throw std::runtime_error("wp");
  }
}
