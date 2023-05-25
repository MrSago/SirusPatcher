#include "QtGui/yserachanger.h"

#include <stdexcept>
#include <string>

#include "DBC/dbchandler.h"
#include "DBC/record.h"

void YseraChanger::Change() {
  const std::string kDbcPath = "./SpellVisualEffectName.dbc";
  const std::string kDbcPathSave = "./SpellVisualEffectName.dbc.save";

  DBCHandler handler;
  if (handler.Load(kDbcPathSave) != DBCError::kSuccess) {
    throw std::runtime_error("gg");
  }

  Record ysera = handler.GetRecordById(8797);
  ysera.AddStringRef(
      3, "Spells\\Stranglethorn_diamondmines_circle_effect_solid_green.mdx");
  ysera.SetFloat(4, 4.15f);
  ysera.SetFloat(5, 4.15f);
  ysera.SetFloat(6, 4.15f);
  ysera.SetFloat(7, 4.15f);

  if (handler.Save(kDbcPath) != DBCError::kSuccess) {
    throw std::runtime_error("wp");
  }
}