/*
** Taiga
** Copyright (C) 2010-2016, Eren Okka
** 
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <map>

#include "base/file.h"
#include "base/log.h"
#include "base/string.h"
#include "base/time.h"
#include "library/anime.h"
#include "library/anime_db.h"
#include "sync/kitsu_util.h"
#include "sync/kitsu_types.h"
#include "taiga/settings.h"

namespace sync {
namespace kitsu {

int TranslateAgeRatingFrom(const std::string& value) {
  static const std::map<std::string, anime::AgeRating> table{
    {"G", anime::kAgeRatingG},
    {"PG", anime::kAgeRatingPG},
    {"R", anime::kAgeRatingR17},
    {"R18", anime::kAgeRatingR18},
  };

  const auto it = table.find(value);
  if (it != table.end())
    return it->second;

  if (!value.empty())
    LOG(LevelDebug, L"Invalid value: " + StrToWstr(value));

  return anime::kUnknownAgeRating;
}

double TranslateSeriesRatingFrom(double value) {
  return value * 2.0;
}

double TranslateSeriesRatingTo(double value) {
  return value / 2.0;
}

int TranslateSeriesTypeFrom(const std::string& value) {
  static const std::map<std::string, anime::SeriesType> table{
    {"TV", anime::kTv},
    {"special", anime::kSpecial},
    {"OVA", anime::kOva},
    {"ONA", anime::kOna},
    {"movie", anime::kMovie},
    {"music", anime::kMusic},
  };

  const auto it = table.find(value);
  if (it != table.end())
    return it->second;

  LOG(LevelWarning, L"Invalid value: " + StrToWstr(value));
  return anime::kUnknownType;
}

std::wstring TranslateDateFrom(const std::string& value) {
  // Get YYYY-MM-DD from YYYY-MM-DDTHH:MM:SS.000Z (ISO 8601)
  return value.size() >= 10 ? StrToWstr(value.substr(0, 10)) : Date();
}

std::wstring TranslateMyLastUpdatedFrom(const std::string& value) {
  // Get Unix time from ISO 8601
  const auto result = ConvertIso8601(StrToWstr(value));
  return result != -1 ? ToWstr(result) : std::wstring();
}

int TranslateMyRatingFrom(const std::string& value) {
  return static_cast<int>(ToDouble(value) * 2.0);
}

std::wstring TranslateMyRatingTo(int value) {
  return ToWstr(static_cast<double>(value) / 2.0, 1);
}

int TranslateMyStatusFrom(const std::string& value) {
  static const std::map<std::string, anime::MyStatus> table{
    {"current", anime::kWatching},
    {"planned", anime::kPlanToWatch},
    {"completed", anime::kCompleted},
    {"on_hold", anime::kOnHold},
    {"dropped", anime::kDropped},
  };

  const auto it = table.find(value);
  if (it != table.end())
    return it->second;

  LOG(LevelWarning, L"Invalid value: " + StrToWstr(value));
  return anime::kNotInList;
}

std::wstring TranslateMyStatusTo(int value) {
  switch (value) {
    case anime::kWatching: return L"current";
    case anime::kCompleted: return L"completed";
    case anime::kOnHold: return L"on_hold";
    case anime::kDropped: return L"dropped";
    case anime::kPlanToWatch: return L"planned";
  }

  LOG(LevelWarning, L"Invalid value: " + ToWstr(value));
  return L"";
}

////////////////////////////////////////////////////////////////////////////////

static const std::wstring kBaseUrl = L"https://staging.kitsu.io";

std::wstring GetAnimePage(const anime::Item& anime_item) {
  return kBaseUrl + L"/anime/" + anime_item.GetSlug();
}

void ViewAnimePage(int anime_id) {
  auto anime_item = AnimeDatabase.FindItem(anime_id);

  if (anime_item)
    ExecuteLink(GetAnimePage(*anime_item));
}

void ViewDashboard() {
  ExecuteLink(kBaseUrl);
}

void ViewProfile() {
  ExecuteLink(kBaseUrl + L"/users/" +
              Settings[taiga::kSync_Service_Kitsu_Username]);
}

void ViewRecommendations() {
  ExecuteLink(kBaseUrl + L"/recommendations");  // TODO
}

void ViewUpcomingAnime() {
  ExecuteLink(kBaseUrl + L"/anime/upcoming");  // TODO
}

}  // namespace kitsu
}  // namespace sync
