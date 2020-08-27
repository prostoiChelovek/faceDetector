/**
 * @file StandaloneDatabase.h
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.com>
 * @date 26 Aug 2020
 * @copyright MIT License
 *
 * @brief
 */

#ifndef FACES_STANDALONEDATABASE_HPP
#define FACES_STANDALONEDATABASE_HPP

#include <utility>
#include <fstream>

#include <picojson.h> // from third-party/miniconf

#include <Database/Database.hpp>

namespace faces {

    template<typename EntryT, typename IdentifierT = unsigned long>
    class StandaloneDatabase : public Database<EntryT, IdentifierT> {
    public:
        explicit StandaloneDatabase(std::string path) : _path(std::move(path)) {}

        EntryT get(IdentifierT id) override {
            auto &obj = _data.get<picojson::object>();

            std::stringstream ss;
            ss << id;
            std::string strId = ss.str();

            if (obj.find(strId) == obj.end()) {
                throw std::out_of_range("Entry with ID='" + strId + "' does not exist in the database");
            }

            picojson::value entryJson = obj[strId];
            if (!entryJson.is<picojson::object>()) {
                throw std::logic_error("Entry with ID='" + strId + "' has an incorrect format");
            }
            auto const &entryObj = entryJson.get<picojson::object>();

            std::map<std::string, std::any> entryAttributes;
            for (auto const &[name, valueJson] : entryObj) {
                std::any value = _valueToAny(valueJson);
                entryAttributes[name] = value;
            }

            return EntryT(entryAttributes);
        }

        std::vector<IdentifierT> getEntriesList() override {}

        IdentifierT add(EntryT const &entry) override {}

        void update(IdentifierT id, EntryT const &entry) override {}

        bool save() override {
            try {
                std::ofstream file(_path);
                file.exceptions(file.exceptions() | std::ifstream::failbit | std::ifstream::badbit);

                if (!_data.is<picojson::object>()) {
                    _data = picojson::value(picojson::object());
                }

                file << _data;
            } catch (std::exception &e) {
                spdlog::error("Cannot save a database to '{}': {}", _path, e.what());
                return false;
            }

            return true;
        }

        bool load() override {
            try {
                std::ifstream file(_path);
                if (!file.is_open() || file.bad()) {
                    throw std::ios_base::failure(strerror(errno));
                }

                _loadJson(file);
            } catch (std::exception &e) {
                spdlog::error("Cannot load a database from '{}': {}", _path, e.what());
                return false;
            }

            return true;
        }

    protected:
        const std::string _path;

        picojson::value _data;

        void _loadJson(std::ifstream &file) {
            std::string err = picojson::parse(_data, file);
            if (!err.empty()) {
                throw std::logic_error(err);
            }
            if (!_data.is<picojson::object>()) {
                throw std::logic_error("json file has an incorrect format");
            }
        }

        static std::any _valueToAny(picojson::value const &value) {
            std::any res;

            if (value.is<bool>()) {
                res = value.get<bool>();
            } else if (value.is<double>()) {
                res = value.get<double>();
            } else if (value.is<std::string>()) {
                res = value.get<std::string>();
            } else if (value.is<picojson::object>()) {
                res = value.get<picojson::object>();
            }

            return res;
        }

    };

}

#endif //FACES_STANDALONEDATABASE_HPP
